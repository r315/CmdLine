#ifndef _cmdrpm_h_
#define _cmdrpm_h_


#ifdef __cplusplus
extern "C" {
#endif
 
#include "console.h"

class CmdRpm : public ConsoleCommand{
    Console *console;
public:
    void init(void *params) { console = static_cast<Console*>(params); }
    
    void help(void);
    char execute(int argc, char **argv);

    CmdRpm () : ConsoleCommand("rpm") { }
};


#ifdef __cplusplus
}
#endif

#endif