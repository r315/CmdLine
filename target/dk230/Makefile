######################################
# target
######################################
TARGET =console-230dk

#######################################
# Features
#######################################

FEATURE_ENABLE +=ENABLE_I2C

#######################################
# paths
#######################################

BUILD_DIR 		:=build
TARGET_PATH 	=$(CURDIR)

APP_PATH        ?=../../App
LIB_PATH        =$(LIBEMB_PATH)/lib
CMSIS_PATH 		=$(LIBEMB_PATH)/CMSIS
DRIVER_PATH 	=$(LIBEMB_PATH)/drv
DRIVER_SOC_PATH =$(DRIVER_PATH)/SOC/gd32e23x
DRIVER_COMPONENT_PATH =$(LIBEMB_PATH)/component

#######################################
# Includes
#######################################
C_INCLUDES = \
$(TARGET_PATH)/inc \
$(DRIVER_PATH)/inc \
$(DRIVER_SOC_PATH)/inc \
$(CMSIS_PATH)/Core/Include \
$(CMSIS_PATH)/Device/Gigadevice/GD32E23x/inc \
$(LIB_PATH) \
$(LIB_PATH)/inc \
$(APP_PATH)/inc \
$(APP_DIR)/libs \
$(DRIVER_COMPONENT_PATH)/tft \

AS_INCLUDES =\

######################################
# Sources
######################################
DRIVER_SOURCES = \
$(CMSIS_PATH)/Device/Gigadevice/GD32E23x/src/startup_gd32e23x.c\
$(CMSIS_PATH)/Device/Gigadevice/GD32E23x/src/system_gd32e23x.c\
$(DRIVER_SOC_PATH)/src/gd32e23x_rcu.c \
$(DRIVER_SOC_PATH)/src/gd32e23x_gpio.c \
$(DRIVER_SOC_PATH)/src/gd32e23x_misc.c \
$(DRIVER_SOC_PATH)/src/gd32e23x_usart.c \
$(DRIVER_SOC_PATH)/src/gd32e23x_dma.c \
$(DRIVER_SOC_PATH)/src/gd32e23x_i2c.c \
$(DRIVER_SOC_PATH)/src/gd32e23x_timer.c \
$(DRIVER_PATH)/uart/uart_gd32e23x.c \
$(DRIVER_PATH)/wdt/wdt_dummy.c \
$(DRIVER_PATH)/i2c/i2c_gd32e230.c \

C_SOURCES = \
$(DRIVER_SOURCES) \
$(TARGET_PATH)/src/board_230dk.c \
$(TARGET_PATH)/src/serial_230dk.c \
$(TARGET_PATH)/src/main.c \
$(TARGET_PATH)/src/syscalls.c \
$(LIB_PATH)/src/strfunc.c \
$(LIB_PATH)/src/stimer.c \

CPP_SOURCES = \
$(LIB_PATH)/src/console.cpp \
$(APP_DIR)/src/app.cpp \
$(APP_DIR)/src/cmdbenchmark.cpp \
$(APP_DIR)/src/cmdi2c.cpp \
$(APP_DIR)/src/cmdds1086.cpp \
$(APP_DIR)/libs/ds1086/ds1086.cpp \

ASM_SOURCES = \

LDSCRIPT :=$(TARGET_PATH)/GD32E230_FLASH.ld
#######################################
# Misc
#######################################

OCD_CONFIG =$(TARGET_PATH)/gigadevice.cfg

# C defines
C_DEFS +=\
$(DEVICE) \
BOARD_230DK \
$(FEATURE_ENABLE) \

#######################################
# CFLAGS
#######################################

#DEVICE =ARMCM23
DEVICE =GD32E230C8T6
CPU =-mcpu=cortex-m23 -mthumb

# fpu
#at43f415 does not have FPU
#FPU =-mfloat-abi=hard -mfpu=fpv4-sp-d16
#FPU =-mfloat-abi=soft

# float-abi
FLOAT_ABI =#-u_printf_float

# mcu
MCU = $(CPU) $(FPU) $(FLOAT_ABI)

ifdef RELEASE
OPT   =-O2
else
OPT   =-Og -g -gdwarf-2 -Wall #-Wextra
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

OBJECTS = $(addprefix $(BUILD_DIR)/, $(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

OBJECTS += $(addprefix $(BUILD_DIR)/, $(notdir $(CPP_SOURCES:.cpp=.obj)))
vpath %.cpp $(sort $(dir $(CPP_SOURCES)))

OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

#VPATH +=$(sort $(dir $(C_SOURCES)))

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
JLK ="/opt/SEGGER/JLink/JLinkExe"
PRG_CFG = $(TARGET).elf
PRG_CMD = openocd -f at32f415.cfg -c "program $(BUILD_DIR)/$(TARGET).elf verify reset exit"
else
#JLK =jlink
#PRG_CFG = $(TARGET).jlink
#PRG_CMD = $(VERBOSE)$(JLK) -device $(DEVICE) -if SWD -speed auto -CommanderScript $(BUILD_DIR)/$(PRG_CFG)
#-device CORTEX-M4
PRG_CFG = $(TARGET).elf
PRG_CMD = openocd -f gigadevice.cfg -c "program $(BUILD_DIR)/$(TARGET).elf verify reset exit"
endif

#######################################
# Rules
#######################################
all: $(BUILD_DIR) $(BUILD_DIR)/$(TARGET).elf size
#@echo $(CMSIS_PATH)

bin: $(BUILD_DIR)/$(TARGET).bin

program: all $(BUILD_DIR)/$(PRG_CFG)
	$(PRG_CMD)

$(BUILD_DIR)/$(TARGET).jlink: $(BUILD_DIR)/$(TARGET).bin
	@echo "Creating Jlink configuration file"
	@echo "loadfile $< 0x08000000" > $@
	@echo "r" >> $@
	@echo "q" >> $@
#######################################
# build rules
#######################################
$(BUILD_DIR)/%.d: %.c
	$(VERBOSE)$(CC) $(CFLAGS) -MF"$@" -MG -MM -MP -MT"$@" -MT"$(<:.c=.o)" "$<"

$(BUILD_DIR)/%.o: %.c
	@echo "[CC]  $<"
	$(VERBOSE)$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.obj: %.cpp
	@echo "[CP]  $<"
	$(VERBOSE)$(CPP) -c $(CPPFLAGS)  $< -o $@

$(BUILD_DIR)/%.o: %.s
	@echo "[AS]  $<"
	$(VERBOSE)$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS)
	@echo "[LD]  $@"
	$(VERBOSE)$(LD) $(LIBDIR) -T$(LDSCRIPT) $(LDFLAGS) $^ $(LIBS) -o $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf
	$(VERBOSE)$(HEX) $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf
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
