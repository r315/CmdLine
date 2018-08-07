#ifndef _cmdavr_h_
#define _cmdavr_h_


#ifdef __cplusplus
extern "C" {
#endif

#include <common.h>     // all drivers headers should be placed here 
#include "command.h"


#define AVR_RST_PIN_PORT       LPC_GPIO0
#define AVR_RST_PIN            (1<<24)

#define AVR_RST0 AVR_RST_PIN_PORT->FIOCLR = AVR_RST_PIN
#define AVR_RST1 AVR_RST_PIN_PORT->FIOSET = AVR_RST_PIN

class CmdAvr : public Command{

public:
    CmdAvr (Vcom *vc) : Command("avr", vc) { }
    char execute(void *ptr);
    void help(void);
    char avrFuses(void *ptr);
};

uint32_t avrSignature(uint8_t *buf);
void avrErase(void);
char avrProgrammingEnable(uint8_t);

#ifdef __cplusplus
}
#endif

#endif