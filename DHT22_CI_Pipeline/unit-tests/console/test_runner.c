#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP_RUNNER(test_console)
{
    RUN_TEST_CASE(test_console, test_getstr_full);
    RUN_TEST_CASE(test_console, test_getstr_short);
    RUN_TEST_CASE(test_console, test_dbgpln);
    RUN_TEST_CASE(test_console, test_dbgp);
}

