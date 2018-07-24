#ifndef _cmdecho_h_
#define _cmdecho_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "command.h"

class CmdEcho : public Command{

public:
    CmdEcho (Vcom *vc) : Command("echo", vc) { }
    char execute(void *ptr){
        vcom->printf("%s\n", (char*)ptr);
        return CMD_OK;
    }
    void help(void){}
};


#ifdef __cplusplus
}
#endif

#endif