/*
 * rest_helper.c
 * @brief Lightweight REST routing layer for lwIP httpd.
 *
 * This module provides:
 *  - Static endpoint routing table
 *  - JSON response generation (GET-like endpoints)
 *  - JSON request processing (POST endpoints)
 *  - Authorization gating per endpoint
 *
 * Design assumptions:
 *  - No dynamic memory allocation
 *  - Fixed-size buffers
 *  - Deterministic execution
 *  - Explicit authorization control via http_conn_ctx_t
 *
 * Endpoints are defined in a static table and consist of:
 *  - entry  : URL path (e.g. "/api/sensor")
 *  - create : function generating JSON response
 *  - process: function handling POST JSON body
 *
 * The module acts as a bridge between:
 *  - lwIP httpd layer
 *  - session/authorization layer
 *  - application logic (settings, sensor, main task queue)
 *  Created on: Feb 24, 2026
 *      Author: emilr
 */
#include "rest_helper.h"

#include <stdio.h>
#include <string.h>

#include "ip_format.h"
#include "console.h"
#include "sutils.h"
#include "http_defs.h"
#include "sensor.h"
#include "settings.h"
#include "main.h"
#include "FreeRTOS.h"
#include "semphr.h"

#define MAX2(a,b) ((a >= b) ? a : b)

/**
 * @brief Function pointer type for endpoint response creation.
 *
 * @param[in]  c         Connection context.
 * @param[out] buffer    Output buffer for JSON response.
 * @param[in]  MAX_SIZE  Maximum allowed buffer size.
 *
 * @return Length of generated response, or -1 on error.
 */
typedef int (*endpoint_create)(http_conn_ctx_t *c,
                               char *buffer,
                               const uint32_t MAX_SIZE);

/**
 * @brief Function pointer type for endpoint POST processing.
 *
 * @param[in,out] c        Connection context.
 * @param[in]     elements Number of JSON key/value pairs.
 * @param[in]     keys     Parsed JSON keys.
 * @param[in]     values   Parsed JSON values.
 */
typedef void (*endpoint_process)(http_conn_ctx_t *c,
                                 const uint8_t elements,
                                 char **keys,
                                 char **values);

/**
 * @brief Single REST endpoint descriptor.
 */
typedef struct {
  const char       *entry;    /**< URL path (e.g. "/api/sensor") */
  endpoint_create   create;   /**< JSON response generator */
  endpoint_process  process;  /**< POST handler (nullable) */
} endpoint_t;

static int make_sensor_endpoint(http_conn_ctx_t *c, char *buffer, const uint32_t MAX_SIZE);
static int make_network_endpoint(http_conn_ctx_t *c, char *buffer, const uint32_t MAX_SIZE);
static int make_auth_endpoint(http_conn_ctx_t *c, char *buffer, const uint32_t MAX_SIZE);
static int make_login_endpoint(http_conn_ctx_t *c, char *buffer, const uint32_t MAX_SIZE);

static void process_network_endpoint(http_conn_ctx_t *c, const uint8_t elements, char **keys, char **values);
static void process_auth_endpoint(http_conn_ctx_t *c, const uint8_t elements, char **keys, char **values);
static void process_login_endpoint(http_conn_ctx_t *c, const uint8_t elements, char **keys, char **values);


/**
 * @brief Static endpoint registry.
 *
 * To add a new endpoint:
 *  1. Implement make_xxx_endpoint()
 *  2. Optionally implement process_xxx_endpoint()
 *  3. Add entry above the NULL terminator.
 *
 * Order defines endpoint_id used internally.
 */
static const endpoint_t endpoints[] = {
    { "/api/sensor", make_sensor_endpoint  ,  NULL                     },
    { "/api/net"   , make_network_endpoint ,  process_network_endpoint },
    { "/api/auth"  , make_auth_endpoint    ,  process_auth_endpoint    },
    { "/api/login" , make_login_endpoint   ,  process_login_endpoint   },
    { NULL,          NULL,                    NULL }
};

typedef enum{
  EP_SENSOR = 0,
  EP_NETWORK,
  EP_AUTH
}ep_t;

/**
 * @brief Check whether requested path matches endpoint entry.
 *
 * Supports:
 *  - exact match ("/api/net")
 *  - query string ("/api/net?x=1")
 *  - optional subroute ("/api/net/...")
 *
 * @return 1 if matches, otherwise 0.
 */
static int path_matches(const char *name, const char *entry)
{
  const size_t elen = strlen(entry);

  if (strncmp(name, entry, elen) != 0) {
    return 0;
  }

  /* Accept exact match or query string directly after endpoint path. */
  const char next = name[elen];
  return (next == '\0' || next == '?' || next == '/'); /* '/' optional if you plan subroutes */
}

/**
 * @brief Resolve endpoint index by URL path.
 *
 * @param[in] name Requested URI.
 *
 * @return Endpoint index if found, otherwise -1.
 */
int check_endpoint_name(const char *name)
{
  if (name == NULL) {
    return -1;
  }

  for (uint16_t i = 0; endpoints[i].entry != NULL; i++) {
    if (path_matches(name, endpoints[i].entry)) {
      return (int)i;
    }
  }

  return -1;
}

/**
 * @brief Generate JSON response for given endpoint.
 *
 * Performs:
 *  - parameter validation
 *  - endpoint existence check
 *  - authorization check (except login endpoint)
 *  - response generation via endpoint-specific create handler
 *
 * @param[in]  c           Connection context.
 * @param[in]  endpoint_id Index returned by check_endpoint_name().
 * @param[out] buffer      Output JSON buffer.
 * @param[in]  MAX_SIZE    Maximum buffer size.
 *
 * @return Length of generated response, or -1 on error.
 */
int create_endpoint(http_conn_ctx_t *c, int endpoint_id, char *buffer, const uint32_t MAX_SIZE)
{
  bool is_loginpage = false;
  /* Validate parameters to avoid out-of-bounds access. */
  if (buffer == NULL || MAX_SIZE == 0) {
    return -1;
  }
  if (endpoint_id < 0) {
    return -1;
  }

  const uint16_t id = (uint16_t)endpoint_id;
  if (endpoints[id].entry == NULL || endpoints[id].create == NULL) {
    return -1;
  }

  is_loginpage = memcmp(endpoints[id].entry, "/api/login", strlen(endpoints[id].entry)) == 0 ? \
      true : false;

  if(c == NULL && is_loginpage == false)
    return -1;

  if(c->authorized != AUTH_LOGGED && is_loginpage == false)
  {
    c->status = STATUS_FAIL;
    return -1;
  }

  return endpoints[id].create(c, buffer, MAX_SIZE);
}

/**
 * @brief Process POST data for a given endpoint.
 *
 * Performs:
 *  - validation of input arrays
 *  - endpoint existence check
 *  - authorization validation (except login)
 *  - dispatch to endpoint-specific process handler
 *
 * @param[in,out] c         Connection context.
 * @param[in]     endpoint_id Endpoint index.
 * @param[in]     elements   Number of parsed JSON elements.
 * @param[in]     keys       Array of JSON keys.
 * @param[in]     values     Corresponding JSON values.
 */
void process_endpoint(http_conn_ctx_t *c, int endpoint_id, const uint8_t elements, char **keys, char **values)
{
  bool is_loginpage = false;
  /* Validate parameters to avoid out-of-bounds access. */
  if (keys == NULL || values == NULL || elements == 0)
    return;

  if (endpoint_id < 0)
    return;


  const uint16_t id = (uint16_t)endpoint_id;
  if (endpoints[id].entry == NULL || endpoints[id].process == NULL)
    return;


  is_loginpage = \
    memcmp(endpoints[id].entry, "/api/login", strlen(endpoints[id].entry)) == 0 ? \
    true : false;

  if(c == NULL)
    return;

  if(c->authorized != AUTH_LOGGED && is_loginpage == false)
  {
    c->status = STATUS_FAIL;
    return;
  }

  endpoints[id].process(c, elements, keys, values);
}

/**
 * @brief Generate sensor data JSON.
 *
 * Returns:
 * {
 *   "temp": <float>,
 *   "hum": <float>
 * }
 *
 * Data source: get_sensor()
 */
static int make_sensor_endpoint(http_conn_ctx_t *c, char *buffer, const uint32_t MAX_SIZE)
{
  int len = 0;
  char tag_value[16];
  float temp = 0.0f, hum = 0.0f;

  // Acquire current sensor readings (non-blocking).
  get_sensor(&temp, &hum);

  if (!AddTextToBuffer(&len, buffer, MAX_SIZE, JSON_HEAD("/api/sensor"))) {
    conpln(RED "ERROR: buffer is too small" CLRST);
    return -1;
  }

  if (!AddTextToBuffer(&len, buffer, MAX_SIZE, JSON_BODY_BEGG)) {
    conpln(RED "ERROR: buffer is too small" CLRST);
    return -1;
  }

  (void)snprintf(tag_value, sizeof(tag_value), "%.1f", (double)temp);
  if (!AddTextToBuffer(&len, buffer, MAX_SIZE, "\"temp\": %s,", tag_value)) {
    conpln(RED "ERROR: buffer is too small" CLRST);
    return -1;
  }

  (void)snprintf(tag_value, sizeof(tag_value), "%.1f", (double)hum);
  if (!AddTextToBuffer(&len, buffer, MAX_SIZE, "\"hum\": %s", tag_value)) {
    conpln(RED "ERROR: buffer is too small" CLRST);
    return -1;
  }

  if (!AddTextToBuffer(&len, buffer, MAX_SIZE, JSON_BODY_END)) {
    conpln(RED "ERROR: buffer is too small" CLRST);
    return -1;
  }

  return len;
}

/**
 * @brief Generate network configuration JSON.
 *
 * Returns:
 * {
 *   "ip":   "x.x.x.x",
 *   "mask": "x.x.x.x",
 *   "gw":   "x.x.x.x"
 * }
 *
 * Data source: Flash settings storage.
 */
static int make_network_endpoint(http_conn_ctx_t *c, char *buffer, const uint32_t MAX_SIZE)
{
  int len = 0;
  char tag_value[20];
  Flash_settings Settings;

  get_settings(&Settings);

  if (!AddTextToBuffer(&len, buffer, MAX_SIZE, JSON_HEAD("/api/net"))) {
    conpln(RED "ERROR: buffer is too small" CLRST);
    return -1;
  }

  if (!AddTextToBuffer(&len, buffer, MAX_SIZE, JSON_BODY_BEGG)) {
    conpln(RED "ERROR: buffer is too small" CLRST);
    return -1;
  }

  (void)snprintf(tag_value, sizeof(tag_value), IPv4_FORMAT , IPv4_PARAMS(Settings.IP));
  if (!AddTextToBuffer(&len, buffer, MAX_SIZE, "\"ip\": \"%s\",", tag_value)) {
    conpln(RED "ERROR: buffer is too small" CLRST);
    return -1;
  }

  (void)snprintf(tag_value, sizeof(tag_value), IPv4_FORMAT , IPv4_PARAMS(Settings.mask));
  if (!AddTextToBuffer(&len, buffer, MAX_SIZE, "\"mask\": \"%s\",", tag_value)) {
    conpln(RED "ERROR: buffer is too small" CLRST);
    return -1;
  }

  (void)snprintf(tag_value, sizeof(tag_value), IPv4_FORMAT , IPv4_PARAMS(Settings.gate));
  if (!AddTextToBuffer(&len, buffer, MAX_SIZE, "\"gw\": \"%s\"", tag_value)) {
    conpln(RED "ERROR: buffer is too small" CLRST);
    return -1;
  }

  if (!AddTextToBuffer(&len, buffer, MAX_SIZE, JSON_BODY_END)) {
    conpln(RED "ERROR: buffer is too small" CLRST);
    return -1;
  }

  return len;
}

/**
 * @brief Generate authentication configuration JSON.
 *
 * Returns currently configured username.
 *
 * NOTE:
 * Password is intentionally not exposed.
 */
static int make_auth_endpoint(http_conn_ctx_t *c, char *buffer, const uint32_t MAX_SIZE)
{
  int len = 0;
  Flash_settings Settings;

  get_settings(&Settings);

  if (!AddTextToBuffer(&len, buffer, MAX_SIZE, JSON_HEAD("/api/auth"))) {
    conpln(RED "ERROR: buffer is too small" CLRST);
    return -1;
  }

  if (!AddTextToBuffer(&len, buffer, MAX_SIZE, JSON_BODY_BEGG)) {
    conpln(RED "ERROR: buffer is too small" CLRST);
    return -1;
  }

  if (!AddTextToBuffer(&len, buffer, MAX_SIZE, "\"user\": \"%s\"", Settings.user_name)) {
    conpln(RED "ERROR: buffer is too small" CLRST);
    return -1;
  }

  if (!AddTextToBuffer(&len, buffer, MAX_SIZE, JSON_BODY_END)) {
    conpln(RED "ERROR: buffer is too small" CLRST);
    return -1;
  }

  return len;
}

/**
 * @brief Generate login result JSON.
 *
 * Returns:
 * {
 *   "status": true | false
 * }
 *
 * Status reflects result of previous login POST attempt.
 */
static int make_login_endpoint(http_conn_ctx_t *c, char *buffer, const uint32_t MAX_SIZE)
{
  int len = 0;
  Flash_settings Settings;

  if(c == NULL) {
    conpln(RED "ERROR: ctx not found" CLRST);
    return -1;
  }

  get_settings(&Settings);

  if (!AddTextToBuffer(&len, buffer, MAX_SIZE, JSON_HEAD("/api/login"))) {
    conpln(RED "ERROR: buffer is too small" CLRST);
    return -1;
  }

  if (!AddTextToBuffer(&len, buffer, MAX_SIZE, JSON_BODY_BEGG)) {
    conpln(RED "ERROR: buffer is too small" CLRST);
    return -1;
  }

  if (!AddTextToBuffer(&len, buffer, MAX_SIZE, "\"status\": %s", \
     (c->operation == OPERATION_COOKIE && c->status == STATUS_FAIL) ? \
      "false" : "true")) {
    conpln(RED "ERROR: buffer is too small" CLRST);
    return -1;
  }

  if (!AddTextToBuffer(&len, buffer, MAX_SIZE, JSON_BODY_END)) {
    conpln(RED "ERROR: buffer is too small" CLRST);
    return -1;
  }

  return len;
}

/**
 * @brief Process network configuration update.
 *
 * Expected JSON fields:
 *   - ip
 *   - mask
 *   - gw
 *
 * On success:
 *   - Sends OSMSG_UPDATE_IP to main task
 *   - Sets connection status to STATUS_OK
 *
 * On validation failure:
 *   - Sets STATUS_FAIL
 */
static void process_network_endpoint(http_conn_ctx_t *c, const uint8_t elements, char **keys, char **values)
{
  int id;
  Flash_settings newSettings;

  get_settings(&newSettings);

  id = http_find_key(keys, elements, "ip");
  if (id == -1)
  {
    c->status = STATUS_FAIL;
    return;
  }

  if(check_ip(values[id], &newSettings.IP) == false)
  {
    c->status = STATUS_FAIL;
    return;
  }

  id = http_find_key(keys, elements, "mask");
  if (id == -1)
  {
    c->status = STATUS_FAIL;
    return;
  }

  if(check_ip(values[id], &newSettings.mask) == false)
  {
    c->status = STATUS_FAIL;
    return;
  }

  id = http_find_key(keys, elements, "gw");
  if (id == -1)
  {
    c->status = STATUS_FAIL;
    return;
  }

  if(check_ip(values[id], &newSettings.gate) == false)
  {
    c->status = STATUS_FAIL;
    return;
  }

  TOSMessage msg;
  msg.msg = OSMSG_UPDATE_IP;
  memcpy(&msg.new_settings, &newSettings, sizeof(Flash_settings));

  if (xQueueSend(mainTaskQH, &msg, pdMS_TO_TICKS(1000)) == pdTRUE)
    c->status = STATUS_OK;

}

/**
 * @brief Process authentication configuration update.
 *
 * Expected JSON fields:
 *   - user
 *   - pass
 *
 * Performs:
 *   - length validation
 *   - settings update via message queue
 */
static void process_auth_endpoint(http_conn_ctx_t *c, const uint8_t elements, char **keys, char **values)
{
  int id, len;
  Flash_settings newSettings;

  get_settings(&newSettings);

  id = http_find_key(keys, elements, "user");
  if (id == -1)
  {
    c->status = STATUS_FAIL;
    return;
  }

  if((len = strlen(values[id])) >= MAX_USER_NAME)
  {
    c->status = STATUS_FAIL;
    return;
  }

  strncpy(newSettings.user_name, values[id], len);
  newSettings.user_name[len] = '\0';

  id = http_find_key(keys, elements, "pass");
  if (id == -1)
  {
    c->status = STATUS_FAIL;
    return;
  }

  if((len = strlen(values[id])) >= MAX_USER_PWD)
  {
    c->status = STATUS_FAIL;
    return;
  }

  strncpy(newSettings.user_pwd, values[id], len);
  newSettings.user_pwd[len] = '\0';

  TOSMessage msg;
  msg.msg = OSMSG_UPDATE_SETTINGS;
  memcpy(&msg.new_settings, &newSettings, sizeof(Flash_settings));

  if (xQueueSend(mainTaskQH, &msg, pdMS_TO_TICKS(1000)) == pdTRUE)
    c->status = STATUS_OK;

}

/**
 * @brief Process login request.
 *
 * Expected JSON fields:
 *   - user
 *   - pwd
 *
 * Performs:
 *   - credential validation
 *   - session creation on success
 *   - context authorization update
 *
 * On success:
 *   - Creates session
 *   - Sets AUTH_LOGGED
 *   - Stores SID in connection context
 *
 * On failure:
 *   - Sets STATUS_FAIL
 */
static void process_login_endpoint(http_conn_ctx_t *c, const uint8_t elements, char **keys, char **values)
{
  int id, len;
  Flash_settings Settings;
  session_t *session;
  bool login_ok, pwd_ok;

  c->operation = OPERATION_COOKIE;

  get_settings(&Settings);

  id = http_find_key(keys, elements, "user");
  if (id == -1)
  {
    c->status = STATUS_FAIL;
    return;
  }

  len = MAX2(strlen(Settings.user_name),strlen(values[id]));
  login_ok =  \
    memcmp(values[id], Settings.user_name, len) == 0 ?
    true : false;

  id = http_find_key(keys, elements, "pwd");
  if (id == -1)
  {
    c->status = STATUS_FAIL;
    return;
  }

  len = MAX2(strlen(Settings.user_pwd),strlen(values[id]));
  pwd_ok =  \
    memcmp(values[id], Settings.user_pwd, len) == 0 ?
    true : false;

  if(login_ok && pwd_ok)
  {
    conpln("httpd_post_finished: login and password correct");
    session = session_create();
    if (session) {
      conpln("httpd_post_finished: cookie created");
      c->authorized = AUTH_LOGGED;
      c->status = STATUS_OK;
      strcpy(c->sid, session->sid);
    } else
      conpln("httpd_post_finished: " RED "failed to create session" CLRST);
  }
  else
  {
    c->status = STATUS_FAIL;
  }
}
