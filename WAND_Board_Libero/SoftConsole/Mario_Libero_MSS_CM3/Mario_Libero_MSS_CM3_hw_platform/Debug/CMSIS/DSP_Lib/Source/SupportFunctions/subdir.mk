################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CMSIS/DSP_Lib/Source/SupportFunctions/arm_copy_f32.c \
../CMSIS/DSP_Lib/Source/SupportFunctions/arm_copy_q15.c \
../CMSIS/DSP_Lib/Source/SupportFunctions/arm_copy_q31.c \
../CMSIS/DSP_Lib/Source/SupportFunctions/arm_copy_q7.c \
../CMSIS/DSP_Lib/Source/SupportFunctions/arm_fill_f32.c \
../CMSIS/DSP_Lib/Source/SupportFunctions/arm_fill_q15.c \
../CMSIS/DSP_Lib/Source/SupportFunctions/arm_fill_q31.c \
../CMSIS/DSP_Lib/Source/SupportFunctions/arm_fill_q7.c \
../CMSIS/DSP_Lib/Source/SupportFunctions/arm_float_to_q15.c \
../CMSIS/DSP_Lib/Source/SupportFunctions/arm_float_to_q31.c \
../CMSIS/DSP_Lib/Source/SupportFunctions/arm_float_to_q7.c \
../CMSIS/DSP_Lib/Source/SupportFunctions/arm_q15_to_float.c \
../CMSIS/DSP_Lib/Source/SupportFunctions/arm_q15_to_q31.c \
../CMSIS/DSP_Lib/Source/SupportFunctions/arm_q15_to_q7.c \
../CMSIS/DSP_Lib/Source/SupportFunctions/arm_q31_to_float.c \
../CMSIS/DSP_Lib/Source/SupportFunctions/arm_q31_to_q15.c \
../CMSIS/DSP_Lib/Source/SupportFunctions/arm_q31_to_q7.c \
../CMSIS/DSP_Lib/Source/SupportFunctions/arm_q7_to_float.c \
../CMSIS/DSP_Lib/Source/SupportFunctions/arm_q7_to_q15.c \
../CMSIS/DSP_Lib/Source/SupportFunctions/arm_q7_to_q31.c 

OBJS += \
./CMSIS/DSP_Lib/Source/SupportFunctions/arm_copy_f32.o \
./CMSIS/DSP_Lib/Source/SupportFunctions/arm_copy_q15.o \
./CMSIS/DSP_Lib/Source/SupportFunctions/arm_copy_q31.o \
./CMSIS/DSP_Lib/Source/SupportFunctions/arm_copy_q7.o \
./CMSIS/DSP_Lib/Source/SupportFunctions/arm_fill_f32.o \
./CMSIS/DSP_Lib/Source/SupportFunctions/arm_fill_q15.o \
./CMSIS/DSP_Lib/Source/SupportFunctions/arm_fill_q31.o \
./CMSIS/DSP_Lib/Source/SupportFunctions/arm_fill_q7.o \
./CMSIS/DSP_Lib/Source/SupportFunctions/arm_float_to_q15.o \
./CMSIS/DSP_Lib/Source/SupportFunctions/arm_float_to_q31.o \
./CMSIS/DSP_Lib/Source/SupportFunctions/arm_float_to_q7.o \
./CMSIS/DSP_Lib/Source/SupportFunctions/arm_q15_to_float.o \
./CMSIS/DSP_Lib/Source/SupportFunctions/arm_q15_to_q31.o \
./CMSIS/DSP_Lib/Source/SupportFunctions/arm_q15_to_q7.o \
./CMSIS/DSP_Lib/Source/SupportFunctions/arm_q31_to_float.o \
./CMSIS/DSP_Lib/Source/SupportFunctions/arm_q31_to_q15.o \
./CMSIS/DSP_Lib/Source/SupportFunctions/arm_q31_to_q7.o \
./CMSIS/DSP_Lib/Source/SupportFunctions/arm_q7_to_float.o \
./CMSIS/DSP_Lib/Source/SupportFunctions/arm_q7_to_q15.o \
./CMSIS/DSP_Lib/Source/SupportFunctions/arm_q7_to_q31.o 

C_DEPS += \
./CMSIS/DSP_Lib/Source/SupportFunctions/arm_copy_f32.d \
./CMSIS/DSP_Lib/Source/SupportFunctions/arm_copy_q15.d \
./CMSIS/DSP_Lib/Source/SupportFunctions/arm_copy_q31.d \
./CMSIS/DSP_Lib/Source/SupportFunctions/arm_copy_q7.d \
./CMSIS/DSP_Lib/Source/SupportFunctions/arm_fill_f32.d \
./CMSIS/DSP_Lib/Source/SupportFunctions/arm_fill_q15.d \
./CMSIS/DSP_Lib/Source/SupportFunctions/arm_fill_q31.d \
./CMSIS/DSP_Lib/Source/SupportFunctions/arm_fill_q7.d \
./CMSIS/DSP_Lib/Source/SupportFunctions/arm_float_to_q15.d \
./CMSIS/DSP_Lib/Source/SupportFunctions/arm_float_to_q31.d \
./CMSIS/DSP_Lib/Source/SupportFunctions/arm_float_to_q7.d \
./CMSIS/DSP_Lib/Source/SupportFunctions/arm_q15_to_float.d \
./CMSIS/DSP_Lib/Source/SupportFunctions/arm_q15_to_q31.d \
./CMSIS/DSP_Lib/Source/SupportFunctions/arm_q15_to_q7.d \
./CMSIS/DSP_Lib/Source/SupportFunctions/arm_q31_to_float.d \
./CMSIS/DSP_Lib/Source/SupportFunctions/arm_q31_to_q15.d \
./CMSIS/DSP_Lib/Source/SupportFunctions/arm_q31_to_q7.d \
./CMSIS/DSP_Lib/Source/SupportFunctions/arm_q7_to_float.d \
./CMSIS/DSP_Lib/Source/SupportFunctions/arm_q7_to_q15.d \
./CMSIS/DSP_Lib/Source/SupportFunctions/arm_q7_to_q31.d 


# Each subdirectory must supply rules for building sources it contributes
CMSIS/DSP_Lib/Source/SupportFunctions/%.o: ../CMSIS/DSP_Lib/Source/SupportFunctions/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU C Compiler'
	arm-none-eabi-gcc -mthumb -mcpu=cortex-m3 -IP:\fred\OMNI\Libero\Mario-CM_Flex-EMG_golden_STAPL\Mario_SF2_m2s025\SoftConsole\Mario_Libero_MSS_CM3\Mario_Libero_MSS_CM3_hw_platform -IP:\fred\OMNI\Libero\Mario-CM_Flex-EMG_golden_STAPL\Mario_SF2_m2s025\SoftConsole\Mario_Libero_MSS_CM3\Mario_Libero_MSS_CM3_hw_platform\CMSIS -IP:\fred\OMNI\Libero\Mario-CM_Flex-EMG_golden_STAPL\Mario_SF2_m2s025\SoftConsole\Mario_Libero_MSS_CM3\Mario_Libero_MSS_CM3_hw_platform\CMSIS\startup_gcc -I"P:\fred\OMNI\Libero\Mario-CM_Flex-EMG_golden_STAPL\Mario_SF2_m2s025\SoftConsole\Mario_Libero_MSS_CM3\Mario_Libero_MSS_CM3_hw_platform\CMSIS\Include" -IP:\fred\OMNI\Libero\Mario-CM_Flex-EMG_golden_STAPL\Mario_SF2_m2s025\SoftConsole\Mario_Libero_MSS_CM3\Mario_Libero_MSS_CM3_hw_platform\drivers -IP:\fred\OMNI\Libero\Mario-CM_Flex-EMG_golden_STAPL\Mario_SF2_m2s025\SoftConsole\Mario_Libero_MSS_CM3\Mario_Libero_MSS_CM3_hw_platform\drivers\mss_gpio -IP:\fred\OMNI\Libero\Mario-CM_Flex-EMG_golden_STAPL\Mario_SF2_m2s025\SoftConsole\Mario_Libero_MSS_CM3\Mario_Libero_MSS_CM3_hw_platform\drivers\mss_hpdma -IP:\fred\OMNI\Libero\Mario-CM_Flex-EMG_golden_STAPL\Mario_SF2_m2s025\SoftConsole\Mario_Libero_MSS_CM3\Mario_Libero_MSS_CM3_hw_platform\drivers\mss_i2c -IP:\fred\OMNI\Libero\Mario-CM_Flex-EMG_golden_STAPL\Mario_SF2_m2s025\SoftConsole\Mario_Libero_MSS_CM3\Mario_Libero_MSS_CM3_hw_platform\drivers\mss_nvm -IP:\fred\OMNI\Libero\Mario-CM_Flex-EMG_golden_STAPL\Mario_SF2_m2s025\SoftConsole\Mario_Libero_MSS_CM3\Mario_Libero_MSS_CM3_hw_platform\drivers\mss_spi -IP:\fred\OMNI\Libero\Mario-CM_Flex-EMG_golden_STAPL\Mario_SF2_m2s025\SoftConsole\Mario_Libero_MSS_CM3\Mario_Libero_MSS_CM3_hw_platform\drivers\mss_sys_services -IP:\fred\OMNI\Libero\Mario-CM_Flex-EMG_golden_STAPL\Mario_SF2_m2s025\SoftConsole\Mario_Libero_MSS_CM3\Mario_Libero_MSS_CM3_hw_platform\drivers\mss_timer -IP:\fred\OMNI\Libero\Mario-CM_Flex-EMG_golden_STAPL\Mario_SF2_m2s025\SoftConsole\Mario_Libero_MSS_CM3\Mario_Libero_MSS_CM3_hw_platform\drivers\mss_uart -IP:\fred\OMNI\Libero\Mario-CM_Flex-EMG_golden_STAPL\Mario_SF2_m2s025\SoftConsole\Mario_Libero_MSS_CM3\Mario_Libero_MSS_CM3_hw_platform\drivers_config -IP:\fred\OMNI\Libero\Mario-CM_Flex-EMG_golden_STAPL\Mario_SF2_m2s025\SoftConsole\Mario_Libero_MSS_CM3\Mario_Libero_MSS_CM3_hw_platform\drivers_config\sys_config -IP:\fred\OMNI\Libero\Mario-CM_Flex-EMG_golden_STAPL\Mario_SF2_m2s025\SoftConsole\Mario_Libero_MSS_CM3\Mario_Libero_MSS_CM3_hw_platform\hal -IP:\fred\OMNI\Libero\Mario-CM_Flex-EMG_golden_STAPL\Mario_SF2_m2s025\SoftConsole\Mario_Libero_MSS_CM3\Mario_Libero_MSS_CM3_hw_platform\hal\CortexM3 -IP:\fred\OMNI\Libero\Mario-CM_Flex-EMG_golden_STAPL\Mario_SF2_m2s025\SoftConsole\Mario_Libero_MSS_CM3\Mario_Libero_MSS_CM3_hw_platform\hal\CortexM3\GNU -O0 -ffunction-sections -fdata-sections -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


