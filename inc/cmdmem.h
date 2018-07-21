#ifndef _cmdmem_h_
#define _cmdmem_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "command.h"

class CmdMem : public Command{

public:
    CmdMem (Vcom *vc) : Command("mem", vc) { }
    char execute(void *ptr){
    uint32_t *data;
    char *p1 = (char*)ptr;

	// check parameters
    if( p1 == NULL || *p1 == '\0'){
        return CMD_OK;
    }
        data = (uint32_t*)nextHex(&p1);

        vcom->printf("%08X\n", *data);
        return CMD_OK;        
    }
    void help(void){}
};


#ifdef __cplusplus
}
#endif

#endif