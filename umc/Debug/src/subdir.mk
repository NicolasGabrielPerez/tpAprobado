################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/comandos.c \
../src/console-umc.c \
../src/cpu-interfaz.c \
../src/nucleo-interfaz.c \
../src/pages_replacement.c \
../src/swap-interfaz.c \
../src/tlb.c \
../src/umc-structs.c \
../src/umc.c 

OBJS += \
./src/comandos.o \
./src/console-umc.o \
./src/cpu-interfaz.o \
./src/nucleo-interfaz.o \
./src/pages_replacement.o \
./src/swap-interfaz.o \
./src/tlb.o \
./src/umc-structs.o \
./src/umc.o 

C_DEPS += \
./src/comandos.d \
./src/console-umc.d \
./src/cpu-interfaz.d \
./src/nucleo-interfaz.d \
./src/pages_replacement.d \
./src/swap-interfaz.d \
./src/tlb.d \
./src/umc-structs.d \
./src/umc.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/git/tp-2016-1c-Pusheo-luego-existo.../push-library" -I/push-library -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/umc.o: ../src/umc.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/git/tp-2016-1c-Pusheo-luego-existo.../push-library" -I/push-library -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"src/umc.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


