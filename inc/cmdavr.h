#ifndef _cmdavr_h_
#define _cmdavr_h_


#ifdef __cplusplus
extern "C" {
#endif

#include <common.h>     // all drivers headers should be placed here 
#include "command.h"

class CmdAvr : public Command{

public:
    CmdAvr (Vcom *vc) : Command("avr", vc) { }
    char execute(void *ptr);
    void help(void);
    char avrFuses(void *ptr);
};



#ifdef __cplusplus
}
#endif

#endif