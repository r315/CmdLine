######################################
# target
######################################
TARGET =console-artery

#######################################
# Features
#######################################

FEATURE_ENABLE = \
ENABLE_TFT_DISPLAY \
ENABLE_BENCHMAK \
ENABLE_BENCHMAK_DHYSTONE \
_ENABLE_DIGITAL_AUDIO \
ENABLE_I2C \
ENABLE_RGBLED \

#######################################
# paths
#######################################

BUILD_DIR 		:=build
TARGET_PATH 	=$(CURDIR)

APP_DIR			?=../../App
LIB_PATH        =$(LIBEMB_PATH)
DRIVERS_PATH 	=$(LIB_PATH)/drv
CMSIS_PATH 		=$(DRIVERS_PATH)/CMSIS
DRIVERS_PER_PATH =$(DRIVERS_PATH)/StdPeriph_Driver

#######################################
# Includes
#######################################
C_INCLUDES = \
$(TARGET_PATH) \
$(DRIVERS_PER_PATH)/inc \
$(DRIVERS_PATH)/inc \
$(CMSIS_PATH)/Device/ARTERY/AT32F4xx/inc \
$(CMSIS_PATH)/Core/Include \
$(LIB_PATH)/inc \
$(APP_DIR)/inc \
$(APP_DIR)/libs/si5351 \
$(APP_DIR)/libs/ds1086 \
$(APP_DIR)/libs/ws2812b \

AS_INCLUDES =\

######################################
# Sources
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
$(DRIVERS_PER_PATH)/src/at32f4xx_i2c.c \
$(DRIVERS_PER_PATH)/src/at32f4xx_i2c_ex.c \
$(DRIVERS_PATH)/spi/spi_at32f4xx.c \
$(DRIVERS_PATH)/dma/dma_at32f4xx.c \
$(DRIVERS_PATH)/gpio/gpio_at32f4xx.c \
$(DRIVERS_PATH)/uart/uart_at32f4xx.c \
$(DRIVERS_PATH)/tft/st7735.c \
$(DRIVERS_PATH)/i2s/i2s_at32f4xx.c \
$(DRIVERS_PATH)/i2c/i2c_at32f415.c \

CSRCS = \
$(BOARD_SOURCES) \
$(LIB_PATH)/src/fifo.c \
$(LIB_PATH)/src/strfunc.c \
$(DRIVERS_PATH)/rng/rng_software.c \
$(LIB_PATH)/src/liblcd.c \
$(LIB_PATH)/src/lib2d.c \
$(LIB_PATH)/src/font.c \
$(APP_DIR)/libs/ws2812b/ws2812b.c \


CPP_SOURCES = \
$(LIB_PATH)/src/console.cpp \
$(APP_DIR)/src/app.cpp \
$(APP_DIR)/src/cmdmem.cpp \
$(APP_DIR)/src/cmdtft.cpp \
$(APP_DIR)/src/cmdbenchmark.cpp \
$(APP_DIR)/src/cmdi2s.cpp \
$(APP_DIR)/src/cmdi2c.cpp \
$(APP_DIR)/src/cmdrgbled.cpp \

LDSCRIPT :=$(TARGET_PATH)/AT32F415xB_FLASH.ld
#######################################
# Misc
#######################################



# C defines
C_DEFS +=\
BOARD_415DK \
$(DEVICE) \
USE_STDPERIPH_DRIVER \
FIFO_INTERRUPT_SAFE \

C_DEFS +=$(foreach feature, $(FEATURE_ENABLE), $(feature))

#######################################
# CFLAGS
#######################################

DEVICE =AT32F415CBT7
CPU =-mcpu=cortex-m4 -mthumb

# fpu
#at43f415 does not have FPU
#FPU =-mfloat-abi=hard -mfpu=fpv4-sp-d16 
#FPU =-mfloat-abi=soft

# float-abi
FLOAT_ABI =#-u_printf_float

# mcu
MCU = $(CPU) $(FPU) $(FLOAT_ABI)

ifdef RELEASE
DEBUG =0
OPT   =-O2
else
DEBUG =1
OPT   =-O0 -g -gdwarf-2 -Wall #-Wextra
endif

ifndef V
VERBOSE =@
else
VERBOSE =
endif

# Generate dependency information
#CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst))

ASFLAGS  =$(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections
CFLAGS   =$(MCU) $(OPT) $(addprefix -D, $(C_DEFS)) $(addprefix -I, $(C_INCLUDES)) -std=gnu11 -fdata-sections -ffunction-sections #-fstack-usage
CPPFLAGS =$(CPU) $(OPT) $(addprefix -D, $(C_DEFS)) $(addprefix -I, $(C_INCLUDES)) -fdata-sections -ffunction-sections -fno-unwind-tables -fno-exceptions -fno-rtti 
LDFLAGS  =$(MCU) $(SPECS) -Wl,--gc-sections,-Map=$(BUILD_DIR)/$(TARGET).map,--cref

#######################################
# Libraries
#######################################

LIBDIR =
ifdef SEMIHOSTING
SPECS =--specs=rdimon.specs
LDLIBS =-nostartfiles -lc -lrdimon
else
SPECS=--specs=nosys.specs --specs=nano.specs
#-nostdlib -lstd++ -lnosys -lm 
LIBS =-lstdc++
endif

#######################################
# Objects
#######################################

OBJECTS = $(addprefix $(BUILD_DIR)/, $(notdir $(CSRCS:.c=.o)))
vpath %.c $(sort $(dir $(CSRCS)))

OBJECTS += $(addprefix $(BUILD_DIR)/, $(notdir $(CPP_SOURCES:.cpp=.obj)))
vpath %.cpp $(sort $(dir $(CPP_SOURCES)))
#OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
#vpath %.s $(sort $(dir $(ASM_SOURCES)))

#VPATH +=$(sort $(dir $(CSRCS)))

#######################################
# Tool binaries
#######################################
PREFIX = arm-none-eabi-
CC  =$(PREFIX)gcc
CPP =$(PREFIX)g++
AS  =$(PREFIX)gcc -x assembler-with-cpp
LD  =$(PREFIX)gcc
AR  =$(PREFIX)ar
SZ  =$(PREFIX)size
OBJCOPY =$(PREFIX)objcopy
OBJDUMP =$(PREFIX)objdump
HEX = $(OBJCOPY) -O ihex
BIN = $(OBJCOPY) -O binary -S

ifeq ($(shell uname -s), Linux)
#JLK ="/opt/SEGGER/JLink/JLinkExe"
PRG_DEP =bin
PRG_CFG =$(TARGET_PATH)/at32f415.cfg
PRG_CMD =openocd -f $(PRG_CFG) -c "program $(BUILD_DIR)/$(TARGET).elf verify reset exit"
ERASE_CMD =openocd -f $(PRG_CFG) -c "init" -c "halt" -c "stm32f1x unlock 0" -c "stm32f1x mass_erase 0" -c "exit"
else
JLK :=JLink
PRG_DEP =bin $(PRG_CFG)
PRG_CFG =$(BUILD_DIR)/$(TARGET).jlink
PRG_CMD =$(VERBOSE)$(JLK) -device $(DEVICE) -if SWD -speed auto -CommanderScript $(PRG_CFG)
endif

#######################################
# Rules
#######################################
all: $(BUILD_DIR)/$(TARGET).elf size
#@echo $(CMSIS_PATH)

bin: $(BUILD_DIR)/$(TARGET).bin

program: $(PRG_DEP)
	$(PRG_CMD)

$(BUILD_DIR)/$(TARGET).jlink: $(BUILD_DIR)/$(TARGET).bin
	@echo "Creating Jlink configuration file"
	@echo "loadfile $< 0x08000000" > $@
	@echo "r" >> $@
	@echo "q" >> $@
#######################################
# build rules
#######################################
$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	@echo "[CC]  $<"
	$(VERBOSE)$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.obj: %.cpp Makefile | $(BUILD_DIR)
	@echo "[CP]  $<"
	$(VERBOSE)$(CPP) -c $(CPPFLAGS)  $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	@echo "[AS]  $<"
	$(VERBOSE)$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) | Makefile
	@echo "[LD]  $@"
	$(VERBOSE)$(LD) $(LIBDIR) -T$(LDSCRIPT) $(LDFLAGS) $^ $(LIBS) -o $@

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

#######################################
# clean up
#######################################
clean:
	$(VERBOSE)-rm -fR $(BUILD_DIR)

#######################################
# dependencies
#######################################
#-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)
ifeq ($(GCC_COLORS), )
export GCC_COLORS='error=01;31:warning=01;35:note=01;36:caret=01;32:locus=01:quote=01'
#unexport GCC_COLORS
endif

# *** EOF ***
