#ifndef _cmdpwm_h_
#define _cmdpwm_h_

#include "console.h"
#include "pwm.h"

class CmdPwm : public ConsoleCommand{
    Console *console;
    pwmchip_t pwmchip;
public:
    CmdPwm() : ConsoleCommand("pwm") { }
    void init(void *params);
    void help(void);
    char execute(int argc, char **argv);
};

#endif
