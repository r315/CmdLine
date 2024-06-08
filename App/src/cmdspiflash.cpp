#include "board.h"
#include "cmdspiflash.h"
#include "spi_flash.h"


void CmdSpiFlash::flashDump(uint32_t addr){
    uint8_t buf[16];

    if(!flashRead(addr, buf, 16)){
        return;
    }

    console->printf("%08X ",addr);

    for(uint8_t i = 0; i < 16; i++){
        if((i&7) == 0){
            console->printchar(' ');
        }
        console->printf("%02X ",buf[i]);
    }

    console->print(" |");

	for(uint8_t i=0; i < 16; i++){
		if(buf[i] > (' '-1) && buf[i] < 0x7F)
			console->printchar(buf[i]);
		else{
			console->printchar(' ');
		}
	}

	console->print("|\n");
}

void CmdSpiFlash::help(void){ 
    console->println("Usage: spiflash <read <addr> | id | rs | ws>");
    console->println("\tread,\t read 256 bytes from address");
    console->println("\tid,\t read identification");
    console->println("\trs,\t read status register");
    console->println("\tws,\t write status register");
}

char CmdSpiFlash::execute(int argc, char **argv){
    uint32_t value;
    int32_t idx;

    if(argc < 2){
        help();
        return CMD_OK;
    }

    if(strFind("id", argv, argc) != -1){
        console->printf("JEDEC Id: %x\n", flashJEDEC_ID());
        console->printf("MFD Id: %x\n", flashMFD_ID());
        console->printf("Device Id: %x\n", flashDevice_ID());
        return CMD_OK;
    }

    if((idx = strFind("read", argv, argc)) != -1){
        if(ha2i(argv[idx + 1], &value)){
            for(uint16_t i = 0; i < 16; i++){
                flashDump(value + i);
            }
            return CMD_OK;
        }
    }

    if(strFind("rs", argv, argc) != -1){
        console->printf("Status: %2X\n", flashReadStatus());
        return CMD_OK;
    }

    if((idx = strFind("ws", argv, argc)) != -1){
        if(ha2i(argv[idx + 1], &value)){
            flashWriteStatus(value);            
            return CMD_OK;
        }
    }

    return CMD_BAD_PARAM;
}