#ifndef _cmdavr_h_
#define _cmdavr_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "command.h"


#define AVR_DW_SYNC_TIMEOUT     10
#define AVR_INSTRUCTION_SIZE    4

#define AVR_RST_PIN_PORT       LPC_GPIO0
#define AVR_RST_PIN            (1<<24)

#define AVR_RST0 AVR_RST_PIN_PORT->FIOCLR = AVR_RST_PIN
#define AVR_RST1 AVR_RST_PIN_PORT->FIOSET = AVR_RST_PIN
#define AVR_RSTZ AVR_RST_PIN_PORT->FIODIR &= ~AVR_RST_PIN       // Set RST Pin input
#define AVR_RSTY AVR_RST_PIN_PORT->FIODIR |= AVR_RST_PIN        // Set RST Pin output 

#define AVR_DISABLE_RESET               \
            DelayMs(100);               \
            AVR_RST1;


#define LOW_BYTE(W) (uint8_t)(((W) >> 0) & 0xff)
#define HIGH_BYTE(W) (uint8_t)(((W) >> 8) & 0xff)


class CmdAvr : public Command{

public:
    CmdAvr (Vcom *vc) : Command("avr", vc) { }
    char execute(void *ptr);
    void help(void);
    char avrFuses(void *ptr);
};

void avrDeviceCode(uint8_t *buf);
void avrChipErase(void);
char avrProgrammingEnable(uint8_t);

uint16_t avrReadProgram(uint16_t addr);
void avrLoadProgramPage(uint8_t addr, uint16_t value);
void avrWriteProgramPage(uint16_t addr);

#ifdef __cplusplus
}
#endif

#endif
