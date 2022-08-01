#ifndef _cmdtft_h_
#define _cmdtft_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "console.h"

class CmdTft : public ConsoleCommand{
    Console *console;

public:
    void init(void *params) { console = static_cast<Console*>(params);}
    CmdTft () : ConsoleCommand("tft") { }    

    char execute(int argc, char **argv);
    void help(void);
    uint32_t fps(void (*func)(void));
};

#ifdef __cplusplus
}
#endif

#endif