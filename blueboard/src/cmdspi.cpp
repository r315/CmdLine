
#include <spi.h>
#include "cmdspi.h"

Spi_Type spi;

char spistatus = 0;

void spiInit(void){
    spi.bus = 1;
    spi.freq = 500000;
    spi.cfg  = SPI_MODE0 | SPI_8BIT;
    SPI_Init(&spi);
	//LPC_PINCON->PINSEL0 |= SSP1_SSEL;
	spistatus |= SPI_INIT;
}

void spiSetFrequency(uint32_t freq){
    if((spistatus & SPI_INIT) == 0){
		spiInit();
	}    
    spi.freq = 100000;    
    SPI_Init(&spi);
}

#ifdef SPI_BITBANG
uint8_t out_(uint8_t data){
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

void spiWrite(uint8_t *data, uint32_t len){
	if((spistatus & SPI_INIT) == 0){
		spiInit();
	}	
	for(int i = 0; i < len; i++){
		out_(*(data + i));
	}
}
#else

void spiWrite(uint8_t *data, uint32_t len){
	if((spistatus & SPI_INIT) == 0){
		spiInit();
	}	
    SPI_Transfer(&spi, data, len);
}
#endif

void spiWriteBuffer(SpiBuffer *buf){
	spiWrite(buf->data, buf->len);
}
//--------------------------------------
//
//--------------------------------------
void CmdSpi::help(void){
    console->xputs("\nUsage: spi -w [data]");
    console->xputs("  Spi Pins\n"
                    "\tSCK   P0.7\n"
                    "\tMISO  P0.8\n"
                    "\tMOSI  P0.9\n"
                    "\tSSEL  NA\n"); 

}

char CmdSpi::execute(void *ptr){
uint8_t i, data[8];
char *p1;
SpiBuffer spibuf;
uint32_t aux;

	p1 = (char*)ptr;

    //_vcom = this->vcom;

	 // check parameters
    if( p1 == NULL || *p1 == '\0'){
        help();
        return CMD_OK;
    }

    i = 0;
    spibuf.data = data;

	// parse options
	while(*p1 != '\0'){
		if( !xstrcmp(p1,"-w")){
			    p1 = nextWord(p1);
            while(*p1 != '\0'){
                nextHex(&p1, &aux);
                spibuf.data[i] = aux;
                i++;
            }
            spibuf.len = i;
            spiWriteBuffer(&spibuf);
        }else{
			p1 = nextWord(p1);
		}
	}

    return CMD_OK;
}