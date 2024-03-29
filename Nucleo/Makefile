##########################################################################################################################
# File automatically-generated by tool: [projectgenerator] version: [3.3.0] date: [Wed Sep 04 23:31:55 BST 2019]
##########################################################################################################################

# ------------------------------------------------
# Generic Makefile (based on gcc)
#
# ChangeLog :
#	2017-02-10 - Several enhancements + project update mode
#   2015-07-22 - first version
# ------------------------------------------------

######################################
# target
######################################
TARGET =console_nucleo


######################################
# building variables
######################################
ifeq ($(RELEASE), 1)
DEBUG =0
OPT =-Os
else
DEBUG =1
OPT =-Og
endif


#######################################
# paths
#######################################
# Build path
BUILD_DIR :=build
APP_DIR :=../App
PRJ_DIR :=.

ifeq ($(shell uname -s), Linux)
REPOSITORY :=$(HOME)/STM32Cube/Repository/STM32Cube_FW_L4_V1.15.1
else
REPOSITORY :=C:/Users/hmr/STM32Cube/Repository/STM32Cube_FW_L4_V1.15.1
endif


######################################
# source
######################################
# C sources
C_SOURCES =  \
$(PRJ_DIR)/Core/Src/main.c \
$(PRJ_DIR)/Core/Src/stm32l4xx_it.c \
$(PRJ_DIR)/Core/Src/stm32l4xx_hal_msp.c \
$(PRJ_DIR)/Core/Src/system_stm32l4xx.c \
$(PRJ_DIR)/Core/Src/syscalls.c \
$(PRJ_DIR)/Core/Src/serial.c \
$(REPOSITORY)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim.c \
$(REPOSITORY)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim_ex.c \
$(REPOSITORY)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart.c \
$(REPOSITORY)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart_ex.c \
$(REPOSITORY)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.c \
$(REPOSITORY)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c.c \
$(REPOSITORY)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c_ex.c \
$(REPOSITORY)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.c \
$(REPOSITORY)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.c \
$(REPOSITORY)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash.c \
$(REPOSITORY)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ex.c \
$(REPOSITORY)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ramfunc.c \
$(REPOSITORY)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.c \
$(REPOSITORY)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma.c \
$(REPOSITORY)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma_ex.c \
$(REPOSITORY)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr.c \
$(REPOSITORY)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr_ex.c \
$(REPOSITORY)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.c \
$(REPOSITORY)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_exti.c \
$(LIBEMB_PATH)/misc/strfunc.c \
$(LIBEMB_PATH)/misc/fifo.c \
$(LIBEMB_PATH)/sound/stm32_buzzer.c \
$(LIBEMB_PATH)/drv/timer/stm32_scapture.c \
$(LIBEMB_PATH)/drv/pwm/pwm_stm32l4xx.c \
$(LIBEMB_PATH)/drv/tft/st7735.c \
$(LIBEMB_PATH)/drv/rng/rng_stm32l4xx.c \
$(LIBEMB_PATH)/drv/spi/spi_stm32l4xx.c \
$(LIBEMB_PATH)/display/lcd.c \
$(APP_DIR)/agif/agif.c \

CPP_SOURCES = \
$(LIBEMB_PATH)/console/console.cpp \
$(APP_DIR)/app.cpp \
$(APP_DIR)/cmdmem.cpp \
$(APP_DIR)/cmdrfinder.cpp \
$(APP_DIR)/cmdbuz.cpp \
$(APP_DIR)/cmdkeyfob.cpp \
$(APP_DIR)/cmdpwm.cpp \
$(APP_DIR)/cmdtft.cpp \
$(APP_DIR)/agif/AnimatedGIF.cpp \

# ASM sources
ASM_SOURCES =  \
$(PRJ_DIR)/Core/Startup/startup_stm32l412kbux.s

# link script
LDSCRIPT =$(PRJ_DIR)/STM32L412KBUx_FLASH.ld

#######################################
# Include
#######################################

# AS includes
AS_INCLUDES = 

# C includes
C_INCLUDES =  \
-I$(PRJ_DIR)/Core/Inc \
-I$(REPOSITORY)/Drivers/STM32L4xx_HAL_Driver/Inc \
-I$(REPOSITORY)/Drivers/STM32L4xx_HAL_Driver/Inc/Legacy \
-I$(REPOSITORY)/Drivers/CMSIS/Device/ST/STM32L4xx/Include \
-I$(REPOSITORY)/Drivers/CMSIS/Include \
-I$(REPOSITORY)/Drivers/CMSIS/Include \
-I$(LIBEMB_PATH)/include \
-I$(APP_DIR)/../inc \
-I$(APP_DIR)/agif \

#######################################
# binaries
#######################################
PREFIX =arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef _GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
CPP = $(GCC_PATH)/$(PREFIX)g++
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
CPP = $(PREFIX)g++
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
LD = $(PREFIX)gcc
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

TAG_COMMIT := $(shell git rev-list --abbrev-commit --tags --max-count=1)
COMMIT := $(shell git rev-parse --short HEAD)
TAG := $(shell git describe --abbrev=0 --tags ${TAG_COMMIT} 2>/dev/null || true)
DATE := $(shell git log -1 --format=%cd --date=format:"%Y%m%d")
#VERSION := $(TAG:v%=%)

#ifneq ($(COMMIT), $(TAG_COMMIT))
#    VERSION := $(VERSION)-next-$(COMMIT)-$(DATE)
#endif

ifeq ($(TAG),)
    VERSION :=$(COMMIT)-$(DATE)
else
    VERSION :=$(TAG)-$(COMMIT)-$(DATE)
endif

ifneq ($(shell git status --porcelain | grep -v '^??'),)
    VERSION := $(VERSION)-dirty
endif

#######################################
# CFLAGS
#######################################
# cpu
CPU =-mcpu=cortex-m4

# fpu
FPU =-mfpu=fpv4-sp-d16

# float-abi
FLOAT-ABI =-mfloat-abi=hard

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
COMMON_DEFS = \
-D__NUCLEO_L412KB__\
-DSTM32L412xx \
-DUSE_HAL_DRIVER \
'-DVERSION_STRING="$(VERSION)"' \

# AS defines
AS_DEFS = 

# C defines
C_DEFS =  \
$(COMMON_DEFS) \

# CPP defines
CPP_DEFS = \
$(COMMON_DEFS) \
'-D__weak=__attribute__((weak))' \
'-D__packed=__attribute__((__packed__))' \

COMMON_FLAGS =$(MCU) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
COMMON_FLAGS +=-Og -g -gdwarf-2
endif

# compile gcc flags
ASFLAGS =$(AS_DEFS) $(AS_INCLUDES) $(COMMON_FLAGS)

CFLAGS =$(C_DEFS) $(C_INCLUDES) $(COMMON_FLAGS)

CPPFLAGS =$(CPP_DEFS) $(C_INCLUDES) $(COMMON_FLAGS) -fno-exceptions -fno-rtti

# Generate dependency information
#CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"
# Assembly options
#CFLAGS += -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst))


#######################################
# LDFLAGS
#######################################

# libraries
LIBS = -lm
LIBDIR =
LDFLAGS = $(MCU) -specs=nosys.specs -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) -Wl,--gc-sections

#######################################
# Rules
#######################################
# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin
	@echo
ifeq ($(DEBUG), 1)
	@$(SZ) -A -x $(BUILD_DIR)/$(TARGET).elf
else
	@$(SZ) -B -x $(BUILD_DIR)/$(TARGET).elf
endif

$(TARGET).cfg:
	@echo "Creating opencod configuration file for st-link"
	@echo "source [find interface/stlink.cfg]" >> $@
	@echo "transport select "hla_swd"" >> $@
	@echo "source [find target/stm32l4x.cfg]" >> $@
	@echo "adapter speed 4000" >> $@
	

program: $(BUILD_DIR)/$(TARGET).elf $(TARGET).cfg
	openocd -f $(TARGET).cfg -c "program $(BUILD_DIR)/$(TARGET).elf verify reset exit"

test:
	@echo $(VERSION)
#@echo ""; $(foreach d, $(OBJECTS), echo $(d);)
#@echo $(sort $(dir $(C_SOURCES)))
#@echo $(C_SOURCES)

#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(CPP_SOURCES:.cpp=.o)))
vpath %.cpp $(sort $(dir $(CPP_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	@echo "Compiling " $<
	@$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	@echo "Assembling " $<
	@$(AS) -c $(ASFLAGS) $< -o $@

$(BUILD_DIR)/%.o: %.cpp Makefile | $(BUILD_DIR) 
	@echo "Compiling " $<
	@$(CPP) -c $(CPPFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	@echo "Linking " $@
	@$(LD) $(OBJECTS) $(LDFLAGS) -o $@ $(LIBS)

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	@echo "Building hex file:" $@
	@$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	@echo "Building bin file:" $@
	@$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir -p $@		

#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)
  
#######################################
# dependencies
#######################################
#-include $(wildcard $(BUILD_DIR)/*.d)

ifeq ($(GCC_COLORS),)
export GCC_COLORS='error=01;31:warning=01;35:note=01;36:locus=01:quote=01'
unexport GCC_COLORS
endif

# *** EOF ***
