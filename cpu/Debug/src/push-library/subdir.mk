################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/push-library/communication.c \
../src/push-library/config.c \
../src/push-library/error_codes.c \
../src/push-library/pcb.c \
../src/push-library/serialization.c \
../src/push-library/sockets.c 

OBJS += \
./src/push-library/communication.o \
./src/push-library/config.o \
./src/push-library/error_codes.o \
./src/push-library/pcb.o \
./src/push-library/serialization.o \
./src/push-library/sockets.o 

C_DEPS += \
./src/push-library/communication.d \
./src/push-library/config.d \
./src/push-library/error_codes.d \
./src/push-library/pcb.d \
./src/push-library/serialization.d \
./src/push-library/sockets.d 


# Each subdirectory must supply rules for building sources it contributes
src/push-library/%.o: ../src/push-library/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


