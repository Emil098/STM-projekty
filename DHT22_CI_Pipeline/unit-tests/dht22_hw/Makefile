SRC+= unit-tests/dht22_hw/test_dht22_hw.c   
SRC+= unit-tests/dht22_hw/test_runner.c 
SRC+= unit-tests/dht22_hw/test_main.c 

SRC+= STM32CubeIDE/Drivers/libs/dht22_hw.c

INC_DIR+= -ISTM32CubeIDE/Drivers/libs/ 

COVERAGE_SOURCEDIR :=
COVERAGE_SOURCEDIR +=STM32CubeIDE/Drivers/libs/ 

MOCKED_HEADER +=  STM32CubeIDE/Drivers/libs/hal_delay_gpio.h

MOCKED_SOURCE +=  unit-tests/build/cmock/mock_hal_delay_gpio.c

SRC+= $(MOCKED_SOURCE)