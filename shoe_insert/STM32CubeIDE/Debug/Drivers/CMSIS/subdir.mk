################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/projekty/saint_denis/wkladka_v1.0/Core/Src/system_stm32l1xx.c 

OBJS += \
./Drivers/CMSIS/system_stm32l1xx.o 

C_DEPS += \
./Drivers/CMSIS/system_stm32l1xx.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/CMSIS/system_stm32l1xx.o: C:/projekty/saint_denis/wkladka_v1.0/Core/Src/system_stm32l1xx.c Drivers/CMSIS/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L151xBA -c -I../../Core/Inc -I../../Drivers/STM32L1xx_HAL_Driver/Inc -I../../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../../Drivers/CMSIS/Include -I"C:/projekty/saint_denis/wkladka_v1.0/STM32CubeIDE/Drivers/my_libs" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Drivers-2f-CMSIS

clean-Drivers-2f-CMSIS:
	-$(RM) ./Drivers/CMSIS/system_stm32l1xx.cyclo ./Drivers/CMSIS/system_stm32l1xx.d ./Drivers/CMSIS/system_stm32l1xx.o ./Drivers/CMSIS/system_stm32l1xx.su

.PHONY: clean-Drivers-2f-CMSIS

