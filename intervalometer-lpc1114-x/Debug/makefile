################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include freertos/src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: intervalometer-lpc1114-x.axf

# Tool invocations
intervalometer-lpc1114-x.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU C++ Linker'
	arm-none-eabi-c++ -nostdlib -L"C:\Users\jdubovsky\Documents\LPCXpresso_7.9.2_493\workspace\nxp_lpcxpresso_11c24_board_lib\Debug" -L"C:\Users\jdubovsky\Documents\LPCXpresso_7.9.2_493\workspace\lpc_chip_11cxx_lib\Debug" -Xlinker -Map="intervalometer-lpc1114-x.map" -Xlinker --gc-sections -mcpu=cortex-m0 -mthumb -T "intervalometer-lpc1114-x_Debug.ld" -o "intervalometer-lpc1114-x.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS)$(C++_DEPS)$(C_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) intervalometer-lpc1114-x.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "intervalometer-lpc1114-x.axf"; # arm-none-eabi-objcopy -v -O binary "intervalometer-lpc1114-x.axf" "intervalometer-lpc1114-x.bin" ; # checksum -p LPC1114_302 -d "intervalometer-lpc1114-x.bin";
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
