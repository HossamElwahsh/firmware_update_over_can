################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32F1xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
firmware_application.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
firmware_application.list \

OBJCOPY_HEX += \
firmware_application.hex \

OBJCOPY_BIN += \
firmware_application.bin \


# All Target
all: firmware_application.elf secondary-outputs

# Tool invocations
firmware_application.elf: $(OBJS) $(USER_OBJS) /home/hossam/Documents/STM32/custom_bootloader_using_can/firmware_application/STM32F103C8TX_FLASH.ld
	arm-none-eabi-gcc -o "firmware_application.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m3 -T"/home/hossam/Documents/STM32/custom_bootloader_using_can/firmware_application/STM32F103C8TX_FLASH.ld" --specs=nosys.specs -Wl,-Map="firmware_application.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

firmware_application.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "firmware_application.list"
	@echo 'Finished building: $@'
	@echo ' '

firmware_application.hex: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O ihex $(EXECUTABLES) "firmware_application.hex"
	@echo 'Finished building: $@'
	@echo ' '

firmware_application.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "firmware_application.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_HEX) $(OBJCOPY_BIN)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
