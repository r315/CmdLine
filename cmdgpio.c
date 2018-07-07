
#include <LPC17xx.h>
#include "cmdgpio.h"
#include "strfunctions.h"
#include "vcom.h"
#include "command.h"


CmdLine gpioCommands[] =
{
	{"help",gpioHelp},
	{"get", gpioGet},
};

//-------------------------------------------------------
// "p0.22"
//-------------------------------------------------------
char configOutputPin(char *pp, char state){
LPC_GPIO_TypeDef *port = LPC_GPIO0;
unsigned char pin;

	pin = yatoi(&pp[3]);
	if( (pp[0] != 'p' && pp[0] != 'P') || pp[2] !='.' || pin > 31 || pp[1] > '4')
		return 1;
		
	port[pp[1]-'0'].FIODIR |= (1<<pin);
	port[pp[1]-'0'].FIOMASK &= ~(1<<pin);
	if(state)
		port[pp[1]-'0'].FIOSET |= (1<<pin);
	else
		port[pp[1]-'0'].FIOCLR |= (1<<pin);
	return 0;
}

char setPin(char *pstr){
	return configOutputPin(pstr,1);
}
char clrPin(char *pstr){
	return configOutputPin(pstr,0);
}
//-------------------------------------------------------
// "p0 0x12345678" ou "p0 123456789"
//-------------------------------------------------------
char out(char *pstr){
LPC_GPIO_TypeDef *port = LPC_GPIO0;
int outval;	
	if( (pstr[0] != 'p' && pstr[0] != 'P') || pstr[1] < '0' || pstr[1] > '4')
		return 1;
		
	if(pstr[4] == 'x')
		outval = hatoi(&pstr[5]);
	else
		outval = yatoi(&pstr[3]);
		
	port[pstr[1]-'0'].FIODIR = 0xFFFFFFFF; //32bit output
	port[pstr[1]-'0'].FIOMASK = 0; 
	port[pstr[1]-'0'].FIOPIN = outval;
	return 0;


}

char gpioGet(void *ptr){
LPC_GPIO_TypeDef *ports[4] = {LPC_GPIO0,LPC_GPIO1,LPC_GPIO2,LPC_GPIO3};
	for(uint8_t i = 0; i < 4; i++){
		ports[i]->FIOMASK = 0; 
		VCOM_printf("PORT%u = 0x%08X\n",i, ports[i]->FIOPIN);
	}	
	return CMD_OK;
}

char gpioHelp(void *ptr){
	VCOM_printf("\ngpio commands:\n\r");
	VCOM_printf("\ngpio get,\tGet state current value of all ports\n\r");

	VCOM_printf("\n\n\n\r");
	return CMD_OK;
}

char gpioCmd(void *ptr){
	
	return cmdProcess(ptr, gpioCommands, sizeof(gpioCommands)/sizeof(CmdLine));
}
