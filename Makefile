BUILD_DIR  	:=$(CURDIR)/build
APP_DIR 	:=$(CURDIR)/App

all: nc bp bb at

norule:
	@echo "specify board"
	@echo "Available boards:"
	@echo "	bb - blueboard"
	@echo "	bp - bluepill"
	@echo "	nc - Nucleo"
	@echo "	at - at32f415"

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

at:
	@"$(MAKE)" -C target/artery BUILD_DIR=$(BUILD_DIR)/artery APP_DIR=$(APP_DIR)
	
bb-program:
	@"$(MAKE)" -C target/blueboard BUILD_DIR=$(BUILD_DIR)/blueboard APP_DIR=$(APP_DIR) program

bp-program:
	@"$(MAKE)" -C target/bluepill BUILD_DIR=$(BUILD_DIR)/bluepill APP_DIR=$(APP_DIR) program

nc-program:
	@"$(MAKE)" -C target/Nucleo BUILD_DIR=$(BUILD_DIR)/nucleo APP_DIR=$(APP_DIR) program

at-program:
	@"$(MAKE)" -C target/artery BUILD_DIR=$(BUILD_DIR)/artery APP_DIR=$(APP_DIR) program



CMDTEMPLATEHEADER =CmdTemplateHeader.in
CMDTEMPLATEMODULE =CmdTemplateModule.in
L1 =en
SHELL := /bin/bash
command:
ifeq ($(CMDNAME),)
	@echo "usage: make command CMDNAME=<name>"
else
	@cat  $(CMDTEMPLATEHEADER) > App/inc/cmd$(CMDNAME).h
	@sed -i -- "s/%NAME%/$(CMDNAME)/g" App/inc/cmd$(CMDNAME).h
	@sed -i -- "s/%CLASSNAME%/$(shell L1=$(CMDNAME); echo $${L1^})/g" App/inc/cmd$(CMDNAME).h
	@cat  $(CMDTEMPLATEMODULE) > App/src/cmd$(CMDNAME).cpp
	@sed -i -- "s/%CLASSNAME%/$(shell L1=$(CMDNAME); echo $${L1^})/g" App/src/cmd$(CMDNAME).cpp
endif

.PHONY: