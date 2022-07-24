#ifndef _cmdgpio_h_
#define _cmdgpio_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "console.h"

#define GPIO_PORT_TYPE LPC_GPIO_TypeDef

class CmdGpio : public ConsoleCommand{
    Console *console;
public:
    CmdGpio () : ConsoleCommand("gpio") { }
    void init(void *params) { console = static_cast<Console*>(params); }
    char execute(void *ptr);
    void help(void);
    char setPortState(GPIO_PORT_TYPE *port, uint32_t value);
    char getPortState(GPIO_PORT_TYPE *port);
    char setPinState(GPIO_PORT_TYPE *port, uint8_t pin, uint8_t state);
};


#ifdef __cplusplus
}
#endif

#endif