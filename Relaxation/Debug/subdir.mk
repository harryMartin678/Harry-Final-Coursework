################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../MPIMain.c \
../Main.c \
../RelaxationSerial.c \
../TestMPI.c 

OBJS += \
./MPIMain.o \
./Main.o \
./RelaxationSerial.o \
./TestMPI.o 

C_DEPS += \
./MPIMain.d \
./Main.d \
./RelaxationSerial.d \
./TestMPI.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Main.o: ../Main.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -O0 -g3 -Wall -pthread -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"Main.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


