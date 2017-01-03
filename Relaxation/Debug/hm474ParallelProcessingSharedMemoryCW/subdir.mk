################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../hm474ParallelProcessingSharedMemoryCW/hm474Relaxation.c 

OBJS += \
./hm474ParallelProcessingSharedMemoryCW/hm474Relaxation.o 

C_DEPS += \
./hm474ParallelProcessingSharedMemoryCW/hm474Relaxation.d 


# Each subdirectory must supply rules for building sources it contributes
hm474ParallelProcessingSharedMemoryCW/%.o: ../hm474ParallelProcessingSharedMemoryCW/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


