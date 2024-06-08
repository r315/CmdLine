#ifndef _cmdspi_h_
#define _cmdspi_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "console.h"

class CmdSpi : public ConsoleCommand{
    Console *console;
public:
    CmdSpi () : ConsoleCommand("spi") { }
    void init(void *params){console = static_cast<Console*>(params);}
    char execute(int argc, char **argv);
    void help(void);
};

#ifdef __cplusplus
}
#endif

#endif
