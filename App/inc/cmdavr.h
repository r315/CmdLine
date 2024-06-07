#ifndef _cmdavr_h_
#define _cmdavr_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "console.h"
#include "board.h"

class CmdAvr : public ConsoleCommand{ 
public:
    void init(void *params) {console = static_cast<Console*>(params);}
    CmdAvr () : ConsoleCommand("avr") { }
    char execute(int argc, char **argv);
    void help(void);

private:
    Console *console;
    spibus_t avrspibus;
    uint32_t rst_pin;

    uint32_t avrReadFuses(void);
    void avrWriteFuses(uint8_t lh, uint8_t fuses);
    void avrDeviceCode(uint8_t *buf);
    void avrChipErase(void);
    uint8_t avrProgrammingEnable(uint8_t en, uint8_t trydw);
    uint16_t avrReadProgram(uint16_t addr);
    void avrLoadProgramPage(uint8_t addr, uint16_t value);
    void avrWriteProgramPage(uint16_t addr);
    int avrWaitReady(void);
};

#ifdef __cplusplus
}
#endif

#endif
