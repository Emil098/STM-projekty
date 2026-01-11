#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP_RUNNER(dht22_initialization)
{
    RUN_TEST_CASE(dht22_initialization, init_success);
    RUN_TEST_CASE(dht22_initialization, init_null_sensor);
    RUN_TEST_CASE(dht22_initialization, deinit_resets_state);
    RUN_TEST_CASE(dht22_initialization, deinit_null_safe);
}

TEST_GROUP_RUNNER(dht22_state_management)
{
    RUN_TEST_CASE(dht22_state_management, is_ready_initialized);
    RUN_TEST_CASE(dht22_state_management, is_ready_null_sensor);
    RUN_TEST_CASE(dht22_state_management, is_ready_uninitialized);
}

TEST_GROUP_RUNNER(dht22_reading_logic)
{
    RUN_TEST_CASE(dht22_reading_logic, read_temperature_humidity_success);
    RUN_TEST_CASE(dht22_reading_logic, read_with_null_outputs);
    RUN_TEST_CASE(dht22_reading_logic, read_only_temperature);
    RUN_TEST_CASE(dht22_reading_logic, read_only_humidity);
    RUN_TEST_CASE(dht22_reading_logic, read_negative_temperature);
}

TEST_GROUP_RUNNER(dht22_error_handling)
{
    RUN_TEST_CASE(dht22_error_handling, read_null_sensor);
    RUN_TEST_CASE(dht22_error_handling, read_uninitialized_sensor);
    RUN_TEST_CASE(dht22_error_handling, read_response_failure);
    RUN_TEST_CASE(dht22_error_handling, read_checksum_failure);
    RUN_TEST_CASE(dht22_error_handling, hardware_error_propagation);
}

TEST_GROUP_RUNNER(dht22_utilities)
{
    RUN_TEST_CASE(dht22_utilities, status_to_string_all_codes);
    RUN_TEST_CASE(dht22_utilities, status_to_string_boundary);
}

TEST_GROUP_RUNNER(dht22_edge_cases)
{
    RUN_TEST_CASE(dht22_edge_cases, extreme_temperature_values);
    RUN_TEST_CASE(dht22_edge_cases, reinitialization_cycle);
}