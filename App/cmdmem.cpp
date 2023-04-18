#include "board.h"
#include "cmdmem.h"

static uint8_t ram0[1024] __attribute__ ((section(".data")));
static uint8_t ram1[1024] __attribute__ ((section(".data1")));
static uint8_t ram2[1024] __attribute__ ((section(".data2")));


__attribute__ ((section(".data1.code")))
uint32_t memTest(void *block, uint32_t len){
    uint8_t seed = 0x45;
    uint8_t *ptr = (uint8_t*)block;
    
    SysTick->VAL = 0;

    do{
        *ptr++ ^= seed;
    }while(--len);

    return  SysTick->VAL;
}

void CmdMem::help(void){
    console->print("Usage: mem NOT implemented \n\n");
}

char CmdMem::execute(int argc, char **argv){

    if(xstrcmp("test", (const char*)argv[1]) == 0){
        console->print("Ram0: %u\n", memTest(ram0, sizeof(ram0)));
        console->print("Ram1: %u\n", memTest(ram1, sizeof(ram1)));
        console->print("Ram2: %u\n", memTest(ram2, sizeof(ram2)));
    }

    return CMD_OK;
}
