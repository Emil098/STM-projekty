#ifndef INC_DEBUGPRINTF_H_
#define INC_DEBUGPRINTF_H_

#include "stdint.h"
#include "debugUart.h"

void dbg_printf_ln(const char *text, ...);
void dbg_printf(const char *text, ...);
bool dbg_putc(uint8_t d);

#endif