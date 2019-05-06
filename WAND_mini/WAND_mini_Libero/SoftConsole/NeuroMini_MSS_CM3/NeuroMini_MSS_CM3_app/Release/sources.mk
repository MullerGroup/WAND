################################################################################
# Automatically-generated file. Do not edit!
################################################################################

O_SRCS := 
C_SRCS := 
S_UPPER_SRCS := 
S_SRCS := 
OBJ_SRCS := 
MEMORYMAP := 
OBJS := 
C_DEPS := 
SRECFILES := 
IHEXFILES := 
LISTINGS := 
EXECUTABLE := 

# Every subdirectory with source files must be described here
SUBDIRS := \
. \

################################################################################
# Microsemi SoftConsole IDE Variables
################################################################################

BUILDCMD = arm-none-eabi-gcc -mthumb -mcpu=cortex-m3  -L"P:\projects\OMNI\Libero\NeuroMini_golden\SoftConsole\NeuroMini_MSS_CM3\NeuroMini_MSS_CM3_hw_platform\Debug" -L"P:\projects\OMNI\Libero\NeuroMini_golden\SoftConsole\NeuroMini_MSS_CM3\NeuroMini_MSS_CM3_hw_platform\drivers" -L"P:\projects\OMNI\Libero\NeuroMini_golden\SoftConsole\NeuroMini_MSS_CM3\NeuroMini_MSS_CM3_hw_platform\DSP_Lib\Source" -L"P:\projects\OMNI\Libero\NeuroMini_golden\SoftConsole\NeuroMini_MSS_CM3\NeuroMini_MSS_CM3_hw_platform\Lib\GCC" -TP:\projects\OMNI\Libero\NeuroMini_golden\SoftConsole\NeuroMini_MSS_CM3\NeuroMini_MSS_CM3_hw_platform\CMSIS\startup_gcc\debug-in-microsemi-smartfusion2-esram.ld -Wl,-Map=$(EXECUTABLE).map -Xlinker -gc-sections 
SHELL := cmd.exe
EXECUTABLE := NeuroMini_MSS_CM3_app

# Target-specific items to be cleaned up in the top directory.
clean::
	-$(RM) $(wildcard ./*.map) 
