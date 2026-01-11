#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP_RUNNER(dht22_hw_start_signal)
{
    RUN_TEST_CASE(dht22_hw_start_signal, start_signal_complete_sequence);
    RUN_TEST_CASE(dht22_hw_start_signal, start_signal_null_sensor);
    RUN_TEST_CASE(dht22_hw_start_signal, start_signal_uninitialized_sensor);
}

TEST_GROUP_RUNNER(dht22_hw_check_response)
{
    RUN_TEST_CASE(dht22_hw_check_response, check_response_null_sensor);
    RUN_TEST_CASE(dht22_hw_check_response, check_response_success);
    RUN_TEST_CASE(dht22_hw_check_response, check_response_no_low_signal);
    RUN_TEST_CASE(dht22_hw_check_response, check_response_timeout_low);
    RUN_TEST_CASE(dht22_hw_check_response, check_response_multiple_while_iterations);
}

TEST_GROUP_RUNNER(dht22_hw_read_byte)
{
    RUN_TEST_CASE(dht22_hw_read_byte, read_byte_all_zeros);
    RUN_TEST_CASE(dht22_hw_read_byte, read_byte_all_ones);
    RUN_TEST_CASE(dht22_hw_read_byte, read_byte_alternating_pattern);
    RUN_TEST_CASE(dht22_hw_read_byte, read_byte_alternating_pattern_inverse);
    RUN_TEST_CASE(dht22_hw_read_byte, read_byte_null_sensor);
    RUN_TEST_CASE(dht22_hw_read_byte, read_byte_with_delayed_edges);
}

TEST_GROUP_RUNNER(dht22_hw_validate_checksum)
{
    RUN_TEST_CASE(dht22_hw_validate_checksum, validate_checksum_valid);
    RUN_TEST_CASE(dht22_hw_validate_checksum, validate_checksum_invalid);
    RUN_TEST_CASE(dht22_hw_validate_checksum, validate_checksum_all_zeros);
    RUN_TEST_CASE(dht22_hw_validate_checksum, validate_checksum_all_ones);
    RUN_TEST_CASE(dht22_hw_validate_checksum, validate_checksum_overflow);
    RUN_TEST_CASE(dht22_hw_validate_checksum, validate_checksum_null_data);
    RUN_TEST_CASE(dht22_hw_validate_checksum, validate_checksum_multiple_valid);
}

TEST_GROUP_RUNNER(dht22_hw_integration)
{
    RUN_TEST_CASE(dht22_hw_integration, complete_read_sequence);
}