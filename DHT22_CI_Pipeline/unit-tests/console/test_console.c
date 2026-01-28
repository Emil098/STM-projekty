/**
 * @file test_console.c
 * @brief Unit tests for the console module (Unity + CMock).
 *
 * This test suite validates the public API of the console module:
 *  - dbg_getstr(): reading bytes via dbg_getc() into a user buffer
 *  - dbgp():      formatted print without newline
 *  - dbgpln():    formatted print with trailing newline
 *
 * The console module depends on the debug UART driver (debug_uart.*).
 * In unit tests we replace the UART dependency using CMock-generated mocks.
 *
 * Test strategy:
 *  - For dbg_getstr() we emulate an RX stream by programming expectations on
 *    dbg_getc() and providing data via ReturnThruPtr (CMock feature).
 *  - For dbgp()/dbgpln() we verify the exact byte stream sent through
 *    dbg_putc_nonblock().
 *
 * Notes for reviewers:
 *  - dbg_getstr() reads raw bytes and does NOT append '\0'. Tests compare
 *    byte arrays using TEST_ASSERT_EQUAL_CHAR_ARRAY() rather than strings.
 */

#include "unity.h"
#include "unity_fixture.h"
#include "cmock.h"

#include "mock_debug_uart.h"

#include "console.h"
#include "version.h"

#include <string.h>

static uint8_t rx_data[64];
static int rx_len;
static int rx_pos;

TEST_GROUP(test_console);

TEST_SETUP(test_console)
{

}

TEST_TEAR_DOWN(test_console)
{

}

/* -------------------------------------------------------------------------- */
/* Helpers                                                                     */
/* -------------------------------------------------------------------------- */

/**
 * @brief Program a mocked RX stream for dbg_getstr().
 *
 * dbg_getstr() repeatedly calls dbg_getc(dst_ptr) until either:
 *  - requested length is exhausted, or
 *  - dbg_getc() returns false (no more bytes available).
 *
 * This helper prepares a deterministic RX stream:
 *  - for each byte: dbg_getc() returns true and writes one byte into *dst_ptr
 *  - after the last byte: dbg_getc() returns false
 *
 * @param[out] dst Destination buffer passed by dbg_getstr() to dbg_getc().
 * @param[in]  src Byte stream to be "received".
 * @param[in]  len Number of bytes to provide.
 */
static void expect_rx_string(uint8_t *dst, const uint8_t *src, int len)
{
    for (int i = 0; i < len; i++)
    {
        /* dbg_getstr() passes &dst[i] as the destination pointer */
        dbg_getc_ExpectAndReturn(&dst[i], true);

        /* Provide one byte via pointer output parameter */
        dbg_getc_ReturnThruPtr_d((uint8_t*)&src[i]);   
    }

    /* One extra call ends the loop inside dbg_getstr() */
    uint8_t dummy = 0;
    dbg_getc_ExpectAndReturn(&dst[len], false); 
    dbg_getc_ReturnThruPtr_d(&dummy);           
}

/* -------------------------------------------------------------------------- */
/* Tests                                                                       */
/* -------------------------------------------------------------------------- */

/**
 * @test Read full string from debug UART
 * @brief Verifies that dbg_getstr() reads all available bytes until RX is empty.
 */
TEST(test_console, test_getstr_full)
{
    char test_msg[] = "TEST MSG";
    char actual_msg[20];

    int text_len = (int) strlen(test_msg);

    expect_rx_string((uint8_t*)actual_msg, (uint8_t*)test_msg, text_len);

    int actual_len = dbg_getstr((uint8_t *) actual_msg, (text_len + 5));

    TEST_ASSERT_EQUAL_INT(text_len, actual_len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(test_msg,actual_msg,text_len);
    TEST_PASS_MESSAGE("PASSED");
}

/**
 * @test Read partial string when length limit is smaller than RX data
 * @brief Verifies that dbg_getstr() stops reading when the requested length is reached.
 */
TEST(test_console, test_getstr_short)
{
    char test_msg[] = "TEST MSG";
    char actual_msg[20];

    int text_len = (int) strlen(test_msg);

    expect_rx_string((uint8_t*)actual_msg, (uint8_t*)test_msg, text_len);

    int actual_len = dbg_getstr((uint8_t *) actual_msg, 3);

    TEST_ASSERT_EQUAL_INT(3, actual_len);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(test_msg,actual_msg,3);
    TEST_PASS_MESSAGE("PASSED");
}

/**
 * @test Print formatted string with newline
 * @brief Verifies that dbgpln() outputs the formatted message followed by a newline character.
 */
TEST(test_console, test_dbgpln)
{
    const char test_msg[] = "test message";
    int text_len = (int) strlen(test_msg);

    for(int i=0;i<text_len;i++)
        dbg_putc_nonblock_ExpectAndReturn((uint8_t)test_msg[i], true);

    dbg_putc_nonblock_ExpectAndReturn('\n', true);

    dbgpln("%s",test_msg);
    TEST_PASS_MESSAGE("PASSED");
}

/**
 * @test Print formatted string without newline
 * @brief Verifies that dbgp() outputs only the formatted message without appending a newline.
 */
TEST(test_console, test_dbgp)
{
    const char test_msg[] = "test message";
    int text_len = (int)strlen(test_msg);

    for (int i = 0; i < text_len; i++)
        dbg_putc_nonblock_ExpectAndReturn((uint8_t)test_msg[i], true);

    dbgp("%s", test_msg);
    TEST_PASS_MESSAGE("PASSED");
}