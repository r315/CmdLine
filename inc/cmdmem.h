#ifndef _cmdmem_h_
#define _cmdmem_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "command.h"

class CmdMem : public Command{

public:
    CmdMem (Vcom *vc) : Command("mem", vc) { }
    char execute(void *ptr);
    void help(void);
};


#ifdef __cplusplus
}
#endif

#endif