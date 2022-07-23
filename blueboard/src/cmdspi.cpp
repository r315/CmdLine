
#include "board.h"
#include "cmdspi.h"


#ifdef SPI_BITBANG
static uint8_t out_(uint8_t data){
uint8_t datain = 0;
	for(int i = 0; i < 8; i++){
		if(data & (0x80>>i)){
			LPC_GPIO1->FIOSET = (1<<4);
		}else{
			LPC_GPIO1->FIOCLR = (1<<4);
		}
		DelayMs(1);
		LPC_GPIO1->FIOSET = (1<<1);
		datain = (LPC_GPIO1->PIN & (1<<3)) ? (datain | 1) : datain;
		DelayMs(1);
		LPC_GPIO1->FIOCLR = (1<<1);
		data <<= 1;
	}	
	return datain
}

static void spiWrite(uint8_t *data, uint32_t len){
	for(int i = 0; i < len; i++){
		out_(*(data + i));
	}
}
#else

static void spiWrite(uint8_t *data, uint32_t len){	
    BOARD_SPI_Write(data, len);
}
#endif

void spiWriteBuffer(SpiBuffer *buf){
	spiWrite(buf->data, buf->len);
}
//--------------------------------------
//
//--------------------------------------
void CmdSpi::help(void){
    console->putString("\nUsage: spi -w [data]");
    console->putString("  Spi Pins\n"
                    "\tSCK   P0.7\n"
                    "\tMISO  P0.8\n"
                    "\tMOSI  P0.9\n"
                    "\tSSEL  NA\n"); 

}

char CmdSpi::execute(void *ptr){
	char *argv[4] = {0};
    int argc;
	uint8_t i, data[8];
	SpiBuffer spibuf;

	uint32_t aux;

	argc = strToArray((char*)ptr, argv);

    if(argc < 1){
        help();
        return CMD_OK;
    }

    i = 0;
    spibuf.data = data;

	if( !xstrcmp(argv[0],"-w")){	
		while(argv[i] != NULL){
            if(hatoi(argv[i], &aux)){        
                spibuf.data[i] = aux;
            }
            i++;
        }
        spibuf.len = i;
        spiWriteBuffer(&spibuf);
	}

    return CMD_OK;
}