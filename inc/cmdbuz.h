#ifndef _cmdbuz_h_
#define _cmdbuz_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "console.h"

class CmdBuz : public ConsoleCommand{
    Console *console;
    bool hw_init;
    
public:
    void init(void *params) { console = static_cast<Console*>(params);}

    char execute(void *ptr);
    void help(void);

    CmdBuz () : ConsoleCommand("buz") { }    
};


#ifdef __cplusplus
}
#endif

#endif