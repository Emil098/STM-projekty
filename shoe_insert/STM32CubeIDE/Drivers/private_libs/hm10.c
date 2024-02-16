/*
 * hm10.c
 *
 *  Created on: 23 paź 2023
 *      Author: emilr
 */

#include "hm10.h"

#include "main.h"
#include "cmsis_os.h"

#define HM10_CMD_HUART          huart1
#define HM10_CMD_DEF_BAUD       9600 //Default baud rate
#define HM10_CMD_TX_BUF_SIZE      128
#define HM10_CMD_RX_BUF_SIZE      128

extern UART_HandleTypeDef  HM10_CMD_HUART;
static USART_TypeDef      *CmdUart;

#define CMD_TX_BUF_MASK (HM10_CMD_TX_BUF_SIZE - 1)
#define CMD_RX_BUF_MASK (HM10_CMD_RX_BUF_SIZE - 1)

#if (HM10_CMD_TX_BUF_SIZE & CMD_TX_BUF_MASK)
#error M66 CMD TX buffer size is not a power of 2
#endif
#if (HM10_CMD_RX_BUF_SIZE & CMD_RX_BUF_MASK)
#error M66 CMD RX buffer size is not a power of 2
#endif

// circle buffers for rx and tx data
static volatile uint8_t  CmdTxBuf[HM10_CMD_TX_BUF_SIZE];
static volatile uint8_t  CmdRxBuf[HM10_CMD_RX_BUF_SIZE];
// pointers to beginnings and ends of those buffers
static volatile uint32_t CmdTxHead;
static volatile uint32_t CmdTxTail;
static volatile uint32_t CmdRxHead;
static volatile uint32_t CmdRxTail;

void HM10_io_init(void)
{
  CmdTxHead = 0;
  CmdTxTail = 0;
  CmdRxHead = 0;
  CmdRxTail = 0;

  __HAL_UART_DISABLE_IT(&HM10_CMD_HUART, UART_IT_PE);
  __HAL_UART_DISABLE_IT(&HM10_CMD_HUART, UART_IT_TC);
  __HAL_UART_DISABLE_IT(&HM10_CMD_HUART, UART_IT_IDLE);
  __HAL_UART_DISABLE_IT(&HM10_CMD_HUART, UART_IT_CTS);
  __HAL_UART_ENABLE_IT( &HM10_CMD_HUART, UART_IT_RXNE);

  CmdUart = HM10_CMD_HUART.Instance; //must be at the end
}

void HM10_io_deinit(void)
{
  // disabling all uart interrupts
  __HAL_UART_DISABLE_IT(&HM10_CMD_HUART, UART_IT_PE);
  __HAL_UART_DISABLE_IT(&HM10_CMD_HUART, UART_IT_TC);
  __HAL_UART_DISABLE_IT(&HM10_CMD_HUART, UART_IT_IDLE);
  __HAL_UART_DISABLE_IT(&HM10_CMD_HUART, UART_IT_CTS);
  __HAL_UART_DISABLE_IT(&HM10_CMD_HUART, UART_IT_LBD);
  __HAL_UART_DISABLE_IT(&HM10_CMD_HUART, UART_IT_ERR);
  __HAL_UART_DISABLE_IT(&HM10_CMD_HUART, UART_IT_RXNE);
  __HAL_UART_DISABLE_IT(&HM10_CMD_HUART, UART_IT_TXE);
}

void HM10_io_cmd_irq_handler(void)
{
  if (!CmdUart) {
    HM10_io_init();
    return;
  }

  uint32_t sr = CmdUart->SR;

#ifdef CLEAR_ORE_FE_IN_UART_IRQ
  __HAL_UART_CLEAR_OREFLAG(&HM10_CMD_HUART);
  __HAL_UART_CLEAR_FEFLAG(&HM10_CMD_HUART);
#endif

  //RXNE
  if (sr & (USART_SR_RXNE | USART_SR_ORE)) {  //FE flag is never alone without RXNE or ORE, so we don't have to check for it
    uint8_t dr = CmdUart->DR;
    if ((sr & USART_SR_FE) == 0) {
      uint32_t tmphead = (CmdRxHead + 1) & CMD_RX_BUF_MASK;
      if (tmphead == CmdRxTail) {
        HM10_io_deinit();
        HM10_io_init();
        //overflow
      } else {
        CmdRxHead = tmphead;
        CmdRxBuf[tmphead] = dr;
      }
    }
  }

  //TXE
  if (sr & USART_SR_TXE) {
    if (CmdTxHead != CmdTxTail) {
      uint32_t tmptail = (CmdTxTail + 1) & CMD_TX_BUF_MASK;
      CmdTxTail = tmptail;
      CmdUart->DR = CmdTxBuf[tmptail];
    } else
      __HAL_UART_DISABLE_IT(&HM10_CMD_HUART, UART_IT_TXE);
  }
}

bool HM10_io_cmd_getc(uint8_t *d)
{

  if (CmdRxHead == CmdRxTail)
    return false;

  // increasing tail pointer after reaching max value tmptail will equal 0
  uint32_t tmptail = (CmdRxTail + 1) & CMD_RX_BUF_MASK;
    CmdRxTail = tmptail;

  // passing data from tail pt to argument pt
  *d = CmdRxBuf[tmptail];

  return true;
}

bool HM10_io_cmd_putc(uint8_t d, uint32_t timeout)
{
  uint32_t tmphead = (CmdTxHead + 1) & CMD_TX_BUF_MASK;

  uint32_t start = HAL_GetTick();
  // wait for completing last byte transfer (head + 1 = tail)
  while ((tmphead == CmdTxTail) && (HAL_GetTick() - start < timeout)) { }

  if (tmphead != CmdTxTail) {
    // passing byte to buffer and updating head pt
    CmdTxBuf[tmphead] = d;
    CmdTxHead = tmphead;
  } else
    return false;

  // enabling tx interrupt
  __HAL_UART_ENABLE_IT(&HM10_CMD_HUART, UART_IT_TXE);
  return true;
}

void HM10_io_cmd_clear_rx_buf(void)
{
  CmdRxTail = CmdRxHead;
}

