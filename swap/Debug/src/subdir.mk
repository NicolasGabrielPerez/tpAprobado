################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/interfazUmc.c \
../src/pages_admin.c \
../src/swap-structs.c \
../src/swap.c 

OBJS += \
./src/interfazUmc.o \
./src/pages_admin.o \
./src/swap-structs.o \
./src/swap.o 

C_DEPS += \
./src/interfazUmc.d \
./src/pages_admin.d \
./src/swap-structs.d \
./src/swap.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/git/tp/push-library" -I/push-library -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


