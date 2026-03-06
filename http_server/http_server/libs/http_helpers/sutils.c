/*
 * sutils.c
 *
 *  Created on: Feb 20, 2026
 *      Author: emilr
 */

#include "sutils.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>

#include "lwip.h"
#include "ip_format.h"


/**
 * @brief Find the index of a key in an array of strings.
 *
 * Performs a linear search in the provided array of key names.
 *
 * @param[in] names  Array of string pointers (keys).
 * @param[in] count  Number of elements in the array.
 * @param[in] text   Key to search for.
 *
 * @return Index of the matching key (0..count-1),
 *         or -1 if the key was not found.
 *
 * @note
 * - Comparison is case-sensitive.
 * - NULL entries in the array are ignored.
 */
int http_find_key(char **names, int count, char *text)
{
  int a;
  for (a = 0; a < count; a++) {
    if ((names[a]) && (strcmp(names[a], text) == 0)) {
      return a;
    }
  }
  return -1;
}

/**
 * @brief Validate and convert an IPv4 address string.
 *
 * Parses a dotted-decimal IPv4 string (e.g. "192.168.1.10")
 * and converts it into a 32-bit integer representation.
 *
 * @param[in]  value  Null-terminated IPv4 string.
 * @param[out] out    Pointer to a 32-bit variable receiving the result.
 *
 * @return true  if parsing and validation succeeded.
 * @return false if the string is not a valid IPv4 address.
 *
 * @note
 * - Each octet must be in range 0–255.
 * - The resulting value is packed using IP4TOU32 macro.
 * - The function does not check for NULL output pointer.
 */
bool check_ip(const char *value, uint32_t *out)
{
  int a, r[4];

  if (sscanf(value, "%u.%u.%u.%u", &r[0], &r[1], &r[2], &r[3]) == 4) {
    for (a = 0; a < 4; a++)
      if ((r[a] < 0) || (r[a] > 255))
        return false;
    *out = IPv4TOU32(r[0], r[1], r[2], r[3]);
    return true;
  }
  return false;
}


/**
 * @brief Skip ASCII whitespace characters.
 */
static char* skip_ws(char *p)
{
    while (p && *p && isspace((unsigned char)*p)) {
        p++;
    }
    return p;
}

/**
 * @brief Extract key/value pairs from a flat JSON object.
 *
 * This is a lightweight in-place parser intended for embedded HTTP POST bodies.
 * It supports a *flat* JSON object with string keys:
 *   {"key":"value","n":123,"flag":true,"x":null}
 *
 * Key pointers and value pointers will reference the original @p content buffer.
 * The function inserts '\\0' terminators into @p content to split tokens.
 *
 * Limitations:
 * - No nested objects/arrays.
 * - No escaped quotes handling inside strings (e.g. \" is not supported).
 * - Keys must be quoted (standard JSON).
 *
 * @param[in,out] content  Mutable JSON string (will be modified).
 * @param[out]    keys     Output array for pointers to keys.
 * @param[out]    values   Output array for pointers to values (start of token, without quotes).
 * @param[in]     max_pairs Maximum number of pairs to extract.
 *
 * @return Number of extracted pairs (0 on invalid/empty input).
 */
int post_extract_json(char *content, char **keys, char **values)
{
    if (!content || !keys || !values) {
        return 0;
    }

    char *p = skip_ws(content);
    if (*p == '\0') {
        return 0;
    }

    // Expect object start
    if (*p != '{') {
        return 0;
    }
    p++; // after '{'

    int count = 0;

    while (count < MAX_POST_FIELDS) {
        p = skip_ws(p);

        // End of object
        if (*p == '}') {
            return count;
        }

        // Expect key opening quote
        if (*p != '"') {
            return count; // stop on unexpected format
        }
        p++; // key start

        keys[count] = p;

        // Find key closing quote
        char *key_end = strchr(p, '"');
        if (!key_end) {
            return count;
        }
        *key_end = '\0';
        p = key_end + 1;

        p = skip_ws(p);

        // Expect ':'
        if (*p != ':') {
            return count;
        }
        p++; // after ':'
        p = skip_ws(p);

        // Parse value (string or non-string token)
        if (*p == '"') {
            // String value
            p++; // value start
            values[count] = p;

            char *val_end = strchr(p, '"');
            if (!val_end) {
                return count;
            }
            *val_end = '\0';
            p = val_end + 1;
        } else {
            // Non-string value: number / true / false / null
            values[count] = p;

            // Value ends at ',' or '}'
            while (*p && *p != ',' && *p != '}') {
                p++;
            }

            // Trim trailing whitespace inside the token
            char *end = p;
            while (end > values[count] && isspace((unsigned char)end[-1])) {
                end--;
            }
            *end = '\0';
        }

        count++;

        p = skip_ws(p);

        if (*p == ',') {
            p++; // next pair
            continue;
        }

        if (*p == '}') {
            return count;
        }

        // Unexpected character -> stop
        return count;
    }

    return count;
}

/**
 * @brief Append formatted text to a buffer with bounds checking.
 *
 * Works similarly to printf(), but appends formatted output
 * to the provided buffer while tracking its current length.
 *
 * The function uses vsnprintf() to prevent stack overflow
 * and ensures that the final buffer size does not exceed max_size.
 *
 * @param[in,out] len       Pointer to current length of valid data in buffer.
 *                          Updated on success.
 * @param[in,out] buffer    Destination buffer.
 * @param[in]     max_size  Total capacity of the destination buffer.
 * @param[in]     text      printf-style format string.
 * @param[in]     ...       Additional arguments for formatting.
 *
 * @return true  if text was successfully appended.
 * @return false if there was not enough space in the buffer.
 *
 * @note
 * - The function does not append a terminating '\\0' explicitly.
 *   It assumes the buffer is managed externally.
 * - Intended for dynamic HTTP/JSON response generation
 */
bool AddTextToBuffer(int *len, char *buffer, const int max_size, const char *text, ...)
{
  va_list args;
  char help_buffer[max_size] = {};
  int to_be_written;


  va_start(args, text);
  vsnprintf(help_buffer, max_size, text, args);
  va_end(args);

  to_be_written = strlen(help_buffer);
  if((to_be_written + *len) >= max_size)
    return false;

  memcpy(buffer + *len, help_buffer, to_be_written);
  *len += to_be_written;

  return true;
}

