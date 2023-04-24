#ifndef _cmdmsendor_h_
#define _cmdmsensor_h_


#ifdef __cplusplus
extern "C" {
#endif

#include <console.h>

class CmdMSensor : public ConsoleCommand{
	Console *console;
public:
    CmdMSensor () : ConsoleCommand("msensor") {}
    void init(void *params) { console = static_cast<Console*>(params); }

    char execute(int argc, char **argv);
    void help(void);
};


#ifdef __cplusplus
}
#endif

#endif
