################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/adc.c \
../src/controller.c \
../src/diagnostics.c \
../src/guiapp_event_handlers.c \
../src/hal_entry.c \
../src/init.c \
../src/main_thread_entry.c \
../src/sensor.c 

OBJS += \
./src/adc.o \
./src/controller.o \
./src/diagnostics.o \
./src/guiapp_event_handlers.o \
./src/hal_entry.o \
./src/init.o \
./src/main_thread_entry.o \
./src/sensor.o 

C_DEPS += \
./src/adc.d \
./src/controller.d \
./src/diagnostics.d \
./src/guiapp_event_handlers.d \
./src/hal_entry.d \
./src/init.d \
./src/main_thread_entry.d \
./src/sensor.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	C:\Renesas\e2_studio\eclipse\/../\Utilities\\/isdebuild arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g3 -D_RENESAS_SYNERGY_ -I"C:\Users\SEI301\Documents\JC\PI_DSE\synergy_cfg\ssp_cfg\bsp" -I"C:\Users\SEI301\Documents\JC\PI_DSE\synergy_cfg\ssp_cfg\driver" -I"C:\Users\SEI301\Documents\JC\PI_DSE\synergy\ssp\inc" -I"C:\Users\SEI301\Documents\JC\PI_DSE\synergy\ssp\inc\bsp" -I"C:\Users\SEI301\Documents\JC\PI_DSE\synergy\ssp\inc\bsp\cmsis\Include" -I"C:\Users\SEI301\Documents\JC\PI_DSE\synergy\ssp\inc\driver\api" -I"C:\Users\SEI301\Documents\JC\PI_DSE\synergy\ssp\inc\driver\instances" -I"C:\Users\SEI301\Documents\JC\PI_DSE\src" -I"C:\Users\SEI301\Documents\JC\PI_DSE\src\synergy_gen" -I"C:\Users\SEI301\Documents\JC\PI_DSE\synergy_cfg\ssp_cfg\framework" -I"C:\Users\SEI301\Documents\JC\PI_DSE\synergy\ssp\inc\framework\api" -I"C:\Users\SEI301\Documents\JC\PI_DSE\synergy\ssp\inc\framework\instances" -I"C:\Users\SEI301\Documents\JC\PI_DSE\synergy\ssp\inc\framework\tes" -I"C:\Users\SEI301\Documents\JC\PI_DSE\synergy_cfg\ssp_cfg\framework\el" -I"C:\Users\SEI301\Documents\JC\PI_DSE\synergy\ssp\inc\framework\el" -I"C:\Users\SEI301\Documents\JC\PI_DSE\synergy\ssp\src\framework\el\tx" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" -x c "$<"
	@echo 'Finished building: $<'
	@echo ' '


