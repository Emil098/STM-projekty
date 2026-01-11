#include "unity.h"
#include "unity_fixture.h"

static void runAllTests(void)
{
    RUN_TEST_GROUP(dht22_hw_start_signal);
    RUN_TEST_GROUP(dht22_hw_check_response);
    RUN_TEST_GROUP(dht22_hw_read_byte);
    RUN_TEST_GROUP(dht22_hw_validate_checksum);
    RUN_TEST_GROUP(dht22_hw_integration);
}

int main(int argc, const char* argv[])
{
    return UnityMain(argc, argv, runAllTests);
}