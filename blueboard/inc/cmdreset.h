#ifndef _cmdreset_h_
#define _cmdreset_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "board.h" 
#include "console.h"

class CmdReset : public ConsoleCommand{
    Console *console;
public:
    CmdReset () : ConsoleCommand("reset") { }
    void init(void *params) { console = static_cast<Console*>(params); }
    char execute(void *ptr){
        SW_Reset();
        while(1);        
    }
    void help(void){}
};


#ifdef __cplusplus
}
#endif

#endif