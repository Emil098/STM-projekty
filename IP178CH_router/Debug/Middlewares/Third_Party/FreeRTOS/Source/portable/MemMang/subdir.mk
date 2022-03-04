################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c 

OBJS += \
./Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.o 

C_DEPS += \
./Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/%.o: ../Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -D_TIMEVAL_DEFINED -D_SYS_TIME_H_ '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F107xC -I"D:/Moje dokumenty/IP178CH_router/Inc" -I"D:/Moje dokumenty/IP178CH_router/Middlewares/Third_Party/LwIP/src/include" -I"D:/Moje dokumenty/IP178CH_router/Middlewares/Third_Party/LwIP/system" -I"D:/Moje dokumenty/IP178CH_router/Drivers/STM32F1xx_HAL_Driver/Inc" -I"D:/Moje dokumenty/IP178CH_router/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy" -I"D:/Moje dokumenty/IP178CH_router/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3" -I"D:/Moje dokumenty/IP178CH_router/Middlewares/Third_Party/LwIP/src/include/netif/ppp" -I"D:/Moje dokumenty/IP178CH_router/Drivers/CMSIS/Device/ST/STM32F1xx/Include" -I"D:/Moje dokumenty/IP178CH_router/Middlewares/Third_Party/FreeRTOS/Source/include" -I"D:/Moje dokumenty/IP178CH_router/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"D:/Moje dokumenty/IP178CH_router/Middlewares/Third_Party/LwIP/src/include/lwip" -I"D:/Moje dokumenty/IP178CH_router/Middlewares/Third_Party/LwIP/src/include/lwip/apps" -I"D:/Moje dokumenty/IP178CH_router/Middlewares/Third_Party/LwIP/src/include/lwip/priv" -I"D:/Moje dokumenty/IP178CH_router/Middlewares/Third_Party/LwIP/src/include/lwip/prot" -I"D:/Moje dokumenty/IP178CH_router/Middlewares/Third_Party/LwIP/src/include/netif" -I"D:/Moje dokumenty/IP178CH_router/Middlewares/Third_Party/LwIP/src/include/posix" -I"D:/Moje dokumenty/IP178CH_router/Middlewares/Third_Party/LwIP/src/include/posix/sys" -I"D:/Moje dokumenty/IP178CH_router/Middlewares/Third_Party/LwIP/system/arch" -I"D:/Moje dokumenty/IP178CH_router/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


