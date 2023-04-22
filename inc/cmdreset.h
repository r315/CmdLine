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
    void init(void *params) { console = static_cast<Console*>(params); }
    CmdReset () : ConsoleCommand("reset") { }
    
    char execute(int argc, char **argv){
        SW_Reset();
        return CMD_OK;        
    }
    
    void help(void){}
};


#ifdef __cplusplus
}
#endif

#endif