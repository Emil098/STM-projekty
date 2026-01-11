#include "unity.h"
#include "unity_fixture.h"
#include "cmock.h"

#include "mock_fakeUart.h"

#include "fakeUart.h"
#include "debug_uart.h"

#include <string.h>


extern UART_HandleTypeDef debug_huart;
USART_TypeDef fakeUartInstance;

static void fill_rx_buffer(uint8_t *str)
{
    while(*str != 0)
    {

    debug_huart.Instance->ISR = USART_ISR_RXNE;
    debug_huart.Instance->RDR = *str;

    __HAL_UART_CLEAR_OREFLAG_Expect(&debug_huart);
    __HAL_UART_CLEAR_FEFLAG_Expect(&debug_huart);
    
    debug_UART_irq();

    str++;
    }
}

TEST_GROUP(test_debug_uart);

TEST_SETUP(test_debug_uart)
{
    memset(&fakeUartInstance, 0, sizeof(fakeUartInstance));  // Clear the memory
    debug_huart.Instance = &fakeUartInstance;  // Assign the address of fakeUartInstance to the Instance pointer

    __HAL_UART_ENABLE_IT_Expect( &debug_huart, UART_IT_RXNE);
    __HAL_UART_DISABLE_IT_Expect(&debug_huart, UART_IT_PE);
    __HAL_UART_DISABLE_IT_Expect(&debug_huart, UART_IT_TC);
    __HAL_UART_DISABLE_IT_Expect(&debug_huart, UART_IT_IDLE);
    __HAL_UART_DISABLE_IT_Expect(&debug_huart, UART_IT_CTS);

    debug_init();
}

TEST_TEAR_DOWN(test_debug_uart)
{

}

TEST(test_debug_uart, test_rx_single)
{
    uint8_t actual = 0;
    uint8_t test_char = '5';

    debug_huart.Instance->ISR = USART_ISR_RXNE;
    debug_huart.Instance->RDR = test_char;

    __HAL_UART_CLEAR_OREFLAG_Expect(&debug_huart);
    __HAL_UART_CLEAR_FEFLAG_Expect(&debug_huart);
    
    debug_UART_irq();

    TEST_ASSERT_TRUE(dbg_getc(&actual));

    TEST_ASSERT_EQUAL_UINT8(test_char, actual);

    TEST_ASSERT_FALSE(dbg_getc(&actual));

    __HAL_UART_CLEAR_OREFLAG_Ignore();
    __HAL_UART_CLEAR_FEFLAG_Ignore();
    
    for(int i=0;i<DEBUG_RX_SIZE;i++)
    {
        debug_UART_irq();
    }

    debug_huart.Instance->ISR |= USART_ISR_FE;

    debug_UART_irq();
    TEST_PASS_MESSAGE("PASSED");
}

TEST(test_debug_uart, test_tx_single)
{
    __HAL_UART_ENABLE_IT_Expect(&debug_huart, UART_IT_TXE);
    
    TEST_ASSERT_TRUE(dbg_putc_nonblock('1'));
    
    __HAL_UART_CLEAR_OREFLAG_Expect(&debug_huart);
    __HAL_UART_CLEAR_FEFLAG_Expect(&debug_huart);
    
    debug_huart.Instance->ISR = USART_ISR_TXE;
    debug_UART_irq();

    TEST_ASSERT_EQUAL_CHAR('1',(char)(debug_huart.Instance->TDR));
    TEST_PASS_MESSAGE("PASSED");
}

TEST(test_debug_uart, test_tx_overflow)
{
    __HAL_UART_ENABLE_IT_Ignore();
    __HAL_UART_CLEAR_OREFLAG_Ignore();
    __HAL_UART_CLEAR_FEFLAG_Ignore();

    while(dbg_putc_nonblock('1'));

    TEST_ASSERT_FALSE(dbg_putc_nonblock('2'));

    debug_huart.Instance->ISR = USART_ISR_TXE;
    debug_UART_irq();

    TEST_ASSERT_NOT_EQUAL_UINT8('2',(uint8_t)(debug_huart.Instance->TDR));
    TEST_PASS_MESSAGE("PASSED");
}

TEST(test_debug_uart, test_tx_empty_irq)
{
    __HAL_UART_ENABLE_IT_Ignore();
    __HAL_UART_CLEAR_OREFLAG_Ignore();
    __HAL_UART_CLEAR_FEFLAG_Ignore();

    TEST_ASSERT_TRUE(dbg_putc_nonblock('1'));

    debug_huart.Instance->ISR = USART_ISR_TXE;
    debug_UART_irq();
    TEST_ASSERT_EQUAL_UINT8('1',(uint8_t)(debug_huart.Instance->TDR));

    __HAL_UART_DISABLE_IT_Expect(&debug_huart, UART_IT_TXE);
    debug_UART_irq();
    TEST_PASS_MESSAGE("PASSED");
}
