######################################
# target
######################################
TARGET =console_bluepill
BOARD =BOARD_BLUEPILL HSI
#######################################
# Features
#######################################

FEATURE_ENABLE +=ENABLE_SPI
FEATURE_ENABLE +=ENABLE_FLASHROM
#FEATURE_ENABLE +=ENABLE_TONE
#ENABLE_BENCHMARK \
#ENABLE_DISPLAY \
#ENABLE_BENCHMARK_DHRYSTONE \
#ENABLE_DEBUG MSC_CLOCK \
#ENABLE_SERVO
#ENABLE_I2C
#ENABLE_ADC

$(info FEATURES: $(FEATURE_ENABLE))
#######################################
# paths
#######################################
BUILD_DIR 		:=build
TARGET_PATH 	=$(CURDIR)

APP_DIR			?=../../App
LIB_PATH    	=$(LIBEMB_PATH)/lib
DRIVER_PATH 	=$(LIBEMB_PATH)/drv
CMSIS_PATH 		=$(LIBEMB_PATH)/CMSIS
CMSIS_PATH_CORE =$(CMSIS_PATH)/Core
CMSIS_PATH_DEVICE =$(CMSIS_PATH)/Device/ST/STM32F1xx
DRIVER_COMPONENT_PATH =$(LIBEMB_PATH)/component

ifeq ($(STM32REPO), )
$(error Missing STM32REPO environment variable)
endif

REPOSITORY :=$(STM32REPO)/STM32Cube_FW_F1_V1.8.2

$(info REPOSITORY: $(REPOSITORY))
######################################
# Includes
######################################
C_INCLUDES =  \
$(TARGET_PATH)/Inc \
$(CMSIS_PATH_CORE)/Include \
$(CMSIS_PATH_DEVICE)/inc \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Inc \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy \
$(REPOSITORY)/Middlewares/ST/STM32_USB_Device_Library/Core/Inc \
$(REPOSITORY)/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc \
$(DRIVER_PATH)/inc \
$(LIB_PATH) \
$(LIB_PATH)/inc \
$(APP_DIR)/inc \
$(APP_DIR)/libs \

AS_INCLUDES =

######################################
# Sources
######################################

TARGET_SOURCES =\
$(CMSIS_PATH_DEVICE)/src/startup_stm32f103xb.c \
$(CMSIS_PATH_DEVICE)/src/system_stm32f1xx.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pcd_ex.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pcd.c \
$(DRIVER_PATH)/clock/clock_stm32f1xx.c \
$(DRIVER_PATH)/dma/dma_stm32f1xx.c \
$(DRIVER_PATH)/gpio/gpio_stm32f1xx.c \
$(DRIVER_PATH)/wdt/wdt_dummy.c \
$(TARGET_PATH)/Src/bp_board.c \
$(TARGET_PATH)/Src/bp_serial.c \
$(TARGET_PATH)/Src/bp_main.c \
$(TARGET_PATH)/Src/stm32f1xx_it.c \

USB_SOURCES =\
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_usb.c \
$(REPOSITORY)/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c \
$(REPOSITORY)/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c \
$(REPOSITORY)/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c \
$(REPOSITORY)/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.c \
$(TARGET_PATH)/Src/usbd_desc.c \
$(TARGET_PATH)/Src/usbd_conf.c \
$(TARGET_PATH)/Src/usb_device.c \
$(TARGET_PATH)/Src/usbd_cdc_if.c \
$(TARGET_PATH)/Src/stm32f1xx_hal_msp.c \

APP_SOURCES =\
$(DRIVER_PATH)/uart/uart_stm32f10x.c \
$(LIB_PATH)/src/strfunc.c \
$(LIB_PATH)/src/fifo.c \
$(LIB_PATH)/src/stimer.c \
$(TARGET_PATH)/Src/syscalls.c \
$(TARGET_PATH)/Src/sysmem.c \


#$(DRIVER_PATH)/rng/rng_software.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c \

C_SOURCES = \
$(TARGET_SOURCES) \
$(APP_SOURCES) \

CPP_SOURCES = \
$(LIB_PATH)/src/console.cpp \
$(APP_DIR)/src/app.cpp \
#$(APP_DIR)/src/cmdtft.cpp \
$(APP_DIR)/src/cmdadc.cpp \
$(APP_DIR)/src/cmdmem.cpp \
$(APP_DIR)/src/cmdrfinder.cpp \
$(APP_DIR)/src/cmdservo.cpp \
$(APP_DIR)/src/cmdbenchmark.cpp \
$(APP_DIR)/src/cmdbuz.cpp \
#$(APP_DIR)/src/cmdflash.cpp \


# ASM sources
ASM_SOURCES =  \
$(TARGET_PATH)/startup/startup_stm32f103xb.s


ifneq (,$(filter ENABLE_DISPLAY,$(FEATURES_ENABLE)))
C_SOURCES += \
$(LIB_PATH)/src/liblcd.c \
$(LIB_PATH)/src/lib2d.c \
$(LIB_PATH)/src/font.c \
$(DRIVER_COMPONENT_PATH)/tft/st7735.c
endif

ifneq ($(filter ENABLE_SPI,$(FEATURE_ENABLE)),)
C_SOURCES +=$(DRIVER_PATH)/spi/spi_stm32f1xx.c
CPP_SOURCES +=$(APP_DIR)/src/cmdspi.cpp
endif

ifneq ($(filter ENABLE_FLASHROM,$(FEATURE_ENABLE)),)
CPP_SOURCES +=$(APP_DIR)/src/cmdflashrom.cpp
endif

ifneq ($(filter ENABLE_TONE, $(FEATURE_ENABLE)),)
TARGET_SOURCES +=$(DRIVER_PATH)/tone/tone_stm32f1xx.c
APP_SOURCES +=$(LIB_PATH)/src/rtttl.c
CPP_SOURCES +=$(APP_DIR)/src/cmdbuz.cpp
endif

#######################################
# Misc
#######################################

LDSCRIPT =$(TARGET_PATH)/startup/STM32F103C8Tx_FLASH.ld

AS_DEFS =

# C defines
C_DEFS =  \
$(DEVICE) \
$(BOARD)\
USE_HAL_DRIVER \


C_DEFS +=$(FEATURE_ENABLE)

#######################################
# Tool binaries
#######################################
PREFIX = arm-none-eabi-

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
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

ifeq ($(shell uname -s), Linux)
JLINK =JLinkExe
else
JLINK =jlink
endif
#######################################
# CFLAGS
#######################################
# cpu
DEVICE =STM32F103xB
CPU =-mcpu=cortex-m3 -mthumb

# fpu
#FPU =

# float-abi
#FLOAT-ABI =

# mcu
MCU =$(CPU) $(FPU) $(FLOAT-ABI)

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

# compile gcc flags
ASFLAGS  =$(MCU) $(OPT) $(AS_DEFS) $(addprefix -I, $(AS_INCLUDES)) -Wall -fdata-sections -ffunction-sections
CFLAGS   =$(MCU) $(OPT) $(addprefix -D, $(C_DEFS)) $(addprefix -I, $(C_INCLUDES)) -Wall -fdata-sections -ffunction-sections
CPPFLAGS =$(MCU) $(OPT) $(addprefix -D, $(C_DEFS)) $(addprefix -I, $(C_INCLUDES)) -Wall -fdata-sections -ffunction-sections -fno-exceptions -fmessage-length=0 -fno-rtti

# Generate dependency information
#CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"
# Assembly options
#CFLAGS += -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst))

#######################################
# Libraries
#######################################

SPECS =-specs=nosys.specs -specs=nano.specs
LIBS =-lstdc++
LIBDIR =
LDFLAGS = $(MCU) $(SPECS) -Wl,--gc-sections,-Map=$(BUILD_DIR)/$(TARGET).map,--cref

#######################################
# Objects
#######################################
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(CPP_SOURCES:.cpp=.o)))
vpath %.cpp $(sort $(dir $(CPP_SOURCES)))

OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

#######################################
# Rules
#######################################
all: $(BUILD_DIR)/$(TARGET).elf
	@echo
	@$(SZ) -B $<

bin: $(BUILD_DIR)/$(TARGET).bin

program: $(BUILD_DIR)/$(TARGET).jlink
	@echo "Programing device"
	$(VERBOSE)$(JLINK) -device $(DEVICE) -if SWD -speed auto -CommanderScript $<
#	openocd -f  $(BUILD_DIR)/$(TARGET).cfg -c "program $(BUILD_DIR)/$(TARGET).elf verify reset exit"

$(BUILD_DIR)/$(TARGET).cfg:
	@echo "Creating opencod configuration file"
	echo "adapter driver jlink" >> $@
	echo "transport select swd" >> $@
	echo "source [find target/stm32f1x.cfg]" >> $@
	echo "adapter speed 4000" >> $@

$(BUILD_DIR)/$(TARGET).jlink: $(BUILD_DIR)/$(TARGET).hex
	@echo "Creating Jlink configuration file..."
	@echo "erase" > $@
	@echo "loadfile  $<" >> $@
	@echo "r" >> $@
	@echo "q" >> $@

#######################################
# build rules
#######################################
$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	@echo "[CC] " $<
	@$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	@echo "[AS] " $<
	@$(AS) -c $(ASFLAGS) $< -o $@

$(BUILD_DIR)/%.o: %.cpp Makefile | $(BUILD_DIR)
	@echo "[CC] " $<
	@$(CPP) -c $(CPPFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) | Makefile
	@echo "[LD] " $@
	@$(CC) $(LIBDIR) -T$(LDSCRIPT) $(LDFLAGS) $^ $(LIBS) -o $@

$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf | $(BUILD_DIR)
	@echo "Building hex file:" $@
	@$(HEX) $< $@

$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf | $(BUILD_DIR)
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