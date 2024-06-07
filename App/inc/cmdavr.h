#ifndef _cmdavr_h_
#define _cmdavr_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "console.h"
#include "spi.h"

class CmdAvr : public ConsoleCommand{ 
public:
    void init(void *params) {console = static_cast<Console*>(params);}
    CmdAvr () : ConsoleCommand("avr") { }
    char execute(int argc, char **argv);
    void help(void);

private:
    Console *console;
    spibus_t avrspibus;
};

#ifdef __cplusplus
}
#endif

#endif
