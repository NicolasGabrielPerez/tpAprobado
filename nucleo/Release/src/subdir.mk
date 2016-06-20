################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/consola-interfaz.c \
../src/cpu-interfaz.c \
../src/nucleo-structs.c \
../src/nucleo.c \
../src/planificador.c \
../src/testing-serialization.c \
../src/umc-interfaz.c 

OBJS += \
./src/consola-interfaz.o \
./src/cpu-interfaz.o \
./src/nucleo-structs.o \
./src/nucleo.o \
./src/planificador.o \
./src/testing-serialization.o \
./src/umc-interfaz.o 

C_DEPS += \
./src/consola-interfaz.d \
./src/cpu-interfaz.d \
./src/nucleo-structs.d \
./src/nucleo.d \
./src/planificador.d \
./src/testing-serialization.d \
./src/umc-interfaz.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


