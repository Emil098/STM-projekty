/**
 * @file test_dht22.c
 * @brief Comprehensive unit tests for DHT22 sensor driver
 * 
 * This test suite validates the DHT22 temperature and humidity sensor driver
 * using the Unity testing framework and CMock for mocking hardware dependencies.
 * 
 * Test Categories:
 * 1. Initialization and Deinitialization
 * 2. Sensor Readiness and State Management
 * 3. Communication Protocol (Start Signal, Response)
 * 4. Data Parsing and Validation
 * 5. Error Handling and Edge Cases
 * 
 * @note Tests are hardware-independent through HAL mocking
 * @note Uses CMock for precise expectation verification
 */

#include "unity.h"
#include "unity_fixture.h"
#include "cmock.h"

#include "mock_hal_delay_gpio.h" 
#include "mock_dht22_hw.h" 
#include "dht22_types.h"
#include "dht22.h"

#include <string.h>
#include <stdbool.h>

/* Test fixture - shared test data */
static dht22_t sensor;
static const gpio_pin_t DHT22_PIN = {ONE_W_GPIO_Port, ONE_W_Pin};

/* Helper function to create raw DHT22 data */
static void create_test_data(uint8_t* data, uint16_t humidity, int16_t temperature)
{
    /* DHT22 data format: humidity * 10, temperature * 10 */
    data[0] = (uint8_t)(humidity >> 8); /* Humidity high byte */
    data[1] = (uint8_t)humidity;        /* Humidity low byte */

    /* Temperature with sign handling */
    uint16_t temp_abs;
    
    if (temperature < 0)
    {
        temp_abs = (uint16_t)(-temperature);  // Absolute value
        data[2] = (uint8_t)((temp_abs >> 8) | 0x80);  // Set sign bit (MSB)
    }
    else
    {
        temp_abs = (uint16_t)temperature;
        data[2] = (uint8_t)(temp_abs >> 8);  // Clear sign bit
    }
    
    data[3] = (uint8_t)(temp_abs & 0xFF);  // Low byte

    /* Calculate checksum */
    data[4] = (uint8_t)(data[0] + data[1] + data[2] + data[3]);

}

static void mock_read_sequence(dht22_t* sensor,uint8_t* test_data, dht22_status_t start_sig_exp, bool checksum_exp)
{
    dht22_start_signal_Expect(sensor);
    dht22_check_response_ExpectAndReturn(sensor, start_sig_exp);

    dht22_read_byte_ExpectAndReturn(sensor, test_data[0]);
    dht22_read_byte_ExpectAndReturn(sensor, test_data[1]);
    dht22_read_byte_ExpectAndReturn(sensor, test_data[2]);
    dht22_read_byte_ExpectAndReturn(sensor, test_data[3]);
    dht22_read_byte_ExpectAndReturn(sensor, test_data[4]);

    dht22_validate_checksum_ExpectAndReturn(test_data, checksum_exp);
}

TEST_GROUP(dht22_initialization);

TEST_SETUP(dht22_initialization)
{
    /* Reset sensor before each test */
    memset(&sensor, 0, sizeof(sensor));
    
    /* HAL is mocked, so no real hardware calls */
}

TEST_TEAR_DOWN(dht22_initialization)
{
    /* Clean up if needed */
    dht22_deinit(&sensor);
}

/**
 * @test Test successful sensor initialization
 * @brief Verifies dht22_init() initializes sensor structure correctly
 */
TEST(dht22_initialization, init_success)
{
    /* Setup HAL expectations */
    hal_delay_init_Expect();
    hal_gpio_set_mode_input_Expect(DHT22_PIN);
    
    /* Execute */
    dht22_status_t status = dht22_init(&sensor, DHT22_PIN);
    
    /* Verify */
    TEST_ASSERT_EQUAL(DHT22_OK, status);
    TEST_ASSERT_TRUE(sensor.initialized);
    TEST_ASSERT_EQUAL(DHT22_PIN.port, sensor.data_pin.port);
    TEST_ASSERT_EQUAL(DHT22_PIN.pin, sensor.data_pin.pin);

    TEST_PASS_MESSAGE("PASSED");
}

/**
 * @test Test initialization with NULL sensor pointer
 * @brief Verifies error handling for invalid parameters
 */
TEST(dht22_initialization, init_null_sensor)
{
    /* No HAL calls expected for NULL pointer */
    
    dht22_status_t status = dht22_init(NULL, DHT22_PIN);
    
    TEST_ASSERT_EQUAL(DHT22_ERROR_INVALID_DATA, status);

    TEST_PASS_MESSAGE("PASSED");
}

/**
 * @test Test sensor deinitialization
 * @brief Verifies dht22_deinit() resets sensor state
 */
TEST(dht22_initialization, deinit_resets_state)
{
    /* First initialize */
    hal_delay_init_Expect();
    hal_gpio_set_mode_input_Expect(DHT22_PIN);
    dht22_init(&sensor, DHT22_PIN);
    
    TEST_ASSERT_TRUE(sensor.initialized);
    
    /* Deinitialize */
    dht22_deinit(&sensor);
    
    /* Verify reset */
    TEST_ASSERT_FALSE(sensor.initialized);
    TEST_ASSERT_EQUAL(0, sensor.data_pin.port);
    TEST_ASSERT_EQUAL(0, sensor.data_pin.pin);

    TEST_PASS_MESSAGE("PASSED");
}

/**
 * @test Test deinitialization with NULL pointer
 * @brief Verifies graceful handling of NULL parameter
 */
TEST(dht22_initialization, deinit_null_safe)
{
    /* Should not crash */
    dht22_deinit(NULL);
    
    TEST_PASS_MESSAGE("PASSED");
}

/* ========================================================================== */
/* Test Group: dht22_state_management                                         */
/* ========================================================================== */

TEST_GROUP(dht22_state_management);

TEST_SETUP(dht22_state_management)
{
    memset(&sensor, 0, sizeof(sensor));
}

TEST_TEAR_DOWN(dht22_state_management)
{
    /* No cleanup needed */
}

/**
 * @test Test sensor readiness check
 * @brief Verifies dht22_is_ready() returns correct state
 */
TEST(dht22_state_management, is_ready_initialized)
{
    /* Manually set as initialized (bypassing init for unit test) */
    sensor.initialized = true;
    sensor.data_pin = DHT22_PIN;
    
    bool ready = dht22_is_ready(&sensor);
    TEST_ASSERT_TRUE(ready);

    TEST_PASS_MESSAGE("PASSED");
}

/**
 * @test Test sensor not ready when NULL
 * @brief Verifies error handling for NULL parameter
 */
TEST(dht22_state_management, is_ready_null_sensor)
{
    bool ready = dht22_is_ready(NULL);
    TEST_ASSERT_FALSE(ready);

    TEST_PASS_MESSAGE("PASSED");
}

/**
 * @test Test sensor not ready when not initialized
 * @brief Verifies detection of uninitialized sensor
 */
TEST(dht22_state_management, is_ready_uninitialized)
{
    sensor.initialized = false;
    
    bool ready = dht22_is_ready(&sensor);
    TEST_ASSERT_FALSE(ready);

    TEST_PASS_MESSAGE("PASSED");
}

/* ========================================================================== */
/* Test Group: dht22_reading_logic                                            */
/* ========================================================================== */

TEST_GROUP(dht22_reading_logic);

TEST_SETUP(dht22_reading_logic)
{
    memset(&sensor, 0, sizeof(sensor));
    
    /* Initialize sensor for reading tests */
    hal_delay_init_Expect();
    hal_gpio_set_mode_input_Expect(DHT22_PIN);
    dht22_init(&sensor, DHT22_PIN);
    
    /* Reset all mock expectations */
    mock_dht22_hw_Init();
    mock_hal_delay_gpio_Init();
}

TEST_TEAR_DOWN(dht22_reading_logic)
{
    dht22_deinit(&sensor);
    
    /* Verify all mock expectations were met */
    mock_dht22_hw_Verify();
    mock_hal_delay_gpio_Verify();
}

/**
 * @test Test successful temperature and humidity reading
 * @brief Verifies complete path with valid data
 */
TEST(dht22_reading_logic, read_temperature_humidity_success)
{
    float temperature = 0.0f;
    float humidity = 0.0f;
    
    /* Test data: 42.3% humidity, 23.7°C temperature */
    uint8_t test_data[5];
    create_test_data(test_data, 423, 237);  /* 42.3 * 10, 23.7 * 10 */
    
    /* Mock hardware layer sequence */
    mock_read_sequence(&sensor, test_data, DHT22_OK, true);

    /* Execute */
    dht22_status_t status = dht22_read_temperature_humidity(&sensor, 
                                                            &temperature, 
                                                            &humidity);
    
    /* Verify */
    TEST_ASSERT_EQUAL(DHT22_OK, status);
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 23.7f, temperature);
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 42.3f, humidity);

    TEST_PASS_MESSAGE("PASSED");
}

/**
 * @test Test reading with NULL output parameters
 * @brief Verifies function works with optional outputs
 */
TEST(dht22_reading_logic, read_with_null_outputs)
{
    /* Test data */
    uint8_t test_data[5];
    create_test_data(test_data, 567, 189);  /* 56.7% humidity, 18.9°C */
    
    /* Mock sequence */
    mock_read_sequence(&sensor, test_data, DHT22_OK, true);
    
    /* Execute with NULL outputs */
    dht22_status_t status = dht22_read_temperature_humidity(&sensor, NULL, NULL);
    
    TEST_ASSERT_EQUAL(DHT22_OK, status);

    TEST_PASS_MESSAGE("PASSED");
}

/**
 * @test Test reading with only temperature output
 * @brief Verifies function works with partial outputs
 */
TEST(dht22_reading_logic, read_only_temperature)
{
    float temperature = 0.0f;
    
    uint8_t test_data[5];
    create_test_data(test_data, 500, 255);  /* 50.0% humidity, 25.5°C */

    mock_read_sequence(&sensor, test_data, DHT22_OK, true);
    
    dht22_status_t status = dht22_read_temperature_humidity(&sensor, 
                                                            &temperature, 
                                                            NULL);
    
    TEST_ASSERT_EQUAL(DHT22_OK, status);
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 25.5f, temperature);

    TEST_PASS_MESSAGE("PASSED");
}

/**
 * @test Test reading with only humidity output
 * @brief Verifies function works with partial outputs
 */
TEST(dht22_reading_logic, read_only_humidity)
{
    float humidity = 0.0f;
    
    uint8_t test_data[5];
    create_test_data(test_data, 789, 312);  /* 78.9% humidity, 31.2°C */
    
    mock_read_sequence(&sensor, test_data, DHT22_OK, true);
    
    dht22_status_t status = dht22_read_temperature_humidity(&sensor, 
                                                            NULL, 
                                                            &humidity);
    
    TEST_ASSERT_EQUAL(DHT22_OK, status);
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 78.9f, humidity);

    TEST_PASS_MESSAGE("PASSED");
}

/**
 * @test Test reading negative temperature
 * @brief Verifies correct handling of negative temperatures
 */
TEST(dht22_reading_logic, read_negative_temperature)
{
    float temperature = 0.0f;
    float humidity = 0.0f;
    
    /* Test data: 45.6% humidity, -12.3°C temperature */
    uint8_t test_data[5];
    create_test_data(test_data, 456, -123);  /* Note: negative temperature */
    
    /* For negative temps, high byte has MSB set */
    TEST_ASSERT_EQUAL(0x80, test_data[2] & 0x80);  /* Sign bit should be set */
    
    mock_read_sequence(&sensor, test_data, DHT22_OK, true);
    
    dht22_status_t status = dht22_read_temperature_humidity(&sensor, 
                                                            &temperature, 
                                                            &humidity);
    
    TEST_ASSERT_EQUAL(DHT22_OK, status);
    TEST_ASSERT_FLOAT_WITHIN(0.1f, -12.3f, temperature);
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 45.6f, humidity);

    TEST_PASS_MESSAGE("PASSED");
}

/* ========================================================================== */
/* Test Group: dht22_error_handling                                           */
/* ========================================================================== */

TEST_GROUP(dht22_error_handling);

TEST_SETUP(dht22_error_handling)
{
    memset(&sensor, 0, sizeof(sensor));
    
    /* Initialize sensor for error tests */
    hal_delay_init_Expect();
    hal_gpio_set_mode_input_Expect(DHT22_PIN);
    dht22_init(&sensor, DHT22_PIN);
    
    mock_dht22_hw_Init();
    mock_hal_delay_gpio_Init();
}

TEST_TEAR_DOWN(dht22_error_handling)
{
    dht22_deinit(&sensor);
    mock_dht22_hw_Verify();
    mock_hal_delay_gpio_Verify();
}

/**
 * @test Test reading with NULL sensor pointer
 * @brief Verifies error handling for invalid sensor
 */
TEST(dht22_error_handling, read_null_sensor)
{
    float temperature, humidity;
    
    /* No mock expectations - should fail early */
    dht22_status_t status = dht22_read_temperature_humidity(NULL, 
                                                            &temperature, 
                                                            &humidity);
    
    TEST_ASSERT_EQUAL(DHT22_ERROR_UNINITIALIZED, status);

    TEST_PASS_MESSAGE("PASSED");
}

/**
 * @test Test reading with uninitialized sensor
 * @brief Verifies error when sensor not initialized
 */
TEST(dht22_error_handling, read_uninitialized_sensor)
{
    dht22_t uninit_sensor = {0};
    float temperature, humidity;
    
    dht22_status_t status = dht22_read_temperature_humidity(&uninit_sensor, 
                                                            &temperature, 
                                                            &humidity);
    
    TEST_ASSERT_EQUAL(DHT22_ERROR_UNINITIALIZED, status);

    TEST_PASS_MESSAGE("PASSED");
}

/**
 * @test Test sensor response failure
 * @brief Verifies error propagation from hardware layer
 */
TEST(dht22_error_handling, read_response_failure)
{
    float temperature, humidity;
    
    /* Mock start signal */
    dht22_start_signal_Expect(&sensor);
    
    /* Mock response failure */
    dht22_check_response_ExpectAndReturn(&sensor, DHT22_ERROR_NO_RESPONSE);
    
    /* No byte reading or checksum validation expected */
    
    dht22_status_t status = dht22_read_temperature_humidity(&sensor, 
                                                            &temperature, 
                                                            &humidity);
    
    TEST_ASSERT_EQUAL(DHT22_ERROR_NO_RESPONSE, status);

    TEST_PASS_MESSAGE("PASSED");
}

/**
 * @test Test checksum validation failure
 * @brief Verifies error when data integrity check fails
 */
TEST(dht22_error_handling, read_checksum_failure)
{
    float temperature, humidity;
    
    uint8_t test_data[5];
    create_test_data(test_data, 500, 250);
    
    /* Mock checksum failure */
    mock_read_sequence(&sensor, test_data, DHT22_OK, false);
    
    dht22_status_t status = dht22_read_temperature_humidity(&sensor, 
                                                            &temperature, 
                                                            &humidity);
    
    TEST_ASSERT_EQUAL(DHT22_ERROR_CHECKSUM, status);

    TEST_PASS_MESSAGE("PASSED");
}

/**
 * @test Test all hardware layer error codes propagation
 * @brief Verifies all hardware errors are properly propagated
 */
TEST(dht22_error_handling, hardware_error_propagation)
{
    struct {
        dht22_status_t hardware_error;
        dht22_status_t expected_error;
    } test_cases[] = {
        {DHT22_ERROR_NO_RESPONSE, DHT22_ERROR_NO_RESPONSE},
        {DHT22_ERROR_TIMEOUT, DHT22_ERROR_TIMEOUT},
        {DHT22_ERROR_COMMUNICATION, DHT22_ERROR_COMMUNICATION},
    };
    
    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++)
    {
        /* Setup */
        mock_dht22_hw_Init();
        
        dht22_start_signal_Expect(&sensor);
        dht22_check_response_ExpectAndReturn(&sensor, test_cases[i].hardware_error);
        
        /* Execute */
        dht22_status_t status = dht22_read_temperature_humidity(&sensor, NULL, NULL);
        
        /* Verify */
        TEST_ASSERT_EQUAL(test_cases[i].expected_error, status);
        
        mock_dht22_hw_Verify();
    }

    TEST_PASS_MESSAGE("PASSED");
}

/* ========================================================================== */
/* Test Group: dht22_utilities                                                */
/* ========================================================================== */

TEST_GROUP(dht22_utilities);

TEST_SETUP(dht22_utilities)
{
    /* Nothing to setup */
}

TEST_TEAR_DOWN(dht22_utilities)
{
    /* Nothing to teardown */
}

/**
 * @test Test status code to string conversion
 * @brief Verifies all error codes have string representations
 */
TEST(dht22_utilities, status_to_string_all_codes)
{
    /* Test all defined status codes */
    TEST_ASSERT_EQUAL_STRING("OK", dht22_status_to_string(DHT22_OK));
    TEST_ASSERT_EQUAL_STRING("Timeout error", dht22_status_to_string(DHT22_ERROR_TIMEOUT));
    TEST_ASSERT_EQUAL_STRING("Checksum error", dht22_status_to_string(DHT22_ERROR_CHECKSUM));
    TEST_ASSERT_EQUAL_STRING("Sensor not ready", dht22_status_to_string(DHT22_ERROR_NOT_READY));
    TEST_ASSERT_EQUAL_STRING("No response", dht22_status_to_string(DHT22_ERROR_NO_RESPONSE));
    TEST_ASSERT_EQUAL_STRING("Communication error", dht22_status_to_string(DHT22_ERROR_COMMUNICATION));
    TEST_ASSERT_EQUAL_STRING("Invalid data", dht22_status_to_string(DHT22_ERROR_INVALID_DATA));
    TEST_ASSERT_EQUAL_STRING("Sensor uninitialized", dht22_status_to_string(DHT22_ERROR_UNINITIALIZED));
    
    /* Test unknown code */
    TEST_ASSERT_EQUAL_STRING("Unknown error", dht22_status_to_string(999));

    TEST_PASS_MESSAGE("PASSED");
}

/**
 * @test Test status to string with boundary values
 * @brief Verifies robustness with edge cases
 */
TEST(dht22_utilities, status_to_string_boundary)
{
    /* Test minimum and maximum plausible values */
    TEST_ASSERT_EQUAL_STRING("Unknown error", dht22_status_to_string(-1));
    TEST_ASSERT_EQUAL_STRING("Unknown error", dht22_status_to_string(255));

    TEST_PASS_MESSAGE("PASSED");
}

/* ========================================================================== */
/* Test Group: dht22_edge_cases                                               */
/* ========================================================================== */

TEST_GROUP(dht22_edge_cases);

TEST_SETUP(dht22_edge_cases)
{
    memset(&sensor, 0, sizeof(sensor));
    
    hal_delay_init_Expect();
    hal_gpio_set_mode_input_Expect(DHT22_PIN);
    dht22_init(&sensor, DHT22_PIN);
    
    mock_dht22_hw_Init();
    mock_hal_delay_gpio_Init();
}

TEST_TEAR_DOWN(dht22_edge_cases)
{
    dht22_deinit(&sensor);
    mock_dht22_hw_Verify();
    mock_hal_delay_gpio_Verify();
}

/**
 * @test Test reading with extreme temperature values
 * @brief Verifies handling of sensor range limits
 */
TEST(dht22_edge_cases, extreme_temperature_values)
{
    struct {
        int16_t temp_raw;     /* Temperature * 10 */
        float expected_temp;
        uint16_t hum_raw;     /* Humidity * 10 */
        float expected_hum;
    } test_cases[] = {
        /* DHT22 specifications: -40°C to 80°C, 0% to 100% */
        {-400, -40.0f, 0, 0.0f},      /* Minimum */
        {800, 80.0f, 1000, 100.0f},   /* Maximum */
        {0, 0.0f, 500, 50.0f},        /* Zero temperature */
    };
    
    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++)
    {
        /* Setup for each test case */
        mock_dht22_hw_Init();
        
        uint8_t test_data[5];
        create_test_data(test_data, 
                        test_cases[i].hum_raw, 
                        test_cases[i].temp_raw);
        
        float temperature, humidity;
        
        /* Mock sequence */
        mock_read_sequence(&sensor, test_data, DHT22_OK, true);
        
        /* Execute */
        dht22_status_t status = dht22_read_temperature_humidity(&sensor, 
                                                                &temperature, 
                                                                &humidity);
        
        /* Verify */
        TEST_ASSERT_EQUAL(DHT22_OK, status);
        TEST_ASSERT_FLOAT_WITHIN(0.1f, test_cases[i].expected_temp, temperature);
        TEST_ASSERT_FLOAT_WITHIN(0.1f, test_cases[i].expected_hum, humidity);
        
        mock_dht22_hw_Verify();
    }

    TEST_PASS_MESSAGE("PASSED");
}

/**
 * @test Test repeated initialization
 * @brief Verifies sensor can be reinitialized after deinit
 */
TEST(dht22_edge_cases, reinitialization_cycle)
{
    /* First initialization */
    hal_delay_init_Expect();
    hal_gpio_set_mode_input_Expect(DHT22_PIN);
    dht22_status_t status1 = dht22_init(&sensor, DHT22_PIN);
    TEST_ASSERT_EQUAL(DHT22_OK, status1);
    
    /* Deinitialize */
    dht22_deinit(&sensor);
    TEST_ASSERT_FALSE(sensor.initialized);
    
    /* Reinitialize with different pin */
    gpio_pin_t different_pin = {DHT22_PIN.port, DHT22_PIN.pin + 1};
    
    hal_delay_init_Expect();
    hal_gpio_set_mode_input_Expect(different_pin);
    dht22_status_t status2 = dht22_init(&sensor, different_pin);
    TEST_ASSERT_EQUAL(DHT22_OK, status2);
    TEST_ASSERT_EQUAL(different_pin.pin, sensor.data_pin.pin);

    TEST_PASS_MESSAGE("PASSED");
}

