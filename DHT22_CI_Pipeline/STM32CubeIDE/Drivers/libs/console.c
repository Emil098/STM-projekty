
/**
 * @file console.c
 * @brief Console printing helpers using NanoPrintf and debug UART.
 *
 * This implementation builds formatted strings using NanoPrintf (nanoprintf)
 * and emits output via dbg_putc_nonblock(), which is expected to be an
 * interrupt-driven, non-blocking UART transmit function.
 *
 * Unit testing:
 *  - dbg_getstr() can be tested by mocking dbg_getc() (Unity/CMock).
 *  - dbgp()/dbgpln() can be tested by mocking dbg_putc_nonblock() and verifying
 *    the emitted character stream.
 */

#include "console.h"
#include "debug_uart.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>


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
int dbg_getstr(uint8_t *str, int len)
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
 * dbg_putc_nonblock().
 *
 * @param[in] c   Character to output (passed as int per printf conventions).
 * @param[in] ctx Unused context pointer (reserved for future use).
 */
void dbg_npf_putc(int c, void *ctx)
{
  dbg_putc_nonblock(c);
}

/**
 * @brief Print formatted text to debug console (no newline).
 *
 * @param[in] text Format string (printf-like).
 * @param[in] ...  Format arguments.
 */
void dbgp(const char *text, ...)
{
  va_list args;

  va_start(args, text);
  npf_vpprintf(&dbg_npf_putc, NULL, text, args);
  va_end(args);
}

/**
 * @brief Print formatted text to debug console and append newline.
 *
 * @param[in] text Format string (printf-like).
 * @param[in] ...  Format arguments.
 */
void dbgpln(const char *text, ...)
{
  va_list args;

  va_start(args, text);
  npf_vpprintf(&dbg_npf_putc, NULL, text, args);
  dbg_putc_nonblock('\n');
  va_end(args);
}
