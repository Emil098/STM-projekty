#include "debugUart.h"
#include <stdarg.h>
#include <stdbool.h>
#include "types.h"

#ifdef USE_HAL_DRIVER
#include "main.h"
#define VISIBILITY extern 
#else
#include "fakeUart.h"
#define VISIBILITY
#endif

static volatile u8 TxBuf[DEBUG_TX_SIZE];
static volatile u8 RxBuf[DEBUG_RX_SIZE];
static volatile u32 TxHead;
static volatile u32 TxTail;
static volatile u32 RxHead;
static volatile u32 RxTail;


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
  
  __HAL_UART_ENABLE_IT(&debug_huart, UART_IT_RXNE);
  __HAL_UART_DISABLE_IT(&debug_huart, UART_IT_PE);
  __HAL_UART_DISABLE_IT(&debug_huart, UART_IT_TC);
  __HAL_UART_DISABLE_IT(&debug_huart, UART_IT_IDLE);
  __HAL_UART_DISABLE_IT(&debug_huart, UART_IT_CTS);
  //__HAL_UART_DISABLE_IT(&huart, UART_IT_CM);
}

/**
  * @brief  This function is uart interrupt handler and should be called by USARTx_IRQHandler()
  * @retval None
  */
void debug_UART_irq(void)
{
  u32 sr = uart->ISR;
  
  __HAL_UART_CLEAR_OREFLAG(&debug_huart);
  __HAL_UART_CLEAR_FEFLAG(&debug_huart);
  
  //RXNE
  if (sr & (USART_ISR_RXNE | USART_ISR_ORE)) {
    u8 dr = (u8)(uart->RDR);
    if ((sr & USART_ISR_FE) == 0) {
      u32 tmphead = (RxHead + 1) & DEBUG_RX_MASK;
      if (tmphead == RxTail) {
        //overflow
      } else {
        RxHead = tmphead;
        RxBuf[tmphead] = dr;
      }
    }
  }
    
  //TXE
  if (sr & USART_ISR_TXE) {
    if (TxHead != TxTail) {
      u32 tmptail = (TxTail + 1) & DEBUG_TX_MASK;
      TxTail = tmptail;
      uart->TDR = TxBuf[tmptail];
    } else
      __HAL_UART_DISABLE_IT(&debug_huart, UART_IT_TXE); // TxBuf empty
  }  
}

/**
  * @brief  This function reads last unreaded element from RX buffer
	* @param	d : Pointer on buffers last unreaded element
  * @retval true : Pointer has been passed 
	* @retval false : There is no unreaded elements to be passed
  */
bool dbg_getc(u8 *d)
{
  if (RxHead == RxTail)
    return false; 

  u32 tmptail = (RxTail + 1) & DEBUG_RX_MASK;
  RxTail = tmptail;

  *d = RxBuf[tmptail];

  return true;
}

/**
  * @brief   This function reads a string of elements from RX buffer
	* @param	 str : Pointer on string to fill with read data
  * @param	 len : Expected string length
  * @retval  Number of elements passed to str  
  */
int dbg_getstr(u8 *str, int len)
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
  * @brief   Non blocking function for transmitting single byte through interrupt
	* @param	 d : Byte to be sent
  * @retval  true : byte has been passed to the tx buffer and will be sent  
  * @retval  false: tx buffer is full can't add another byte  
  */
bool dbg_putc_nonblock(u8 d)
{
    u32 tmphead = (TxHead + 1) & DEBUG_TX_MASK;

    if (tmphead == TxTail)
      return false;

    TxBuf[tmphead] = d;
    TxHead = tmphead;

    __HAL_UART_ENABLE_IT(&debug_huart, UART_IT_TXE);
    return true;
}