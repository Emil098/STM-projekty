#ifndef INC_DEBUGUART_H_
#define INC_DEBUGUART_H_
#include "stdint.h"
#include <stdbool.h>
#include "types.h"

#define UNUSED(X) (void)X      /* To avoid gcc/g++ warnings */

#define DEBUG_TX_SIZE  256
#define DEBUG_RX_SIZE   64

#define DEBUG_TX_MASK (DEBUG_TX_SIZE - 1)
#define DEBUG_RX_MASK (DEBUG_RX_SIZE - 1)


bool dbg_putc_nonblock(u8 d);
bool dbg_getc(u8 *d);
int dbg_getstr(u8 *str, int len);
void debug_UART_irq(void);
void debug_init(void);

#endif 