################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../arm_cfft_q15.c \
../arm_cmplx_mag_squared_q15.c \
../arm_common_tables.c \
../arm_const_structs.c \
../arm_rfft_q15.c \
../artifact_cancellation.c \
../loop.c 

OBJS += \
./arm_cfft_q15.o \
./arm_cmplx_mag_squared_q15.o \
./arm_common_tables.o \
./arm_const_structs.o \
./arm_rfft_q15.o \
./artifact_cancellation.o \
./loop.o 

C_DEPS += \
./arm_cfft_q15.d \
./arm_cmplx_mag_squared_q15.d \
./arm_common_tables.d \
./arm_const_structs.d \
./arm_rfft_q15.d \
./artifact_cancellation.d \
./loop.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU C Compiler'
	arm-none-eabi-gcc -mthumb -mcpu=cortex-m3 -DNDEBUG -IP:\projects\OMNI\Libero\NeuroMini_golden\SoftConsole\NeuroMini_MSS_CM3\NeuroMini_MSS_CM3_hw_platform -IP:\projects\OMNI\Libero\NeuroMini_golden\SoftConsole\NeuroMini_MSS_CM3\NeuroMini_MSS_CM3_hw_platform\CMSIS -IP:\projects\OMNI\Libero\NeuroMini_golden\SoftConsole\NeuroMini_MSS_CM3\NeuroMini_MSS_CM3_hw_platform\CMSIS\startup_gcc -IP:\projects\OMNI\Libero\NeuroMini_golden\SoftConsole\NeuroMini_MSS_CM3\NeuroMini_MSS_CM3_hw_platform\drivers -IP:\projects\OMNI\Libero\NeuroMini_golden\SoftConsole\NeuroMini_MSS_CM3\NeuroMini_MSS_CM3_hw_platform\drivers\mss_gpio -IP:\projects\OMNI\Libero\NeuroMini_golden\SoftConsole\NeuroMini_MSS_CM3\NeuroMini_MSS_CM3_hw_platform\drivers\mss_hpdma -IP:\projects\OMNI\Libero\NeuroMini_golden\SoftConsole\NeuroMini_MSS_CM3\NeuroMini_MSS_CM3_hw_platform\drivers\mss_nvm -IP:\projects\OMNI\Libero\NeuroMini_golden\SoftConsole\NeuroMini_MSS_CM3\NeuroMini_MSS_CM3_hw_platform\drivers\mss_spi -IP:\projects\OMNI\Libero\NeuroMini_golden\SoftConsole\NeuroMini_MSS_CM3\NeuroMini_MSS_CM3_hw_platform\drivers\mss_sys_services -IP:\projects\OMNI\Libero\NeuroMini_golden\SoftConsole\NeuroMini_MSS_CM3\NeuroMini_MSS_CM3_hw_platform\drivers\mss_timer -IP:\projects\OMNI\Libero\NeuroMini_golden\SoftConsole\NeuroMini_MSS_CM3\NeuroMini_MSS_CM3_hw_platform\drivers\mss_uart -IP:\projects\OMNI\Libero\NeuroMini_golden\SoftConsole\NeuroMini_MSS_CM3\NeuroMini_MSS_CM3_hw_platform\drivers_config -IP:\projects\OMNI\Libero\NeuroMini_golden\SoftConsole\NeuroMini_MSS_CM3\NeuroMini_MSS_CM3_hw_platform\drivers_config\sys_config -IP:\projects\OMNI\Libero\NeuroMini_golden\SoftConsole\NeuroMini_MSS_CM3\NeuroMini_MSS_CM3_hw_platform\hal -IP:\projects\OMNI\Libero\NeuroMini_golden\SoftConsole\NeuroMini_MSS_CM3\NeuroMini_MSS_CM3_hw_platform\hal\CortexM3 -IP:\projects\OMNI\Libero\NeuroMini_golden\SoftConsole\NeuroMini_MSS_CM3\NeuroMini_MSS_CM3_hw_platform\hal\CortexM3\GNU -O2 -ffunction-sections -fdata-sections -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


