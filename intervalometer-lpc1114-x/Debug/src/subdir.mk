################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/adc.cpp \
../src/avalon_time.cpp \
../src/blinky.cpp \
../src/camera.cpp \
../src/cr_cpp_config.cpp \
../src/cr_startup_lpc11xx.cpp \
../src/eeprom.cpp \
../src/intervalometer-lpc1114-x.cpp \
../src/logger.cpp \
../src/main.cpp \
../src/rtc.cpp 

C_SRCS += \
../src/cr_startup_lpc11xx.c \
../src/crp.c \
../src/sysinit.c 

OBJS += \
./src/adc.o \
./src/avalon_time.o \
./src/blinky.o \
./src/camera.o \
./src/cr_cpp_config.o \
./src/cr_startup_lpc11xx.o \
./src/crp.o \
./src/eeprom.o \
./src/intervalometer-lpc1114-x.o \
./src/logger.o \
./src/main.o \
./src/rtc.o \
./src/sysinit.o 

C_DEPS += \
./src/cr_startup_lpc11xx.d \
./src/crp.d \
./src/sysinit.d 

CPP_DEPS += \
./src/adc.d \
./src/avalon_time.d \
./src/blinky.d \
./src/camera.d \
./src/cr_cpp_config.d \
./src/cr_startup_lpc11xx.d \
./src/eeprom.d \
./src/intervalometer-lpc1114-x.d \
./src/logger.d \
./src/main.d \
./src/rtc.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C++ Compiler'
	arm-none-eabi-c++ -std=c++11 -D__NEWLIB__ -DDEBUG -D__CODE_RED -DCORE_M0 -D__USE_LPCOPEN -DCPP_USE_HEAP -D__LPC11XX__ -I"C:\Users\jdubovsky\Documents\LPCXpresso_7.9.2_493\workspace\nxp_lpcxpresso_11c24_board_lib\inc" -I"C:\Users\jdubovsky\Documents\LPCXpresso_7.9.2_493\workspace\intervalometer-lpc1114-x\inc" -I"C:\Users\jdubovsky\Documents\LPCXpresso_7.9.2_493\workspace\intervalometer-lpc1114-x\freertos\inc" -I"C:\Users\jdubovsky\Documents\LPCXpresso_7.9.2_493\workspace\lpc_chip_11cxx_lib\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions -mcpu=cortex-m0 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=c11 -D__NEWLIB__ -DDEBUG -D__CODE_RED -DCORE_M0 -D__USE_LPCOPEN -DCPP_USE_HEAP -D__LPC11XX__ -I"C:\Users\jdubovsky\Documents\LPCXpresso_7.9.2_493\workspace\nxp_lpcxpresso_11c24_board_lib\inc" -I"C:\Users\jdubovsky\Documents\LPCXpresso_7.9.2_493\workspace\intervalometer-lpc1114-x\inc" -I"C:\Users\jdubovsky\Documents\LPCXpresso_7.9.2_493\workspace\intervalometer-lpc1114-x\freertos\inc" -I"C:\Users\jdubovsky\Documents\LPCXpresso_7.9.2_493\workspace\lpc_chip_11cxx_lib\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m0 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


