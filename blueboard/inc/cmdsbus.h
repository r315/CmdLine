#ifndef _cmdsbus_h_
#define _cmdsbus_h_


#ifdef __cplusplus
extern "C" {
#endif
 
#include "console.h"

class CmdSbus : public ConsoleCommand{
    Console *console;
    void Flags(void);
public:
    CmdSbus () : ConsoleCommand("sbus") { }
    void init(void *params) { console = static_cast<Console*>(params); }
    char execute(void *ptr);
    void help(void);
};


#ifdef __cplusplus
}
#endif

#endif