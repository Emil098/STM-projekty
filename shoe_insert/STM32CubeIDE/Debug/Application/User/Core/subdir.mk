################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/projekty/saint_denis/wkladka_v1.0/Core/Src/freertos.c \
C:/projekty/saint_denis/wkladka_v1.0/Core/Src/main.c \
C:/projekty/saint_denis/wkladka_v1.0/Core/Src/stm32l1xx_hal_msp.c \
C:/projekty/saint_denis/wkladka_v1.0/Core/Src/stm32l1xx_hal_timebase_tim.c \
C:/projekty/saint_denis/wkladka_v1.0/Core/Src/stm32l1xx_it.c \
../Application/User/Core/syscalls.c \
../Application/User/Core/sysmem.c 

OBJS += \
./Application/User/Core/freertos.o \
./Application/User/Core/main.o \
./Application/User/Core/stm32l1xx_hal_msp.o \
./Application/User/Core/stm32l1xx_hal_timebase_tim.o \
./Application/User/Core/stm32l1xx_it.o \
./Application/User/Core/syscalls.o \
./Application/User/Core/sysmem.o 

C_DEPS += \
./Application/User/Core/freertos.d \
./Application/User/Core/main.d \
./Application/User/Core/stm32l1xx_hal_msp.d \
./Application/User/Core/stm32l1xx_hal_timebase_tim.d \
./Application/User/Core/stm32l1xx_it.d \
./Application/User/Core/syscalls.d \
./Application/User/Core/sysmem.d 


# Each subdirectory must supply rules for building sources it contributes
Application/User/Core/freertos.o: C:/projekty/saint_denis/wkladka_v1.0/Core/Src/freertos.c Application/User/Core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L151xBA -c -I../../Core/Inc -I../../Drivers/STM32L1xx_HAL_Driver/Inc -I../../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../../Drivers/CMSIS/Include -I"C:/projekty/saint_denis/wkladka_v1.0/STM32CubeIDE/Drivers/my_libs" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Application/User/Core/main.o: C:/projekty/saint_denis/wkladka_v1.0/Core/Src/main.c Application/User/Core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L151xBA -c -I../../Core/Inc -I../../Drivers/STM32L1xx_HAL_Driver/Inc -I../../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../../Drivers/CMSIS/Include -I"C:/projekty/saint_denis/wkladka_v1.0/STM32CubeIDE/Drivers/my_libs" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Application/User/Core/stm32l1xx_hal_msp.o: C:/projekty/saint_denis/wkladka_v1.0/Core/Src/stm32l1xx_hal_msp.c Application/User/Core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L151xBA -c -I../../Core/Inc -I../../Drivers/STM32L1xx_HAL_Driver/Inc -I../../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../../Drivers/CMSIS/Include -I"C:/projekty/saint_denis/wkladka_v1.0/STM32CubeIDE/Drivers/my_libs" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Application/User/Core/stm32l1xx_hal_timebase_tim.o: C:/projekty/saint_denis/wkladka_v1.0/Core/Src/stm32l1xx_hal_timebase_tim.c Application/User/Core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L151xBA -c -I../../Core/Inc -I../../Drivers/STM32L1xx_HAL_Driver/Inc -I../../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../../Drivers/CMSIS/Include -I"C:/projekty/saint_denis/wkladka_v1.0/STM32CubeIDE/Drivers/my_libs" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Application/User/Core/stm32l1xx_it.o: C:/projekty/saint_denis/wkladka_v1.0/Core/Src/stm32l1xx_it.c Application/User/Core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L151xBA -c -I../../Core/Inc -I../../Drivers/STM32L1xx_HAL_Driver/Inc -I../../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../../Drivers/CMSIS/Include -I"C:/projekty/saint_denis/wkladka_v1.0/STM32CubeIDE/Drivers/my_libs" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Application/User/Core/%.o Application/User/Core/%.su Application/User/Core/%.cyclo: ../Application/User/Core/%.c Application/User/Core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L151xBA -c -I../../Core/Inc -I../../Drivers/STM32L1xx_HAL_Driver/Inc -I../../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../../Drivers/CMSIS/Include -I"C:/projekty/saint_denis/wkladka_v1.0/STM32CubeIDE/Drivers/my_libs" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Application-2f-User-2f-Core

clean-Application-2f-User-2f-Core:
	-$(RM) ./Application/User/Core/freertos.cyclo ./Application/User/Core/freertos.d ./Application/User/Core/freertos.o ./Application/User/Core/freertos.su ./Application/User/Core/main.cyclo ./Application/User/Core/main.d ./Application/User/Core/main.o ./Application/User/Core/main.su ./Application/User/Core/stm32l1xx_hal_msp.cyclo ./Application/User/Core/stm32l1xx_hal_msp.d ./Application/User/Core/stm32l1xx_hal_msp.o ./Application/User/Core/stm32l1xx_hal_msp.su ./Application/User/Core/stm32l1xx_hal_timebase_tim.cyclo ./Application/User/Core/stm32l1xx_hal_timebase_tim.d ./Application/User/Core/stm32l1xx_hal_timebase_tim.o ./Application/User/Core/stm32l1xx_hal_timebase_tim.su ./Application/User/Core/stm32l1xx_it.cyclo ./Application/User/Core/stm32l1xx_it.d ./Application/User/Core/stm32l1xx_it.o ./Application/User/Core/stm32l1xx_it.su ./Application/User/Core/syscalls.cyclo ./Application/User/Core/syscalls.d ./Application/User/Core/syscalls.o ./Application/User/Core/syscalls.su ./Application/User/Core/sysmem.cyclo ./Application/User/Core/sysmem.d ./Application/User/Core/sysmem.o ./Application/User/Core/sysmem.su

.PHONY: clean-Application-2f-User-2f-Core

