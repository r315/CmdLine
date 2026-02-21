#include "board.h"
#include "cmdflash.h"
#include "spiflash/flash_dev.h"


void CmdFlash::flashDump(uint32_t addr){
    uint8_t buf[16];

    if(flashRead(buf, addr, 16) != FLASH_DEV_OK){
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

void CmdFlash::help(void){
    console->println("Usage: spiflash <read <addr> | id | rs | ws | se | ce>");
    console->println("\trd <addr>,\t read 256 bytes from address");
    console->println("\tid,\t read identification");
    //console->println("\trs,\t read status register");
    //console->println("\tws,\t write status register");
    console->println("\tse <addr>,\t 4k sector erase");
}

char CmdFlash::execute(int argc, char **argv){
    uint32_t value = 0;
    int32_t idx;

    if(argc < 2){
        help();
        return CMD_OK;
    }

    if(strFind("id", argv, argc) != -1){
        flashReadId(&value);
        console->printf("RDID: %x\n", value);
        return CMD_OK;
    }

    if((idx = strFind("rd", argv, argc)) != -1){
        if(ha2u(argv[idx + 1], &value)){
            for(uint16_t i = 0; i < 16; i++){
                flashDump(value + (i << 4));
            }
            return CMD_OK;
        }
    }

    if( !xstrcmp("wr", argv[1])){
        uint8_t buf[16];idx = 0;
        uint32_t addr;
        if(!ha2u(argv[2], &addr)){return CMD_BAD_PARAM;}
        while(ha2u(argv[3 + idx], (uint32_t*)&value)){
            buf[idx++] = (uint8_t)value;
        }

        if(flashWrite(buf, addr, idx)){
            console->println("Failed to write");
        }

        return CMD_OK;
    }

    if((idx = strFind("se", argv, argc)) != -1){
        if(ha2u(argv[idx + 1], &value)){
            flashErase(value, FLASH_DEV_BLK_4K);
            return CMD_OK;
        }
    }

    if((idx = strFind("ce", argv, argc)) != -1){
        flashErase(0, FLASH_DEV_BLK_ALL);
        return CMD_OK;
    }
#if 0
    if(strFind("rs", argv, argc) != -1){
        console->printf("Status: %2X\n", flashReadStatus());
        return CMD_OK;
    }

    if((idx = strFind("ws", argv, argc)) != -1){
        if(ha2u(argv[idx + 1], &value)){
            flashWriteStatus(value);
            return CMD_OK;
        }
    }
#endif
    return CMD_BAD_PARAM;
}