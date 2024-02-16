################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/projekty/saint_denis/wkladka_v1.0/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.c \
C:/projekty/saint_denis/wkladka_v1.0/Middlewares/Third_Party/FreeRTOS/Source/croutine.c \
C:/projekty/saint_denis/wkladka_v1.0/Middlewares/Third_Party/FreeRTOS/Source/event_groups.c \
C:/projekty/saint_denis/wkladka_v1.0/Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c \
C:/projekty/saint_denis/wkladka_v1.0/Middlewares/Third_Party/FreeRTOS/Source/list.c \
C:/projekty/saint_denis/wkladka_v1.0/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3/port.c \
C:/projekty/saint_denis/wkladka_v1.0/Middlewares/Third_Party/FreeRTOS/Source/queue.c \
C:/projekty/saint_denis/wkladka_v1.0/Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c \
C:/projekty/saint_denis/wkladka_v1.0/Middlewares/Third_Party/FreeRTOS/Source/tasks.c \
C:/projekty/saint_denis/wkladka_v1.0/Middlewares/Third_Party/FreeRTOS/Source/timers.c 

OBJS += \
./Middlewares/FreeRTOS/cmsis_os.o \
./Middlewares/FreeRTOS/croutine.o \
./Middlewares/FreeRTOS/event_groups.o \
./Middlewares/FreeRTOS/heap_4.o \
./Middlewares/FreeRTOS/list.o \
./Middlewares/FreeRTOS/port.o \
./Middlewares/FreeRTOS/queue.o \
./Middlewares/FreeRTOS/stream_buffer.o \
./Middlewares/FreeRTOS/tasks.o \
./Middlewares/FreeRTOS/timers.o 

C_DEPS += \
./Middlewares/FreeRTOS/cmsis_os.d \
./Middlewares/FreeRTOS/croutine.d \
./Middlewares/FreeRTOS/event_groups.d \
./Middlewares/FreeRTOS/heap_4.d \
./Middlewares/FreeRTOS/list.d \
./Middlewares/FreeRTOS/port.d \
./Middlewares/FreeRTOS/queue.d \
./Middlewares/FreeRTOS/stream_buffer.d \
./Middlewares/FreeRTOS/tasks.d \
./Middlewares/FreeRTOS/timers.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/FreeRTOS/cmsis_os.o: C:/projekty/saint_denis/wkladka_v1.0/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.c Middlewares/FreeRTOS/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L151xBA -c -I../../Core/Inc -I../../Drivers/STM32L1xx_HAL_Driver/Inc -I../../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../../Drivers/CMSIS/Include -I"C:/projekty/saint_denis/wkladka_v1.0/STM32CubeIDE/Drivers/my_libs" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Middlewares/FreeRTOS/croutine.o: C:/projekty/saint_denis/wkladka_v1.0/Middlewares/Third_Party/FreeRTOS/Source/croutine.c Middlewares/FreeRTOS/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L151xBA -c -I../../Core/Inc -I../../Drivers/STM32L1xx_HAL_Driver/Inc -I../../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../../Drivers/CMSIS/Include -I"C:/projekty/saint_denis/wkladka_v1.0/STM32CubeIDE/Drivers/my_libs" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Middlewares/FreeRTOS/event_groups.o: C:/projekty/saint_denis/wkladka_v1.0/Middlewares/Third_Party/FreeRTOS/Source/event_groups.c Middlewares/FreeRTOS/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L151xBA -c -I../../Core/Inc -I../../Drivers/STM32L1xx_HAL_Driver/Inc -I../../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../../Drivers/CMSIS/Include -I"C:/projekty/saint_denis/wkladka_v1.0/STM32CubeIDE/Drivers/my_libs" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Middlewares/FreeRTOS/heap_4.o: C:/projekty/saint_denis/wkladka_v1.0/Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c Middlewares/FreeRTOS/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L151xBA -c -I../../Core/Inc -I../../Drivers/STM32L1xx_HAL_Driver/Inc -I../../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../../Drivers/CMSIS/Include -I"C:/projekty/saint_denis/wkladka_v1.0/STM32CubeIDE/Drivers/my_libs" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Middlewares/FreeRTOS/list.o: C:/projekty/saint_denis/wkladka_v1.0/Middlewares/Third_Party/FreeRTOS/Source/list.c Middlewares/FreeRTOS/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L151xBA -c -I../../Core/Inc -I../../Drivers/STM32L1xx_HAL_Driver/Inc -I../../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../../Drivers/CMSIS/Include -I"C:/projekty/saint_denis/wkladka_v1.0/STM32CubeIDE/Drivers/my_libs" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Middlewares/FreeRTOS/port.o: C:/projekty/saint_denis/wkladka_v1.0/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3/port.c Middlewares/FreeRTOS/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L151xBA -c -I../../Core/Inc -I../../Drivers/STM32L1xx_HAL_Driver/Inc -I../../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../../Drivers/CMSIS/Include -I"C:/projekty/saint_denis/wkladka_v1.0/STM32CubeIDE/Drivers/my_libs" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Middlewares/FreeRTOS/queue.o: C:/projekty/saint_denis/wkladka_v1.0/Middlewares/Third_Party/FreeRTOS/Source/queue.c Middlewares/FreeRTOS/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L151xBA -c -I../../Core/Inc -I../../Drivers/STM32L1xx_HAL_Driver/Inc -I../../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../../Drivers/CMSIS/Include -I"C:/projekty/saint_denis/wkladka_v1.0/STM32CubeIDE/Drivers/my_libs" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Middlewares/FreeRTOS/stream_buffer.o: C:/projekty/saint_denis/wkladka_v1.0/Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c Middlewares/FreeRTOS/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L151xBA -c -I../../Core/Inc -I../../Drivers/STM32L1xx_HAL_Driver/Inc -I../../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../../Drivers/CMSIS/Include -I"C:/projekty/saint_denis/wkladka_v1.0/STM32CubeIDE/Drivers/my_libs" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Middlewares/FreeRTOS/tasks.o: C:/projekty/saint_denis/wkladka_v1.0/Middlewares/Third_Party/FreeRTOS/Source/tasks.c Middlewares/FreeRTOS/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L151xBA -c -I../../Core/Inc -I../../Drivers/STM32L1xx_HAL_Driver/Inc -I../../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../../Drivers/CMSIS/Include -I"C:/projekty/saint_denis/wkladka_v1.0/STM32CubeIDE/Drivers/my_libs" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Middlewares/FreeRTOS/timers.o: C:/projekty/saint_denis/wkladka_v1.0/Middlewares/Third_Party/FreeRTOS/Source/timers.c Middlewares/FreeRTOS/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L151xBA -c -I../../Core/Inc -I../../Drivers/STM32L1xx_HAL_Driver/Inc -I../../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../../Drivers/CMSIS/Include -I"C:/projekty/saint_denis/wkladka_v1.0/STM32CubeIDE/Drivers/my_libs" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Middlewares-2f-FreeRTOS

clean-Middlewares-2f-FreeRTOS:
	-$(RM) ./Middlewares/FreeRTOS/cmsis_os.cyclo ./Middlewares/FreeRTOS/cmsis_os.d ./Middlewares/FreeRTOS/cmsis_os.o ./Middlewares/FreeRTOS/cmsis_os.su ./Middlewares/FreeRTOS/croutine.cyclo ./Middlewares/FreeRTOS/croutine.d ./Middlewares/FreeRTOS/croutine.o ./Middlewares/FreeRTOS/croutine.su ./Middlewares/FreeRTOS/event_groups.cyclo ./Middlewares/FreeRTOS/event_groups.d ./Middlewares/FreeRTOS/event_groups.o ./Middlewares/FreeRTOS/event_groups.su ./Middlewares/FreeRTOS/heap_4.cyclo ./Middlewares/FreeRTOS/heap_4.d ./Middlewares/FreeRTOS/heap_4.o ./Middlewares/FreeRTOS/heap_4.su ./Middlewares/FreeRTOS/list.cyclo ./Middlewares/FreeRTOS/list.d ./Middlewares/FreeRTOS/list.o ./Middlewares/FreeRTOS/list.su ./Middlewares/FreeRTOS/port.cyclo ./Middlewares/FreeRTOS/port.d ./Middlewares/FreeRTOS/port.o ./Middlewares/FreeRTOS/port.su ./Middlewares/FreeRTOS/queue.cyclo ./Middlewares/FreeRTOS/queue.d ./Middlewares/FreeRTOS/queue.o ./Middlewares/FreeRTOS/queue.su ./Middlewares/FreeRTOS/stream_buffer.cyclo ./Middlewares/FreeRTOS/stream_buffer.d ./Middlewares/FreeRTOS/stream_buffer.o ./Middlewares/FreeRTOS/stream_buffer.su ./Middlewares/FreeRTOS/tasks.cyclo ./Middlewares/FreeRTOS/tasks.d ./Middlewares/FreeRTOS/tasks.o ./Middlewares/FreeRTOS/tasks.su ./Middlewares/FreeRTOS/timers.cyclo ./Middlewares/FreeRTOS/timers.d ./Middlewares/FreeRTOS/timers.o ./Middlewares/FreeRTOS/timers.su

.PHONY: clean-Middlewares-2f-FreeRTOS

