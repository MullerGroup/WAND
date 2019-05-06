################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CMSIS/system_m2sxxx.c 

OBJS += \
./CMSIS/system_m2sxxx.o 

C_DEPS += \
./CMSIS/system_m2sxxx.d 


# Each subdirectory must supply rules for building sources it contributes
CMSIS/%.o: ../CMSIS/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU C Compiler'
	arm-none-eabi-gcc -mthumb -mcpu=cortex-m3 -IP:\projects\OMNI\Libero\NeuroMini_golden\SoftConsole\NeuroMini_MSS_CM3\NeuroMini_MSS_CM3_hw_platform -IP:\projects\OMNI\Libero\NeuroMini_golden\SoftConsole\NeuroMini_MSS_CM3\NeuroMini_MSS_CM3_hw_platform\CMSIS -IP:\projects\OMNI\Libero\NeuroMini_golden\SoftConsole\NeuroMini_MSS_CM3\NeuroMini_MSS_CM3_hw_platform\CMSIS\startup_gcc -IP:\projects\OMNI\Libero\NeuroMini_golden\SoftConsole\NeuroMini_MSS_CM3\NeuroMini_MSS_CM3_hw_platform\drivers -IP:\projects\OMNI\Libero\NeuroMini_golden\SoftConsole\NeuroMini_MSS_CM3\NeuroMini_MSS_CM3_hw_platform\drivers\mss_hpdma -IP:\projects\OMNI\Libero\NeuroMini_golden\SoftConsole\NeuroMini_MSS_CM3\NeuroMini_MSS_CM3_hw_platform\drivers\mss_nvm -IP:\projects\OMNI\Libero\NeuroMini_golden\SoftConsole\NeuroMini_MSS_CM3\NeuroMini_MSS_CM3_hw_platform\drivers\mss_sys_services -IP:\projects\OMNI\Libero\NeuroMini_golden\SoftConsole\NeuroMini_MSS_CM3\NeuroMini_MSS_CM3_hw_platform\drivers\mss_timer -IP:\projects\OMNI\Libero\NeuroMini_golden\SoftConsole\NeuroMini_MSS_CM3\NeuroMini_MSS_CM3_hw_platform\drivers\mss_uart -IP:\projects\OMNI\Libero\NeuroMini_golden\SoftConsole\NeuroMini_MSS_CM3\NeuroMini_MSS_CM3_hw_platform\drivers_config -IP:\projects\OMNI\Libero\NeuroMini_golden\SoftConsole\NeuroMini_MSS_CM3\NeuroMini_MSS_CM3_hw_platform\drivers_config\sys_config -IP:\projects\OMNI\Libero\NeuroMini_golden\SoftConsole\NeuroMini_MSS_CM3\NeuroMini_MSS_CM3_hw_platform\hal -IP:\projects\OMNI\Libero\NeuroMini_golden\SoftConsole\NeuroMini_MSS_CM3\NeuroMini_MSS_CM3_hw_platform\hal\CortexM3 -IP:\projects\OMNI\Libero\NeuroMini_golden\SoftConsole\NeuroMini_MSS_CM3\NeuroMini_MSS_CM3_hw_platform\hal\CortexM3\GNU -O0 -ffunction-sections -fdata-sections -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


