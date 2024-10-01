#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP_RUNNER(test_consoleToUT)
{
    RUN_TEST_CASE(test_consoleToUT, test_print_hello);
    RUN_TEST_CASE(test_consoleToUT, test_print_version);
    RUN_TEST_CASE(test_consoleToUT, test_invalid_cmd);
    RUN_TEST_CASE(test_consoleToUT, test_echo);
    RUN_TEST_CASE(test_consoleToUT, test_echo_invalid_param);
    RUN_TEST_CASE(test_consoleToUT, test_too_long_cmd);
    RUN_TEST_CASE(test_consoleToUT, test_empty_cmd);
    RUN_TEST_CASE(test_consoleToUT, test_backspace);
}

