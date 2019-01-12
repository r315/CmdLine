#ifndef _cmdawg_h_
#define _cmdawg_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "command.h"

class CmdAwg : public Command{

public:
    CmdAwg (Vcom *vc) : Command("awg", vc) { }
    char execute(void *ptr);
    void help(void);
};


#ifdef __cplusplus
}
#endif

#endif
