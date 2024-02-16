################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../private_libs/bme68x.c \
../private_libs/bme_handle.c \
../private_libs/bme_spi.c \
../private_libs/gui_handle.c 

C_DEPS += \
./private_libs/bme68x.d \
./private_libs/bme_handle.d \
./private_libs/bme_spi.d \
./private_libs/gui_handle.d 

OBJS += \
./private_libs/bme68x.o \
./private_libs/bme_handle.o \
./private_libs/bme_spi.o \
./private_libs/gui_handle.o 


# Each subdirectory must supply rules for building sources it contributes
private_libs/%.o private_libs/%.su private_libs/%.cyclo: ../private_libs/%.c private_libs/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DUSE_BPP=16 -DSTM32F746xx -DDEBUG -c -I../../Core/Inc -I../../TouchGFX/App -I../../TouchGFX/target/generated -I../../TouchGFX/target -I../../Drivers/STM32F7xx_HAL_Driver/Inc -I../../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1 -I../../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../../Drivers/CMSIS/Include -I../../Middlewares/ST/touchgfx/framework/include -I../../TouchGFX/generated/fonts/include -I../../TouchGFX/generated/gui_generated/include -I../../TouchGFX/generated/images/include -I../../TouchGFX/generated/texts/include -I../../TouchGFX/gui/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../Drivers/BSP/STM32746G-Discovery -I../../Drivers/BSP/Components/ft5336 -I../../LIBJPEG/App -I../../LIBJPEG/Target -I../../Middlewares/Third_Party/LibJPEG/include -I../../TouchGFX/generated/videos/include -I"C:/projekty/prywatne/STM-projekty/GUI_sensors_light/STM32CubeIDE/private_libs" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-private_libs

clean-private_libs:
	-$(RM) ./private_libs/bme68x.cyclo ./private_libs/bme68x.d ./private_libs/bme68x.o ./private_libs/bme68x.su ./private_libs/bme_handle.cyclo ./private_libs/bme_handle.d ./private_libs/bme_handle.o ./private_libs/bme_handle.su ./private_libs/bme_spi.cyclo ./private_libs/bme_spi.d ./private_libs/bme_spi.o ./private_libs/bme_spi.su ./private_libs/gui_handle.cyclo ./private_libs/gui_handle.d ./private_libs/gui_handle.o ./private_libs/gui_handle.su

.PHONY: clean-private_libs

