/*
 * console.h
 *
 *  Created on: Jan 11, 2026
 *      Author: emilr
 */

#ifndef LIB_console_H_
#define LIB_console_H_

#include <stdbool.h>
#include "main.h"

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

int   con_getstr(uint8_t *str, int len);
void  con_npf_putc(int c, void *ctx);

void  conp(const char *text, ...);
void  conpln(const char *text, ...);

#define conpln_fn(fmt,...) conpln("%s:%d: " fmt, __func__, __LINE__, ##__VA_ARGS__)


#endif /* LIB_console_H_ */
