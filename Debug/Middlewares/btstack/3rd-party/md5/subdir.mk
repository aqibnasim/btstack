################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/btstack/3rd-party/md5/md5.c 

OBJS += \
./Middlewares/btstack/3rd-party/md5/md5.o 

C_DEPS += \
./Middlewares/btstack/3rd-party/md5/md5.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/btstack/3rd-party/md5/md5.o: ../Middlewares/btstack/3rd-party/md5/md5.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DPWRUI_LOAD_FROM_RAM -DMEM_VERIFY -DSTM32F411xE -DDEBUG -DAUD_LOAD_DSPCFG -DAUD_LOAD_ADJ -DPWRUI_LCNTRL -DUSE_HAL_DRIVER -DAUDIO_DEBUG -DAUD_LOAD_CFG -c -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../RTOS -I../sub -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Include -I../Core/Inc -I../peripherals -I../Middlewares/btstack/3rd-party/bluedroid/encoder/include -I../Middlewares/btstack/3rd-party/bluedroid/decoder/include -I../Middlewares/btstack/3rd-party/micro-ecc -I../Middlewares/btstack/chipset/cc256x -I../Middlewares/btstack/platform/embedded -I../Middlewares/btstack/tool/metrics -I../Middlewares/btstack/src -I../Middlewares/btstack/3rd-party/yxml -I../Middlewares/btstack/3rd-party/md5 -I../Core/port -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/btstack/3rd-party/md5/md5.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

