#ifndef _cmdspi_h_
#define _cmdspi_h_

typedef struct _SpiBuffer{
    uint8_t len;
    uint8_t *data;
}SpiBuffer;

#ifdef __cplusplus
extern "C" {
#endif

#include "console.h"

class CmdSpi : public ConsoleCommand{
    Console *console;
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
