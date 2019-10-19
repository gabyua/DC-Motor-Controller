################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

-include makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include synergy/ssp_supplemental/touch_drivers/touch_panel_i2c_sx8654/subdir.mk
-include synergy/ssp/src/framework/tes/dave2d/subdir.mk
-include synergy/ssp/src/framework/sf_touch_panel_i2c/subdir.mk
-include synergy/ssp/src/framework/sf_tes_2d_drw/subdir.mk
-include synergy/ssp/src/framework/sf_message/subdir.mk
-include synergy/ssp/src/framework/sf_jpeg_decode/subdir.mk
-include synergy/ssp/src/framework/sf_external_irq/subdir.mk
-include synergy/ssp/src/framework/sf_el_gx/subdir.mk
-include synergy/ssp/src/driver/r_sci_spi/subdir.mk
-include synergy/ssp/src/driver/r_riic/subdir.mk
-include synergy/ssp/src/driver/r_jpeg_decode/subdir.mk
-include synergy/ssp/src/driver/r_jpeg_common/subdir.mk
-include synergy/ssp/src/driver/r_ioport/subdir.mk
-include synergy/ssp/src/driver/r_icu/subdir.mk
-include synergy/ssp/src/driver/r_glcd/subdir.mk
-include synergy/ssp/src/driver/r_fmi/subdir.mk
-include synergy/ssp/src/driver/r_elc/subdir.mk
-include synergy/ssp/src/driver/r_dtc/subdir.mk
-include synergy/ssp/src/driver/r_cgc/hw/subdir.mk
-include synergy/ssp/src/driver/r_cgc/subdir.mk
-include synergy/ssp/src/bsp/mcu/s7g2/subdir.mk
-include synergy/ssp/src/bsp/mcu/all/subdir.mk
-include synergy/ssp/src/bsp/cmsis/Device/RENESAS/S7G2/Source/subdir.mk
-include synergy/board/s7g2_sk/subdir.mk
-include src/synergy_gen/subdir.mk
-include src/hardware/subdir.mk
-include src/gui/subdir.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
DC_Motor_Controller.srec \

SECONDARY_SIZE += \
DC_Motor_Controller.siz \


# All Target
all: DC_Motor_Controller.elf secondary-outputs

# Tool invocations
DC_Motor_Controller.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross ARM C Linker'
	$(file > DC_Motor_Controller.elf.in,-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g3 -T "C:\\diplomado\\dse_workspace\\GU\\DC_Motor_Controller\\script\\S7G2.ld" -Xlinker --gc-sections -L "C:\\diplomado\\dse_workspace\\GU\\DC_Motor_Controller\\synergy\\ssp\\src\\bsp\\cmsis\\DSP_Lib\\cm4_gcc" -L "C:\\diplomado\\dse_workspace\\GU\\DC_Motor_Controller\\synergy\\ssp\\src\\framework\\el\\gx\\cm4_gcc" -L "C:\\diplomado\\dse_workspace\\GU\\DC_Motor_Controller\\synergy\\ssp\\src\\framework\\el\\tx\\cm4_gcc" -Wl,-Map,"DC_Motor_Controller.map" --specs=nano.specs --specs=rdimon.specs -o "DC_Motor_Controller.elf" -Wl,--start-group $(OBJS) $(USER_OBJS) $(LIBS) -Wl,--end-group)
	arm-none-eabi-gcc @"DC_Motor_Controller.elf.in"
	@echo 'Finished building target: $@'
	@echo ' '

DC_Motor_Controller.srec: DC_Motor_Controller.elf
	@echo 'Invoking: Cross ARM GNU Create Flash Image'
	arm-none-eabi-objcopy -O srec "DC_Motor_Controller.elf"  "DC_Motor_Controller.srec"
	@echo 'Finished building: $@'
	@echo ' '

DC_Motor_Controller.siz: DC_Motor_Controller.elf
	@echo 'Invoking: Cross ARM GNU Print Size'
	arm-none-eabi-size --format=berkeley "DC_Motor_Controller.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
ifneq ($(strip $(OBJS)),)
	-$(file > clean_OBJS.tmp, $(OBJS))
	-@ xargs -s 6000 -t -a clean_OBJS.tmp $(RM)
	-@$(RM) clean_OBJS.tmp
endif
ifneq ($(strip $(SECONDARY_FLASH)),)
	-$(file > clean_SECONDARY_FLASH.tmp, $(SECONDARY_FLASH))
	-@ xargs -s 6000 -t -a clean_SECONDARY_FLASH.tmp $(RM)
	-@$(RM) clean_SECONDARY_FLASH.tmp
endif
ifneq ($(strip $(SECONDARY_SIZE)),)
	-$(file > clean_SECONDARY_SIZE.tmp, $(SECONDARY_SIZE))
	-@ xargs -s 6000 -t -a clean_SECONDARY_SIZE.tmp $(RM)
	-@$(RM) clean_SECONDARY_SIZE.tmp
endif
ifneq ($(strip $(ASM_DEPS)),)
	-$(file > clean_ASM_DEPS.tmp, $(ASM_DEPS))
	-@ xargs -s 6000 -t -a clean_ASM_DEPS.tmp $(RM)
	-@$(RM) clean_ASM_DEPS.tmp
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
	-$(file > clean_S_UPPER_DEPS.tmp, $(S_UPPER_DEPS))
	-@ xargs -s 6000 -t -a clean_S_UPPER_DEPS.tmp $(RM)
	-@$(RM) clean_S_UPPER_DEPS.tmp
endif
ifneq ($(strip $(C_DEPS)),)
	-$(file > clean_C_DEPS.tmp, $(C_DEPS))
	-@ xargs -s 6000 -t -a clean_C_DEPS.tmp $(RM)
	-@$(RM) clean_C_DEPS.tmp
endif
	-$(RM)  DC_Motor_Controller.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
