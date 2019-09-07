BUILD_DIR  =build

all: bb

$(BUILD_DIR):
	mkdir -p $@
	
clean: 
	@${RM} -Rf $(BUILD_DIR)
#@$(MAKE) -C blueboard clean
#@$(MAKE) -C bsp/Blueboard clean
#@$(MAKE) -C bsp/Discovery clean
	
emu: $(BUILD_DIR)
	@$(MAKE) -C bsp/emu BUILD_DIR=../../$(BUILD_DIR)/emu

nuc:
	@$(MAKE) -C Nucleo BUILD_DIR=../$(BUILD_DIR)/nucleo-l412

bb:
	@$(MAKE) -C blueboard BUILD_DIR=../$(BUILD_DIR)/blueboard
# BUILD_DIR=../$(BUILD_DIR)/blueboard
	
bb-program:
	@$(MAKE) -C bsp/Blueboard BUILD_DIR=../../$(BUILD_DIR)/blueboard program


DISCO: 
	@$(MAKE) -C bsp/Discovery BUILD_DIR=../../$(BUILD_DIR)/discovery
