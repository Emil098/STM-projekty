SRC+= unit-tests/debug_uart/test_debug_uart.c   
SRC+= unit-tests/debug_uart/test_runner.c 
SRC+= unit-tests/debug_uart/test_main.c 

SRC+= STM32CubeIDE/Drivers/libs/debug_uart.c

INC_DIR+= -ISTM32CubeIDE/Drivers/libs/ 

COVERAGE_SOURCEDIR :=
COVERAGE_SOURCEDIR +=STM32CubeIDE/Drivers/libs/ 

MOCKED_HEADER +=  STM32CubeIDE/Drivers/libs/fakeUart.h

MOCKED_SOURCE +=  unit-tests/build/cmock/mock_fakeUart.c

SRC+= $(MOCKED_SOURCE)