################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT.c \
../Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT_Syscalls_GCC.c \
../Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT_Syscalls_IAR.c \
../Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT_Syscalls_KEIL.c \
../Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT_Syscalls_SES.c \
../Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT_printf.c 

S_UPPER_SRCS += \
../Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT_ASM_ARMv7M.S 

OBJS += \
./Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT.o \
./Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT_ASM_ARMv7M.o \
./Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT_Syscalls_GCC.o \
./Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT_Syscalls_IAR.o \
./Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT_Syscalls_KEIL.o \
./Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT_Syscalls_SES.o \
./Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT_printf.o 

S_UPPER_DEPS += \
./Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT_ASM_ARMv7M.d 

C_DEPS += \
./Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT.d \
./Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT_Syscalls_GCC.d \
./Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT_Syscalls_IAR.d \
./Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT_Syscalls_KEIL.d \
./Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT_Syscalls_SES.d \
./Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT_printf.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT.o: ../Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DPWRUI_LOAD_FROM_RAM -DMEM_VERIFY -DSTM32F411xE -DDEBUG -DAUD_LOAD_DSPCFG -DAUD_LOAD_ADJ -DPWRUI_LCNTRL -DUSE_HAL_DRIVER -DAUDIO_DEBUG -DAUD_LOAD_CFG -c -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../RTOS -I../sub -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Include -I../Core/Inc -I../peripherals -I../Middlewares/btstack/3rd-party/bluedroid/encoder/include -I../Middlewares/btstack/3rd-party/bluedroid/decoder/include -I../Middlewares/btstack/3rd-party/micro-ecc -I../Middlewares/btstack/chipset/cc256x -I../Middlewares/btstack/platform/embedded -I../Middlewares/btstack/tool/metrics -I../Middlewares/btstack/src -I../Middlewares/btstack/3rd-party/yxml -I../Middlewares/btstack/3rd-party/md5 -I../Core/port -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT_ASM_ARMv7M.o: ../Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT_ASM_ARMv7M.S
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -c -x assembler-with-cpp -MMD -MP -MF"Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT_ASM_ARMv7M.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"
Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT_Syscalls_GCC.o: ../Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT_Syscalls_GCC.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DPWRUI_LOAD_FROM_RAM -DMEM_VERIFY -DSTM32F411xE -DDEBUG -DAUD_LOAD_DSPCFG -DAUD_LOAD_ADJ -DPWRUI_LCNTRL -DUSE_HAL_DRIVER -DAUDIO_DEBUG -DAUD_LOAD_CFG -c -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../RTOS -I../sub -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Include -I../Core/Inc -I../peripherals -I../Middlewares/btstack/3rd-party/bluedroid/encoder/include -I../Middlewares/btstack/3rd-party/bluedroid/decoder/include -I../Middlewares/btstack/3rd-party/micro-ecc -I../Middlewares/btstack/chipset/cc256x -I../Middlewares/btstack/platform/embedded -I../Middlewares/btstack/tool/metrics -I../Middlewares/btstack/src -I../Middlewares/btstack/3rd-party/yxml -I../Middlewares/btstack/3rd-party/md5 -I../Core/port -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT_Syscalls_GCC.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT_Syscalls_IAR.o: ../Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT_Syscalls_IAR.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DPWRUI_LOAD_FROM_RAM -DMEM_VERIFY -DSTM32F411xE -DDEBUG -DAUD_LOAD_DSPCFG -DAUD_LOAD_ADJ -DPWRUI_LCNTRL -DUSE_HAL_DRIVER -DAUDIO_DEBUG -DAUD_LOAD_CFG -c -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../RTOS -I../sub -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Include -I../Core/Inc -I../peripherals -I../Middlewares/btstack/3rd-party/bluedroid/encoder/include -I../Middlewares/btstack/3rd-party/bluedroid/decoder/include -I../Middlewares/btstack/3rd-party/micro-ecc -I../Middlewares/btstack/chipset/cc256x -I../Middlewares/btstack/platform/embedded -I../Middlewares/btstack/tool/metrics -I../Middlewares/btstack/src -I../Middlewares/btstack/3rd-party/yxml -I../Middlewares/btstack/3rd-party/md5 -I../Core/port -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT_Syscalls_IAR.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT_Syscalls_KEIL.o: ../Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT_Syscalls_KEIL.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DPWRUI_LOAD_FROM_RAM -DMEM_VERIFY -DSTM32F411xE -DDEBUG -DAUD_LOAD_DSPCFG -DAUD_LOAD_ADJ -DPWRUI_LCNTRL -DUSE_HAL_DRIVER -DAUDIO_DEBUG -DAUD_LOAD_CFG -c -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../RTOS -I../sub -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Include -I../Core/Inc -I../peripherals -I../Middlewares/btstack/3rd-party/bluedroid/encoder/include -I../Middlewares/btstack/3rd-party/bluedroid/decoder/include -I../Middlewares/btstack/3rd-party/micro-ecc -I../Middlewares/btstack/chipset/cc256x -I../Middlewares/btstack/platform/embedded -I../Middlewares/btstack/tool/metrics -I../Middlewares/btstack/src -I../Middlewares/btstack/3rd-party/yxml -I../Middlewares/btstack/3rd-party/md5 -I../Core/port -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT_Syscalls_KEIL.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT_Syscalls_SES.o: ../Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT_Syscalls_SES.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DPWRUI_LOAD_FROM_RAM -DMEM_VERIFY -DSTM32F411xE -DDEBUG -DAUD_LOAD_DSPCFG -DAUD_LOAD_ADJ -DPWRUI_LCNTRL -DUSE_HAL_DRIVER -DAUDIO_DEBUG -DAUD_LOAD_CFG -c -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../RTOS -I../sub -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Include -I../Core/Inc -I../peripherals -I../Middlewares/btstack/3rd-party/bluedroid/encoder/include -I../Middlewares/btstack/3rd-party/bluedroid/decoder/include -I../Middlewares/btstack/3rd-party/micro-ecc -I../Middlewares/btstack/chipset/cc256x -I../Middlewares/btstack/platform/embedded -I../Middlewares/btstack/tool/metrics -I../Middlewares/btstack/src -I../Middlewares/btstack/3rd-party/yxml -I../Middlewares/btstack/3rd-party/md5 -I../Core/port -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT_Syscalls_SES.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT_printf.o: ../Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT_printf.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DPWRUI_LOAD_FROM_RAM -DMEM_VERIFY -DSTM32F411xE -DDEBUG -DAUD_LOAD_DSPCFG -DAUD_LOAD_ADJ -DPWRUI_LCNTRL -DUSE_HAL_DRIVER -DAUDIO_DEBUG -DAUD_LOAD_CFG -c -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../RTOS -I../sub -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Include -I../Core/Inc -I../peripherals -I../Middlewares/btstack/3rd-party/bluedroid/encoder/include -I../Middlewares/btstack/3rd-party/bluedroid/decoder/include -I../Middlewares/btstack/3rd-party/micro-ecc -I../Middlewares/btstack/chipset/cc256x -I../Middlewares/btstack/platform/embedded -I../Middlewares/btstack/tool/metrics -I../Middlewares/btstack/src -I../Middlewares/btstack/3rd-party/yxml -I../Middlewares/btstack/3rd-party/md5 -I../Core/port -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/btstack/3rd-party/segger-rtt/SEGGER_RTT_printf.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

