#ifndef INC_FAKEUART_H_
#define INC_FAKEUART_H_

#include <stdint.h>

typedef struct
{
  volatile uint32_t CR1;    /*!< USART Control register 1,                 Address offset: 0x00 */ 
  volatile uint32_t CR2;    /*!< USART Control register 2,                 Address offset: 0x04 */ 
  volatile uint32_t CR3;    /*!< USART Control register 3,                 Address offset: 0x08 */
  volatile uint32_t BRR;    /*!< USART Baud rate register,                 Address offset: 0x0C */  
  volatile uint32_t GTPR;   /*!< USART Guard time and prescaler register,  Address offset: 0x10 */
  volatile uint32_t RTOR;   /*!< USART Receiver Time Out register,         Address offset: 0x14 */  
  volatile uint32_t RQR;    /*!< USART Request register,                   Address offset: 0x18 */
  volatile uint32_t ISR;    /*!< USART Interrupt and status register,      Address offset: 0x1C */
  volatile uint32_t ICR;    /*!< USART Interrupt flag Clear register,      Address offset: 0x20 */
  volatile uint32_t RDR;    /*!< USART Receive Data register,              Address offset: 0x24 */
  volatile uint32_t TDR;    /*!< USART Transmit Data register,             Address offset: 0x28 */
} USART_TypeDef;

typedef struct __UART_HandleTypeDef
{
  USART_TypeDef            *Instance;                /*!< UART registers base address        */

  const uint8_t            *pTxBuffPtr;              /*!< Pointer to UART Tx transfer Buffer */

  uint16_t                 TxXferSize;               /*!< UART Tx Transfer size              */

  volatile uint16_t            TxXferCount;              /*!< UART Tx Transfer Counter           */

  uint8_t                  *pRxBuffPtr;              /*!< Pointer to UART Rx transfer Buffer */

  uint16_t                 RxXferSize;               /*!< UART Rx Transfer size              */

  volatile uint16_t            RxXferCount;              /*!< UART Rx Transfer Counter           */

  uint16_t                 Mask;                     /*!< UART Rx RDR register mask          */

  volatile uint32_t                 ErrorCode;           /*!< UART Error code                    */


} UART_HandleTypeDef;

#define USART_ISR_PE_Pos              (0U)    
#define USART_ISR_PE_Msk              (0x1UL << USART_ISR_PE_Pos)               /*!< 0x00000001 */
#define USART_ISR_PE                  USART_ISR_PE_Msk                         /*!< Parity Error */
#define USART_ISR_FE_Pos              (1U)    
#define USART_ISR_FE_Msk              (0x1UL << USART_ISR_FE_Pos)               /*!< 0x00000002 */
#define USART_ISR_FE                  USART_ISR_FE_Msk                         /*!< Framing Error */
#define USART_ISR_NE_Pos              (2U)    
#define USART_ISR_NE_Msk              (0x1UL << USART_ISR_NE_Pos)               /*!< 0x00000004 */
#define USART_ISR_NE                  USART_ISR_NE_Msk                         /*!< Noise detected Flag */
#define USART_ISR_ORE_Pos             (3U)    
#define USART_ISR_ORE_Msk             (0x1UL << USART_ISR_ORE_Pos)              /*!< 0x00000008 */
#define USART_ISR_ORE                 USART_ISR_ORE_Msk                        /*!< OverRun Error */
#define USART_ISR_IDLE_Pos            (4U)    
#define USART_ISR_IDLE_Msk            (0x1UL << USART_ISR_IDLE_Pos)             /*!< 0x00000010 */
#define USART_ISR_IDLE                USART_ISR_IDLE_Msk                       /*!< IDLE line detected */
#define USART_ISR_RXNE_Pos            (5U)    
#define USART_ISR_RXNE_Msk            (0x1UL << USART_ISR_RXNE_Pos)             /*!< 0x00000020 */
#define USART_ISR_RXNE                USART_ISR_RXNE_Msk                       /*!< Read Data Register Not Empty */
#define USART_ISR_TC_Pos              (6U)    
#define USART_ISR_TC_Msk              (0x1UL << USART_ISR_TC_Pos)               /*!< 0x00000040 */
#define USART_ISR_TC                  USART_ISR_TC_Msk                         /*!< Transmission Complete */
#define USART_ISR_TXE_Pos             (7U)    
#define USART_ISR_TXE_Msk             (0x1UL << USART_ISR_TXE_Pos)              /*!< 0x00000080 */
#define USART_ISR_TXE                 USART_ISR_TXE_Msk                        /*!< Transmit Data Register Empty */
#define USART_ISR_LBDF_Pos            (8U)    
#define USART_ISR_LBDF_Msk            (0x1UL << USART_ISR_LBDF_Pos)             /*!< 0x00000100 */
#define USART_ISR_LBDF                USART_ISR_LBDF_Msk                       /*!< LIN Break Detection Flag */
#define USART_ISR_CTSIF_Pos           (9U)    
#define USART_ISR_CTSIF_Msk           (0x1UL << USART_ISR_CTSIF_Pos)            /*!< 0x00000200 */
#define USART_ISR_CTSIF               USART_ISR_CTSIF_Msk                      /*!< CTS interrupt flag */
#define USART_ISR_CTS_Pos             (10U)   
#define USART_ISR_CTS_Msk             (0x1UL << USART_ISR_CTS_Pos)              /*!< 0x00000400 */
#define USART_ISR_CTS                 USART_ISR_CTS_Msk                        /*!< CTS flag */
#define USART_ISR_RTOF_Pos            (11U)   
#define USART_ISR_RTOF_Msk            (0x1UL << USART_ISR_RTOF_Pos)             /*!< 0x00000800 */
#define USART_ISR_RTOF                USART_ISR_RTOF_Msk                       /*!< Receiver Time Out */
#define USART_ISR_EOBF_Pos            (12U)   
#define USART_ISR_EOBF_Msk            (0x1UL << USART_ISR_EOBF_Pos)             /*!< 0x00001000 */
#define USART_ISR_EOBF                USART_ISR_EOBF_Msk                       /*!< End Of Block Flag */
#define USART_ISR_ABRE_Pos            (14U)   
#define USART_ISR_ABRE_Msk            (0x1UL << USART_ISR_ABRE_Pos)             /*!< 0x00004000 */
#define USART_ISR_ABRE                USART_ISR_ABRE_Msk                       /*!< Auto-Baud Rate Error */
#define USART_ISR_ABRF_Pos            (15U)   
#define USART_ISR_ABRF_Msk            (0x1UL << USART_ISR_ABRF_Pos)             /*!< 0x00008000 */
#define USART_ISR_ABRF                USART_ISR_ABRF_Msk                       /*!< Auto-Baud Rate Flag */
#define USART_ISR_BUSY_Pos            (16U)   
#define USART_ISR_BUSY_Msk            (0x1UL << USART_ISR_BUSY_Pos)             /*!< 0x00010000 */
#define USART_ISR_BUSY                USART_ISR_BUSY_Msk                       /*!< Busy Flag */
#define USART_ISR_CMF_Pos             (17U)   
#define USART_ISR_CMF_Msk             (0x1UL << USART_ISR_CMF_Pos)              /*!< 0x00020000 */
#define USART_ISR_CMF                 USART_ISR_CMF_Msk                        /*!< Character Match Flag */
#define USART_ISR_SBKF_Pos            (18U)   
#define USART_ISR_SBKF_Msk            (0x1UL << USART_ISR_SBKF_Pos)             /*!< 0x00040000 */
#define USART_ISR_SBKF                USART_ISR_SBKF_Msk                       /*!< Send Break Flag */
#define USART_ISR_RWU_Pos             (19U)   
#define USART_ISR_RWU_Msk             (0x1UL << USART_ISR_RWU_Pos)              /*!< 0x00080000 */
#define USART_ISR_RWU                 USART_ISR_RWU_Msk                        /*!< Receive Wake Up from mute mode Flag */
#define USART_ISR_WUF_Pos             (20U)   
#define USART_ISR_WUF_Msk             (0x1UL << USART_ISR_WUF_Pos)              /*!< 0x00100000 */
#define USART_ISR_WUF                 USART_ISR_WUF_Msk                        /*!< Wake Up from stop mode Flag */
#define USART_ISR_TEACK_Pos           (21U)   
#define USART_ISR_TEACK_Msk           (0x1UL << USART_ISR_TEACK_Pos)            /*!< 0x00200000 */
#define USART_ISR_TEACK               USART_ISR_TEACK_Msk                      /*!< Transmit Enable Acknowledge Flag */
#define USART_ISR_REACK_Pos           (22U)   
#define USART_ISR_REACK_Msk           (0x1UL << USART_ISR_REACK_Pos)            /*!< 0x00400000 */
#define USART_ISR_REACK               USART_ISR_REACK_Msk                      /*!< Receive Enable Acknowledge Flag */

#define UART_IT_PE                          0x0028U              /*!< UART parity error interruption                 */
#define UART_IT_TXE                         0x0727U              /*!< UART transmit data register empty interruption */
#define UART_IT_TC                          0x0626U              /*!< UART transmission complete interruption        */
#define UART_IT_RXNE                        0x0525U              /*!< UART read data register not empty interruption */
#define UART_IT_IDLE                        0x0424U              /*!< UART idle interruption                         */
#define UART_IT_LBD                         0x0846U              /*!< UART LIN break detection interruption          */
#define UART_IT_CTS                         0x096AU              /*!< UART CTS interruption                          */
#define UART_IT_CM                          0x112EU              /*!< UART character match interruption              */
#define UART_IT_WUF                         0x1476U              /*!< UART wake-up from stop mode interruption       */
#define UART_IT_RTO                         0x0B3AU              /*!< UART receiver timeout interruption             */

#define UART_IT_ERR                         0x0060U              /*!< UART error interruption                        */

#define UART_IT_ORE                         0x0300U              /*!< UART overrun error interruption                */
#define UART_IT_NE                          0x0200U              /*!< UART noise error interruption                  */
#define UART_IT_FE                          0x0100U              /*!< UART frame error interruption                  */


void get_huart(UART_HandleTypeDef *huart);

void __HAL_UART_CLEAR_OREFLAG(UART_HandleTypeDef *huart);

void __HAL_UART_CLEAR_FEFLAG(UART_HandleTypeDef *huart);

void __HAL_UART_ENABLE_IT(UART_HandleTypeDef *huart, uint32_t it);

void __HAL_UART_DISABLE_IT(UART_HandleTypeDef *huart, uint32_t it);


#endif