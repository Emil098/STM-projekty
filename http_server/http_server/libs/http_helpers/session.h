/*
 * session.h
 *
 *  Created on: Feb 18, 2026
 *      Author: emilr
 */

#ifndef LIB_SESSION_H_
#define LIB_SESSION_H_

#include <stdbool.h>


/* ----------------------------- Configuration ----------------------------- */

/** @brief Maximum number of simultaneous connection contexts. */
#define MAX_CONN_CTX    8

/** @brief Session ID length (32 hex characters). */
#define SID_LEN         32

/** @brief Maximum number of active sessions stored in the table. */
#define MAX_SESSIONS    8

/**
 * @brief Session time-to-live in milliseconds.
 *
 * Used with HAL_GetTick() timestamps.
 * Example: 15 minutes -> (15 * 60 * 1000).
 */
#define SESSION_TTL_S   (1 * 60 * 1000)  /* NOTE: currently 1 minute (60,000 ms). */

/** @brief Maximum length of requested URI stored per connection. */
#define MAX_FILENAME    100

/* ------------------------------- Enumerations ------------------------------- */

/**
 * @brief Authorization state of a connection.
 */
typedef enum {
  AUTH_NONE = 0,   /**< Not authenticated (no valid session cookie). */
  AUTH_LOGGED,     /**< Authenticated (valid session cookie present). */
} auth_result_t;

/**
 * @brief Status of the last operation performed within the connection context.
 *
 * Used by higher layers to decide how to respond (e.g. redirect after login).
 */
typedef enum {
  STATUS_NONE = 0x00000000, /**< No status set / not processed yet. */
  STATUS_FAIL = 0x00000001, /**< Operation failed (e.g. invalid credentials). */
  STATUS_OK   = 0x80000000, /**< Operation succeeded. */
} conn_status;

/**
 * @brief Operation type performed within the connection context.
 *
 * Used to distinguish flows like cookie/session creation during login.
 */
typedef enum {
  OPERATION_NONE   = 0x00000000, /**< No special operation. */
  OPERATION_COOKIE = 0x00000001  /**< Session cookie creation/login flow. */
} conn_operation;

/* --------------------------------- Structures -------------------------------- */

/**
 * @brief Per-connection context associated with lwIP httpd connection pointer.
 *
 * The @ref connection field acts as a key that allows mapping lwIP callback
 * parameters to a persistent context structure.
 *
 * This structure tracks:
 *  - Authentication state derived from the Cookie header
 *  - Session ID (SID)
 *  - POST buffering information (shared static buffer is used by upper layer)
 *  - Last requested URI (used for routing/redirects)
 *  - Operation status flags used by higher layers (REST/router/filesystem)
 */
typedef struct {
  void *connection;                   /**< lwIP connection pointer (context key). */
  char  sid[SID_LEN + 1];             /**< Session ID extracted from "Cookie: sid=...". */

  /* POST handling */
  void     *post_buf;                 /**< Pointer to POST buffer assigned by POST layer. */
  uint32_t  post_allocated;           /**< Allocated/allowed POST payload size (bytes). */
  uint32_t  post_len;                 /**< Current length of received POST body (bytes). */
  char      file_name[MAX_FILENAME];  /**< Requested URI/path saved for routing. */

  /* State flags */
  conn_status     status;             /**< Result of last processed operation. */
  conn_operation  operation;          /**< Operation type performed (e.g. cookie creation). */
  auth_result_t   authorized;         /**< Authorization state for the connection. */
} http_conn_ctx_t;

/**
 * @brief Session table entry.
 *
 * Stores a single active session with an expiration timestamp.
 */
typedef struct {
  uint8_t  used;                      /**< Slot used flag (1 = active, 0 = free). */
  char     sid[SID_LEN + 1];          /**< Session ID (null-terminated). */
  uint32_t expires_at;                /**< Expiration tick timestamp (HAL_GetTick() + TTL). */
} session_t;

/* ---------------------------------- API ---------------------------------- */

void sessions_init(void);

void ctx_free(http_conn_ctx_t *c);

http_conn_ctx_t* ctx_get(void *connection, uint8_t create);

int session_is_valid(const char *sid);

session_t *cookie_find(const char *begin);

void session_touch(session_t *s);

session_t *session_create();

#endif /* LIB_SESSION_H_ */
