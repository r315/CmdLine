#ifndef _cmdgpio_h_
#define _cmdgpio_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "console.h"


class CmdGpio : public ConsoleCommand{
    Console *console;
public:
    CmdGpio () : ConsoleCommand("gpio") { }
    void init(void *params) { console = static_cast<Console*>(params); }
    char execute(int argc, char **argv);
    void help(void);
    char setPortState(uint32_t port, uint32_t value);
    char getPortState(uint32_t port);
    char setPinState(uint32_t port, uint8_t pin, uint8_t state);
};


#ifdef __cplusplus
}
#endif

#endif