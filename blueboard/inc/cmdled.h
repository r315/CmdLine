#ifndef _cmdled_h_
#define _cmdled_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "command.h"

class CmdLed : public Command{

public:
    CmdLed (Vcom *vc) : Command("led", vc) { }
    void help(void){}
    char execute(void *ptr){
        vcom->puts("Name  Color  Designator  Pin\n"
                    "LED1  Blue   D8          P1.29\n"
                    "LED2  Red    D7          P1.18\n"
                    "LED3  Red    D1          P2.9\n"); 
        return CMD_OK;
    }
};


#ifdef __cplusplus
}
#endif

#endif