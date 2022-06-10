#ifndef _cmdawg_h_
#define _cmdawg_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "console.h"
#include "dac.h"

class CmdAwg : public ConsoleCommand{
    Console *console;
    dactype_t dac;

public:
    void init(void *params) { console = static_cast<Console*>(params); }

    char execute(void *ptr);
    void help(void);
    
    CmdAwg () : ConsoleCommand("awg") { flags = 0; }

private:
    uint8_t flags;

    void dacEnable(uint8_t en);

};


#ifdef __cplusplus
}
#endif

#endif
