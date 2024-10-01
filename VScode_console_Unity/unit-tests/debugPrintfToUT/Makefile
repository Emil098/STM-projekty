SRC+= unit-tests/debugPrintfToUT/test_debugPrintf.c   
SRC+= unit-tests/debugPrintfToUT/test_runner.c 
SRC+= unit-tests/debugPrintfToUT/test_main.c 

SRC+= Src/debugPrintf.c

INC_DIR+= -IInc/ 

COVERAGE_SOURCEDIR :=
COVERAGE_SOURCEDIR +=Src/

MOCKED_HEADER +=  Inc/debugPrintf.h
MOCKED_HEADER +=  Inc/debugUart.h

MOCKED_SOURCE +=  build/UnitTests/cmock/mock_debugUart.c

SRC+= $(MOCKED_SOURCE)
