SRC+= unit-tests/debugUartToUT/test_debugUart.c   
SRC+= unit-tests/debugUartToUT/test_runner.c 
SRC+= unit-tests/debugUartToUT/test_main.c 

SRC+= Src/debugUart.c 

INC_DIR+= -IInc/

COVERAGE_SOURCEDIR :=
COVERAGE_SOURCEDIR +=Src/

MOCKED_HEADER +=  Inc/debugUart.h
MOCKED_HEADER +=  Inc/fakeUart.h

MOCKED_SOURCE +=  build/UnitTests/cmock/mock_fakeUart.c

SRC+= $(MOCKED_SOURCE)
