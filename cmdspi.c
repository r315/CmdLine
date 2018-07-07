#include <LPC17xx.h>
#include "cmdspi.h"
#include "strfunctions.h"
#include "vcom.h"
#include "command.h"
#include <spi.h>

CmdLine spiCommands[] =
{
	{"help",spiHelp},
	{"write",spiWrite},
};

static volatile char spistatus = 0;


void spiInit(void){
	SSP_Init(SPI_IF, 1000, 8);
	//LPC_PINCON->PINSEL0 |= SSP1_SSEL;
	spistatus |= SPI_INIT;
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

char spiWriteBuffer(SpiBuffer *buf){
		
	for(int i = 0; i < buf->len; i++){
		out_(*(buf->data + i));
	}

	return CMD_OK;
}
#else
char spiWriteBuffer(SpiBuffer *buf){
	if((spistatus & SPI_INIT) == 0){
		spiInit();
	}
	
	SSP_Transfer(SPI_IF, buf->data, buf->len);
	return CMD_OK;
}
#endif
char spiHelp(void *ptr){
	VCOM_printf("\n\rspi commands:\n\r");
	VCOM_printf("\nwrite,\tsend data through spi port\n\r");

	VCOM_printf("\n\n\n\r");
	return CMD_OK;
}

char spiWrite(void *ptr){
	//SSP_Transfer(SPI_IF,buf, sizeof(buf));	
	//VCOM_bufferHex(buf, sizeof(buf));
	return CMD_OK;
}

char spiCmd(void *ptr){	
	if(cmdProcess(ptr, spiCommands, sizeof(spiCommands)/sizeof(CmdLine)) == CMD_NOT_FOUND){
        spiHelp(NULL);
    }
	return CMD_OK;
}