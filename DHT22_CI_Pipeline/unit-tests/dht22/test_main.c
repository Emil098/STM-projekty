#include "unity.h"
#include "unity_fixture.h"

static void runAllTests(void)
{
    RUN_TEST_GROUP(dht22_initialization);
    RUN_TEST_GROUP(dht22_state_management);
    RUN_TEST_GROUP(dht22_reading_logic);
    RUN_TEST_GROUP(dht22_error_handling);
    RUN_TEST_GROUP(dht22_utilities);
    RUN_TEST_GROUP(dht22_edge_cases);
}

int main(int argc, const char* argv[])
{
    return UnityMain(argc, argv, runAllTests);
}