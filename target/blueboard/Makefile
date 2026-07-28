
#########################################################
# project files
#########################################################
TARGET 		=console-blueboard
EXEC		=$(BUILD_DIR)/$(TARGET)
BOARD       =BOARD_BLUEBOARD
DBG_PROBE   ="JLINK"
#######################################
# Features
#######################################
FEATURE_ENABLE +=ENABLE_USB_VCP
#FEATURE_ENABLE +=ENABLE_DISPLAY
#FEATURE_ENABLE +=ENABLE_AVR_PROGRAMMING
FEATURE_ENABLE +=ENABLE_BENCHMARK
FEATURE_ENABLE +=ENABLE_BENCHMARK_DHRYSTONE MSC_CLOCK
FEATURE_ENABLE +=ENABLE_BENCHMARK_COREMARK

#######################################
# paths
#######################################

TARGET_PATH  =$(CURDIR)
BUILD_DIR 	 :=build

LIB_PATH 	 =$(LIBEMB_PATH)/lib
DRIVER_PATH  =$(LIBEMB_PATH)/drv
CMSIS_PATH   =$(LIBEMB_PATH)/CMSIS
DRIVER_COMPONENT_PATH =$(LIBEMB_PATH)/component
BSP_PATH     =$(LIBEMB_PATH)/bsp/blueboard

#######################################
# Includes
#######################################
C_INCLUDES =\
$(TARGET_PATH)/inc \
$(TARGET_PATH)/usbdrv \
$(APP_DIR)/inc \
$(APP_DIR)/libs \
$(APP_DIR)/libs/minimp3 \
$(APP_DIR)/libs/agif \
$(APP_DIR)/libs/stk500 \
$(APP_DIR)/libs/si5351 \
$(APP_DIR)/libs/ds1086 \
$(APP_DIR)/libs/dhrystone \
$(LIB_PATH) \
$(LIB_PATH)/inc \
$(DRIVER_PATH)/inc \
$(CMSIS_PATH)/Device/NXP/LPC17xx/inc \
$(CMSIS_PATH)/Core/Include \
$(DRIVER_COMPONENT_PATH)/display \
$(BSP_PATH) \

######################################
# Sources
######################################
TARGET_SOURCES =\
$(CMSIS_PATH)/Device/NXP/LPC17xx/src/startup_LPC17xx.c \
$(CMSIS_PATH)/Device/NXP/LPC17xx/src/system_LPC17xx.c \
$(DRIVER_PATH)/clock/clock_lpc17xx.c \
$(DRIVER_PATH)/gpio/gpio_lpc17xx.c \
$(DRIVER_PATH)/dma/dma_lpc17xx.c \
$(DRIVER_PATH)/uart/uart_lpc17xx.c \
$(DRIVER_PATH)/timer/tim_lpc17xx.c \
$(DRIVER_PATH)/wdt/wdt_dummy.c \
$(BSP_PATH)/blueboard.c \
$(TARGET_PATH)/src/sysmem.c \
$(TARGET_PATH)/src/syscalls.c \
$(TARGET_PATH)/src/bb_serial.c \
$(TARGET_PATH)/src/bb_main.c \

APP_SOURCES =\
$(LIB_PATH)/src/strfunc.c \
$(LIB_PATH)/src/fifo.c \
$(LIB_PATH)/src/stimer.c \

#$(DRIVER_PATH)/pwm/pwm_lpc17xx.c \
$(DRIVER_PATH)/i2c/i2c_lpc17xx.c \
$(DRIVER_PATH)/i2s/i2s_lpc17xx.c \
$(DRIVER_PATH)/spi/spi_lpc17xx.c \
$(DRIVER_PATH)/dac/dac_lpc17xx.c \
$(DRIVER_PATH)/adc/adc_lpc17xx.c \
$(DRIVER_PATH)/storage/memcard.c \
$(LIB_PATH)/src/debug.c \
$(LIB_PATH)/src/button.c \
$(LIB_PATH)/pff/pff.c \
$(APP_DIR)/libs/stk500/avr_if.c \
$(APP_DIR)/libs/stk500/stk500.c \

CPP_SOURCES = \
$(LIB_PATH)/src/console.cpp \
$(APP_DIR)/src/app.cpp \
#$(APP_DIR)/src/cmdi2s.cpp \
$(APP_DIR)/src/cmdgpio.cpp \
$(APP_DIR)/src/cmdplayer.cpp \
$(APP_DIR)/src/cmdmem.cpp \
$(TARGET_PATH)/src/cmdspi.cpp \
$(TARGET_PATH)/src/cmdlsd.cpp \
$(TARGET_PATH)/src/cmdpwm.cpp \
$(TARGET_PATH)/src/cmdawg.cpp \
$(TARGET_PATH)/src/cmdadc.cpp \
$(TARGET_PATH)/src/cmdi2c.cpp \
$(TARGET_PATH)/src/cmdavr.cpp \
#$(APP_DIR)/src/cmdsbus.cpp \
$(APP_DIR)/libs/stk500/stk500_service.cpp \
$(APP_DIR)/src/cmdmsensor.cpp \

C_SOURCES =\
$(TARGET_SOURCES) \
$(APP_SOURCES)

ifneq ($(filter ENABLE_USB_VCP, $(FEATURE_ENABLE)),)
C_SOURCES +=\
$(TARGET_PATH)/usbdrv/usbserial.c \
$(TARGET_PATH)/usbdrv/usbhw_lpc.c \
$(TARGET_PATH)/usbdrv/usbcontrol.c \
$(TARGET_PATH)/usbdrv/usbstdreq.c
endif

ifneq ($(filter ENABLE_DISPLAY, $(FEATURE_ENABLE)),)
C_SOURCES +=\
$(DRIVER_COMPONENT_PATH)/display/ili9328.c \
$(LIB_PATH)/src/font.c \
$(LIB_PATH)/src/lib2d.c \
$(DRIVER_PATH)/rng/rng_software.c

CPP_SOURCES +=$(APP_DIR)/src/cmdtft.cpp
endif

ifneq ($(filter ENABLE_DEMO_GIF, $(FEATURE_ENABLE)),)
C_SOURCES +=$(APP_DIR)/libs/agif/agif.c
CPP_SOURCES +=$(APP_DIR)/libs/agif/AnimatedGIF.cpp
endif

ifneq ($(filter ENABLE_BENCHMARK,$(FEATURE_ENABLE)),)
CPP_SOURCES +=$(APP_DIR)/src/cmdbenchmark.cpp
endif

ifneq ($(filter ENABLE_BENCHMARK_DHRYSTONE,$(FEATURE_ENABLE)),)
C_SOURCES +=\
$(APP_DIR)/libs/dhrystone/dhry_1.c \
$(APP_DIR)/libs/dhrystone/dhry_2.c
endif

ifneq ($(filter ENABLE_BENCHMARK_COREMARK,$(FEATURE_ENABLE)),)
C_SOURCES +=\
$(APP_DIR)/libs/CoreMark/core_main.c \
$(APP_DIR)/libs/CoreMark/core_state.c \
$(APP_DIR)/libs/CoreMark/core_util.c \
$(APP_DIR)/libs/CoreMark/core_list_join.c \
$(APP_DIR)/libs/CoreMark/core_matrix.c
endif

LDSCRIPT =$(TARGET_PATH)/lpc1768.ld

#######################################
# Objects
#######################################

OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(CPP_SOURCES:.cpp=.obj)))
vpath %.cpp $(sort $(dir $(CPP_SOURCES)))

OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

CHECKSUM =$(LIBEMB_PATH)/tools/checksum

#######################################
# Misc
#######################################
DEVICE =LPC1768
CPU =-mcpu=cortex-m3 -mthumb
OPT =-O0 -g -Wall -Wextra

ifneq ($(V),1)
VERBOSE =@
endif

#######################################
# CFLAGS
#######################################

C_DEFS = \
$(BOARD) \
$(foreach feature, $(FEATURE_ENABLE), $(feature)) \
CONSOLE_WIDTH=128 \

GCFLAGS =$(CPU) $(OPT) -fdata-sections -ffunction-sections -fno-unwind-tables #-fstack-usage
GPPFLAGS=$(GCFLAGS) -fno-exceptions -fno-rtti

#######################################
# Libraries
#######################################

SPECS=--specs=nosys.specs --specs=nano.specs
LIBS =-lstdc++
#LIBS +=-nostdlib
LIBSPATH +=
LDFLAGS =$(CPU) $(OPT) $(SPECS) -Wl,--gc-sections,-Map=$(EXEC).map,--cref

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

ifeq ($(DBG_PROBE),"JLINK")
# jlink works on valid Jlink probes
ifeq ($(shell uname -s),Linux)
JLK =JLinkExe
else
JLK =JLink
endif
PRG_DEP =$(EXEC).bin $(PRG_CFG)
PRG_CFG =$(BUILD_DIR)/blueboard.jlink
PRG_CMD =$(JLK) -device $(DEVICE) -if SWD -speed auto -CommanderScript $(PRG_CFG)
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
	$(VERBOSE)$(GCC) $(LDFLAGS) -T$(LDSCRIPT) $(addprefix -L, $(LIBSPATH)) $(OBJECTS) $(LIBS) -o $@

$(EXEC).hex: $(EXEC).elf
	$(VERBOSE)$(OBJCOPY) -O ihex -j .startup -j .text -j .data -j .ram_code -j .rodata $< $@

$(EXEC).bin: $(EXEC).elf #$(CHECKSUM)
	@echo "Generating bin file" $@
	$(VERBOSE)$(OBJCOPY) -O binary -j .startup -j .text -j .data -j .ram_code -j .rodata $< $@
#hd -n 128 $@
	$(VERBOSE)$(CHECKSUM) $@

bin: $(EXEC).bin
hex: $(EXEC).hex

stats: $(EXEC).elf
	@echo "---- Sections Summary ---"
	$(VERBOSE)$(SIZE) -A -x $<
	$(VERBOSE)$(SIZE) -B $<

aslist: $(EXEC).elf
	$(VERBOSE)$(OBJDUMP) -D $< > $(EXEC).lst

clean:
#$(REMOVE) $(BUILD_DIR)/*.obj $(BUILD_DIR)/*.o *.hex *.elf *.bin
	$(VERBOSE)$(REMOVE) $(BUILD_DIR)

rebuild: clean all


$(BUILD_DIR)/blueboard.jlink: $(EXEC).hex
	@echo "Creating Jlink configuration file"
	@echo "loadfile $<" > $@
	@echo "r" >> $@
	@echo "q" >> $@
	@echo "erase" >> $@

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
	$(VERBOSE)$(GCC) $(GCFLAGS) $(addprefix -I, $(C_INCLUDES)) $(addprefix -D, $(C_DEFS)) -c $< -o $@

$(BUILD_DIR)/%.obj : %.cpp Makefile | $(BUILD_DIR)
	@echo "[CXX]  $<"
	$(VERBOSE)$(GPP) $(GPPFLAGS) $(addprefix -I, $(C_INCLUDES)) $(addprefix -D, $(C_DEFS)) -c $< -o $@

$(BUILD_DIR)/%.o : %.S | $(BUILD_DIR)
	@echo "[AS]  $<"
	$(VERBOSE)$(AS) $(ASFLAGS) $< -o $@

ifeq ($(GCC_COLORS),)
export GCC_COLORS='error=01;31:warning=01;35:note=01;36:locus=01:quote=01'
unexport GCC_COLORS
endif
