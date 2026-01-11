#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP_RUNNER(test_debug_uart)
{
    RUN_TEST_CASE(test_debug_uart, test_tx_single);
    RUN_TEST_CASE(test_debug_uart, test_rx_single);
    RUN_TEST_CASE(test_debug_uart, test_tx_overflow);
    RUN_TEST_CASE(test_debug_uart, test_tx_empty_irq);
}

