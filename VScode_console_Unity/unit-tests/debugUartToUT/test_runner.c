#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP_RUNNER(test_debugUartToUT)
{
    RUN_TEST_CASE(test_debugUartToUT, test_tx_single);
    RUN_TEST_CASE(test_debugUartToUT, test_rx_single);
    RUN_TEST_CASE(test_debugUartToUT, test_tx_overflow);
    RUN_TEST_CASE(test_debugUartToUT, test_tx_empty_irq);
    RUN_TEST_CASE(test_debugUartToUT, test_rx_overflow);
    RUN_TEST_CASE(test_debugUartToUT, test_getstr);
}

