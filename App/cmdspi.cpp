
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
#endif

//--------------------------------------
//
//--------------------------------------
void CmdSpi::help(void){
    console->putString("\nUsage: spi <option> [param]");
	console->putString("\tsend <data0> [dataN] : Send data");
	console->putString("\tinit : Initialise auxiliary SPI peripheral");
    /*console->putString("  Spi Pins\n"
                    "\tSCK   P0.7\n"
                    "\tMISO  P0.8\n"
                    "\tMOSI  P0.9\n"
                    "\tSSEL  NA\n"); */

}

char CmdSpi::execute(int argc, char **argv){
	uint8_t i, data[8];
	uint32_t aux;

    if(argc < 1){
        help();
        return CMD_OK;
    }

    i = 0;

	if( !xstrcmp(argv[1],"send")){	
		while(argv[i + 1] != NULL){
            if(hatoi(argv[i + 1], &aux)){        
                data[i] = aux;
            }
            i++;
        }
		SPI_Write(m_spi, data, i);
		return CMD_OK;
	}

	if( !xstrcmp(argv[1],"init")){
		m_spi = BOARD_GetSpiAux();
		m_spi->freq = 100000;
    	m_spi->flags  = SPI_MODE0 | SPI_HW_CS;
    	SPI_Init(m_spi);
		return CMD_OK;
	}

    return CMD_BAD_PARAM;
}