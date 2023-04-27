
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
    console->putString("\nUsage: spi <bus> <option>  [param]");
	console->putString("\tsend <data0> [dataN] : Send data");
	console->putString("\tinit : Initialise auxiliary SPI peripheral");
	console->putString("\tspeed <speed> : set clock frequency");
    console->putString("\tstatus : bus status");
    console->putString("\tmode <0-3>: set spi mode");
    console->putString("\tcs <0-1>: Enable HW CS");
    /*console->putString("  Spi Pins\n"
                    "\tSCK   P0.7\n"
                    "\tMISO  P0.8\n"
                    "\tMOSI  P0.9\n"
                    "\tSSEL  NA\n"); */

}

char CmdSpi::execute(int argc, char **argv){
	uint8_t data[8];
	uint32_t aux;

    if(argc < 2){
        help();
        return CMD_OK;
    }

    if(ia2i(argv[1], (int32_t*)&aux) == 0){
 		return CMD_BAD_PARAM;
	}

	spibus_t *spi = (aux != 0) ? BOARD_GetSpiAux() : BOARD_GetSpiMain();

	if(xstrcmp("send", argv[2]) == 0){
		if(spi == NULL){
			SPI_Init(spi);
		}

		uint8_t n = 0;
		while(argv[n + 3] != NULL){
            if(hatoi(argv[n + 3], &aux)){        
                data[n] = aux;
            }
            n++;
        }
		SPI_Transfer(spi, data, n);
		return CMD_OK;
	}else if(xstrcmp("init", argv[2]) == 0){
		SPI_Init(spi);
		return CMD_OK;
	}else if(xstrcmp("speed", argv[2]) == 0){
		if(ia2i(argv[3], (int32_t*)&aux)){			
            spi->freq = aux;
            SPI_Init(spi);
    		return CMD_OK;
		}
	}else if(xstrcmp("status", argv[2]) == 0){
        console->print("Bus: %d\n", spi->bus);
        console->print("Speed: %d Hz\n", spi->freq);
        console->print("Flags: MODE%d", spi->flags >> 6);
        if(spi->flags & SPI_HW_CS){
            console->print(" | HW_CS");
        }

        if(spi->flags & SPI_ENABLED){
            console->print(" | ENABLED");
        }
        
        console->print(" (%02x)\n", spi->flags);        
        return CMD_OK;
	}else if(xstrcmp("mode", argv[2]) == 0){
        if(ia2i(argv[3], (int32_t*)&aux)){
            aux = (aux & 3) << 6;
            spi->flags = (spi->flags & ~SPI_MODE3) | aux;
		    SPI_Init(spi);
		    return CMD_OK;
        }
	}else if(xstrcmp("cs", argv[2]) == 0){
        if(ia2i(argv[3], (int32_t*)&aux)){
            aux = (aux & 1) << 3;
            spi->flags = (spi->flags & ~SPI_HW_CS) | aux;
		    SPI_Init(spi);
		    return CMD_OK;
        }
	}

    return CMD_BAD_PARAM;
}