#########################################################
# 
#########################################################
GCC_EXEC_PREFIX = arm-none-eabi
GCC = $(GCC_EXEC_PREFIX)-gcc
GPP = $(GCC_EXEC_PREFIX)-g++
AS = $(GCC_EXEC_PREFIX)-as
LD = $(GCC_EXEC_PREFIX)-ld
SIZE = $(GCC_EXEC_PREFIX)-size
OBJCOPY = $(GCC_EXEC_PREFIX)-objcopy
OBJDUMP = $(GCC_EXEC_PREFIX)-objdump
DBG = $(GCC_EXEC_PREFIX)-insight
REMOVE = rm -f
CHECKSUM =$(BSPPATH)/tools/checksum
#########################################################
# project files
#########################################################
TARGET = LPCbus
PRJPATH =. usbdrv
INCSPATH =. usbdrv
CSRCS = #command.c
#CSRCS +=spi_lpc1768.c cmdgpio.c cmdspi.c cmdavr.c
CSRCS +=ili9328.c lcd.c display.c blueboard.c clock.c timer.c pwm.c
CPPSRCS =LPCbus.cpp vcom.cpp usbserial.cpp usbhw_lpc.cpp usbcontrol.cpp usbstdreq.cpp strfunctions.cpp

#########################################################
#Startup files and libraries
#########################################################
BSPPATH   = $(LIBEMB_PATH)/bsp

INCSPATH +=inc $(LIBEMB_PATH)/include $(BSPPATH)/CMSISv2p00_LPC17xx/inc
LIBSPATH +=
OBJPATH =obj

GCSYMBOLS =-D__NEWLIB__ -D__BB__
GCFLAGS =-mcpu=cortex-m3 -mthumb -Wall -O0 -g -fno-exceptions -fno-unwind-tables #-ffunction-sections
LDFLAGS =-mcpu=cortex-m3 -mthumb -nostdlib -lgcc -lstdc++ -fno-exceptions -fno-unwind-tables -Wl,--gc-sections -nostartfiles 

CSRCS   +=startup_lpc1768.c #syscalls.c
LDSCRIPT =$(BSPPATH)/lpc17xx/lpc1768.ld
##########################################################
OBJECTS =$(addprefix $(OBJPATH)/,$(CPPSRCS:.cpp=.obj)) $(addprefix $(OBJPATH)/,$(CSRCS:.c=.o)) $(addprefix $(OBJPATH)/,$(ASRCS:.S=.o))
VPATH = $(PRJPATH) $(BSPPATH)/lpc17xx/ $(LIBEMB_PATH)/display $(LIBEMB_PATH)/drv/tft $(LIBEMB_PATH)/drv/spi \
$(LIBEMB_PATH)/drv/timer \
$(LIBEMB_PATH)/drv/pwm \

ifeq ($(LIBEMB_PATH), )
erro:
	@echo "Please export LIBEMB_PATH"
endif

all: $(TARGET).elf stats


$(TARGET).elf:  $(OBJECTS)
	@echo "---- Linking ---->" $@
	@$(GCC) -T$(LDSCRIPT) $(addprefix -L, $(LIBSPATH)) $(OBJECTS) $(LDFLAGS) -o $(TARGET).elf

$(TARGET).hex: $(TARGET).elf
	@$(OBJCOPY) -O ihex -j .startup -j .text -j $(TARGET).elf $(TARGET).hex

bin: $(TARGET).bin

$(TARGET).bin: $(TARGET).elf
	@$(OBJCOPY) -O binary  -j .startup -j .text -j .data -j .loader $(TARGET).elf $@
	hd -n 128 $@
	$(CHECKSUM) $@

stats: $(TARGET).elf
	@echo "---- Sections Summary ---"
	@$(SIZE) -A -x $<

aslist: $(TARGET).elf
	@$(OBJDUMP) -D $(TARGET).elf > $(TARGET).lst

clean:
	$(REMOVE) $(OBJPATH)/*.obj $(OBJPATH)/*.o *.hex *.elf *.bin	

rebuild: clean all

inline_asm:
	$(GCC) -Og -g -mcpu=cortex-m3 -mthumb -Wall -c inline_asm.c -o inline_asm.o
	$(GCC) -Tinline_asm.ld inline_asm.o -nostdlib -nostartfiles -o inline_asm.elf

$(TARGET).jlink:
	@echo "Creating Jlink configuration file"
	echo "erase\nloadbin $(TARGET).bin , 0x0000000\nr\nq" > $(TARGET).jlink
	
flash-jlink: $(TARGET).jlink #tdso.bin #$(TARGET).bin
	$(JLINK) -device $(DEVICE) -if SWD -speed auto -CommanderScript $(TARGET).jlink

$(TARGET).cfg:
	@echo "Creating opencod configuration file"
	echo "interface jlink\ntransport select swd\nsource [find target/lpc17xx.cfg]\nadapter_khz 4000" > $(TARGET).cfg

flash-openocd: $(TARGET).bin $(TARGET).cfg
	openocd -f $(TARGET).cfg -c "program $(TARGET).bin 0x00000000 verify reset exit"

minicom:
	minicom -b 115200 -D /dev/ttyACM0
########################################################
$(OBJPATH)/%.o : %.c
	@echo "---- Compile" $< "---->" $@
	@$(GCC) $(GCFLAGS) $(addprefix -I, $(INCSPATH)) $(GCSYMBOLS) -c $< -o $@

$(OBJPATH)/%.obj : %.cpp
	@echo "---- Compile" $< "---->" $@
	@$(GPP) $(GCFLAGS) $(addprefix -I, $(INCSPATH)) $(GCSYMBOLS) -c $< -o $@
	
$(OBJPATH)/%.o : %.S
	@echo "---- Assemble" $< "---->" $@
	@$(AS) $(ASFLAGS) $< -o $@

