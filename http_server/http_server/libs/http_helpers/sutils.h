/*
 * sutils.h
 * @brief Lightweight HTTP and JSON helper utilities for embedded systems.
 *
 * This module provides small, dependency-free helper functions
 * designed for embedded HTTP servers (e.g., lwIP httpd).
 *
 * Features:
 *  - Flat JSON key/value extraction (in-place parsing)
 *  - HTTP parameter lookup helpers
 *  - IPv4 string validation and conversion
 *  - Safe formatted buffer appending for dynamic response generation
 *
 * Design goals:
 *  - No dynamic memory allocation
 *  - Minimal RAM footprint
 *  - In-place parsing to reduce copying
 *  - Suitable for FreeRTOS + lwIP environments
 *
 * Limitations:
 *  - JSON parser supports only flat objects (no nesting/arrays)
 *  - No escaped string handling
 *  - Designed for trusted/local web interfaces
 *
 * Typical usage:
 *  - Parsing REST API POST bodies
 *  - Generating JSON responses
 *  - Building dynamic HTML fragments
 *
 *  Created on: Feb 20, 2026
 *      Author: emilr
 */

#ifndef HTTP_HANDLERS_SUTILS_H_
#define HTTP_HANDLERS_SUTILS_H_

#include <stdbool.h>
#include "main.h"

#define MAX_POST_FIELDS  16

int http_find_key(char **names, int count, char *text);

bool check_ip(const char *value, uint32_t *out);

int post_extract_json(char *content, char **keys, char **values);

bool AddTextToBuffer(int *len, char *buffer, const int max_size, const char *text, ...);

#endif /* HTTP_HANDLERS_SUTILS_H_ */
