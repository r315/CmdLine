#ifndef _cmdpwm_h_
#define _cmdpwm_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "console.h"

class CmdPwm : public ConsoleCommand{
    Console *console;
    uint8_t ch_en;
public:
    CmdPwm() : ConsoleCommand("pwm") { ch_en = 0; }
    void init(void *params) { console = static_cast<Console*>(params); }    
    void enable(uint8_t ch);
    void disable(uint8_t ch);
    void start(uint32_t period);
    char execute(void *ptr);
    void help(void);
};


#ifdef __cplusplus
}
#endif

#endif
