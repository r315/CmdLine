#ifndef _cmdpwm_h_
#define _cmdpwm_h_

#ifdef __cplusplus
extern "C" {
#endif
#include <common.h>
#include "command.h"


void pwm_start(void *ptr);

class CmdPwm : public Command{
/*Fmap functions [1] = {
    {"-s", CmdPwm::start},
};
*/
uint8_t running;

public:
    CmdPwm (Vcom *vc) : Command("pwm", vc) { 
        running = OFF;
    }
    void start(void);
    char execute(void *ptr);
    void help(void);
};

typedef struct _Fmap{
    const char *name;
    //void CmdPwm::(*call)(void*);
}Fmap;

#ifdef __cplusplus
}
#endif

#endif