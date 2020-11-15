#ifndef _cmd_servo_h_
#define _cmd_servo_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "console.h"

class CmdServo : public ConsoleCommand{
    Console *console;
public:
    void init(void *params) { console = static_cast<Console*>(params); }
    CmdServo () : ConsoleCommand("servo") { }
    char execute(void *ptr);
    void help(void);
};


#ifdef __cplusplus
}
#endif

#endif
