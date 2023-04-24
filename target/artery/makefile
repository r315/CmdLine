######################################
# target
######################################
TARGET =console-artery

#######################################
# paths
#######################################
BUILD_DIR 		:=build
TARGET_PATH 	=$(CURDIR)

LIB_PATH        =$(LIBEMB_PATH)
DRIVERS_PATH 	=$(LIB_PATH)/drv
CMSIS_PATH 		=$(DRIVERS_PATH)/CMSIS

DRIVERS_PER_PATH =$(DRIVERS_PATH)/StdPeriph_Driver


C_INCLUDES = \
$(TARGET_PATH) \
$(DRIVERS_PER_PATH)/inc \
$(DRIVERS_PATH)/inc \
$(CMSIS_PATH)/Device/ARTERY/AT32F4xx/inc \
$(CMSIS_PATH)/Core/Include \
$(EMU_PATH)/inc \
$(LIB_PATH)/inc \

AS_INCLUDES =\

######################################
# sources
######################################

BOARD_SOURCES = \
$(CMSIS_PATH)/Device/ARTERY/AT32F4xx/src/startup_at32f415cbt7.c\
$(CMSIS_PATH)/Device/ARTERY/AT32F4xx/src/system_at32f4xx.c\
$(TARGET_PATH)/at_board.c \
$(TARGET_PATH)/at_main.c \
$(TARGET_PATH)/at_serial.c \
$(TARGET_PATH)/syscalls.c \
$(TARGET_PATH)/sysmem.c \
$(DRIVERS_PER_PATH)/src/at32f4xx_rcc.c \
$(DRIVERS_PATH)/src/spi_at32f4xx.c \
$(DRIVERS_PATH)/src/dma_at32f4xx.c \
$(DRIVERS_PATH)/src/gpio_at32f4xx.c \
$(DRIVERS_PATH)/src/uart_at32f4xx.c \
#$(DRIVERS_COMPONENT_PATH)/src/st7789.c \
#$(DRIVERS_COMPONENT_PATH)/src/ili9341.c \
#$(DRIVERS_COMPONENT_PATH)/src/st7735.c \


CSRCS = \
$(BOARD_SOURCES) \
$(LIB_PATH)/src/fifo.c \
#$(LIB_PATH)/src/liblcd.c \
$(LIB_PATH)/src/lib2d.c \
$(LIB_PATH)/src/font.c \
$(LIB_PATH)/src/libbutton.c \
$(EMU_PATH)/src/instrs.c \
$(EMU_PATH)/src/cartridge.c \
$(EMU_PATH)/src/dmgcpu.c \
$(EMU_PATH)/src/video.c \
$(EMU_PATH)/src/decoder.c \
$(EMU_PATH)/src/cgbmu.c \

# ASM sources
#ASM_SOURCES =startup_at32f415cbt7.s

#######################################
# Misc files
#######################################

# Linker script
LDSCRIPT :=$(TARGET_PATH)/AT32F415xB_FLASH.ld

# Openocd configuration
OCD_CONFIG =$(TARGET_PATH)/at32f415.cfg

# C defines
C_DEFS +=\
$(DEVICE) \
USE_STDPERIPH_DRIVER
#######################################
# binaries
#######################################
BINPATH = 
PREFIX = arm-none-eabi-
CC =$(BINPATH)$(PREFIX)gcc
CPP =$(BINPATH)$(PREFIX)g++
AS =$(BINPATH)$(PREFIX)gcc -x assembler-with-cpp
LD =$(BINPATH)$(PREFIX)gcc
AR =$(BINPATH)$(PREFIX)ar
SZ =$(BINPATH)$(PREFIX)size
OBJCOPY =$(BINPATH)$(PREFIX)objcopy
OBJDUMP =$(BINPATH)$(PREFIX)objdump
HEX = $(OBJCOPY) -O ihex
BIN = $(OBJCOPY) -O binary -S

ifeq ($(GCC_COLORS), )
export GCC_COLORS='error=01;31:warning=01;35:note=01;36:caret=01;32:locus=01:quote=01'
#unexport GCC_COLORS
endif

ifeq ($(shell uname -s), Linux)
JLK ="/opt/SEGGER/JLink/JLinkExe"
else
JLK =$(JLINK)/jlink
endif
#######################################
# CFLAGS
#######################################
# cpu
DEVICE =AT32F415CBT7
CPU =-mcpu=cortex-m4 -mthumb

# fpu
#at43f415 does not have FPU
#FPU =-mfloat-abi=hard -mfpu=fpv4-sp-d16 

# float-abi
FLOAT-ABI = #-u_printf_float

# mcu
MCU = $(CPU) $(FPU) $(FLOAT-ABI)

ifdef RELEASE
DEBUG =0
OPT   =-O2
else
DEBUG =1
OPT   =-O0 -g -gdwarf-2 -std=gnu11 -Wall -Wextra
endif


ifndef V
VERBOSE =@
else
VERBOSE =
endif

ifdef SEMIHOSTING
SPECS =--specs=rdimon.specs
LDLIBS =-nostartfiles -lc -lrdimon
else
SPECS =--specs=nano.specs
#-nostdlib -lstd++ -lnosys -lm 
LDLIBS =
endif

LDLIBS_PATH =
# Generate dependency information
#CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst))

CFLAGS   =$(MCU) $(OPT) $(addprefix -D, $(C_DEFS)) $(addprefix -I, $(C_INCLUDES)) -fdata-sections -ffunction-sections

CPPFLAGS =$(CFLAGS)
ASFLAGS  =$(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections
LDFLAGS  =$(MCU) $(SPECS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref,--gc-sections

#######################################
# Objects
#######################################

OBJECTS = $(addprefix $(BUILD_DIR)/, $(notdir $(CSRCS:.c=.o)))
#vpath %.c $(sort $(dir $(CSRCS)))
#OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(CPP_SOURCES:.cpp=.obj)))
#vpath %.cpp $(sort $(dir $(CPP_SOURCES)))
#OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
#vpath %.s $(sort $(dir $(ASM_SOURCES)))

VPATH +=$(sort $(dir $(CSRCS)))
#######################################
# Rules
#######################################
# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf size
#@echo $(CMSIS_PATH)

bin: $(BUILD_DIR)/$(TARGET).bin

program: $(BUILD_DIR)/$(TARGET).jlink bin
	$(VERBOSE)$(JLK) -device $(DEVICE) -if SWD -speed auto -CommanderScript $<

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@echo "[CC]  $<"
	$(VERBOSE)$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.obj: %.cpp | $(BUILD_DIR)
	@echo "[CP]  $<"
	$(VERBOSE)$(CPP) -c $(CPPFLAGS)  -fno-exceptions -fno-unwind-tables -fno-rtti $< -o $@

$(BUILD_DIR)/%.o: %.s | $(BUILD_DIR)
	@echo "[AS]  $<"
	$(VERBOSE)$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) $(ROM_OBJ)
	@echo "[LD]  $@"
	$(VERBOSE)$(LD) -T$(LDSCRIPT) $(LDFLAGS) $(LDOBJECTS) $^ -o $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(VERBOSE)$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(VERBOSE)$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir -p $@		

size: $(BUILD_DIR)/$(TARGET).elf
	@echo "--- Size ---"
	$(VERBOSE)$(SZ) -A -x $<
	$(VERBOSE)$(SZ) -B $<

$(BUILD_DIR)/$(TARGET).jlink: $(BUILD_DIR)/$(TARGET).bin
	@echo "Creating Jlink configuration file"
	@echo "loadfile $< 0x00000000" > $@
	@echo "r" >> $@
	@echo "q" >> $@
#######################################
# clean up
#######################################
clean:
	$(VERBOSE)-rm -fR $(BUILD_DIR)

#######################################
# dependencies
#######################################
#-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)

# *** EOF ***
