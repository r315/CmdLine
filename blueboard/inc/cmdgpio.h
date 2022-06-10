#ifndef _cmdgpio_h_
#define _cmdgpio_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "command.h"

#define GPIO_PORT_TYPE LPC_GPIO_TypeDef

class CmdGpio : public Command{
    
public:
    CmdGpio (Vcom *vc) : Command("gpio", vc) { }
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