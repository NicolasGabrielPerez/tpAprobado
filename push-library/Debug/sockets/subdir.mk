################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../sockets/communication.c \
../sockets/config.c \
../sockets/error_codes.c \
../sockets/pcb.c \
../sockets/serialization.c \
../sockets/sockets.c 

OBJS += \
./sockets/communication.o \
./sockets/config.o \
./sockets/error_codes.o \
./sockets/pcb.o \
./sockets/serialization.o \
./sockets/sockets.o 

C_DEPS += \
./sockets/communication.d \
./sockets/config.d \
./sockets/error_codes.d \
./sockets/pcb.d \
./sockets/serialization.d \
./sockets/sockets.d 


# Each subdirectory must supply rules for building sources it contributes
sockets/%.o: ../sockets/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


