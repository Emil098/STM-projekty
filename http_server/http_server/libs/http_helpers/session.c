/*
 * session.c
 * @brief Session store and per-connection context manager for lwIP httpd.
 *
 * This module provides:
 *  - A fixed-size session table (cookie-based authentication)
 *  - Session creation and validation with TTL expiration
 *  - A fixed-size per-connection context pool (http_conn_ctx_t)
 *
 * Design goals:
 *  - No dynamic memory allocation (static pools only)
 *  - Deterministic RAM footprint
 *  - Simple and fast lookup (linear scan, small table sizes)
 *
 * Notes:
 *  - Session IDs are 32 hex characters (SID_LEN).
 *  - Expiration is based on HAL_GetTick() with a configurable TTL.
 *  Created on: Feb 18, 2026
 *      Author: emilr
 */

#include <console.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "opt.h"
#include "session.h"
#include "console.h"

#define DEBUG_CTX 1
//#define DEBUG_SESSION 1

#if (DEBUG_CTX == 1)
#define ctx_print(...) conpln_fn(__VA_ARGS__)
#else
#define ctx_print(...) { }
#endif

#if (DEBUG_SESSION == 1)
#define session_print(...) conpln_fn(__VA_ARGS__)
#else
#define session_print(...) { }
#endif


static http_conn_ctx_t g_ctx[MAX_CONN_CTX];
static session_t g_sess[MAX_SESSIONS];

static void generate_session_key(char *sid)
{
  // Generates a 32-character hexadecimal session ID.
  // Each loop appends 4 hex digits (total 8 * 4 = 32).
  // NOTE: This is intended for a local embedded UI; for high-security systems,
  // a stronger RNG source should be used.
  int a;
  char rs[11];

  srand(HAL_GetTick());
  for (a = 0; a < 8; a++) {
    sprintf(rs, "%04x", rand());
    rs[4] = '\0'; // enforce exactly 4 hex chars per chunk
    strcat(sid, rs);
  }
  sid[32] = '\0';
}

static int find_free_slot(void)
{
  for (int i = 0; i < MAX_SESSIONS; i++)
    if (!g_sess[i].used) return i;
  return -1;
}

static int find_oldest_slot(void)
{
  int idx = 0;
  uint32_t best = 0xFFFFFFFFu;
  for (int i = 0; i < MAX_SESSIONS; i++) {
    if (!g_sess[i].used) continue;
    if (g_sess[i].expires_at < best) {
      best = g_sess[i].expires_at;
      idx = i;
    }
  }
  return idx;
}

static int sid_is_valid_32(const char *s)
{
  for (int i = 0; i < SID_LEN; i++) {
    char c = s[i];
    if (c == 0) return 0;

    // HEX only:
    if (!((c >= '0' && c <= '9') ||
          (c >= 'a' && c <= 'f') ||
          (c >= 'A' && c <= 'F'))) {
      return 0;
    }
  }
  return 1;
}

static void cleanup_expired(void)
{
  // Removes expired sessions to keep the table clean.
  // Expiration is based on HAL_GetTick() timestamps.
  uint32_t t = HAL_GetTick();

  for (int i = 0; i < MAX_SESSIONS; i++) {
    if (g_sess[i].used && g_sess[i].expires_at <= t) {

      session_print("Releasing session at %08X", &g_sess[i]);
      g_sess[i].used = 0;
      g_sess[i].sid[0] = 0;
      g_sess[i].expires_at = 0;
    }
  }
}

/**
 * @brief Get or create a per-connection HTTP context.
 *
 * Searches for an existing context.
 * If none exists and create is non-zero, allocates a new context
 * from a fixed-size pool.
 *
 * @param[in] connection Pointer uniquely identifying the lwIP connection.
 * @param[in] create     If non-zero, allocate new context when missing.
 *
 * @return Pointer to context structure, or NULL if not found / no free slot.
 */
http_conn_ctx_t* ctx_get(void *connection, uint8_t create)
{
  if(connection == NULL)
  {
    ctx_print("NULL connection");
    return NULL;
  }

  // 1) Try to find an existing context for this connection pointer.
  for (int i = 0; i < MAX_CONN_CTX; i++)
    if (g_ctx[i].connection == connection) return &g_ctx[i];

  // 2) If not found and not allowed to create, return NULL.
  if (!create) return NULL;

  // 3) Allocate a new context from the fixed-size pool.
  for (int i = 0; i < MAX_CONN_CTX; i++) {
    if (g_ctx[i].connection == NULL) {
      memset(&g_ctx[i], 0, sizeof(g_ctx[i]));
      g_ctx[i].connection = connection;
      ctx_print("creating new ctx connection at %08X", &g_ctx[i]);
      return &g_ctx[i];
    }
  }

  // Pool exhausted -> caller must handle the error.
  ctx_print("creating new connection failed");
  return NULL;
}

/**
 * @brief Release a per-connection context back to the pool.
 *
 * Clears context fields and marks the slot as free.
 *
 * @param[in] c Pointer to context returned by ctx_get().
 */
void ctx_free(http_conn_ctx_t *c)
{
  // Returns a context to the pool. The connection pointer acts as the slot key.
  ctx_print("releasing ctx at %08X", c);
  c->connection = NULL;

  // Reset security/session related fields.
  c->authorized = 0;
  c->sid[0] = 0;

  // Reset POST buffering state.
  c->post_len = 0;
  c->post_buf = NULL;
}

/**
 * @brief Find an active session by SID extracted from HTTP cookie header.
 *
 * Validates SID format (32 hex chars), removes expired sessions,
 * and searches the session table.
 *
 * @param[in] begin Pointer to first SID character (immediately after "sid=").
 *
 * @return Pointer to session entry if found and valid, otherwise NULL.
 */
session_t *cookie_find(const char *begin)
{
  if (!begin) return NULL;

  // Validate SID format early (exactly 32 hex characters).
  // This prevents parsing ambiguous/hostile cookie content.
  if (!sid_is_valid_32(begin)) return NULL;

  // Remove expired sessions before searching.
  cleanup_expired();

  // Find matching SID in the session table.
  for (int i = 0; i < MAX_SESSIONS; i++) {
    if (!g_sess[i].used) continue;
    if (memcmp(g_sess[i].sid, begin, SID_LEN) == 0) {
      // TTL already handled in cleanup_expired(), but keep a defensive check.
      if (g_sess[i].expires_at > HAL_GetTick()) {
        session_print("cookie session found at %08X", &g_sess[i]);
        return &g_sess[i];
      }
      // Session expired so invalidate and return NULL.
      g_sess[i].used = 0;
      return NULL;
    }
  }
  return NULL;
}

/**
 * @brief Initialize session and connection context pools.
 *
 * Clears session table and connection context pool.
 * Should be called once at system startup.
 */
void sessions_init(void)
{
  memset(g_sess, 0, (sizeof(session_t) * MAX_SESSIONS));
  memset(g_ctx, 0, (sizeof(http_conn_ctx_t) * MAX_CONN_CTX));
}

/**
 * @brief Check whether a given SID exists and is not expired.
 *
 * @param[in] sid 32-byte SID string (not necessarily null-terminated).
 *
 * @return 1 if valid, 0 otherwise.
 */
int session_is_valid(const char *sid)
{
  uint32_t t = HAL_GetTick();
  for (int i = 0; i < MAX_SESSIONS; i++) {
    if (!g_sess[i].used) continue;
    if (g_sess[i].expires_at <= t) { g_sess[i].used = 0; continue; }

    if (memcmp(g_sess[i].sid, sid, SID_LEN) == 0) {
      return 1;
    }
  }
  return 0;
}

/**
 * @brief Extend session lifetime (sliding expiration).
 *
 * Updates expiration timestamp for an active session.
 *
 * @param[in,out] s Pointer to active session entry.
 */
void session_touch(session_t *s)
{
  if (!s || !s->used) return;
  s->expires_at = HAL_GetTick() + (uint32_t)SESSION_TTL_S;
  session_print("extending time for session at %08X", s);
}

/**
 * @brief Create a new session entry (may reuse slot if table is full).
 *
 * Removes expired sessions, then:
 *  - Uses a free slot if available
 *  - Otherwise replaces the oldest active session
 *
 * @return Pointer to the created session entry.
 */
session_t *session_create()
{
  // Ensure the table is cleaned before allocating a slot.
  cleanup_expired();

  // Prefer a free slot; if full, replace the oldest active session.
  int slot = find_free_slot();
  if (slot < 0)
    slot = find_oldest_slot();

  g_sess[slot].used = 1;
  g_sess[slot].sid[SID_LEN] = 0;
  g_sess[slot].expires_at = HAL_GetTick() + (uint32_t)SESSION_TTL_S;

  // Generate a new SID for this session.
  generate_session_key(&g_sess[slot].sid[0]);

  session_print("session created at %08X", &g_sess[slot]);

  return &g_sess[slot];
}

