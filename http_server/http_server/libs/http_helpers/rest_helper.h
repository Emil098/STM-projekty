/*
 * rest_helper.h
 *
 *  Created on: Feb 24, 2026
 *      Author: emilr
 */

#ifndef HTTP_HELPERS_REST_HELPER_H_
#define HTTP_HELPERS_REST_HELPER_H_

#include "main.h"
#include "session.h"

int check_endpoint_name(const char *name);
int create_endpoint(http_conn_ctx_t *c, int endpoint_id, char *buffer, const uint32_t MAX_SIZE);
void process_endpoint(http_conn_ctx_t *c, int endpoint_id, const uint8_t elements, char **keys, char **values);


#endif /* HTTP_HELPERS_REST_HELPER_H_ */
