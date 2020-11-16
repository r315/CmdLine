
#ifndef _cmdflashrom_h_
#define _cmdflashrom_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "console.h"

class CmdFlashRom : public ConsoleCommand{
public:
    CmdFlashRom () : ConsoleCommand("flashrom") { }
    void init(void *params) { console = static_cast<Console*>(params); }
    char execute(void *ptr);
    void help(void);
private:
    Console *console;
};


#ifdef __cplusplus
}
#endif

#endif
