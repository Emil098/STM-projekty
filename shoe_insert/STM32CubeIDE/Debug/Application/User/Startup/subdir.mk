################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Application/User/Startup/startup_stm32l151c8txa.s 

OBJS += \
./Application/User/Startup/startup_stm32l151c8txa.o 

S_DEPS += \
./Application/User/Startup/startup_stm32l151c8txa.d 


# Each subdirectory must supply rules for building sources it contributes
Application/User/Startup/%.o: ../Application/User/Startup/%.s Application/User/Startup/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m3 -g3 -DDEBUG -c -I"C:/projekty/saint_denis/wkladka_v1.0/STM32CubeIDE/Drivers/my_libs" -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@" "$<"

clean: clean-Application-2f-User-2f-Startup

clean-Application-2f-User-2f-Startup:
	-$(RM) ./Application/User/Startup/startup_stm32l151c8txa.d ./Application/User/Startup/startup_stm32l151c8txa.o

.PHONY: clean-Application-2f-User-2f-Startup

