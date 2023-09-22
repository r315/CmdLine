#include "board.h"
#include "cmdgpio.h"

//-------------------------------------------------------
// 
//-------------------------------------------------------
char CmdGpio::setPinState(uint32_t p, uint8_t pin, uint8_t state){
    LPC_GPIO_TypeDef *port = (LPC_GPIO_TypeDef*)p;

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
char CmdGpio::setPortState(uint32_t p, uint32_t value){
    LPC_GPIO_TypeDef *port = (LPC_GPIO_TypeDef*)p;
	port->FIODIR = 0xFFFFFFFF; //32bit output
	port->FIOMASK = 0; 
	port->FIOPIN = value;
	return CMD_OK;
}
/**
 * 
 * */
char CmdGpio::getPortState(uint32_t p){
    uint32_t value;	
    LPC_GPIO_TypeDef *port = (LPC_GPIO_TypeDef*)p;
	//ports[port]->FIOMASK = 0; 

	value = port->FIOPIN;
	console->printf("%08X = 0x%08X\n",port, value);

	for (uint8_t i = 0; i < 32; i++){
		console->printf("|%02u", 31 - i);
	}
	console->printchar('\n');
	for (uint8_t i = 0; i < 32; i++){
		console->printf("+--", 31 - i);
	}
	console->printchar('\n');
	for (uint8_t i = 0; i < 32; i++){
		console->printf("| %c", (value & (uint32_t)(1<<31))? '1' : '0');
		value = (uint32_t)(value << 1);
	}
	console->printchar('\n');
	return CMD_OK;
}

void CmdGpio::help(void){
 	console->println("Usage: gpio -p <port> [option] \n");
    console->println("\t -p, port 0-3");
    //console->printfln("\t -r, read 32bit port");
    console->println("\t -b <bit>, set bit value");
}

char CmdGpio::execute(int argc, char **argv){
	uint32_t aux, pin;

	if( !xstrcmp(argv[1],"init")){		
		if(ha2i(argv[2], &pin)){
        	if(ha2i(argv[3], &aux)){
				GPIO_Init((pinName_e)pin, aux);
			}
		}
        
		return CMD_OK;
	}

	if( !xstrcmp(argv[1],"mode")){		
		if(ha2i(argv[2], &pin)){
        	if(ha2i(argv[3], &aux)){
				GPIO_Mode((pinName_e)pin, aux);
			}
		}
        
		return CMD_OK;
	}

	if( !xstrcmp(argv[1],"func")){		
		if(ha2i(argv[2], &pin)){
        	if(ha2i(argv[3], &aux)){
				GPIO_Function((pinName_e)pin, aux);
			}
		}
        
		return CMD_OK;
	}

	if( !xstrcmp(argv[1],"dir")){		
		if(ha2i(argv[2], &pin)){
        	if(ha2i(argv[3], &aux)){
				GPIO_Direction((pinName_e)pin, aux);
			}
		}
        
		return CMD_OK;
	}

	return CMD_BAD_PARAM;
}
