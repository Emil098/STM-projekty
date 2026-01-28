/**
 * @file test_debug_uart.c
 * @brief Unit tests for the debug UART driver (Unity + CMock, host-based).
 *
 * This test suite validates the behavior of the interrupt-driven debug UART
 * module (debug_uart.*) without requiring real STM32 hardware.
 *
 * Test approach:
 *  - The UART peripheral registers are emulated using a local USART_TypeDef
 *    instance (fakeUartInstance) assigned to debug_huart.Instance.
 *  - HAL macros used by the driver (e.g. __HAL_UART_ENABLE_IT, clearing flags)
 *    are mocked via CMock (mock_fakeUart.h) to verify correct configuration and
 *    interrupt control.
 *  - The ISR entry point is modeled by calling debug_UART_irq() after setting
 *    appropriate bits in the emulated ISR register and providing data in RDR.
 *
 * Notes for reviewers:
 *  - These are deterministic unit tests intended to run in CI on a host
 *    compiler (gcc), proving correctness of ring-buffer logic and ISR handling.
 */

#include "unity.h"
#include "unity_fixture.h"
#include "cmock.h"

#include "mock_fakeUart.h"

#include "fakeUart.h"
#include "debug_uart.h"

#include <string.h>


extern UART_HandleTypeDef debug_huart;
USART_TypeDef fakeUartInstance;

/* -------------------------------------------------------------------------- */
/* Helpers                                                                     */
/* -------------------------------------------------------------------------- */

/**
 * @brief Feed a zero-terminated byte stream into the RX path of the driver.
 *
 * The helper emulates receiving bytes from UART by:
 *  - setting RXNE in the emulated ISR register,
 *  - placing a byte into RDR,
 *  - expecting error flags to be cleared (driver behavior),
 *  - calling debug_UART_irq() to execute the ISR logic.
 *
 * @param[in] str Zero-terminated input stream to inject into RX.
 */
static void fill_rx_buffer(uint8_t *str)
{
    while(*str != 0)
    {
        /* Emulate "RX data available" */
        debug_huart.Instance->ISR = USART_ISR_RXNE;
        debug_huart.Instance->RDR = *str;

        /* Driver clears common error flags on each IRQ entry */
        __HAL_UART_CLEAR_OREFLAG_Expect(&debug_huart);
        __HAL_UART_CLEAR_FEFLAG_Expect(&debug_huart);

        debug_UART_irq();

        str++;
    }
}

/* -------------------------------------------------------------------------- */
/* Test group                                                                  */
/* -------------------------------------------------------------------------- */

TEST_GROUP(test_debug_uart);

TEST_SETUP(test_debug_uart)
{
    
    memset(&fakeUartInstance, 0, sizeof(fakeUartInstance));  // Clear the memory
    debug_huart.Instance = &fakeUartInstance;  // Assign the address of fakeUartInstance to the Instance pointer

    /* Arrange: debug_init() must enable RX and disable unused IRQ sources */
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

/* -------------------------------------------------------------------------- */
/* Tests                                                                       */
/* -------------------------------------------------------------------------- */

/**
 * @test Receive a single byte and then report RX empty
 * @brief Verifies that one received byte is stored in the RX ring buffer and
 *        dbg_getc() returns false when no more data is available.
 */
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

/**
 * @test Transmit a single byte via TX ring buffer
 * @brief Verifies that dbg_putc_nonblock() enqueues a byte and the ISR writes it
 *        to TDR when TXE is signaled.
 */
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

/**
 * @test TX buffer overflow handling
 * @brief Verifies that dbg_putc_nonblock() returns false when the TX ring buffer
 *        is full and does not overwrite queued data.
 */
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

/**
 * @test Disable TXE interrupt when TX buffer becomes empty
 * @brief Verifies that the ISR disables the TXE interrupt once all queued bytes
 *        have been transmitted.
 */
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
