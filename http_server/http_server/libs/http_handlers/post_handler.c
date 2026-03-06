/*
 * post_handler.c
 * @brief HTTP POST handling layer for lwIP httpd (REST-style API).
 *
 * This module implements the lwIP httpd POST callback interface:
 *
 *   - httpd_preview_headers()
 *   - httpd_post_begin()
 *   - httpd_post_receive_data()
 *   - httpd_post_finished()
 *
 * Responsibilities:
 *   - Session validation based on Cookie header
 *   - Safe buffering of POST payload (static allocation)
 *   - JSON parsing (flat objects)
 *   - Endpoint dispatching
 *   - Conditional redirection (e.g. login flow)
 *
 * Design assumptions:
 *   - No dynamic memory allocation
 *   - Single shared POST buffer protected by FreeRTOS mutex
 *   - Intended for low-resource embedded systems (STM32 + lwIP + FreeRTOS)
 *
 *  Created on: Feb 20, 2026
 *      Author: emilr
 */

#include "fs_handler.h"
#include "http_defs.h"
#include "post_handler.h"
#include "sutils.h"
#include "rest_helper.h"
#include <string.h>

#include "FreeRTOS.h"
#include "semphr.h"
#include "lwip.h"

#include "session.h"
#include "console.h"
#include "settings.h"

#define MAX_POSTDATA_SIZE  4096

#define DEBUG_POST 1

#if (DEBUG_POST == 1)
#define post_print(...) conpln_fn(__VA_ARGS__)
#else
#define post_print(...) { }
#endif

/**
 * lwIP httpd POST callback lifecycle (typical order):
 *   1) httpd_preview_headers()     - inspect headers (e.g. Cookie)
 *   2) httpd_post_begin()          - allocate/prepare per-connection state + buffer
 *   3) httpd_post_receive_data()   - receive body fragments (may be called multiple times)
 *   4) httpd_post_finished()       - finalize, parse payload, dispatch endpoint, set response URI
 *
 * Design notes:
 *  - POST payload is stored in a single shared static buffer (no malloc).
 *  - The buffer is protected by a FreeRTOS mutex to keep the implementation deterministic.
 */
static char POSTDataBuffer[MAX_POSTDATA_SIZE];
SemaphoreHandle_t POSTDataMutex;



/**
 * @brief Initialize POST handler synchronization primitives.
 *
 * Creates a static FreeRTOS mutex used to protect the global POST buffer.
 *
 * Static allocation is used to:
 *   - Avoid heap usage
 *   - Guarantee deterministic memory footprint
 */
void post_handlers_init(void)
{
  // Static FreeRTOS mutex: avoids heap usage and keeps memory footprint predictable.
  static StaticSemaphore_t POSTDataMutexBuffer;
  POSTDataMutex = xSemaphoreCreateMutexStatic(&POSTDataMutexBuffer);
}

/**
 * @brief Inspect HTTP headers before POST processing.
 *
 * Extracts the "sid" session cookie from the HTTP header,
 * validates it against the session store,
 * and marks the connection context as authorized if valid.
 *
 * Flow:
 *   1. Search for "Cookie:" header
 *   2. Extract "sid=" token
 *   3. Validate session via cookie_find()
 *   4. Refresh session timeout (session_touch())
 *   5. Mark connection context as AUTH_LOGGED
 *
 * Security note:
 *   - Only 32-character session IDs are accepted
 *   - Missing or invalid cookies do not authorize the connection
 */
void httpd_preview_headers(void *connection, const char *uri, const char *http_request, u16_t http_request_len)
{

  (void)uri;

  /**
   * Session authorization stage:
   *  - Extract "sid" cookie from incoming HTTP headers.
   *  - If a valid session exists, refresh its lifetime (touch)
   *    and mark the per-connection context as AUTH_LOGGED.
   *
   * This allows later layers (fs_open_custom / endpoint handlers)
   * to enforce access control based on c->authorized.
   */

  char *ck = lwip_strnstr(http_request, "Cookie: ", http_request_len);

  if (ck) {
    char *begin = lwip_strnstr(ck, "sid=", http_request_len - (ck - http_request)) + 4;
    if (begin) {
      // Cookie header may end either at CRLF or at ';' (multiple cookies).
      char *end = lwip_strnstr(begin, "\r\n", http_request_len - (begin - http_request));
      if (!end)
        end = lwip_strnstr(begin, ";", http_request_len - (begin - http_request));

      // Accept only fixed-length session IDs (32 chars) to reduce parsing ambiguity.
      if (end && (end-begin == 32)) {
        session_t *session = cookie_find(begin);
        if (session) {
          session_touch(session);

          // Create/retrieve connection context and mark it as authorized.
          http_conn_ctx_t *c = ctx_get(connection, 1);
          if(c)
            c->authorized = AUTH_LOGGED;
          else
            post_print("httpd_preview_headers: ctx creation failed");
        }
        else
          post_print("httpd_preview_headers: session not found");
      }
    }
  }
  else
    post_print("httpd_preview_headers: no cookie in http header");
}

/**
 * @brief Called by lwIP when a POST request begins.
 *
 * Responsibilities:
 *   - Create or retrieve connection context
 *   - Store requested URI
 *   - Validate POST content length
 *   - Lock global POST buffer
 *   - Prepare buffer for incoming payload
 *
 * Memory model:
 *   - Single shared static POST buffer
 *   - Protected by FreeRTOS mutex
 *   - No dynamic allocation
 *
 * @return ERR_OK on success, ERR_MEM on failure.
 */
err_t httpd_post_begin(void *connection, const char *uri, const char *http_request,
                       u16_t http_request_len, int content_len, char *response_uri,
                       u16_t response_uri_len, u8_t *post_auto_wnd)
{
  (void)http_request;
  (void)http_request_len;
  (void)post_auto_wnd;

  // By default respond with the same URI; httpd_post_finished() may override it (redirect flow).
  strncpy(response_uri, uri, response_uri_len);

  // Create/retrieve connection context for this request.
  http_conn_ctx_t *c = ctx_get(connection, 1);
  if (!c) {
    post_print("httpd_post_begin: " RED "failed to create connection context. Exiting" CLRST);
    return ERR_MEM;
  }

  // Save requested URI in the connection context for later use.
  strncpy(c->file_name, uri, MAX_FILENAME);

  /**
   * POST body buffering strategy:
   *  - Use a single shared static buffer (POSTDataBuffer).
   *  - Protect it with POSTDataMutex.
   *  - Reject payloads exceeding MAX_POSTDATA_SIZE.
   */
  if (content_len+1 <= MAX_POSTDATA_SIZE) {
    if (xSemaphoreTake(POSTDataMutex, 2000) != pdTRUE) {
      post_print(YELLOW "failed to take mutex" CLRST);
      return ERR_MEM;
    }
    c->post_buf = POSTDataBuffer;
    c->post_allocated = content_len+1; // +1 for terminating '\0' appended in post_finished()
    c->post_len = 0;
    c->status = STATUS_NONE;
  }
  else {

    post_print(YELLOW "POST content_len+1 exceeds maximum (%lu > %lu)" CLRST, content_len+1, MAX_POSTDATA_SIZE);
    return ERR_MEM;
  }

  return ERR_OK;
}

/**
 * @brief Receive incoming POST data fragments.
 *
 * Copies payload from lwIP pbuf into the connection's POST buffer.
 * Supports fragmented TCP packets.
 *
 * Ensures:
 *   - No buffer overflow
 *   - Graceful error handling
 *
 * This function may be called multiple times
 * until the full POST body is received.
 */
err_t httpd_post_receive_data(void *connection, struct pbuf *p)
{
  // Retrieve per-connection context created earlier.
  http_conn_ctx_t *c = fs_state_init(connection, "");
  if (!c) {
    post_print("httpd_post_receive_data: " RED "connection context not created" CLRST);
    pbuf_free(p);
    return ERR_MEM;
  }

  // Copy incoming payload fragment into the prepared POST buffer.
  if (c->post_buf) {
    if (c->post_len + p->len <= c->post_allocated) {
      memcpy(c->post_buf + c->post_len, p->payload, p->len);
      c->post_len  += p->len;
    } else {
      post_print("httpd_post_receive_data: " RED "ERROR" CLRST ", tried to copy %u data to %u buffer space", p->len, c->post_allocated - c->post_len);
      pbuf_free(p);
      return ERR_BUF;
    }
  } else {
    post_print("httpd_post_receive_data: " RED "ERROR" CLRST ", post data not allocated");
    pbuf_free(p);
    return ERR_BUF;
  }

  // Always free pbuf
  pbuf_free(p);
  return ERR_OK;
}

/**
 * @brief Finalize POST processing.
 *
 * Called by lwIP after full POST payload has been received.
 *
 * Processing steps:
 *   1. Null-terminate POST buffer
 *   2. Determine endpoint based on URI
 *   3. Parse JSON payload (flat object)
 *   4. Dispatch endpoint handler
 *   5. Optionally modify response URI (e.g. login redirect)
 *   6. Release POST buffer mutex
 *
 * Redirect logic:
 *   - If endpoint sets OPERATION_COOKIE and STATUS_OK,
 *     response is redirected to COOKIE_REDIR_HTM.
 *
 * This function acts as the REST dispatcher layer.
 */
void httpd_post_finished(void *connection, char *response_uri, u16_t response_uri_len)
{
  char     *keys[MAX_POST_FIELDS];
  char     *values[MAX_POST_FIELDS];
  int       count, endpoint;
  char     *redir = COOKIE_REDIR_HTM;

  // Retrieve per-connection context created earlier.
  http_conn_ctx_t *c = fs_state_init(connection, "");
  if (!c) {
    post_print("httpd_post_finished: " RED "connection context not created" CLRST);
    return;
  }

  // Restore original request URI (may be overridden for redirect purposes below).
  strncpy(response_uri, c->file_name, response_uri_len);

  // Null-terminate payload for JSON parsing (buffer reserved +1 in post_begin()).
  *((char*)(c->post_buf) + c->post_len) = '\0';

  // Map URI to known REST endpoint.
  endpoint = check_endpoint_name(response_uri);

  if(endpoint >= 0)
  {

    // Parse flat JSON object into key/value pairs (in-place, no allocations).
    c->status = STATUS_OK;
    count = post_extract_json(c->post_buf, keys, values);

    // execute endpoint implementation (business logic layer).
    process_endpoint(c, endpoint, count, keys, values);

    /**
     * Login flow integration:
     * If endpoint resulted in a valid cookie operation (session created),
     * redirect to a special HTML resource handled by fs_open_custom().
     *
     * The filesystem layer then returns an HTTP response that sets the cookie
     * and redirects to dashboard.
     */
    if(c->status == STATUS_OK && c->operation == OPERATION_COOKIE)
      strncpy(response_uri, redir, strlen(redir));
  }

  // Release shared POST buffer for the next request.
  xSemaphoreGive(POSTDataMutex);
}
