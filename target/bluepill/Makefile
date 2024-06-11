######################################
# target
######################################
TARGET =console_bluepill

#######################################
# Features
#######################################

FEATURE_ENABLE = \
ENABLE_TFT_DISPLAY \
ENABLE_BENCHMARK \
#ENABLE_BENCHMAK_DHYSTONE \
#ENABLE_DEBUG MSC_CLOCK \


#######################################
# paths
#######################################
BUILD_DIR 		:=build
TARGET_PATH 	=$(CURDIR)

LIB_PATH 		=$(LIBEMB_PATH)
DRIVERS_PATH 	=$(LIB_PATH)/drv

ifeq ($(STM32REPO), )
$(error Missing STM32REPO environment variable)
endif

REPOSITORY :=$(STM32REPO)/STM32Cube_FW_F1_V1.8.2

$(info $(REPOSITORY))

######################################
# Includes
######################################
C_INCLUDES =  \
Inc \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Inc \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy \
$(REPOSITORY)/Drivers/CMSIS/Device/ST/STM32F1xx/Include \
$(REPOSITORY)/Drivers/CMSIS/Include \
$(REPOSITORY)/Middlewares/ST/STM32_USB_Device_Library/Core/Inc \
$(REPOSITORY)/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc \
$(LIB_PATH)/inc \
$(DRIVERS_PATH)/inc \
$(APP_DIR)/inc \
$(APP_DIR)/libs/agif \
$(APP_DIR)/libs/si5351 \
$(APP_DIR)/libs/ds1086 \

AS_INCLUDES = 

######################################
# Sources
######################################
C_SOURCES =  \
$(APP_DIR)/libs/spiflash/spi_flash.c \
$(TARGET_PATH)/Src/bp_main.c \
$(TARGET_PATH)/Src/usbd_desc.c \
$(TARGET_PATH)/Src/usbd_conf.c \
$(TARGET_PATH)/Src/usb_device.c \
$(TARGET_PATH)/Src/stm32f1xx_it.c \
$(TARGET_PATH)/Src/usbd_cdc_if.c \
$(TARGET_PATH)/Src/system_stm32f1xx.c \
$(TARGET_PATH)/Src/stm32f1xx_hal_msp.c \
$(TARGET_PATH)/Src/stm32f1xx_hal_timebase_tim.c \
$(TARGET_PATH)/Src/board.c \
$(TARGET_PATH)/Src/bp_serial.c \
$(REPOSITORY)/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c \
$(REPOSITORY)/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c \
$(REPOSITORY)/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c \
$(REPOSITORY)/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_i2c.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pcd_ex.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_usb.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pcd.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim_ex.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_adc.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_adc_ex.c \
$(DRIVERS_PATH)/uart/uart_stm32f10x.c \
$(DRIVERS_PATH)/spi/spi_stm32f1xx.c \
$(DRIVERS_PATH)/tft/st7735.c \
$(DRIVERS_PATH)/rng/rng_software.c \
$(DRIVERS_PATH)/gpio/gpio_stm32f1xx.c \
$(DRIVERS_PATH)/dma/dma_stm32f1xx.c \
$(DRIVERS_PATH)/wdt/wdt_dummy.c \
$(LIB_PATH)/src/strfunc.c \
$(LIB_PATH)/src/fifo.c \
$(LIB_PATH)/src/liblcd.c \
$(LIB_PATH)/src/lib2d.c \
$(LIB_PATH)/src/font.c \
#$(LIBEMB_PATH)/bsp/bluepill/bluepill.c \

CPP_SOURCES = \
$(LIB_PATH)/src/console.cpp \
$(APP_DIR)/src/app.cpp \
$(APP_DIR)/src/cmdtft.cpp \
$(APP_DIR)/src/cmdadc.cpp \
$(APP_DIR)/src/cmdmem.cpp \
$(APP_DIR)/src/cmdrfinder.cpp \
$(APP_DIR)/src/cmdservo.cpp \
$(APP_DIR)/src/cmdspiflash.cpp \
$(APP_DIR)/src/cmdflashrom.cpp \
$(APP_DIR)/src/cmdbenchmark.cpp \
#$(APP_DIR)/src/cmdsbus.cpp \
$(APP_DIR)/src/cmdencoder.cpp \

# ASM sources
ASM_SOURCES =  \
$(TARGET_PATH)/startup/startup_stm32f103xb.s

#######################################
# Misc
#######################################

LDSCRIPT =$(TARGET_PATH)/startup/STM32F103C8Tx_FLASH.ld

AS_DEFS = 

# C defines
C_DEFS =  \
$(DEVICE) \
USE_HAL_DRIVER \
BOARD_BLUEPILL \

# CPP defines
CPP_DEFS = \
$(DEVICE) \
USE_HAL_DRIVER \
BOARD_BLUEPILL \

C_DEFS +=$(foreach feature, $(FEATURE_ENABLE), $(feature))

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
CPPFLAGS =$(MCU) $(OPT) $(addprefix -D, $(CPP_DEFS)) $(addprefix -I, $(C_INCLUDES)) -Wall -fdata-sections -ffunction-sections -fno-exceptions -fmessage-length=0 -fno-rtti

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

program: $(TARGET).jlink
	@echo "Programing device"
#	openocd -f  $(BUILD_DIR)/$(TARGET).cfg -c "program $(BUILD_DIR)/$(TARGET).elf verify reset exit"
	$(VERBOSE)$(JLINK) -device $(DEVICE) -if SWD -speed auto -CommanderScript $(TARGET).jlink

$(BUILD_DIR)/$(TARGET).cfg:
	@echo "Creating opencod configuration file"
	echo "adapter driver jlink" >> $@
	echo "transport select swd" >> $@
	echo "source [find target/stm32f1x.cfg]" >> $@
	echo "adapter speed 4000" >> $@

$(BUILD_DIR)/$(TARGET).jlink: $(TARGET).hex
	@echo "Creating Jlink configuration file..."
	@echo "erase" > $@
	@echo "loadfile  $(TARGET).hex" >> $@
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

$(BUILD_DIR)/$(TARGET).hex: $(TARGET).elf | $(BUILD_DIR)
	@echo "Building hex file:" $@
	@$(HEX) $< $@
	
$(BUILD_DIR)/$(TARGET).bin: $(TARGET).elf | $(BUILD_DIR)
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