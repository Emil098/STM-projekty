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


static void expect_rx_string(uint8_t *dst, const uint8_t *src, int len)
{
    for (int i = 0; i < len; i++)
    {
        dbg_getc_ExpectAndReturn(&dst[i], true);
        dbg_getc_ReturnThruPtr_d((uint8_t*)&src[i]);   
    }

    uint8_t dummy = 0;
    dbg_getc_ExpectAndReturn(&dst[len], false); 
    dbg_getc_ReturnThruPtr_d(&dummy);           
}

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

TEST(test_console, test_dbgp)
{
    const char test_msg[] = "test message";
    int text_len = (int)strlen(test_msg);

    for (int i = 0; i < text_len; i++)
        dbg_putc_nonblock_ExpectAndReturn((uint8_t)test_msg[i], true);

    dbgp("%s", test_msg);
    TEST_PASS_MESSAGE("PASSED");
}