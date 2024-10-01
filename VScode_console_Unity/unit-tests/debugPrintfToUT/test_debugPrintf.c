#include "unity.h"
#include "unity_fixture.h"
#include "cmock.h"

#include "mock_debugUart.h"

#include "debugUart.h"
#include "debugPrintf.h"
#include <string.h>


TEST_GROUP(test_debugPrintfToUT);

TEST_SETUP(test_debugPrintfToUT)
{

}

TEST_TEAR_DOWN(test_debugPrintfToUT)
{

}

TEST(test_debugPrintfToUT, test_dbg_printf_ln)
{
    const char test_msg[] = "test message";
    int text_len = (int) strlen(test_msg);

    for(int i=0;i<text_len;i++)
        dbg_putc_nonblock_ExpectAndReturn((uint8_t)test_msg[i], true);

    dbg_putc_nonblock_ExpectAndReturn('\n', true);

    dbg_printf_ln("%s",test_msg);
}

TEST(test_debugPrintfToUT, test_dbg_printf)
{
    const char test_msg[] = "test message";
    int text_len = (int)strlen(test_msg);

    for (int i = 0; i < text_len; i++)
        dbg_putc_nonblock_ExpectAndReturn((uint8_t)test_msg[i], true);

    dbg_printf("%s", test_msg);
}