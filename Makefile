BUILD_DIR  =build

all: bb

$(BUILD_DIR):
	mkdir -p $@
	
clean: 
	@${RM} -rf $(BUILD_DIR)
	@$(MAKE) -C blueboard clean
	@$(MAKE) -C Nucleo clean
	@$(MAKE) -C bluepill clean
	
emu: $(BUILD_DIR)
	@$(MAKE) -C bsp/emu BUILD_DIR=../../$(BUILD_DIR)/emu

nuc:
	@$(MAKE) -C Nucleo BUILD_DIR=../$(BUILD_DIR)/nucleo-l412

pill:
	@$(MAKE) -C bluepill BUILD_DIR=../$(BUILD_DIR)/bluepill

bb:
	@$(MAKE) -C blueboard BUILD_DIR=../$(BUILD_DIR)/blueboard
# BUILD_DIR=../$(BUILD_DIR)/blueboard
	
program-bb:
	@$(MAKE) -C bsp/Blueboard BUILD_DIR=../../$(BUILD_DIR)/blueboard program

program-pill:
	@$(MAKE) -C bluepill BUILD_DIR=../../$(BUILD_DIR)/bluepill program


DISCO: 
	@$(MAKE) -C bsp/Discovery BUILD_DIR=../../$(BUILD_DIR)/discovery


ifeq ($(GCC_COLORS),)
export GCC_COLORS='error=01;31:warning=01;35:note=01;36:caret=01;32:locus=01:quote=01'
#unexport GCC_COLORS
endif