SRC+= unit-tests/consoleToUT/test_console.c   
SRC+= unit-tests/consoleToUT/test_runner.c 
SRC+= unit-tests/consoleToUT/test_main.c 

SRC+= Src/console.c 

INC_DIR+= -IInc/

COVERAGE_SOURCEDIR :=
COVERAGE_SOURCEDIR +=Src/

MOCKED_HEADER +=  Inc/fakeUart.h
MOCKED_HEADER +=  Inc/debugUart.h
MOCKED_HEADER +=  Inc/debugPrintf.h

MOCKED_SOURCE +=  build/UnitTests/cmock/mock_fakeUart.c
MOCKED_SOURCE +=  build/UnitTests/cmock/mock_debugUart.c
MOCKED_SOURCE +=  build/UnitTests/cmock/mock_debugPrintf.c

SRC+= $(MOCKED_SOURCE)
