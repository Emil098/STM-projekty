/*
 * debug_uart.c
 *
 *  Created on: Jan 11, 2026
 *      Author: emilr
 */
/**
 * @file debug_uart.c
 * @brief Interrupt-driven debug UART implementation.
 *
 * This file implements a minimal UART driver with:
 *  - TX ring buffer serviced by TXE interrupt
 *  - RX ring buffer filled on RXNE interrupt
 *
 * Concurrency model:
 *  - TX/RX head/tail indices are modified both in ISR and non-ISR context.
 *    They are declared volatile. On Cortex-M, 32-bit reads/writes are atomic,
 *    but if you extend this design, consider critical sections if needed.
 *
 * Integration:
 *  - When USE_HAL_DRIVER is defined, the driver uses STM32 HAL types/macros.
 *  - Otherwise it can use a fake backend (fakeUart.h) for host builds.
 */

#include "debug_uart.h"

#ifdef USE_HAL_DRIVER
#include "main.h"
#define VISIBILITY extern
#define debug_huart huart2
#else
#include "fakeUart.h"
#define VISIBILITY
#endif


static volatile uint8_t TxBuf[DEBUG_TX_SIZE];
static volatile uint8_t RxBuf[DEBUG_RX_SIZE];
static volatile uint32_t TxHead;
static volatile uint32_t TxTail;
static volatile uint32_t RxHead;
static volatile uint32_t RxTail;

VISIBILITY UART_HandleTypeDef debug_huart;
static USART_TypeDef *uart;


/**
  * @brief  This function initialize UART interrupt flags
  * @retval None
  */
void debug_init(void)
{
  uart = debug_huart.Instance;

  TxHead = 0;
  TxTail = 0;
  RxHead = 0;
  RxTail = 0;

  /* Enable RX interrupt; disable unused interrupts for this driver */
  __HAL_UART_ENABLE_IT(&debug_huart, UART_IT_RXNE);
  __HAL_UART_DISABLE_IT(&debug_huart, UART_IT_PE);
  __HAL_UART_DISABLE_IT(&debug_huart, UART_IT_TC);
  __HAL_UART_DISABLE_IT(&debug_huart, UART_IT_IDLE);
  __HAL_UART_DISABLE_IT(&debug_huart, UART_IT_CTS);
}

/**
  * @brief  This function is uart interrupt handler and should be called by USARTx_IRQHandler()
  * @retval None
  */
void debug_UART_irq(void)
{
  uint32_t sr = uart->ISR;

  /* Clear common error flags early */
  __HAL_UART_CLEAR_OREFLAG(&debug_huart);
  __HAL_UART_CLEAR_FEFLAG(&debug_huart);

  /* RX: data received */
  if (sr & (USART_ISR_RXNE | USART_ISR_ORE))
  {
    uint8_t dr = (uint8_t)(uart->RDR);

    /* Ignore framing errors */
    if ((sr & USART_ISR_FE) == 0)
    {
      uint32_t tmphead = (RxHead + 1) & DEBUG_RX_MASK;
      if (tmphead == RxTail)
      {
        //overflow
      }
      else
      {
        RxHead = tmphead;
        RxBuf[tmphead] = dr;
      }
    }
  }

  /* TX: transmit buffer empty */
  if (sr & USART_ISR_TXE)
  {
    if (TxHead != TxTail)
    {
      uint32_t tmptail = (TxTail + 1) & DEBUG_TX_MASK;
      TxTail = tmptail;
      uart->TDR = TxBuf[tmptail];
    }
    else
    {
      /* TX buffer empty -> disable TXE interrupt until new data is queued */
      __HAL_UART_DISABLE_IT(&debug_huart, UART_IT_TXE);
    }
  }
}

/**
  * @brief  This function reads last unreaded element from RX buffer
  * @param  d : Pointer on buffers last unreaded element
  * @retval true : Pointer has been passed
  * @retval false : There is no unreaded elements to be passed
  */
bool dbg_getc(uint8_t *d)
{
  if (RxHead == RxTail)
    return false;

  uint32_t tmptail = (RxTail + 1) & DEBUG_RX_MASK;
  RxTail = tmptail;

  *d = RxBuf[tmptail];

  return true;
}

/**
  * @brief   Non blocking function for transmitting single byte through interrupt
  * @param   d : Byte to be sent
  * @retval  true : byte has been passed to the tx buffer and will be sent
  * @retval  false: tx buffer is full can't add another byte
  */
bool dbg_putc_nonblock(uint8_t d)
{
  uint32_t tmphead = (TxHead + 1) & DEBUG_TX_MASK;

    if (tmphead == TxTail)
      return false;

    TxBuf[tmphead] = d;
    TxHead = tmphead;

    __HAL_UART_ENABLE_IT(&debug_huart, UART_IT_TXE);
    return true;
}
