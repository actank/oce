################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/client/TripodSchema.pb.cc \
../src/client/adapter.cc \
../src/client/cluster.cc \
../src/client/node.cc \
../src/client/registry.cc 

OBJS += \
./src/client/TripodSchema.pb.o \
./src/client/adapter.o \
./src/client/cluster.o \
./src/client/node.o \
./src/client/registry.o 

CC_DEPS += \
./src/client/TripodSchema.pb.d \
./src/client/adapter.d \
./src/client/cluster.d \
./src/client/node.d \
./src/client/registry.d 


# Each subdirectory must supply rules for building sources it contributes
src/client/%.o: ../src/client/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


