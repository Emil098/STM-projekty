/*
 * debug.h
 *
 *  Created on: 11 lut 2016
 *      Author: admin
 */
/**
 * @file console.h
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
 */

#ifndef LIB_DEBUG_H_
#define LIB_DEBUG_H_

#include <stdint.h>

#define RED         "\033[31m"
#define GREEN       "\033[32m"
#define YELLOW      "\033[33m"
#define BLUE        "\033[34m"
#define MAGENTA     "\033[35m"
#define CYAN        "\033[36m"
#define WHITE       "\033[37m"
#define CLRST       "\033[0m"

#define CLEAR_SCREEN "\033[2J"
#define CURSOR_UP    "\033[H"

int dbg_getstr(uint8_t *str, int len);
void dbg_npf_putc(int c, void *ctx);

void dbgp(const char *text, ...);
void dbgpln(const char *text, ...);


#endif /* LIB_DEBUG_H_ */
