######################################
# target
######################################
TARGET =console-dk415
BOARD =BOARD_415DK
#######################################
# Features
#######################################

#FEATURE_ENABLE +=ENABLE_AVR_PROGRAMMING
#FEATURE_ENABLE +=ENABLE_SPI
FEATURE_ENABLE +=ENABLE_I2C
#FEATURE_ENABLE +=ENABLE_FLASH ENABLE_FLASH_SPI
FEATURE_ENABLE +=ENABLE_BENCHMARK
FEATURE_ENABLE +=ENABLE_BENCHMARK_DHRYSTONE MSC_CLOCK
FEATURE_ENABLE +=ENABLE_BENCHMARK_COREMARK
FEATURE_ENABLE +=ENABLE_RPM
FEATURE_ENABLE +=ENABLE_IR ENABLE_IR_DUPLEX
FEATURE_ENABLE +=ENABLE_DEBUG

#######################################
# paths
#######################################

BUILD_DIR 		:=build
TARGET_PATH 	=$(CURDIR)

APP_DIR			?=../../App
LIB_PATH        =$(LIBEMB_PATH)/lib
DRIVER_PATH 	=$(LIBEMB_PATH)/drv
DRIVER_COMPONENT_PATH =$(LIBEMB_PATH)/component
CMSIS_PATH 		=$(LIBEMB_PATH)/CMSIS

#######################################
# Includes
#######################################
C_INCLUDES = \
$(TARGET_PATH) \
$(DRIVER_PATH)/inc \
$(CMSIS_PATH)/Device/ARTERY/AT32F4xx/inc \
$(CMSIS_PATH)/Core/Include \
$(LIB_PATH) \
$(LIB_PATH)/inc \
$(APP_DIR)/inc \
$(APP_DIR)/libs \
$(DRIVER_COMPONENT_PATH)/display \

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

LIB_SOURCES = \
$(LIB_PATH)/src/fifo.c \
$(LIB_PATH)/src/strfunc.c \
$(LIB_PATH)/src/stimer.c \
$(LIB_PATH)/src/debug.c \

DRV_SOURCES = \
$(DRIVER_PATH)/clock/clock_at32f4xx.c \
$(DRIVER_PATH)/dma/dma_at32f4xx.c \
$(DRIVER_PATH)/gpio/gpio_at32f4xx.c \
$(DRIVER_PATH)/uart/uart_at32f4xx.c \
$(DRIVER_PATH)/wdt/wdt_dummy.c \

APP_SOURCES = \
$(LIB_PATH)/src/console.cpp \
$(APP_DIR)/src/app.cpp \
$(APP_DIR)/src/cmdmem.cpp \

ifneq ($(filter ENABLE_DISPLAY, $(FEATURE_ENABLE)),)
DRV_SOURCES += \
$(DRIVER_COMPONENT_PATH)/display/st7789.c \
$(DRIVER_PATH)/rng/rng_software.c

LIB_SOURCES += \
$(LIB_PATH)/src/liblcd.c \
$(LIB_PATH)/src/lib2d.c \
$(LIB_PATH)/src/font.c

APP_SOURCES +=$(APP_DIR)/src/cmdtft.cpp
endif

ifneq ($(filter ENABLE_I2C, $(FEATURE_ENABLE)),)
DRV_SOURCES +=$(DRIVER_PATH)/i2c/i2c_at32f4xx.c
APP_SOURCES +=$(APP_DIR)/src/cmdi2c.cpp
endif

ifneq ($(filter ENABLE_I2S, $(FEATURE_ENABLE)),)
DRV_SOURCES += $(DRIVER_PATH)/i2s/i2s_at32f4xx.c
APP_SOURCES += $(APP_DIR)/src/cmdi2s.cpp
endif

ifneq ($(filter ENABLE_SPI, $(FEATURE_ENABLE)),)
DRV_SOURCES += $(DRIVER_PATH)/spi/spi_at32f4xx.c
APP_SOURCES += $(APP_DIR)/src/cmdspi.cpp
endif

ifneq ($(filter ENABLE_BENCHMARK, $(FEATURE_ENABLE)),)
APP_SOURCES += $(APP_DIR)/src/cmdbenchmark.cpp
endif

ifneq ($(filter ENABLE_BENCHMARK_DHRYSTONE, $(FEATURE_ENABLE)),)
APP_SOURCES += \
$(APP_DIR)/libs/dhrystone/dhry_1.c \
$(APP_DIR)/libs/dhrystone/dhry_2.c
endif

ifneq ($(filter ENABLE_BENCHMARK_COREMARK,$(FEATURE_ENABLE)),)
APP_SOURCES +=\
$(APP_DIR)/libs/CoreMark/core_main.c \
$(APP_DIR)/libs/CoreMark/core_state.c \
$(APP_DIR)/libs/CoreMark/core_util.c \
$(APP_DIR)/libs/CoreMark/core_list_join.c \
$(APP_DIR)/libs/CoreMark/core_matrix.c
endif

ifneq ($(filter ENABLE_RGBLED, $(FEATURE_ENABLE)),)
DRV_SOURCES +=$(APP_DIR)/libs/ws2812b/ws2812b.c
APP_SOURCES +=$(APP_DIR)/src/cmdrgbled.cpp
endif

ifneq ($(filter ENABLE_DS1086, $(FEATURE_ENABLE)),)
APP_SOURCES += \
$(APP_DIR)/src/cmdds1086.cpp \
$(APP_DIR)/libs/ds1086/ds1086.cpp
endif

ifneq ($(filter ENABLE_AVR_PROGRAMMING, $(FEATURE_ENABLE)),)
APP_SOURCES += \
$(APP_DIR)/src/cmdavr.cpp \
$(APP_DIR)/libs/stk500/avr_if.c \
$(APP_DIR)/libs/stk500/stk500.c
endif

ifneq ($(filter ENABLE_TONE, $(FEATURE_ENABLE)),)
DRV_SOURCES += $(DRIVER_PATH)/tone/tone_at32f4xx.c
LIB_SOURCES += $(LIB_PATH)/src/rtttl.c
APP_SOURCES += $(APP_DIR)/src/cmdbuz.cpp
endif

ifneq ($(filter ENABLE_PWM, $(FEATURE_ENABLE)),)
DRV_SOURCES += $(DRIVER_PATH)/pwm/pwm_at32f4xx.c
APP_SOURCES += $(APP_DIR)/src/cmdpwm.cpp
endif

ifneq ($(filter ENABLE_DEMO_GIF, $(FEATURE_ENABLE)),)
APP_SOURCES += $(APP_DIR)/libs/agif/AnimatedGIF.cpp
APP_SOURCES += $(APP_DIR)/libs/agif/agif.c
endif

ifneq ($(filter ENABLE_FLASH, $(FEATURE_ENABLE)),)
APP_SOURCES += $(APP_DIR)/src/cmdflash.cpp
LIB_SOURCES += \
$(APP_DIR)/libs/spiflash/flash_dev.c \
$(APP_DIR)/libs/spiflash/flash_dev_spi.c \
$(TARGET_PATH)/flashspi_renessas.c
endif

ifneq ($(filter ENABLE_RPM, $(FEATURE_ENABLE)),)
APP_SOURCES += $(APP_DIR)/src/cmdrpm.cpp

ifneq ($(filter ENABLE_IR, $(FEATURE_ENABLE)),)
DRV_SOURCES += $(DRIVER_PATH)/ir/ir_at32f4xx.c
LIB_SOURCES += $(LIB_PATH)/src/rc6.c
APP_SOURCES += $(APP_DIR)/src/cmdir.cpp
endif
endif

CPP_SOURCES = $(filter %.cpp, $(APP_SOURCES))
C_SOURCES = \
$(BOARD_SOURCES) \
$(DRV_SOURCES) \
$(LIB_SOURCES) \
$(filter %.c,$(APP_SOURCES))

LDSCRIPT :=$(TARGET_PATH)/AT32F415xB_FLASH.ld

#######################################
# Objects
#######################################

OBJECTS = $(addprefix $(BUILD_DIR)/, $(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

OBJECTS += $(addprefix $(BUILD_DIR)/, $(notdir $(CPP_SOURCES:.cpp=.obj)))
vpath %.cpp $(sort $(dir $(CPP_SOURCES)))

#OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
#vpath %.s $(sort $(dir $(ASM_SOURCES)))

#######################################
# CFLAGS
#######################################

DEVICE =AT32F415CBT7
CPU =-mcpu=cortex-m4 -mthumb
#at43f415 does not have FPU
#FPU =-mfloat-abi=hard -mfpu=fpv4-sp-d16
#FPU =-mfloat-abi=soft
#FLOAT_ABI =#-u_printf_float
MCU = $(CPU) $(FPU) $(FLOAT_ABI)

C_DEFS +=\
$(BOARD) \
$(DEVICE) \
CONSOLE_WIDTH=128 \
FIFO_INTERRUPT_SAFE \
UART_TX_MODE=UART_MODE_FIFO \
UART_RX_MODE=UART_MODE_FIFO \
$(FEATURE_ENABLE) \

ifdef RELEASE
DEBUG =0
OPT   =-O2
else
DEBUG =1
OPT   =-O0 -g -gdwarf-2 -Wall -Wextra
endif

ifndef V
VERBOSE =@
else
VERBOSE =
endif

# Generate dependency information
#CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst))

ASFLAGS  =$(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections
CFLAGS   =$(MCU) $(OPT) $(addprefix -D, $(C_DEFS)) $(addprefix -I, $(C_INCLUDES)) -fdata-sections -ffunction-sections -std=gnu11 #-fstack-usage
CPPFLAGS =$(CPU) $(OPT) $(addprefix -D, $(C_DEFS)) $(addprefix -I, $(C_INCLUDES)) -fdata-sections -ffunction-sections -fno-unwind-tables -fno-exceptions -fno-rtti
LDFLAGS  =$(MCU) $(SPECS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,-gc-sections,-cref

#######################################
# Libraries
#######################################

LIBDIR =-L.
ifdef SEMIHOSTING
SPECS =--specs=rdimon.specs
LDLIBS =-nostartfiles -lc -lrdimon
else
SPECS =--specs=nano.specs --specs=nosys.specs
LIBS =-lstdc++ #-lusbpwrkt
endif

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

ifeq ($(ST_LINK_PROBE),1)
PRG_DEP =bin
PRG_CFG =$(TARGET_PATH)/at32f415.cfg
PRG_CMD =openocd -f $(PRG_CFG) -c "program $(BUILD_DIR)/$(TARGET).elf verify reset exit"
ERASE_CMD =openocd -f $(PRG_CFG) -c "init" -c "halt" -c "stm32f1x unlock 0" -c "stm32f1x mass_erase 0" -c "exit"
else
ifeq ($(shell uname -s), Linux)
JLK =JLinkExe
else
JLK =JLink
endif
PRG_DEP =bin $(PRG_CFG)
PRG_CFG =$(BUILD_DIR)/$(TARGET).jlink
PRG_CMD =$(JLK) -device $(DEVICE) -if SWD -speed auto -CommanderScript $(PRG_CFG)
endif

#######################################
# Misc
#######################################



#######################################
# Rules
#######################################
all: $(BUILD_DIR)/$(TARGET).elf size
#@echo $(CMSIS_PATH)

bin: $(BUILD_DIR)/$(TARGET).bin

program: $(PRG_DEP)
	$(VERBOSE)$(PRG_CMD)

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

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS)
	@echo "[LD]  $@"
	$(VERBOSE)$(LD) $(LIBDIR) -T$(LDSCRIPT) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@

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
