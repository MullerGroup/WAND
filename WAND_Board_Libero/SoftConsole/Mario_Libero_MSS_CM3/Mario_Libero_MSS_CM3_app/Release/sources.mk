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

BUILDCMD = arm-none-eabi-gcc -mthumb -mcpu=cortex-m3 -specs=bare.specs -LC:\Users\subnets\Documents\NHP\artifact\Mario_SF2\SoftConsole\Mario_Libero_MSS_CM3\Mario_Libero_MSS_CM3_hw_platform\Release -L"C:\Users\subnets\Documents\NHP\artifact\Mario_SF2\SoftConsole\Mario_Libero_MSS_CM3\Mario_Libero_MSS_CM3_hw_platform\drivers" -TC:\Users\subnets\Documents\NHP\artifact\Mario_SF2\SoftConsole\Mario_Libero_MSS_CM3\Mario_Libero_MSS_CM3_hw_platform\CMSIS\startup_gcc\debug-in-microsemi-smartfusion2-envm.ld -Wl,-Map=$(EXECUTABLE).map -Xlinker -gc-sections 
SHELL := cmd.exe
EXECUTABLE := Mario_Libero_MSS_CM3_app

# Target-specific items to be cleaned up in the top directory.
clean::
	-$(RM) $(wildcard ./*.map) 
