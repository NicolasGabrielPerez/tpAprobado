################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ansiop.c \
../src/cpu.c \
../src/nucleoFunctions.c \
../src/test.c \
../src/umcFunctions.c 

OBJS += \
./src/ansiop.o \
./src/cpu.o \
./src/nucleoFunctions.o \
./src/test.o \
./src/umcFunctions.o 

C_DEPS += \
./src/ansiop.d \
./src/cpu.d \
./src/nucleoFunctions.d \
./src/test.d \
./src/umcFunctions.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2016-1c-Pusheo-luego-existo.../push-library" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


