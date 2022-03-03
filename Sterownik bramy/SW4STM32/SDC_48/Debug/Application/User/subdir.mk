################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/Moje\ dokumenty/SDC\ 48/SDC_48/Src/main.c \
D:/Moje\ dokumenty/SDC\ 48/SDC_48/Src/stm32f0xx_hal_msp.c \
D:/Moje\ dokumenty/SDC\ 48/SDC_48/Src/stm32f0xx_it.c 

OBJS += \
./Application/User/main.o \
./Application/User/stm32f0xx_hal_msp.o \
./Application/User/stm32f0xx_it.o 

C_DEPS += \
./Application/User/main.d \
./Application/User/stm32f0xx_hal_msp.d \
./Application/User/stm32f0xx_it.d 


# Each subdirectory must supply rules for building sources it contributes
Application/User/main.o: D:/Moje\ dokumenty/SDC\ 48/SDC_48/Src/main.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -mfloat-abi=soft '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F030x6 -I"D:/Moje dokumenty/SDC 48/SDC_48/Inc" -I"D:/Moje dokumenty/SDC 48/SDC_48/Drivers/STM32F0xx_HAL_Driver/Inc" -I"D:/Moje dokumenty/SDC 48/SDC_48/Drivers/STM32F0xx_HAL_Driver/Inc/Legacy" -I"D:/Moje dokumenty/SDC 48/SDC_48/Drivers/CMSIS/Device/ST/STM32F0xx/Include" -I"D:/Moje dokumenty/SDC 48/SDC_48/Drivers/CMSIS/Include" -I"D:/Moje dokumenty/SDC 48/SDC_48/Inc"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"Application/User/main.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/User/stm32f0xx_hal_msp.o: D:/Moje\ dokumenty/SDC\ 48/SDC_48/Src/stm32f0xx_hal_msp.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -mfloat-abi=soft '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F030x6 -I"D:/Moje dokumenty/SDC 48/SDC_48/Inc" -I"D:/Moje dokumenty/SDC 48/SDC_48/Drivers/STM32F0xx_HAL_Driver/Inc" -I"D:/Moje dokumenty/SDC 48/SDC_48/Drivers/STM32F0xx_HAL_Driver/Inc/Legacy" -I"D:/Moje dokumenty/SDC 48/SDC_48/Drivers/CMSIS/Device/ST/STM32F0xx/Include" -I"D:/Moje dokumenty/SDC 48/SDC_48/Drivers/CMSIS/Include" -I"D:/Moje dokumenty/SDC 48/SDC_48/Inc"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"Application/User/stm32f0xx_hal_msp.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/User/stm32f0xx_it.o: D:/Moje\ dokumenty/SDC\ 48/SDC_48/Src/stm32f0xx_it.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -mfloat-abi=soft '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F030x6 -I"D:/Moje dokumenty/SDC 48/SDC_48/Inc" -I"D:/Moje dokumenty/SDC 48/SDC_48/Drivers/STM32F0xx_HAL_Driver/Inc" -I"D:/Moje dokumenty/SDC 48/SDC_48/Drivers/STM32F0xx_HAL_Driver/Inc/Legacy" -I"D:/Moje dokumenty/SDC 48/SDC_48/Drivers/CMSIS/Device/ST/STM32F0xx/Include" -I"D:/Moje dokumenty/SDC 48/SDC_48/Drivers/CMSIS/Include" -I"D:/Moje dokumenty/SDC 48/SDC_48/Inc"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"Application/User/stm32f0xx_it.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


