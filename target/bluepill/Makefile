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
TARGET =$(BUILD_DIR)/console_bluepill


######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
OPT = -Og


#######################################
# paths
#######################################
# Build path
BUILD_DIR :=build
TARGET_PATH =$(CURDIR)

ifeq ($(STM32REPO), )
$(error Missing STM32REPO environment variable)
endif

REPOSITORY :=$(STM32REPO)/STM32Cube_FW_F1_V1.8.2

$(info $(REPOSITORY))

######################################
# source
######################################
# C sources
C_SOURCES =  \
$(APP_DIR)/spi_flash.c \
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
$(TARGET_PATH)/Src/bp_it.c \
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
$(LIBEMB_PATH)/misc/strfunc.c \
$(LIBEMB_PATH)/misc/fifo.c \
$(LIBEMB_PATH)/drv/uart/uart_stm32f10x.c \
$(LIBEMB_PATH)/drv/spi/spi_stm32f1xx.c \
$(LIBEMB_PATH)/drv/tft/st7735.c \
$(LIBEMB_PATH)/display/lcd.c \
$(LIBEMB_PATH)/drv/rng/rng_software.c \
#$(LIBEMB_PATH)/bsp/bluepill/bluepill.c \


CPP_SOURCES = \
$(LIBEMB_PATH)/console/console.cpp \
$(APP_DIR)/app.cpp \
$(APP_DIR)/cmdsbus.cpp \
$(APP_DIR)/cmdtft.cpp \
$(APP_DIR)/cmdadc.cpp \
$(APP_DIR)/cmdmem.cpp \
$(APP_DIR)/cmdrst.cpp \
$(APP_DIR)/cmdrfinder.cpp \
$(APP_DIR)/cmdservo.cpp \
$(APP_DIR)/cmdspiflash.cpp \
$(APP_DIR)/cmdflashrom.cpp \
$(APP_DIR)/cmdencoder.cpp \



#$(wildcard $(APP_DIR)/*cpp) \

CPP_SOURCES := $(filter-out $(APP_DIR)/cmdsbus.cpp, $(CPP_SOURCES))

# ASM sources
ASM_SOURCES =  \
$(TARGET_PATH)/startup/startup_stm32f103xb.s

# link script
LDSCRIPT =$(TARGET_PATH)/startup/STM32F103C8Tx_FLASH.ld

# AS includes
AS_INCLUDES = 

# C includes
C_INCLUDES =  \
-IInc \
-I$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Inc \
-I$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy \
-I$(REPOSITORY)/Drivers/CMSIS/Device/ST/STM32F1xx/Include \
-I$(REPOSITORY)/Drivers/CMSIS/Include \
-I$(REPOSITORY)/Middlewares/ST/STM32_USB_Device_Library/Core/Inc \
-I$(REPOSITORY)/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc \
-I$(LIBEMB_PATH)/include \
-I$(LIBEMB_PATH)/drv/inc \
-I$(APP_DIR)/../inc \
-I$(APP_DIR)/agif

#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
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
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S


ifeq ($(shell uname -s), Linux)
JLINK =JLinkExe
else
JLINK ="C:\Tools\JLink_V500\jlink"
endif


#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m3
DEVICE =STM32F103xB

# fpu
#FPU =

# float-abi
#FLOAT-ABI =

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI) -D$(DEVICE) -DBOARD_BLUEPILL

# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS =  \
-DUSE_HAL_DRIVER \
-D_STDOUT_UART \
-D_UART_IT \

# CPP defines
CPP_DEFS = \
'-D__weak=__attribute__((weak))' \
'-D__packed=__attribute__((__packed__))' \
-DUSE_HAL_DRIVER \
-D_STDOUT_UART \

# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CPPFLAGS = $(MCU) $(CPP_DEFS) $(C_INCLUDES) $(OPT) -Wall -fmessage-length=0 -ffunction-sections -c -fno-exceptions -fno-rtti

 

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
CPPFLAGS += -g -gdwarf-2
endif


# Generate dependency information
#CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"
# Assembly options
#CFLAGS += -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst))


#######################################
# LDFLAGS
#######################################
# libraries
LIBS = -lm# -lnosys 
LIBDIR = 
LDFLAGS = $(MCU) -specs=nosys.specs -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) -Wl,-Map=$(TARGET).map,--cref -Wl,--gc-sections -fno-exceptions -fno-rtti

# default action: build all
all: $(TARGET).elf
	@echo
	@$(SZ) -B $(TARGET).elf

 $(TARGET).cfg:
	@echo "Creating opencod configuration file"
	echo "adapter driver jlink" >> $@
	echo "transport select swd" >> $@
	echo "source [find target/stm32f1x.cfg]" >> $@
	echo "adapter speed 4000" >> $@

 $(TARGET).jlink: $(TARGET).hex
	@echo "Creating Jlink configuration file..."
	@echo "erase" > $@
	@echo "loadfile  $(TARGET).hex" >> $@
	@echo "r" >> $@
	@echo "q" >> $@

program: $(TARGET).jlink
	@echo "Programing device"
#	openocd -f  $(BUILD_DIR)/$(TARGET).cfg -c "program $(BUILD_DIR)/$(TARGET).elf verify reset exit"
	$(VERBOSE)$(JLINK) -device $(DEVICE) -if SWD -speed auto -CommanderScript $(TARGET).jlink


#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(CPP_SOURCES:.cpp=.o)))
vpath %.cpp $(sort $(dir $(CPP_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	@echo "Compiling " $<
	@$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	@echo "Assembling " $<
	@$(AS) -c $(ASFLAGS) $< -o $@

$(BUILD_DIR)/%.o: %.cpp Makefile | $(BUILD_DIR) 
	@echo "Compiling " $<
	@$(CPP) -c $(CPPFLAGS) $< -o $@

$(TARGET).elf: $(OBJECTS) Makefile
	@echo "Linking " $@
	@$(CC) $(OBJECTS) $(LDFLAGS) -o $@ $(LIBS)

$(TARGET).hex: $(TARGET).elf | $(BUILD_DIR)
	@echo "Building hex file:" $@
	@$(HEX) $< $@
	
$(TARGET).bin: $(TARGET).elf | $(BUILD_DIR)
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