BUILD_DIR  	:=$(CURDIR)/build
APP_DIR 	:=$(CURDIR)/App

norule:
	@echo "specify board"
	@echo "Available boards:"
	@echo "	bb - blueboard"
	@echo "	bp - bluepill"
	@echo "	nc - Nucleo"

clean: 
	@${RM} -rf build
	@"$(MAKE)" -C target/blueboard clean
	@"$(MAKE)" -C target/Nucleo clean
	@"$(MAKE)" -C target/bluepill clean

nc:
	@"$(MAKE)" -C target/Nucleo BUILD_DIR=$(BUILD_DIR)/nucleo APP_DIR=$(APP_DIR)

bp:
	@"$(MAKE)" -C target/bluepill BUILD_DIR=$(BUILD_DIR)/bluepill APP_DIR=$(APP_DIR)

bb:
	@"$(MAKE)" -C target/blueboard BUILD_DIR=$(BUILD_DIR)/blueboard APP_DIR=$(APP_DIR)
	
bb-program:
	@"$(MAKE)" -C target/blueboard BUILD_DIR=$(BUILD_DIR)/blueboard APP_DIR=$(APP_DIR) program

bp-program:
	@"$(MAKE)" -C target/bluepill BUILD_DIR=$(BUILD_DIR)/bluepill APP_DIR=$(APP_DIR) program

nc-program:
	@"$(MAKE)" -C target/Nucleo BUILD_DIR=$(BUILD_DIR)/nucleo APP_DIR=$(APP_DIR) program

.PHONY: