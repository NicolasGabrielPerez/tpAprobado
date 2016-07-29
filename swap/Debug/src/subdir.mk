################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/comandos.c \
../src/console-swap.c \
../src/interfazUmc.c \
../src/pages_admin.c \
../src/swap-structs.c \
../src/swap.c 

OBJS += \
./src/comandos.o \
./src/console-swap.o \
./src/interfazUmc.o \
./src/pages_admin.o \
./src/swap-structs.o \
./src/swap.o 

C_DEPS += \
./src/comandos.d \
./src/console-swap.d \
./src/interfazUmc.d \
./src/pages_admin.d \
./src/swap-structs.d \
./src/swap.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2016-1c-Pusheo-luego-existo.../push-library" -I/push-library -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


