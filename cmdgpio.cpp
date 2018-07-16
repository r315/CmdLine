

#include "command.h"
#include "cmdgpio.h"
#include "strfunctions.h"

//-------------------------------------------------------
// 
//-------------------------------------------------------
char CmdGpio::setPinState(GPIO_PORT_TYPE *port, uint8_t pin, uint8_t state){

	port->FIODIR |= (1<<pin);
	port->FIOMASK &= ~(1<<pin);

	if(state)
		port->FIOSET |= (1<<pin);
	else
		port->FIOCLR |= (1<<pin);
	
	return CMD_OK;
}
//-------------------------------------------------------
//
//-------------------------------------------------------
char CmdGpio::setPortState(GPIO_PORT_TYPE *port, uint32_t value){
	port->FIODIR = 0xFFFFFFFF; //32bit output
	port->FIOMASK = 0; 
	port->FIOPIN = value;
	return CMD_OK;
}
/**
 * 
 * */
char CmdGpio::getPortState(GPIO_PORT_TYPE *port){
uint32_t value;	
	//ports[port]->FIOMASK = 0; 

	value = port->FIOPIN;
	vcom->printf("%08X = 0x%08X\n",port, value);

	for (uint8_t i = 0; i < 32; i++){
		vcom->printf("|%02u", 31 - i);
	}
	vcom->putc('\n');
	for (uint8_t i = 0; i < 32; i++){
		vcom->printf("+--", 31 - i);
	}
	vcom->putc('\n');
	for (uint8_t i = 0; i < 32; i++){
		vcom->printf("| %c", (value & (uint32_t)(1<<31))? '1' : '0');
		value = (uint32_t)(value << 1);
	}
	vcom->putc('\n');
	return CMD_OK;
}

void CmdGpio::help(void){
	
}

char CmdGpio::execute(void *ptr){
GPIO_PORT_TYPE *ports[4] = {LPC_GPIO0,LPC_GPIO1,LPC_GPIO2,LPC_GPIO3};
uint8_t args[3], i = 0;

	if( *((char*)ptr) == '\0'){
		return CMD_BAD_PARAM;
	}

	while( *((uint8_t*)ptr) != '\0'){
		args[i++] = nextInt((char**)&ptr);
	}
	
	 for(int j=0 ; j < 3; j++){
      vcom->printf("%d ", args[j]);
  	}
	//getPortState(ports[args[0]]);
	setPinState(ports[args[0]], args[1], args[2]);
	return CMD_OK;
}
