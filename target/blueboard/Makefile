
#########################################################
# project files
#########################################################
TARGET 		=console-blueboard
EXEC		=$(BUILD_DIR)/$(TARGET)

#######################################
# Features
#######################################

FEATURE_ENABLE = \
ENABLE_AVR_PROGRAMMING \
#ENABLE_BENCHMAK \
ENABLE_TFT_DISPLAY \
ENABLE_BENCHMAK_DHYSTONE \
ENABLE_BENCHMAK_DHYSTONE \
ENABLE_DEBUG MSC_CLOCK \

#######################################
# paths
#######################################

TARGET_PATH  =$(CURDIR)
BUILD_DIR 	 :=build

LIB_PATH 	 =$(LIBEMB_PATH)
DRIVERS_PATH =$(LIBEMB_PATH)/drv
CMSIS_PATH   =$(DRIVERS_PATH)/CMSIS
CMSIS_DEVICE =$(CMSIS_PATH)/Device/NXP/LPC17xx

#######################################
# Includes
#######################################
C_INCLUDES =\
$(TARGET_PATH)/inc \
$(TARGET_PATH)/usbdrv \
$(APP_DIR)/inc \
$(APP_DIR)/libs/minimp3 \
$(APP_DIR)/libs/agif \
$(APP_DIR)/libs/stk500 \
$(APP_DIR)/libs/si5351 \
$(APP_DIR)/libs/ds1086 \
$(LIB_PATH)/inc \
$(LIB_PATH)/pff \
$(DRIVERS_PATH)/inc \
$(CMSIS_DEVICE)/inc \
$(CMSIS_PATH)/Core/Include \

######################################
# Sources
######################################
C_SOURCES =\
$(CMSIS_DEVICE)/src/startup_LPC17xx.c \
$(CMSIS_DEVICE)/src/system_LPC17xx.c \
$(TARGET_PATH)/src/blueboard.c \
$(TARGET_PATH)/src/sysmem.c \
$(TARGET_PATH)/src/syscalls.c \
$(DRIVERS_PATH)/clock/clock_lpc17xx.c \
$(DRIVERS_PATH)/timer/tim_lpc17xx.c \
$(DRIVERS_PATH)/tft/ili9328.c \
$(DRIVERS_PATH)/pwm/pwm_lpc17xx.c \
$(DRIVERS_PATH)/i2c/i2c_lpc17xx.c \
$(DRIVERS_PATH)/i2s/i2s_lpc17xx.c \
$(DRIVERS_PATH)/spi/spi_lpc17xx.c \
$(DRIVERS_PATH)/dac/dac_lpc17xx.c \
$(DRIVERS_PATH)/gpio/gpio_lpc17xx.c \
$(DRIVERS_PATH)/uart/uart_lpc17xx.c \
$(DRIVERS_PATH)/dma/dma_lpc17xx.c \
$(DRIVERS_PATH)/adc/adc_lpc17xx.c \
$(DRIVERS_PATH)/rng/rng_software.c \
$(DRIVERS_PATH)/storage/memcard.c \
$(LIB_PATH)/src/font.c \
$(LIB_PATH)/src/fifo.c \
$(LIB_PATH)/src/strfunc.c \
$(LIB_PATH)/src/debug.c \
$(LIB_PATH)/src/button.c \
$(LIB_PATH)/pff/pff.c \
$(TARGET_PATH)/usbdrv/usbserial.c \
$(TARGET_PATH)/usbdrv/usbhw_lpc.c \
$(TARGET_PATH)/usbdrv/usbcontrol.c \
$(TARGET_PATH)/usbdrv/usbstdreq.c \
$(TARGET_PATH)/src/bb_serial.c \
$(TARGET_PATH)/src/bb_main.c \
$(APP_DIR)/libs/stk500/avr_if.c \
$(APP_DIR)/libs/stk500/stk500.c \
#$(LIBEMB_PATH)/drv/lib2d.c \

CPP_SOURCES = \
$(LIB_PATH)/src/console.cpp \
$(APP_DIR)/src/app.cpp \
$(APP_DIR)/src/cmdi2s.cpp \
$(APP_DIR)/src/cmdgpio.cpp \
$(APP_DIR)/src/cmdplayer.cpp \
$(APP_DIR)/src/cmdmem.cpp \
$(APP_DIR)/src/cmdtft.cpp \
$(APP_DIR)/src/cmdbenchmark.cpp \
$(TARGET_PATH)/src/cmdspi.cpp \
$(TARGET_PATH)/src/cmdlsd.cpp \
$(TARGET_PATH)/src/cmdpwm.cpp \
$(TARGET_PATH)/src/cmdawg.cpp \
$(TARGET_PATH)/src/cmdadc.cpp \
$(TARGET_PATH)/src/cmdi2c.cpp \
$(TARGET_PATH)/src/cmdavr.cpp \
#$(APP_DIR)/src/cmdsbus.cpp \
$(APP_DIR)/src/cmdbenchmark.cpp \
$(APP_DIR)/libs/stk500/stk500_service.cpp \
$(APP_DIR)/src/cmdmsensor.cpp \

LDSCRIPT =$(TARGET_PATH)/lpc1768.ld
#######################################
# Misc
#######################################

C_DEFS = \
BOARD_BLUEBOARD \
_ENABLE_BUTTONS \
_ENABLE_SDCARD \

C_DEFS +=$(foreach feature, $(FEATURE_ENABLE), $(feature))

#######################################
# CFLAGS
#######################################

DEVICE =LPC1768
CPU =-mcpu=cortex-m3 -mthumb

OPT =-O0 -g

GCFLAGS =$(CPU) $(OPT) -Wall -fdata-sections -ffunction-sections -fno-unwind-tables #-fstack-usage
GPPFLAGS=$(GCFLAGS) -fno-exceptions -fno-rtti

#######################################
# Libraries
#######################################

SPECS=--specs=nosys.specs --specs=nano.specs #-u _printf_float 
LIBS =-lstdc++
#LIBS +=-nostdlib
LIBSPATH +=
LDFLAGS =$(CPU) $(OPT) $(SPECS) -Wl,--gc-sections,-Map=$(EXEC).map,--cref

#######################################
# Objects
#######################################

OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(CPP_SOURCES:.cpp=.obj)))
vpath %.cpp $(sort $(dir $(CPP_SOURCES)))

OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

CHECKSUM =$(LIB_PATH)/tools/checksum

#######################################
# Tool binaries
#######################################
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

ifeq ($(JLINK_PROBE),1)
# jlink works on valid Jlink probes
PRG_DEP =$(PRG_CFG)
PRG_CFG =$(BUILD_DIR)/blueboard.jlink
PRG_CMD =JLinkExe -device $(DEVICE) -if SWD -speed auto -CommanderScript $(PRG_CFG)
else
# openocd works but requires checksum
# jlink probe driver has to be change to libusb
# works fine for st-link probe
PRG_DEP =$(EXEC).bin $(PRG_CFG)
PRG_CFG =$(BUILD_DIR)/blueboard_st-link.cfg
PRG_CMD =openocd -f $(PRG_CFG) -c "program $(EXEC).bin 0x00000000 verify reset exit"
endif

##########################################################
# RULES
##########################################################
all: $(EXEC).elf stats

$(EXEC).elf: $(OBJECTS)
	@echo "Linking" $@
	@$(GCC) $(LDFLAGS) -T$(LDSCRIPT) $(addprefix -L, $(LIBSPATH)) $(OBJECTS) $(LIBS) -o $@

$(EXEC).hex: $(EXEC).elf
	@$(OBJCOPY) -O ihex -j .startup -j .text -j .data -j .ram_code -j .rodata $< $@

$(EXEC).bin: $(EXEC).elf #$(CHECKSUM)
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


$(BUILD_DIR)/blueboard.jlink: $(EXEC).hex
	@echo "Creating Jlink configuration file"
	@echo "loadfile $<" >> $@
	@echo "r" >> $@
	@echo "q" >> $@
	#@echo "erase" > $@
	
$(BUILD_DIR)/blueboard.cfg:
	@echo "Creating opencod configuration file"
	@echo "adapter driver jlink" > $(EXEC).cfg
	@echo "transport select swd" >> $(EXEC).cfg
	@echo "source [find target/lpc17xx.cfg]" >> $(EXEC).cfg
	@echo "adapter speed 4000" >> $(EXEC).cfg

$(BUILD_DIR)/blueboard_st-link.cfg:
	@echo "Creating opencod configuration file"
	@echo "source [find interface/stlink.cfg]" >> $@
	@echo "transport select hla_swd" >> $@
	@echo "source [find target/lpc17xx.cfg]" >> $@
	@echo "adapter speed 4000" >> $@

program: $(PRG_DEP)
	$(VERBOSE)$(PRG_CMD)

$(BUILD_DIR):
	mkdir -p $@

#$(CHECKSUM):
#	$(MAKE) -C $(LIB_PATH)/tools checksum

minicom:
	minicom -b 115200 -D /dev/ttyACM0
	
#######################################
# build rules
#######################################
$(BUILD_DIR)/%.o : %.c Makefile | $(BUILD_DIR)
	@echo "[CC]  $<"
	@$(GCC) $(GCFLAGS) $(addprefix -I, $(C_INCLUDES)) $(addprefix -D, $(C_DEFS)) -c $< -o $@

$(BUILD_DIR)/%.obj : %.cpp Makefile | $(BUILD_DIR)
	@echo "[CC]  $<"
	@$(GPP) $(GPPFLAGS) $(addprefix -I, $(C_INCLUDES)) $(addprefix -D, $(C_DEFS)) -c $< -o $@
	
$(BUILD_DIR)/%.o : %.S | $(BUILD_DIR)
	@echo "[AS]  $<"
	@$(AS) $(ASFLAGS) $< -o $@

ifeq ($(GCC_COLORS),)
export GCC_COLORS='error=01;31:warning=01;35:note=01;36:locus=01:quote=01'
unexport GCC_COLORS
endif
