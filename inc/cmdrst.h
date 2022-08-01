#ifndef _cmdrst_h_
#define _cmdrst_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "console.h"

class CmdRst : public ConsoleCommand{
    Console *console;

public:
    void init(void *params) { console = static_cast<Console*>(params);}

    char execute(int argc, char **argv);
    void help(void);

    CmdRst () : ConsoleCommand("reset") { }    
};


#ifdef __cplusplus
}
#endif

#endif