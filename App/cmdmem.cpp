#include "cmdmem.h"


void CmdMem::help(void){
    console->print("Usage: mem <address> [number of bytes] \n\n");
}

char CmdMem::execute(void *ptr){
uint8_t *pdata;
int32_t i, n = 1;
char *p1;


	p1 = (char*)ptr;

	 // check parameters
    if( p1 == NULL || *p1 == '\0'){
        help();
        return CMD_OK;
    }

    if(*p1 != '\0'){
        if(!nextHex(&p1, (uint32_t*)&pdata)){
        	return CMD_BAD_PARAM;
        }
    }
    
    if(*p1 != '\0'){
        nextInt(&p1, &n);
    }

    for( i = 0; i < n; i++){
        if((i & 0x0F) == 0){console->xputchar('\n');}
        console->print("%02X ", *(pdata++) );
    }
	
    console->xputchar('\n');

    return CMD_OK;
}
