#include "board.h"
#include "cmdspiflash.h"
#include "spi_flash.h"


void CmdSpiFlash::flashDump(uint32_t addr){
    uint8_t buf[16];

    if(!flashRead(addr, buf, 16)){
        return;
    }

    console->print("%08X ",addr);

    for(uint8_t i = 0; i < 16; i++){
        if((i&7) == 0){
            console->putChar(' ');
        }
        console->print("%02X ",buf[i]);
    }

    console->print(" |");

	for(uint8_t i=0; i < 16; i++){
		if(buf[i] > (' '-1) && buf[i] < 0x7F)
			console->putChar(buf[i]);
		else{
			console->putChar(' ');
		}
	}

	console->print("|\n");
}

void CmdSpiFlash::help(void){ 
    console->print("Usage: spiflash <read <addr> | id | rs | ws>\n");
    console->print("\tread,\t read 256 bytes from address\n");
    console->print("\tid,\t read identification\n");
    console->print("\trs,\t read status register\n");
    console->print("\tws,\t write status register\n");
}

char CmdSpiFlash::execute(int argc, char **argv){
    uint32_t value;
    int32_t idx;

    if(argc < 2){
        help();
        return CMD_OK;
    }

    if(strFind("id", argv, argc) != -1){
        console->print("JEDEC Id: %x\n", flashJEDEC_ID());
        console->print("MFD Id: %x\n", flashMFD_ID());
        console->print("Device Id: %x\n", flashDevice_ID());
        return CMD_OK;
    }

    if((idx = strFind("read", argv, argc)) != -1){
        if(hatoi(argv[idx + 1], &value)){
            for(uint16_t i = 0; i < 16; i++){
                flashDump(value + i);
            }
            return CMD_OK;
        }
    }

    if(strFind("rs", argv, argc) != -1){
        console->print("Status: %2X\n", flashReadStatus());
        return CMD_OK;
    }

    if((idx = strFind("ws", argv, argc)) != -1){
        if(hatoi(argv[idx + 1], &value)){
            flashWriteStatus(value);            
            return CMD_OK;
        }
    }

    return CMD_BAD_PARAM;
}