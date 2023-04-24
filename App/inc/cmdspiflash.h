
#ifndef _cmdspiflash_h_
#define _cmdspiflash_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "console.h"

class CmdSpiFlash : public ConsoleCommand{
public:
    CmdSpiFlash () : ConsoleCommand("spiflash") { }
    void init(void *params) { console = static_cast<Console*>(params); }
    char execute(int argc, char **argv);
    void help(void);
private:
    Console *console;
    void flashDump(uint32_t addr);
};

#ifdef __cplusplus
}
#endif

#endif
