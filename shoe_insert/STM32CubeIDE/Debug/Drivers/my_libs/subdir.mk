################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/my_libs/ble.c \
../Drivers/my_libs/drv2605.c \
../Drivers/my_libs/hm10.c \
../Drivers/my_libs/lsm6.c \
../Drivers/my_libs/motor.c \
../Drivers/my_libs/sens.c 

OBJS += \
./Drivers/my_libs/ble.o \
./Drivers/my_libs/drv2605.o \
./Drivers/my_libs/hm10.o \
./Drivers/my_libs/lsm6.o \
./Drivers/my_libs/motor.o \
./Drivers/my_libs/sens.o 

C_DEPS += \
./Drivers/my_libs/ble.d \
./Drivers/my_libs/drv2605.d \
./Drivers/my_libs/hm10.d \
./Drivers/my_libs/lsm6.d \
./Drivers/my_libs/motor.d \
./Drivers/my_libs/sens.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/my_libs/%.o Drivers/my_libs/%.su Drivers/my_libs/%.cyclo: ../Drivers/my_libs/%.c Drivers/my_libs/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L151xBA -c -I../../Core/Inc -I../../Drivers/STM32L1xx_HAL_Driver/Inc -I../../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../../Drivers/CMSIS/Include -I"C:/projekty/saint_denis/wkladka_v1.0/STM32CubeIDE/Drivers/my_libs" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Drivers-2f-my_libs

clean-Drivers-2f-my_libs:
	-$(RM) ./Drivers/my_libs/ble.cyclo ./Drivers/my_libs/ble.d ./Drivers/my_libs/ble.o ./Drivers/my_libs/ble.su ./Drivers/my_libs/drv2605.cyclo ./Drivers/my_libs/drv2605.d ./Drivers/my_libs/drv2605.o ./Drivers/my_libs/drv2605.su ./Drivers/my_libs/hm10.cyclo ./Drivers/my_libs/hm10.d ./Drivers/my_libs/hm10.o ./Drivers/my_libs/hm10.su ./Drivers/my_libs/lsm6.cyclo ./Drivers/my_libs/lsm6.d ./Drivers/my_libs/lsm6.o ./Drivers/my_libs/lsm6.su ./Drivers/my_libs/motor.cyclo ./Drivers/my_libs/motor.d ./Drivers/my_libs/motor.o ./Drivers/my_libs/motor.su ./Drivers/my_libs/sens.cyclo ./Drivers/my_libs/sens.d ./Drivers/my_libs/sens.o ./Drivers/my_libs/sens.su

.PHONY: clean-Drivers-2f-my_libs

