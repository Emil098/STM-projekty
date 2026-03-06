/*
 * console.c
 * @brief Lightweight console/printf utilities built on top of debug UART.
 *
 * This module provides simple, dependency-light printing helpers for embedded
 * targets. Formatted output is produced using NanoPrintf (nanoprintf) and is
 * written via the non-blocking debug UART driver.
 *
 * Design goals:
 *  - Minimal footprint compared to full printf
 *  - Non-blocking transmit path (interrupt-driven UART)
 *  - Testability: UART I/O can be mocked (Unity/CMock) on host builds
 *
 * Notes:
 *  - dbg_getstr() reads raw bytes and does NOT append a terminating '\0'.
 *  - ANSI color macros are intended for terminals that support escape sequences.
 *
 * @note This module is unit-test friendly when dbg_getc()/dbg_putc_nonblock()
 *       are mocked.
 *
 *  Created on: Jan 11, 2026
 *      Author: emilr
 */


#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include "console.h"
#include "debug_uart.h"
#include "usart.h"


#define NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_LARGE_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_FLOAT_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_BINARY_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_WRITEBACK_FORMAT_SPECIFIERS 0

#define NANOPRINTF_IMPLEMENTATION
#include "nanoprintf.h"


/**
  * @brief   This function reads a string of elements from RX buffer
  * @param   str : Pointer on string to fill with read data
  * @param   len : Expected string length
  * @retval  Number of elements passed to str
  */
int con_getstr(uint8_t *str, int len)
{
  int count = 0;
  while (len && dbg_getc(str)) {
    str++;
    len--;
    count++;
  }
  return count;
}

/**
 * @brief NanoPrintf character output callback.
 *
 * Adapter function used by nanoprintf to output one character through
 * con_putc_nonblock().
 *
 * @param[in] c   Character to output (passed as int per printf conventions).
 * @param[in] ctx Unused context pointer (reserved for future use).
 */
void con_npf_putc(int c, void *ctx)
{
  (void)(ctx);
  dbg_putc_nonblock(c);
}

/**
 * @brief Print formatted text to debug console (no newline).
 *
 * @param[in] text Format string (printf-like).
 * @param[in] ...  Format arguments.
 */
void conp(const char *text, ...)
{
  va_list args;

  va_start(args, text);
  npf_vpprintf(&con_npf_putc, NULL, text, args);
  va_end(args);
}

/**
 * @brief Print formatted text to debug console and append newline.
 *
 * @param[in] text Format string (printf-like).
 * @param[in] ...  Format arguments.
 */
void conpln(const char *text, ...)
{
  va_list args;

  va_start(args, text);
  npf_vpprintf(&con_npf_putc, NULL, text, args);
  dbg_putc_nonblock('\n');
  va_end(args);
}
