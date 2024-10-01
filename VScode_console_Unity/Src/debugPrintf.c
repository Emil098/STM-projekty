#include "debugUart.h"
#include "debugPrintf.h"
#include <stdbool.h>
#include <string.h>

#define NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS  1 //Set to 0 or 1. Enables field width specifiers.
#define NANOPRINTF_USE_LARGE_FORMAT_SPECIFIERS        1 //Set to 0 or 1. Enables oversized modifiers.
#define NANOPRINTF_USE_BINARY_FORMAT_SPECIFIERS       0 //Set to 0 or 1. Enables binary specifiers.
#define NANOPRINTF_USE_WRITEBACK_FORMAT_SPECIFIERS    0 //Set to 0 or 1. Enables %n for write-back.
#define NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS    1 //Set to 0 or 1. Enables precision specifiers.
#define NANOPRINTF_USE_FLOAT_FORMAT_SPECIFIERS        1 //Set to 0 or 1. Enables floating-point specifiers.
#define NANOPRINTF_SNPRINTF_SAFE_TRIM_STRING_ON_OVERFLOW

#define NANOPRINTF_IMPLEMENTATION
#include "nanoprintf.h"


/**
 * @brief Function for printing single character
 * @param d character
 * @retval true : character has been passed to the tx buffer and will be sent  
 * @retval false: tx buffer is full can't add another byte 
 */
bool dbg_putc(uint8_t d)
{
  return dbg_putc_nonblock(d);
}

/**
 * @brief Function for nanoprintf to print single character
 * @param c character to be printed out
 * @param ctx void pointer required by npf_vpprintf (not used here)
 * @retval None
 */
static void npf_dbg_putc(int c, void *ctx)
{
  UNUSED(ctx);
  dbg_putc_nonblock((uint8_t)c);
}

/**
  * @brief debug printf function for printing output string on UART console and adding new line sign at the end
  * @param text is a string that specifies the data to be printed. It may also contain a format specifier to print the value of any variable such as a character and an integer.
  * @param arguments_list: These are the variable names corresponding to the format specifier.
  * @retval None
  */
void dbg_printf_ln(const char *text, ...)
{
  va_list args;

  va_start(args, text);
  npf_vpprintf(&npf_dbg_putc, NULL, text, args);
  va_end(args);

  dbg_putc('\n');
}

/**
  * @brief debug printf function for printing output string on UART console
  * @param text is a string that specifies the data to be printed. It may also contain a format specifier to print the value of any variable such as a character and an integer.
  * @param arguments_list: These are the variable names corresponding to the format specifier.
  * @retval None
  */
void dbg_printf(const char *text, ...)
{
  va_list args;

  va_start(args, text);
  npf_vpprintf(&npf_dbg_putc, NULL, text, args);
  va_end(args);
}
