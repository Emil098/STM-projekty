UT_CC = gcc

#DEBUG = -O0 -g
UT_CFLAGS += -std=c99 -Wall -Wextra -DUNITY_FIXTURE_NO_EXTRAS -DUNIT_TESTING
# # These extended flags DO get included before any target build runs
UT_CFLAGS += -Wbad-function-cast
UT_CFLAGS += -Wunused-parameter
UT_CFLAGS += -Wcast-qual
UT_CFLAGS += -Wconversion
UT_CFLAGS += -Wformat=2
UT_CFLAGS += -Wmissing-prototypes
UT_CFLAGS += -Wold-style-definition
UT_CFLAGS += -Wpointer-arith
UT_CFLAGS += -Wshadow
UT_CFLAGS += -Wstrict-overflow=5
UT_CFLAGS += -Wstrict-prototypes
UT_CFLAGS += -Wswitch-default
UT_CFLAGS += -Wundef
UT_CFLAGS += -Wno-error=undef  # Warning only, this should not stop the build
UT_CFLAGS += -Wunreachable-code
UT_CFLAGS += -Wunused
UT_CFLAGS += -fstrict-aliasing
UT_CFLAGS += -fprofile-arcs 
UT_CFLAGS += -ftest-coverage -g -O0 -lgcov

UT_EXE_DIR = unit-tests/build/UTexe
BUILD_DIR = unit-tests/build
COV_DIR = unit-tests/docs_coverage
THIRD_PARTY_DIR = third_party
OBJECTS :=
COVERAGE_SOURCEDIR :=
ALL_MOCKED_HEADER :=
MOCK_OUT := unit-tests/build/cmock

include unit-tests/makefile

EXECUTE_TEST_PROGRAMS = $(TEST_PROGRAM)
OBJECTS_FOLDERS := $(addprefix $(BUILD_DIR)'/'UT_COMPILATION_OUTPUT'/',$(TEST_PROGRAM))



$(UT_EXE_DIR):
	mkdir -p $(UT_EXE_DIR)

$(BUILD_DIR): $(UT_EXE_DIR)
	mkdir -p $(BUILD_DIR)

$(OBJECTS_FOLDERS):
	@mkdir -p $@


define COLLECT_TEST_SOURCES

SRC :=
SRC+= $(THIRD_PARTY_DIR)/Unity/extras/fixture/src/unity_fixture.c 
SRC+= $(THIRD_PARTY_DIR)/Unity/src/unity.c   
SRC+= $(THIRD_PARTY_DIR)/cmock/src/cmock.c  

INC_DIR :=
INC_DIR+= -I$(THIRD_PARTY_DIR)/Unity/extras/fixture/src 
INC_DIR+= -I$(THIRD_PARTY_DIR)/Unity/extras/memory/src 
INC_DIR+= -I$(THIRD_PARTY_DIR)/Unity/src/ 
INC_DIR+= -I$(THIRD_PARTY_DIR)/cmock/src/ 
INC_DIR+= -Iunit-tests/build/cmock


MOCKED_HEADER :=
MOCKED_SOURCE :=  
OBJECTS :=
#
#Include the ("Makefile") testing program on which the EXECUTE file will be created
#
-include unit-tests/$(1)/Makefile

ALL_MOCKED_HEADER += $$(MOCKED_HEADER)


OBJECTS = $$(addprefix $$(BUILD_DIR)/UT_COMPILATION_OUTPUT/$(1)/,$$(notdir $$(SRC:.c=.o)))
vpath %.c $$(sort $$(dir $$(SRC)))
UT_CFLAGS +=$$(INC_DIR)
endef





EXECUTE_COMPILATION :=

define CREATE_EXECTUE_FILES
EXECUTE_COMPILATION += $(UT_EXE_DIR)/$(1)

$(UT_EXE_DIR)/$(1): $(UT_EXE_DIR) $(OBJECTS)
	# @echo
#	@echo $(UT_EXE_DIR) $$@ $(OBJECTS)
	$(UT_CC) $(OBJECTS) -o $$@ --coverage -g -O0 -lgcov
	@echo done

endef





define CREATE_OBJECTS
EXECUTE_COMPILATION += $(1) $(2)
$(1):
$(2): $(1)
	$(UT_CC) -c $$(UT_CFLAGS) -MMD -MP $(1) -o $$@
endef

define FOR_EACH_OBJECTS
$(foreach SRC,$(SRC),$(eval $(call CREATE_OBJECTS,$(SRC),$$(OBJECTS))))
endef

$(foreach test_program,$(TEST_PROGRAM),$(eval $(call COLLECT_TEST_SOURCES,$(test_program))) \
$(eval $(call FOR_EACH_OBJECTS,$(TEST_PROGRAM))) $(eval $(call CREATE_EXECTUE_FILES,$$(test_program))) )






$(EXECUTE_TEST_PROGRAMS):
	@echo
	@echo ----------------------------------------------------------------------------
	@echo ------------------------- $@ -------------------------
	@echo ----------------------------------------------------------------------------
	@$(UT_EXE_DIR)/$@
	@echo -------------------------

Reminder:
	@echo ------------------------- 
#Header it's not source (%.c FILE)
	@echo Reminder - after each change in header files, please use: 'test-ra' '('Dependecies are on @TODO list')'
	@echo -------------------------

clean-test:
	@echo Czyszczenie
	-rm -fR $(BUILD_DIR)

clean-docs:
	-rm -fR $(COV_DIR)

clean-third-party:
	-rm -fR $(THIRD_PARTY_DIR)

test-ra: clean-test createmock $(OBJECTS_FOLDERS) $(EXECUTE_COMPILATION) $(EXECUTE_TEST_PROGRAMS) coverage

test-dra: clean-third-party test-ra

test-s: $(EXECUTE_COMPILATION) $(EXECUTE_TEST_PROGRAMS) Reminder

test: test-ra

test-build: clean-test createmock $(OBJECTS_FOLDERS) $(EXECUTE_COMPILATION)

test-run: $(EXECUTE_TEST_PROGRAMS)

$(COV_DIR):
	@mkdir -p $@

coverage: $(COV_DIR)
	@echo Generate HTML coverage report
	gcovr  -f $(COVERAGE_SOURCEDIR) --object-directory=$(BUILD_DIR) --html --html-details -o $(COV_DIR)/coverage.html



CMOCK_DIR = ./$(THIRD_PARTY_DIR)/cmock

$(MOCK_OUT):
	@mkdir -p $@

.PHONY: createmock
createmock: $(MOCK_OUT)
	@set -e; \
	for h in $(sort $(ALL_MOCKED_HEADER)); do \
		echo "[CMOCK] $$h"; \
		CMOCK_DIR=$(CMOCK_DIR) ruby unit-tests/create_mock.rb $$h; \
	done




