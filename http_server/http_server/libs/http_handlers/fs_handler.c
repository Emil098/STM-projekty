/*
 * fs_handle.c
 * @brief Custom filesystem hooks for lwIP httpd (dynamic pages + REST endpoints).
 *
 * This module implements lwIP httpd filesystem callbacks:
 *   - fs_open_custom()
 *   - fs_close_custom()
 *   - fs_state_init()
 *   - fs_state_free()
 *
 * Responsibilities:
 *   - Serve static HTML pages from fsdata (login form)
 *   - Serve dynamic HTML responses (login redirect page, logout page, 404)
 *   - Generate REST API responses using a shared static buffer
 *   - Enforce authentication rules (cookie/session based)
 *   - Provide per-connection state (http_conn_ctx_t) via fs_state_* callbacks
 *
 * Memory model:
 *   - One shared PageBuffer used for dynamic responses
 *   - Protected by a FreeRTOS mutex (FSDataMutex)
 *   - No dynamic memory allocation
 *
 *  Created on: Feb 17, 2026
 *      Author: emilr
 */
#include "fs_handler.h"
#include "http_defs.h"
#include "sutils.h"
#include <string.h>

#include "FreeRTOS.h"
#include "semphr.h"

#include "session.h"
#include "console.h"
#include "rest_helper.h"

#define PAGE_BUFFER_SIZE 300

#define DEBUG_FS 1

#if (DEBUG_FS == 1)
#define fs_print(...) conpln_fn(__VA_ARGS__)
#else
#define fs_print(...) { }
#endif

static char PageBuffer[PAGE_BUFFER_SIZE];

SemaphoreHandle_t FSDataMutex;

extern const struct fsdata_file file__login_shtml[];


/**
 * @brief Initialize custom filesystem handler module.
 *
 * Creates synchronization primitives required for dynamic
 * page generation. Must be called once during system startup
 * before HTTP server begins serving requests.
 */
void fs_handlers_init(void)
{
  // Static FreeRTOS mutex to protect the shared PageBuffer.
  // Using static allocation avoids heap usage and keeps memory deterministic.
  static StaticSemaphore_t FSDataMutexBuffer;
  FSDataMutex = xSemaphoreCreateMutexStatic(&FSDataMutexBuffer);
}

/**
 * @brief Close custom file and release associated resources.
 *
 * Called by lwIP httpd after a custom file response
 * has been fully transmitted.
 *
 * Releases the shared PageBuffer mutex if the response
 * was dynamically generated.
 */
void fs_close_custom(struct fs_file *file)
{
  // lwIP calls this when it finishes serving a file.
  // For dynamically generated pages, we release FSDataMutex
  // (unless the served content is static / constant and no mutex was taken).
  if (file->is_custom_file) {
    // These responses do not use the shared PageBuffer,
    // therefore we must NOT release the mutex here.
    if (    (file->data == (void*)&PAGE_404)
        ||  (file->data == (void*)&HTTP_204)
        ||  (file->data == (void*)&LOGOUT_PAGE)
        ||  (file->data == (void*)file__login_shtml->data)) {

    }
    else {
      // Dynamic responses were generated into PageBuffer so release lock.
      xSemaphoreGive(FSDataMutex);
    }
  }
}

/**
 * @brief Handle custom file open requests from lwIP httpd.
 *
 * Intercepts file requests and provides:
 *   - Static protected pages (login, logout, 404)
 *   - Dynamic login redirect responses (with Set-Cookie header)
 *   - REST API responses generated at runtime
 *
 * Implements access control based on session authorization.
 *
 * @return 1 if the file was handled by this function,
 *         0 if lwIP should fall back to default fsdata.
 */
int fs_open_custom(struct fs_file *file, const char *name)
{
  const char *ext = name + strlen(name);
  http_conn_ctx_t *c = ctx_get(file,0);
  int endpoint = 0;

  // Find file extension ('.htm', '.html', '.shtml', or REST endpoint without extension)
  while ((ext > name) && (*ext != '.')) ext--;

  // -------- HTML / SHTML content (web pages) --------
  if ((strcmp(ext, ".htm") == 0) || (strcmp(ext, ".html") == 0) \
   || (strcmp(ext, ".shtml") == 0))
  {
    // If user is authenticated, allow access to protected pages.
    if (c && (c->authorized == AUTH_LOGGED)) {

      // Serve a custom 404 page (with full HTTP header included).
      if (strcmp(name, "/404.html") == 0) {
        file->len = sizeof(PAGE_404);
        file->data = (void*)&PAGE_404;
        file->index = file->len;
        file->pextension = NULL;
        file->flags = FS_FILE_FLAGS_HEADER_INCLUDED;
        return 1;
      }

      // Login redirect handler:
      // When POST /api/login succeeds and cookie is generated,
      // POST handler redirects to COOKIE_REDIR_HTM which is handled here.
      else if (strcmp(name, COOKIE_REDIR_HTM) == 0) {

        // Only valid after successful cookie operation.
        if ((c) && (c->operation == OPERATION_COOKIE && c->status == STATUS_OK)) {

          // Lock shared PageBuffer before generating dynamic response.
          if (xSemaphoreTake(FSDataMutex, 1000)) {
            fs_print("cookie found, redirecting to dashboard");

            // Build HTTP response with Set-Cookie header + HTML that redirects to dashboard.
            strcpy(PageBuffer, LOGIN_OK_HEAD);
            strncat(PageBuffer, c->sid, 32);
            strcat(PageBuffer, LOGIN_OK_PAGE);
            file->data = PageBuffer;
            file->len = strlen(PageBuffer);
            file->index = file->len;
            file->pextension = NULL;
            file->flags = FS_FILE_FLAGS_HEADER_INCLUDED;
            return 1;
          } else
            fs_print(YELLOW "failed to take mutex" CLRST);
        }
      }

      // Logout endpoint: serve a static "logout" response/page.
      else if (strcmp(name, "/logout.htm") == 0) {
        fs_print("loggin out");
        file->len = sizeof(LOGOUT_PAGE);
        file->data = (void*)&LOGOUT_PAGE;
        file->index = file->len;
        file->pextension = NULL;
        file->flags = FS_FILE_FLAGS_HEADER_INCLUDED;
        return 1;
      }
    }// AUTH_LOGGED

    // If not authenticated:
    // Always serve login form regardless of requested page.
    // This acts as a simple access control gate for the UI.
    else {
      file->data = (const char *)file__login_shtml->data;
      file->len = file__login_shtml->len;
      file->index = file__login_shtml->len;
      file->pextension = NULL;
      file->flags = file__login_shtml->flags;
#if HTTPD_PRECALCULATED_CHECKSUM
      file->chksum_count = file__login_shtml->chksum_count;
      file->chksum = file__login_shtml->chksum;
#endif /* HTTPD_PRECALCULATED_CHECKSUM */
      return 1;
    }
  }

  // -------- REST endpoints / dynamic API responses --------
  else
  {
    // If the requested resource matches a known endpoint name
    if((endpoint = check_endpoint_name(name)) >= 0)
    {
      c = ctx_get(file,0);

      // Allow REST calls either for authenticated users,
      // or for special case: cookie operation finished with STATUS_FAIL
      // (e.g. login attempt with wrong credentials -> allow returning JSON error).
      if (c && ((c->authorized == AUTH_LOGGED) || \
         (c->operation == OPERATION_COOKIE && c->status == STATUS_FAIL))) {

        // Lock shared PageBuffer before generating dynamic JSON response.
        if (xSemaphoreTake(FSDataMutex, 1000))
        {
          // Generate endpoint response into PageBuffer.
          file->len = create_endpoint(c, endpoint, PageBuffer, PAGE_BUFFER_SIZE);;
          file->data = PageBuffer;
          file->index = file->len;
          file->pextension = NULL;
          file->flags = FS_FILE_FLAGS_HEADER_INCLUDED;
          return 1;
        }
        else
          fs_print(YELLOW "failed to take mutex" CLRST);
      }
      else
      {
        // Missing/unauthorized connection context -> return 404.
        fs_print("rest api - connection missing");
        file->len = sizeof(PAGE_404);
        file->data = (void*)&PAGE_404;
        file->index = file->len;
        file->pextension = NULL;
        file->flags = FS_FILE_FLAGS_HEADER_INCLUDED;
        return 1;
      }
    }
  }

  // Not a custom file handled by this module -> allow lwIP to try the default fsdata.
  return 0;
}

/**
 * @brief Initialize per-connection state.
 *
 * Called by lwIP httpd to associate a connection-specific
 * context structure with the current file request.
 *
 * Returns pointer to http_conn_ctx_t structure.
 */
void *fs_state_init(struct fs_file *file, const char *name)
{
  // Called by lwIP to obtain per-file/per-connection state.
  // We map it to our connection context structure.
  (void)name;
  return ctx_get(file, 0);
}

/**
 * @brief Free per-connection state.
 *
 * Called by lwIP httpd when a file/connection
 * is being closed. Ensures that connection context
 * is properly released.
 */
void fs_state_free(struct fs_file *file, void *state)
{
  // Called by lwIP when the connection/file is being closed.
  // Ensures connection context is released to avoid leaks.
  if (state)
    ctx_free(state);
  else if (file) {
    // Fallback: try to fetch context from file pointer.
    http_conn_ctx_t *c = ctx_get(file, 0);
    if (c)
      ctx_free(c);
  }
}
