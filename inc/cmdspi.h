#ifndef _cmdspi_h_
#define _cmdspi_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "console.h"
#include "spi.h"

class CmdSpi : public ConsoleCommand{
    Console *console;
    spibus_t *m_spi;
public:
    CmdSpi () : ConsoleCommand("spi") { }
    void init(void *params){console = static_cast<Console*>(params);}
    char execute(void *ptr);
    void help(void);
};

#ifdef __cplusplus
}
#endif

#endif