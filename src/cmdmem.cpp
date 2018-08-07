#include "cmdmem.h"


void CmdMem::help(void){
    vcom->printf("Usage: mem [option] \n\n");  
    vcom->printf("\t n, number of bytes\n");
}

char CmdMem::execute(void *ptr){
uint8_t *pdata,i, n = 1;
char *p1;

	p1 = (char*)ptr;

	 // check parameters
    if( p1 == NULL || *p1 == '\0'){
        help();
        return CMD_OK;
    }

    if(*p1 != '\0'){
        pdata = (uint8_t*)nextHex(&p1);
    }
    
    if(*p1 != '\0'){
        n = (uint8_t)nextInt(&p1);
    }

    for( i = 0; i < n; i++){
        if((i & 0x0F) == 0){vcom->putchar('\n');}
        vcom->printf("%02X ", *(pdata++) );
    }
	
    vcom->putchar('\n');

    return CMD_OK;
}