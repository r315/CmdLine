#include "board.h"
#include "cmdgpio.h"

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
	console->print("%08X = 0x%08X\n",port, value);

	for (uint8_t i = 0; i < 32; i++){
		console->print("|%02u", 31 - i);
	}
	console->putChar('\n');
	for (uint8_t i = 0; i < 32; i++){
		console->print("+--", 31 - i);
	}
	console->putChar('\n');
	for (uint8_t i = 0; i < 32; i++){
		console->print("| %c", (value & (uint32_t)(1<<31))? '1' : '0');
		value = (uint32_t)(value << 1);
	}
	console->putChar('\n');
	return CMD_OK;
}

void CmdGpio::help(void){
 	console->print("Usage: gpio -p <port> [option] \n\n");  
    console->print("\t -p, port 0-3\n");
    //console->printf("\t -r, read 32bit port\n");
    console->print("\t -b <bit>, set bit value\n");
}

char CmdGpio::execute(void *ptr){
	char *argv[4] = {0};
    int argc;
	uint32_t aux, pin;

	argc = strToArray((char*)ptr, argv);

	if( !xstrcmp(argv[0],"init")){		
		if(hatoi(argv[1], &pin)){
        	if(hatoi(argv[2], &aux)){
				GPIO_Init((pinName_e)pin, aux);
			}
		}
        
		return CMD_OK;
	}

	if( !xstrcmp(argv[0],"mode")){		
		if(hatoi(argv[1], &pin)){
        	if(hatoi(argv[2], &aux)){
				GPIO_Mode((pinName_e)pin, aux);
			}
		}
        
		return CMD_OK;
	}

	if( !xstrcmp(argv[0],"func")){		
		if(hatoi(argv[1], &pin)){
        	if(hatoi(argv[2], &aux)){
				GPIO_Function((pinName_e)pin, aux);
			}
		}
        
		return CMD_OK;
	}

	if( !xstrcmp(argv[0],"dir")){		
		if(hatoi(argv[1], &pin)){
        	if(hatoi(argv[2], &aux)){
				GPIO_Direction((pinName_e)pin, aux);
			}
		}
        
		return CMD_OK;
	}


	return CMD_BAD_PARAM;
}
