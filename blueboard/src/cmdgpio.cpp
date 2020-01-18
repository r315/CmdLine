

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
 	vcom->printf("Usage: gpio -p <port> [option] \n\n");  
    vcom->printf("\t -p, port 0-3\n");
    //vcom->printf("\t -r, read 32bit port\n");
    vcom->printf("\t -b <bit>, set bit value\n");
}

char CmdGpio::execute(void *ptr){
GPIO_PORT_TYPE *ports[4] = {LPC_GPIO0,LPC_GPIO1,LPC_GPIO2,LPC_GPIO3};
uint8_t port, setbit, setstate;
char *p1;

	p1 = (char*)ptr;

	 // check parameters
    if( p1 == NULL || *p1 == '\0'){
        help();
        return CMD_OK;
    }

	port = 255;
	setbit = 255;  

	// parse options
	while(*p1 != '\0'){
		if( !xstrcmp(p1,"-p")){
			p1 = nextWord(p1);
			port = nextInt(&p1);
		}else if( !xstrcmp(p1,"-r")){
			p1 = nextWord(p1);
		}else if( !xstrcmp(p1,"-b")){
			p1 = nextWord(p1);
			setbit = nextInt(&p1);
			setstate = nextInt(&p1);
		}else{
			p1 = nextWord(p1);
		}
	}

	if(port > 3){
		return CMD_BAD_PARAM;
	}

	if(setbit < 32){
		setPinState(ports[port], setbit, setstate);
	}else{
		getPortState(ports[port]);
	}

	//vcom->printf(" Port %d, Read %d, Bit %d\n", port,readport,setbit);

	return CMD_OK;
}
