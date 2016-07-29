################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: ansisop

dependents:
	-cd /home/utnso/tp-2016-1c-Pusheo-luego-existo.../push-library/Debug && $(MAKE) clean all

# Tool invocations
ansisop: $(OBJS) $(USER_OBJS) /home/utnso/tp-2016-1c-Pusheo-luego-existo.../push-library/Debug/libpush-library.so
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"/home/utnso/tp-2016-1c-Pusheo-luego-existo.../push-library/Debug" -o "ansisop" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) ansisop
	-@echo ' '

post-build:
	-sudo cp ansisop "/urs/bin/";
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build
/home/utnso/tp-2016-1c-Pusheo-luego-existo.../push-library/Debug/libpush-library.so:

-include ../makefile.targets
