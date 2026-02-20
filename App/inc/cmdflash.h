
#ifndef _cmdflash_h_
#define _cmdflash_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "console.h"

class CmdFlash : public ConsoleCommand{
public:
    CmdFlash () : ConsoleCommand("flash") { }
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
