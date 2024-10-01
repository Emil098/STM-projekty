#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP_RUNNER(test_debugPrintfToUT)
{
    RUN_TEST_CASE(test_debugPrintfToUT, test_dbg_printf_ln);
    RUN_TEST_CASE(test_debugPrintfToUT, test_dbg_printf);
}

