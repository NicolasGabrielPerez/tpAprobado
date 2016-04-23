################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/CPU.c \
../src/ansiop.c \
../src/configInit.c \
../src/librery.c 

OBJS += \
./src/CPU.o \
./src/ansiop.o \
./src/configInit.o \
./src/librery.o 

C_DEPS += \
./src/CPU.d \
./src/ansiop.d \
./src/configInit.d \
./src/librery.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


