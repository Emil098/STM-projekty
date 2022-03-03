################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
D:/Moje\ dokumenty/SDC\ 48/SDC_48/SW4STM32/startup_stm32f030x6.s 

OBJS += \
./Application/SW4STM32/startup_stm32f030x6.o 


# Each subdirectory must supply rules for building sources it contributes
Application/SW4STM32/startup_stm32f030x6.o: D:/Moje\ dokumenty/SDC\ 48/SDC_48/SW4STM32/startup_stm32f030x6.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Assembler'
	@echo $(PWD)
	arm-none-eabi-as -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -g -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


