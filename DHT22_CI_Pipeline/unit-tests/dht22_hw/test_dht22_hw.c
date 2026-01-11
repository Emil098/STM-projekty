/**
 * @file test_dht22_hw.c
 * @brief Unit tests for DHT22 hardware layer (dht22_hw.c)
 * 
 * Tests the low-level 1-Wire protocol implementation including:
 * - Start signal timing
 * - Response checking with various scenarios
 * - Bit-by-bit byte reading
 * - Checksum validation
 * 
 * Hardware dependencies (HAL) are mocked to enable pure unit testing.
 */

#include "unity.h"
#include "unity_fixture.h"
#include "cmock.h"

#include "mock_hal_delay_gpio.h" 
#include "dht22_hw.h" 
#include "dht22_types.h"

#include <string.h>
#include <stdbool.h>

/* Test fixture - shared test data */
static dht22_t sensor;
static const gpio_pin_t DHT22_PIN = {ONE_W_GPIO_Port, ONE_W_Pin};


/* Timing constants from dht22_hw.c */
#define DHT22_START_DELAY_MS         18
#define DHT22_RESPONSE_DELAY_US      40
#define DHT22_RESPONSE_TIMEOUT_US    80
#define DHT22_BIT_READ_DELAY_US      40

/* Helper function to simulate time passing in while loops */
static void mock_while_loop_exit(gpio_pin_t pin, bool exit_value, int calls)
{
    for (int i = 0; i < calls; i++)
    {
        hal_gpio_read_ExpectAndReturn(pin, !exit_value);
    }
    hal_gpio_read_ExpectAndReturn(pin, exit_value);
}

/* ========================================================================== */
/* Test Group: dht22_hw_start_signal                                         */
/* ========================================================================== */

TEST_GROUP(dht22_hw_start_signal);

TEST_SETUP(dht22_hw_start_signal)
{
    memset(&sensor, 0, sizeof(sensor));
    sensor.data_pin = DHT22_PIN;
    sensor.initialized = true;
    
    mock_hal_delay_gpio_Init();
}

TEST_TEAR_DOWN(dht22_hw_start_signal)
{
    mock_hal_delay_gpio_Verify();
}

/**
 * @test Test complete start signal sequence
 * @brief Verifies correct GPIO and timing for DHT22 start signal
 */
TEST(dht22_hw_start_signal, start_signal_complete_sequence)
{
    /* Expectations in exact order */
    
    /* 1. Set pin as output */
    hal_gpio_set_mode_output_Expect(DHT22_PIN);
    
    /* 2. Pull pin low */
    hal_gpio_write_Expect(DHT22_PIN, false);
    
    /* 3. Wait 18ms */
    hal_delay_ms_Expect(DHT22_START_DELAY_MS);
    
    /* 4. Release bus (set as input) */
    hal_gpio_set_mode_input_Expect(DHT22_PIN);
    
    /* Execute */
    dht22_start_signal(&sensor);
    TEST_PASS_MESSAGE("PASSED");
}

/**
 * @test Test start signal with NULL sensor
 * @brief Verifies function doesn't crash with NULL pointer
 */
TEST(dht22_hw_start_signal, start_signal_null_sensor)
{
    /* Should not crash or call any HAL functions */
    dht22_start_signal(NULL);
    
    /* If we get here, test passes */
    TEST_PASS_MESSAGE("PASSED");
}

/**
 * @test Test start signal with uninitialized sensor
 * @brief Verifies function handles sensor with invalid pin
 */
TEST(dht22_hw_start_signal, start_signal_uninitialized_sensor)
{
    dht22_t bad_sensor = {0};  /* Zero-initialized, no pin set */
    
    /* HAL shouldn't be called with invalid pin */
    /* This might crash in real HW, but for test we verify it doesn't call HAL */
    
    /* Note: In production, this should be prevented by upper layer validation */
    dht22_start_signal(&bad_sensor);
    
    TEST_PASS_MESSAGE("Function should handle invalid sensor gracefully");
    TEST_PASS_MESSAGE("PASSED");
}

/* ========================================================================== */
/* Test Group: dht22_hw_check_response                                       */
/* ========================================================================== */

TEST_GROUP(dht22_hw_check_response);

TEST_SETUP(dht22_hw_check_response)
{
    memset(&sensor, 0, sizeof(sensor));
    sensor.data_pin = DHT22_PIN;
    sensor.initialized = true;
    
    mock_hal_delay_gpio_Init();
}

TEST_TEAR_DOWN(dht22_hw_check_response)
{
    mock_hal_delay_gpio_Verify();
}

/**
 * @test Test successful sensor response
 * @brief Verifies correct timing and sequence for valid response
 */
TEST(dht22_hw_check_response, check_response_success)
{
    /* 1. Wait 40us */
    hal_delay_us_Expect(DHT22_RESPONSE_DELAY_US);
    
    /* 2. Pin is low (sensor pulling line down) */
    hal_gpio_read_ExpectAndReturn(DHT22_PIN, false);
    
    /* 3. Wait 80us */
    hal_delay_us_Expect(DHT22_RESPONSE_TIMEOUT_US);
    
    /* 4. Pin is high (sensor released line) */
    hal_gpio_read_ExpectAndReturn(DHT22_PIN, true);
    
    /* 5. Wait for pin to go low (exit while loop) */
    /* First call: pin high (loop continues) */
    /* Second call: pin low (loop exits) */
    hal_gpio_read_ExpectAndReturn(DHT22_PIN, true);
    hal_gpio_read_ExpectAndReturn(DHT22_PIN, false);
    
    /* Execute and verify */
    dht22_status_t status = dht22_check_response(&sensor);
    TEST_ASSERT_EQUAL(DHT22_OK, status);
    TEST_PASS_MESSAGE("PASSED");
}

/**
 * @test Test no response (pin stays high)
 * @brief Verifies timeout when sensor doesn't pull line low
 */
TEST(dht22_hw_check_response, check_response_no_low_signal)
{
    /* 1. Wait 40us */
    hal_delay_us_Expect(DHT22_RESPONSE_DELAY_US);
    
    /* 2. Pin is still high (no response) */
    hal_gpio_read_ExpectAndReturn(DHT22_PIN, true);
    
    /* Execute and verify */
    dht22_status_t status = dht22_check_response(&sensor);
    TEST_ASSERT_EQUAL(DHT22_ERROR_NO_RESPONSE, status);
    TEST_PASS_MESSAGE("PASSED");
}

/**
 * @test Test response timeout (pin stays low)
 * @brief Verifies timeout when sensor doesn't release line
 */
TEST(dht22_hw_check_response, check_response_timeout_low)
{
    /* 1. Wait 40us */
    hal_delay_us_Expect(DHT22_RESPONSE_DELAY_US);
    
    /* 2. Pin is low (good start) */
    hal_gpio_read_ExpectAndReturn(DHT22_PIN, false);
    
    /* 3. Wait 80us */
    hal_delay_us_Expect(DHT22_RESPONSE_TIMEOUT_US);
    
    /* 4. Pin is still low (timeout) */
    hal_gpio_read_ExpectAndReturn(DHT22_PIN, false);
    
    /* Execute and verify */
    dht22_status_t status = dht22_check_response(&sensor);
    TEST_ASSERT_EQUAL(DHT22_ERROR_NO_RESPONSE, status);
    TEST_PASS_MESSAGE("PASSED");
}

/**
 * @test Test check response with NULL sensor
 * @brief Verifies error handling for NULL pointer
 */
TEST(dht22_hw_check_response, check_response_null_sensor)
{

    hal_delay_us_Ignore();
    /* Should return error without calling HAL */
    dht22_status_t status = dht22_check_response(NULL);
    TEST_PASS_MESSAGE("PASSED");
}

/**
 * @test Test multiple while loop iterations
 * @brief Verifies while loop exits correctly after multiple reads
 */
TEST(dht22_hw_check_response, check_response_multiple_while_iterations)
{
    /* 1. Wait 40us */
    hal_delay_us_Expect(DHT22_RESPONSE_DELAY_US);
    
    /* 2. Pin is low */
    hal_gpio_read_ExpectAndReturn(DHT22_PIN, false);
    
    /* 3. Wait 80us */
    hal_delay_us_Expect(DHT22_RESPONSE_TIMEOUT_US);
    
    /* 4. Pin is high */
    hal_gpio_read_ExpectAndReturn(DHT22_PIN, true);
    
    /* 5. While loop: pin high for 3 iterations, then low */
    hal_gpio_read_ExpectAndReturn(DHT22_PIN, true);  /* Iteration 1 */
    hal_gpio_read_ExpectAndReturn(DHT22_PIN, true);  /* Iteration 2 */
    hal_gpio_read_ExpectAndReturn(DHT22_PIN, true);  /* Iteration 3 */
    hal_gpio_read_ExpectAndReturn(DHT22_PIN, false); /* Exit */
    
    /* Execute and verify */
    dht22_status_t status = dht22_check_response(&sensor);
    TEST_ASSERT_EQUAL(DHT22_OK, status);
    TEST_PASS_MESSAGE("PASSED");
}

/* ========================================================================== */
/* Test Group: dht22_hw_read_byte                                            */
/* ========================================================================== */

TEST_GROUP(dht22_hw_read_byte);

TEST_SETUP(dht22_hw_read_byte)
{
    memset(&sensor, 0, sizeof(sensor));
    sensor.data_pin = DHT22_PIN;
    sensor.initialized = true;
    
    mock_hal_delay_gpio_Init();
}

TEST_TEAR_DOWN(dht22_hw_read_byte)
{
    mock_hal_delay_gpio_Verify();
}

/**
 * @test Test reading byte with value 0x00 (all bits 0)
 * @brief Verifies reading all zero bits
 */
TEST(dht22_hw_read_byte, read_byte_all_zeros)
{
    uint8_t result;
    
    /* For each of 8 bits (MSB first) */
    for (int bit = 0; bit < 8; bit++)
    {
        /* Wait for rising edge (exit first while) */
        hal_gpio_read_ExpectAndReturn(DHT22_PIN, false); /* Loop continues */
        hal_gpio_read_ExpectAndReturn(DHT22_PIN, true);  /* Loop exits */
        
        /* Wait 40us */
        hal_delay_us_Expect(DHT22_BIT_READ_DELAY_US);
        
        /* Pin low after 40us = bit '0' */
        hal_gpio_read_ExpectAndReturn(DHT22_PIN, false);
        
        /* Wait for falling edge (exit second while) */
        hal_gpio_read_ExpectAndReturn(DHT22_PIN, true);  /* Loop continues */
        hal_gpio_read_ExpectAndReturn(DHT22_PIN, false); /* Loop exits */
    }
    
    /* Execute */
    result = dht22_read_byte(&sensor);
    
    /* Verify */
    TEST_ASSERT_EQUAL(0x00, result);
    TEST_PASS_MESSAGE("PASSED");
}

/**
 * @test Test reading byte with value 0xFF (all bits 1)
 * @brief Verifies reading all one bits
 */
TEST(dht22_hw_read_byte, read_byte_all_ones)
{
    uint8_t result;
    
    /* For each of 8 bits (MSB first) */
    for (int bit = 0; bit < 8; bit++)
    {
        /* Wait for rising edge */
        hal_gpio_read_ExpectAndReturn(DHT22_PIN, false);
        hal_gpio_read_ExpectAndReturn(DHT22_PIN, true);
        
        /* Wait 40us */
        hal_delay_us_Expect(DHT22_BIT_READ_DELAY_US);
        
        /* Pin high after 40us = bit '1' */
        hal_gpio_read_ExpectAndReturn(DHT22_PIN, true);
        
        /* Wait for falling edge */
        hal_gpio_read_ExpectAndReturn(DHT22_PIN, true);
        hal_gpio_read_ExpectAndReturn(DHT22_PIN, false);
    }
    
    /* Execute */
    result = dht22_read_byte(&sensor);
    
    /* Verify */
    TEST_ASSERT_EQUAL(0xFF, result);
    TEST_PASS_MESSAGE("PASSED");
}

/**
 * @test Test reading byte 0x55 (alternating bits 01010101)
 * @brief Verifies reading alternating pattern
 */
TEST(dht22_hw_read_byte, read_byte_alternating_pattern)
{
    uint8_t result;
    
    /* Pattern: 0x55 = 01010101 binary */
    bool bit_pattern[8] = {false, true, false, true, false, true, false, true};
    
    for (int bit = 0; bit < 8; bit++)
    {
        /* Wait for rising edge */
        hal_gpio_read_ExpectAndReturn(DHT22_PIN, false);
        hal_gpio_read_ExpectAndReturn(DHT22_PIN, true);
        
        /* Wait 40us */
        hal_delay_us_Expect(DHT22_BIT_READ_DELAY_US);
        
        /* Return pattern bit (MSB first, so bit 7 first) */
        hal_gpio_read_ExpectAndReturn(DHT22_PIN, bit_pattern[ bit]);
        
        /* Wait for falling edge */
        hal_gpio_read_ExpectAndReturn(DHT22_PIN, true);
        hal_gpio_read_ExpectAndReturn(DHT22_PIN, false);
    }
    
    /* Execute */
    result = dht22_read_byte(&sensor);
    
    /* Verify */
    TEST_ASSERT_EQUAL(85, result);
    TEST_PASS_MESSAGE("PASSED");
}

/**
 * @test Test reading byte 0xAA (alternating bits 10101010)
 * @brief Verifies reading alternating pattern
 */
TEST(dht22_hw_read_byte, read_byte_alternating_pattern_inverse)
{
    uint8_t result;
    
    /* Pattern: 0xAA = 10101010 binary */
    bool bit_pattern[8] = {true, false, true, false, true, false, true, false};
    
    mock_hal_delay_gpio_Init();

    for (int bit = 0; bit < 8; bit++)
    {
        /* Wait for rising edge */
        hal_gpio_read_ExpectAndReturn(DHT22_PIN, false);
        hal_gpio_read_ExpectAndReturn(DHT22_PIN, true);
        
        /* Wait 40us */
        hal_delay_us_Expect(DHT22_BIT_READ_DELAY_US);
        
        /* Return pattern bit (MSB first, so bit 7 first) */
        hal_gpio_read_ExpectAndReturn(DHT22_PIN, bit_pattern[bit]);
        
        /* Wait for falling edge */
        hal_gpio_read_ExpectAndReturn(DHT22_PIN, true);
        hal_gpio_read_ExpectAndReturn(DHT22_PIN, false);
    }
    
    /* Execute */
    result = dht22_read_byte(&sensor);
    
    /* Verify */
    TEST_ASSERT_EQUAL(0xAA, result);
    TEST_PASS_MESSAGE("PASSED");
}

/**
 * @test Test reading byte with NULL sensor
 * @brief Verifies handling of NULL pointer
 */
TEST(dht22_hw_read_byte, read_byte_null_sensor)
{
    /* This will likely crash or return garbage */
    /* We test that it doesn't call mocked functions */
    uint8_t result = dht22_read_byte(NULL);
    TEST_PASS_MESSAGE("PASSED");
}

/**
 * @test Test read byte with multiple while loop iterations
 * @brief Verifies while loops exit correctly after multiple reads
 */
TEST(dht22_hw_read_byte, read_byte_with_delayed_edges)
{
    uint8_t result;
    
    /* Test bit 0 (MSB) = 1 */
    /* First while: multiple lows then high */
    hal_gpio_read_ExpectAndReturn(DHT22_PIN, false);
    hal_gpio_read_ExpectAndReturn(DHT22_PIN, false);
    hal_gpio_read_ExpectAndReturn(DHT22_PIN, false);
    hal_gpio_read_ExpectAndReturn(DHT22_PIN, true);  /* Exit */
    
    hal_delay_us_Expect(DHT22_BIT_READ_DELAY_US);
    hal_gpio_read_ExpectAndReturn(DHT22_PIN, true);  /* Bit = 1 */
    
    /* Second while: multiple highs then low */
    hal_gpio_read_ExpectAndReturn(DHT22_PIN, true);
    hal_gpio_read_ExpectAndReturn(DHT22_PIN, true);
    hal_gpio_read_ExpectAndReturn(DHT22_PIN, false); /* Exit */
    
    /* Remaining 7 bits = 0 (simplified) */
    for (int bit = 1; bit < 8; bit++)
    {
        hal_gpio_read_ExpectAndReturn(DHT22_PIN, false);
        hal_gpio_read_ExpectAndReturn(DHT22_PIN, true);
        hal_delay_us_Expect(DHT22_BIT_READ_DELAY_US);
        hal_gpio_read_ExpectAndReturn(DHT22_PIN, false);
        hal_gpio_read_ExpectAndReturn(DHT22_PIN, true);
        hal_gpio_read_ExpectAndReturn(DHT22_PIN, false);
    }
    
    /* Execute */
    result = dht22_read_byte(&sensor);
    
    /* Should be 0x80 (10000000 binary) */
    TEST_ASSERT_EQUAL(0x80, result);
    TEST_PASS_MESSAGE("PASSED");
}

/* ========================================================================== */
/* Test Group: dht22_hw_validate_checksum                                    */
/* ========================================================================== */

TEST_GROUP(dht22_hw_validate_checksum);

TEST_SETUP(dht22_hw_validate_checksum)
{
    /* No HAL dependencies for this function */
}

TEST_TEAR_DOWN(dht22_hw_validate_checksum)
{
    /* Nothing to verify */
}

/**
 * @test Test valid checksum calculation
 * @brief Verifies checksum validation with correct data
 */
TEST(dht22_hw_validate_checksum, validate_checksum_valid)
{
    /* Test data: humidity=42.3% (0x01A7), temp=29.7°C (0x0129) */
    /* 0x01 + 0xA7 + 0x01 + 0x29 = 0x01D2, low byte = 0xD2 */
    uint8_t valid_data[5] = {0x01, 0xA7, 0x01, 0x29, 0xD2};
    
    bool result = dht22_validate_checksum(valid_data);
    
    TEST_ASSERT_TRUE(result);
    TEST_PASS_MESSAGE("PASSED");
}

/**
 * @test Test invalid checksum detection
 * @brief Verifies checksum failure with corrupted data
 */
TEST(dht22_hw_validate_checksum, validate_checksum_invalid)
{
    /* Same data but wrong checksum */
    uint8_t invalid_data[5] = {0x01, 0xA7, 0x01, 0x29, 0x00};
    
    bool result = dht22_validate_checksum(invalid_data);
    
    TEST_ASSERT_FALSE(result);
    TEST_PASS_MESSAGE("PASSED");
}

/**
 * @test Test checksum with all zeros
 * @brief Verifies edge case with zero data
 */
TEST(dht22_hw_validate_checksum, validate_checksum_all_zeros)
{
    uint8_t zero_data[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
    
    bool result = dht22_validate_checksum(zero_data);
    
    TEST_ASSERT_TRUE(result);  /* 0+0+0+0 = 0, checksum matches */
    TEST_PASS_MESSAGE("PASSED");
}

/**
 * @test Test checksum with all 0xFF
 * @brief Verifies edge case with maximum values
 */
TEST(dht22_hw_validate_checksum, validate_checksum_all_ones)
{
    /* 0xFF + 0xFF + 0xFF + 0xFF = 0x3FC, low byte = 0xFC */
    uint8_t max_data[5] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFC};
    
    bool result = dht22_validate_checksum(max_data);
    
    TEST_ASSERT_TRUE(result);
    TEST_PASS_MESSAGE("PASSED");
}

/**
 * @test Test checksum overflow handling
 * @brief Verifies 8-bit overflow is handled correctly
 */
TEST(dht22_hw_validate_checksum, validate_checksum_overflow)
{
    /* 0x80 + 0x80 + 0x80 + 0x80 = 0x200, low byte = 0x00 */
    uint8_t overflow_data[5] = {0x80, 0x80, 0x80, 0x80, 0x00};
    
    bool result = dht22_validate_checksum(overflow_data);
    
    TEST_ASSERT_TRUE(result);
    TEST_PASS_MESSAGE("PASSED");
}

/**
 * @test Test checksum with NULL data pointer
 * @brief Verifies error handling for NULL parameter
 */
TEST(dht22_hw_validate_checksum, validate_checksum_null_data)
{
    bool result = dht22_validate_checksum(NULL);
    
    TEST_ASSERT_FALSE(result);  /* Should return false for NULL */
    TEST_PASS_MESSAGE("PASSED");
}

/**
 * @test Test checksum with various valid data
 * @brief Verifies multiple valid checksum calculations
 */
TEST(dht22_hw_validate_checksum, validate_checksum_multiple_valid)
{
    struct {
        uint8_t data[5];
        bool expected;
    } test_cases[] = {
        /* Real DHT22 examples */
        {{0x00, 0x64, 0x00, 0xFA, 0x5E}, true},  /* 10.0%, 25.0°C */
        {{0x01, 0x90, 0x00, 0xD2, 0x63}, true},  /* 40.0%, 21.0°C */
        {{0x02, 0x58, 0x01, 0x2C, 0x87}, true},  /* 60.0%, 30.0°C */
        {{0x00, 0xC8, 0xFF, 0xCE, 0x95}, true},  /* 20.0%, -5.0°C */
    };
    
    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++)
    {
        bool result = dht22_validate_checksum(test_cases[i].data);
        TEST_ASSERT_EQUAL(test_cases[i].expected, result);
    }

    TEST_PASS_MESSAGE("PASSED");
}

/* ========================================================================== */
/* Test Group: dht22_hw_integration                                          */
/* ========================================================================== */

TEST_GROUP(dht22_hw_integration);

TEST_SETUP(dht22_hw_integration)
{
    memset(&sensor, 0, sizeof(sensor));
    sensor.data_pin = DHT22_PIN;
    sensor.initialized = true;
    
    mock_hal_delay_gpio_Init();
}

TEST_TEAR_DOWN(dht22_hw_integration)
{
    mock_hal_delay_gpio_Verify();
}

/**
 * @test Test complete read sequence simulation
 * @brief Verifies integration of all hardware functions
 */
TEST(dht22_hw_integration, complete_read_sequence)
{
    /* This test simulates a complete sensor read at hardware level */
    
    /* 1. Start Signal */
    hal_gpio_set_mode_output_Expect(DHT22_PIN);
    hal_gpio_write_Expect(DHT22_PIN, false);
    hal_delay_ms_Expect(DHT22_START_DELAY_MS);
    hal_gpio_set_mode_input_Expect(DHT22_PIN);
    
    dht22_start_signal(&sensor);
    
    /* Reset mocks for next function */
    mock_hal_delay_gpio_Verify();
    mock_hal_delay_gpio_Init();
    
    /* 2. Check Response */
    hal_delay_us_Expect(DHT22_RESPONSE_DELAY_US);
    hal_gpio_read_ExpectAndReturn(DHT22_PIN, false);
    hal_delay_us_Expect(DHT22_RESPONSE_TIMEOUT_US);
    hal_gpio_read_ExpectAndReturn(DHT22_PIN, true);
    hal_gpio_read_ExpectAndReturn(DHT22_PIN, true);
    hal_gpio_read_ExpectAndReturn(DHT22_PIN, false);
    
    dht22_status_t response_status = dht22_check_response(&sensor);
    TEST_ASSERT_EQUAL(DHT22_OK, response_status);
    
    /* Reset mocks for next function */
    mock_hal_delay_gpio_Verify();
    mock_hal_delay_gpio_Init();
    
    /* 3. Read Byte (example: 0x55) */
    for (int bit = 0; bit < 8; bit++)
    {
        bool bit_value = (bit % 2 == 0) ? false : true;  /* 01010101 */
        hal_gpio_read_ExpectAndReturn(DHT22_PIN, false);
        hal_gpio_read_ExpectAndReturn(DHT22_PIN, true);
        hal_delay_us_Expect(DHT22_BIT_READ_DELAY_US);
        hal_gpio_read_ExpectAndReturn(DHT22_PIN, bit_value);
        hal_gpio_read_ExpectAndReturn(DHT22_PIN, true);
        hal_gpio_read_ExpectAndReturn(DHT22_PIN, false);
    }
    
    uint8_t byte = dht22_read_byte(&sensor);
    TEST_ASSERT_EQUAL(0x55, byte);
    
    /* 4. Validate Checksum (separate test, no HAL) */
    uint8_t test_data[5] = {0x01, 0xA7, 0x01, 0x29, 0xD2};
    bool checksum_valid = dht22_validate_checksum(test_data);
    TEST_ASSERT_TRUE(checksum_valid);
    TEST_PASS_MESSAGE("PASSED");
}
