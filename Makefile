BUILD_DIR  	:=$(PWD)/build
APP_DIR 	:=$(PWD)/App

norule:
	@echo "specify board"
	@echo "Available boards:"
	@echo "	bb - blueboard"
	@echo "	bp - bluepill"
	@echo "	nc - Nucleo"

clean: 
	@${RM} -rf build
	@"$(MAKE)" -C blueboard clean
	@"$(MAKE)" -C Nucleo clean
	@"$(MAKE)" -C bluepill clean

nc:
	@"$(MAKE)" -C Nucleo BUILD_DIR=$(BUILD_DIR)/nucleo-l412 APP_DIR=$(APP_DIR) PRJ_DIR=$(PWD)/Nucleo

bp:
	@"$(MAKE)" -C bluepill BUILD_DIR=$(BUILD_DIR)/bluepill APP_DIR=$(APP_DIR) PRJ_DIR=$(PWD)/bluepill

bb:
	@"$(MAKE)" -C blueboard BUILD_DIR=$(BUILD_DIR)/blueboard APP_DIR=$(APP_DIR) PRJ_DIR=$(PWD)/blueboard
	
program-bb:
	@"$(MAKE)" -C blueboard BUILD_DIR=$(BUILD_DIR)/blueboard APP_DIR=$(APP_DIR) PRJ_DIR=$(PWD)/blueboard program

program-bp:
	@"$(MAKE)" -C bluepill BUILD_DIR=$(BUILD_DIR)/bluepill APP_DIR=$(APP_DIR) PRJ_DIR=$(PWD)/bluepill program

program-nc:
	@"$(MAKE)" -C Nucleo BUILD_DIR=$(BUILD_DIR)/nucleo-l412 APP_DIR=$(APP_DIR) PRJ_DIR=$(PWD)/Nucleo program

.PHONY: