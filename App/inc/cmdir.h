#ifndef _cmdir_h_
#define _cmdir_h_


#ifdef __cplusplus
extern "C" {
#endif
 
#include "console.h"

class CmdIr : public ConsoleCommand{
    Console *console;
public:
    void init(void *params) { console = static_cast<Console*>(params); }
    
    void help(void);
    char execute(int argc, char **argv);

    CmdIr () : ConsoleCommand("ir") { }
};


#ifdef __cplusplus
}
#endif

#endif