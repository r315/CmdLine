
#########################################################
# project files
#########################################################
TARGET 		=bb-console
TARGET_PATH =$(CURDIR)
BUILD_DIR 	:=build
EXEC		=$(BUILD_DIR)/$(TARGET)

CMSIS_PATH   =$(TARGET_PATH)/../Drivers/CMSIS
CMSIS_DEVICE =$(CMSIS_PATH)/Device/NXP/LPC17xx

INCSPATH =\
$(TARGET_PATH)/inc \
$(TARGET_PATH)/usbdrv \
$(APP_DIR)/../inc \
$(APP_DIR)/minimp3 \
$(LIBEMB_PATH)/include \
$(LIBEMB_PATH)/pff \
$(LIBEMB_PATH)/drv/inc \
$(CMSIS_DEVICE)/inc \
$(CMSIS_PATH)/Core/Include \

C_SOURCES =\
$(TARGET_PATH)/src/startup_lpc1768.c \
$(TARGET_PATH)/src/blueboard.c \
$(LIBEMB_PATH)/drv/clock/clock_lpc17xx.c \
$(LIBEMB_PATH)/drv/timer/tim_lpc17xx.c \
$(LIBEMB_PATH)/drv/tft/ili9328.c \
$(LIBEMB_PATH)/display/lcd.c \
$(LIBEMB_PATH)/display/display.c \
$(LIBEMB_PATH)/display/font.c \
$(LIBEMB_PATH)/drv/pwm/pwm_lpc17xx.c \
$(LIBEMB_PATH)/drv/i2c/i2c_lpc17xx.c \
$(LIBEMB_PATH)/drv/i2s/i2s_lpc17xx.c \
$(LIBEMB_PATH)/drv/spi/spi_lpc17xx.c \
$(LIBEMB_PATH)/drv/dac/dac_lpc17xx.c \
$(LIBEMB_PATH)/drv/gpio/gpio_lpc17xx.c \
$(LIBEMB_PATH)/drv/uart/uart_lpc17xx.c \
$(LIBEMB_PATH)/drv/dma/dma_lpc17xx.c \
$(LIBEMB_PATH)/drv/adc/adc_lpc17xx.c \
$(LIBEMB_PATH)/drv/rng/rng_software.c \
$(LIBEMB_PATH)/misc/fifo.c \
$(LIBEMB_PATH)/misc/strfunc.c \
$(LIBEMB_PATH)/misc/debug.c \
$(LIBEMB_PATH)/button/button.c \
$(LIBEMB_PATH)/drv/storage/memcard.c \
$(LIBEMB_PATH)/pff/pff.c \
$(TARGET_PATH)/usbdrv/usbserial.c \
$(TARGET_PATH)/usbdrv/usbhw_lpc.c \
$(TARGET_PATH)/usbdrv/usbcontrol.c \
$(TARGET_PATH)/usbdrv/usbstdreq.c \
$(TARGET_PATH)/stk500/stk500.c \
$(TARGET_PATH)/src/bb_serial.c \
$(TARGET_PATH)/src/bb_main.c \

#$(LIBEMB_PATH)/drv/dac/dac_lpc17xx.c \
$(LIBEMB_PATH)/drv/dma/dma_lpc17xx.c \

CPP_SOURCES = \
$(LIBEMB_PATH)/console/console.cpp \
$(APP_DIR)/app.cpp \
$(APP_DIR)/cmdsbus.cpp \
$(APP_DIR)/cmdtft.cpp \
$(APP_DIR)/cmdi2s.cpp \
$(APP_DIR)/cmdgpio.cpp \
$(APP_DIR)/cmdmsensor.cpp \
$(APP_DIR)/cmdplayer.cpp \
$(APP_DIR)/cmdmem.cpp \
$(TARGET_PATH)/src/cmdspi.cpp \
$(TARGET_PATH)/src/cmdavr.cpp \
$(TARGET_PATH)/src/cmdlsd.cpp \
$(TARGET_PATH)/src/cmdpwm.cpp \
$(TARGET_PATH)/src/cmdawg.cpp \
$(TARGET_PATH)/src/cmdadc.cpp \
$(TARGET_PATH)/src/cmdi2c.cpp \
$(TARGET_PATH)/stk500/stk500_service.cpp \


LDSCRIPT =$(TARGET_PATH)/lpc1768.ld
#########################################################
#Startup files and libraries
#########################################################

LIBSPATH +=

GCSYMBOLS =-D__NEWLIB__ -DBOARD_BLUEBOARD -DENABLE_DEBUG
GCFLAGS =-mcpu=cortex-m3 -mthumb -Wall -Wextra -Og -g -fdata-sections -ffunction-sections -fno-unwind-tables #-fstack-usage
GPPFLAGS=$(GCFLAGS) -fno-exceptions -fno-rtti
LDFLAGS =-mcpu=cortex-m3 -mthumb -nostdlib -lgcc -Wl,--gc-sections -nodefaultlibs #-nostartfiles #-lstdc++ 

DEVICE =LPC1768

#########################################################
GCC_EXEC_PREFIX = arm-none-eabi
GCC = $(GCC_EXEC_PREFIX)-gcc
GPP = $(GCC_EXEC_PREFIX)-g++
AS = $(GCC_EXEC_PREFIX)-as
LD = $(GCC_EXEC_PREFIX)-ld
SIZE = $(GCC_EXEC_PREFIX)-size
OBJCOPY = $(GCC_EXEC_PREFIX)-objcopy
OBJDUMP = $(GCC_EXEC_PREFIX)-objdump
DBG = $(GCC_EXEC_PREFIX)-insight
REMOVE = rm -fR


ifeq ($(shell uname -s), Linux)
JLINK ="/opt/SEGGER/JLink/JLinkExe"
else
JLINK ="C:\Tools\JLink_V500\jlink"
endif
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(CPP_SOURCES:.cpp=.obj)))
vpath %.cpp $(sort $(dir $(CPP_SOURCES)))

OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

ifeq ($(LIBEMB_PATH), )
erro:
	@echo "Please export LIBEMB_PATH"
endif

CHECKSUM =$(LIBEMB_PATH)/tools/checksum
##########################################################
# RULES
##########################################################
all: $(EXEC).elf stats


$(EXEC).elf:  $(OBJECTS)
	@echo "Linking" $@
	@$(GCC) -T$(LDSCRIPT) $(addprefix -L, $(LIBSPATH)) $(OBJECTS) $(LDFLAGS) -o $@

$(EXEC).hex: $(EXEC).elf
	@$(OBJCOPY) -O ihex -j .startup -j .text -j .data -j .ram_code -j .rodata $< $@

$(EXEC).bin: $(EXEC).elf $(CHECKSUM)
	@echo "Generating bin file" $@
	$(OBJCOPY) -O binary -j .startup -j .text -j .data -j .ram_code -j .rodata $< $@
#hd -n 128 $@
	$(CHECKSUM) $@

bin: $(EXEC).bin
hex: $(EXEC).hex

stats: $(EXEC).elf
	@echo "---- Sections Summary ---"
	@$(SIZE) -A -x $<
	@$(SIZE) -B $<

aslist: $(EXEC).elf
	@$(OBJDUMP) -D $< > $(EXEC).lst

clean:
#$(REMOVE) $(BUILD_DIR)/*.obj $(BUILD_DIR)/*.o *.hex *.elf *.bin	
	$(REMOVE) $(BUILD_DIR)  

rebuild: clean all

inline_asm:
	$(GCC) -Og -g -mcpu=cortex-m3 -mthumb -Wall -c inline_asm.c -o inline_asm.o
	$(GCC) -Tinline_asm.ld inline_asm.o -nostdlib -nostartfiles -o inline_asm.elf

$(EXEC).jlink: $(EXEC).hex
	@echo "Creating Jlink configuration file"
	@echo "loadfile $<" >> $@
	@echo "r" >> $@
	@echo "q" >> $@
	#@echo "erase" > $@
	
$(EXEC).cfg:
	@echo "Creating opencod configuration file"
	@echo "adapter driver jlink" > $(EXEC).cfg
	@echo "transport select swd" >> $(EXEC).cfg
	@echo "source [find target/lpc17xx.cfg]" >> $(EXEC).cfg
	@echo "adapter speed 4000" >> $(EXEC).cfg

program: $(EXEC).jlink
#openocd -f $(TARGET).cfg -c "program $(TARGET).bin 0x00000000 verify reset exit"
	$(VERBOSE)$(JLINK) -device $(DEVICE) -if SWD -speed auto -CommanderScript $<

$(BUILD_DIR):
	mkdir -p $@

$(CHECKSUM):
	$(MAKE) -C $(LIBEMB_PATH)/tools

minicom:
	minicom -b 115200 -D /dev/ttyACM0

#make command CMDNAME=<name>
TEMPLATEIN =cmdTemplate.in
L1 =en
SHELL := /bin/bash
command:	
	@cat  $(TEMPLATEIN) > inc/cmd$(CMDNAME).h
	@sed -i -- "s/%NAME%/$(CMDNAME)/g" inc/cmd$(CMDNAME).h
	@sed -i -- "s/%CLASSNAME%/$(shell L1=$(CMDNAME); echo $${L1^})/g" inc/cmd$(CMDNAME).h
	
#########################################################
# 
########################################################
$(BUILD_DIR)/%.o : %.c | $(BUILD_DIR)
	@echo "Compile" $<
	@$(GCC) $(GCFLAGS) $(addprefix -I, $(INCSPATH)) $(GCSYMBOLS) -c $< -o $@

$(BUILD_DIR)/%.obj : %.cpp | $(BUILD_DIR)
	@echo "Compile" $<
	@$(GPP) $(GPPFLAGS) $(addprefix -I, $(INCSPATH)) $(GCSYMBOLS) -c $< -o $@
	
$(BUILD_DIR)/%.o : %.S | $(BUILD_DIR)
	@echo "Assemble" $<
	@$(AS) $(ASFLAGS) $< -o $@

ifeq ($(GCC_COLORS),)
export GCC_COLORS='error=01;31:warning=01;35:note=01;36:locus=01:quote=01'
unexport GCC_COLORS
endif