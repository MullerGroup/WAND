################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CMSIS/DSP_Lib/Source/CommonTables/arm_common_tables.c \
../CMSIS/DSP_Lib/Source/CommonTables/arm_const_structs.c 

OBJS += \
./CMSIS/DSP_Lib/Source/CommonTables/arm_common_tables.o \
./CMSIS/DSP_Lib/Source/CommonTables/arm_const_structs.o 

C_DEPS += \
./CMSIS/DSP_Lib/Source/CommonTables/arm_common_tables.d \
./CMSIS/DSP_Lib/Source/CommonTables/arm_const_structs.d 


# Each subdirectory must supply rules for building sources it contributes
CMSIS/DSP_Lib/Source/CommonTables/%.o: ../CMSIS/DSP_Lib/Source/CommonTables/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU C Compiler'
	arm-none-eabi-gcc -mthumb -mcpu=cortex-m3 -IP:\fred\OMNI\Libero\Mario-CM_Flex-EMG_golden_STAPL\Mario_SF2_m2s025\SoftConsole\Mario_Libero_MSS_CM3\Mario_Libero_MSS_CM3_hw_platform -IP:\fred\OMNI\Libero\Mario-CM_Flex-EMG_golden_STAPL\Mario_SF2_m2s025\SoftConsole\Mario_Libero_MSS_CM3\Mario_Libero_MSS_CM3_hw_platform\CMSIS -IP:\fred\OMNI\Libero\Mario-CM_Flex-EMG_golden_STAPL\Mario_SF2_m2s025\SoftConsole\Mario_Libero_MSS_CM3\Mario_Libero_MSS_CM3_hw_platform\CMSIS\startup_gcc -I"P:\fred\OMNI\Libero\Mario-CM_Flex-EMG_golden_STAPL\Mario_SF2_m2s025\SoftConsole\Mario_Libero_MSS_CM3\Mario_Libero_MSS_CM3_hw_platform\CMSIS\Include" -IP:\fred\OMNI\Libero\Mario-CM_Flex-EMG_golden_STAPL\Mario_SF2_m2s025\SoftConsole\Mario_Libero_MSS_CM3\Mario_Libero_MSS_CM3_hw_platform\drivers -IP:\fred\OMNI\Libero\Mario-CM_Flex-EMG_golden_STAPL\Mario_SF2_m2s025\SoftConsole\Mario_Libero_MSS_CM3\Mario_Libero_MSS_CM3_hw_platform\drivers\mss_gpio -IP:\fred\OMNI\Libero\Mario-CM_Flex-EMG_golden_STAPL\Mario_SF2_m2s025\SoftConsole\Mario_Libero_MSS_CM3\Mario_Libero_MSS_CM3_hw_platform\drivers\mss_hpdma -IP:\fred\OMNI\Libero\Mario-CM_Flex-EMG_golden_STAPL\Mario_SF2_m2s025\SoftConsole\Mario_Libero_MSS_CM3\Mario_Libero_MSS_CM3_hw_platform\drivers\mss_i2c -IP:\fred\OMNI\Libero\Mario-CM_Flex-EMG_golden_STAPL\Mario_SF2_m2s025\SoftConsole\Mario_Libero_MSS_CM3\Mario_Libero_MSS_CM3_hw_platform\drivers\mss_nvm -IP:\fred\OMNI\Libero\Mario-CM_Flex-EMG_golden_STAPL\Mario_SF2_m2s025\SoftConsole\Mario_Libero_MSS_CM3\Mario_Libero_MSS_CM3_hw_platform\drivers\mss_spi -IP:\fred\OMNI\Libero\Mario-CM_Flex-EMG_golden_STAPL\Mario_SF2_m2s025\SoftConsole\Mario_Libero_MSS_CM3\Mario_Libero_MSS_CM3_hw_platform\drivers\mss_sys_services -IP:\fred\OMNI\Libero\Mario-CM_Flex-EMG_golden_STAPL\Mario_SF2_m2s025\SoftConsole\Mario_Libero_MSS_CM3\Mario_Libero_MSS_CM3_hw_platform\drivers\mss_timer -IP:\fred\OMNI\Libero\Mario-CM_Flex-EMG_golden_STAPL\Mario_SF2_m2s025\SoftConsole\Mario_Libero_MSS_CM3\Mario_Libero_MSS_CM3_hw_platform\drivers\mss_uart -IP:\fred\OMNI\Libero\Mario-CM_Flex-EMG_golden_STAPL\Mario_SF2_m2s025\SoftConsole\Mario_Libero_MSS_CM3\Mario_Libero_MSS_CM3_hw_platform\drivers_config -IP:\fred\OMNI\Libero\Mario-CM_Flex-EMG_golden_STAPL\Mario_SF2_m2s025\SoftConsole\Mario_Libero_MSS_CM3\Mario_Libero_MSS_CM3_hw_platform\drivers_config\sys_config -IP:\fred\OMNI\Libero\Mario-CM_Flex-EMG_golden_STAPL\Mario_SF2_m2s025\SoftConsole\Mario_Libero_MSS_CM3\Mario_Libero_MSS_CM3_hw_platform\hal -IP:\fred\OMNI\Libero\Mario-CM_Flex-EMG_golden_STAPL\Mario_SF2_m2s025\SoftConsole\Mario_Libero_MSS_CM3\Mario_Libero_MSS_CM3_hw_platform\hal\CortexM3 -IP:\fred\OMNI\Libero\Mario-CM_Flex-EMG_golden_STAPL\Mario_SF2_m2s025\SoftConsole\Mario_Libero_MSS_CM3\Mario_Libero_MSS_CM3_hw_platform\hal\CortexM3\GNU -O0 -ffunction-sections -fdata-sections -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


