#ifndef _cmdadc_h_
#define _cmdadc_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "console.h"

class CmdAdc : public ConsoleCommand{
    Console *console;

public:
    void init(void *params) { console = static_cast<Console*>(params);}

    char execute(void *ptr);
    void help(void);

    CmdAdc () : ConsoleCommand("adc") { }    
};


#ifdef __cplusplus
}
#endif

#endif