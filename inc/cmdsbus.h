#ifndef _cmdsbus_h_
#define _cmdsbus_h_


#ifdef __cplusplus
extern "C" {
#endif
 
#include "command.h"

class CmdSbus : public Command{

public:
    CmdSbus (Vcom *vc) : Command("sbus", vc) { }
    char execute(void *ptr);
    void help(void);
};


#ifdef __cplusplus
}
#endif

#endif