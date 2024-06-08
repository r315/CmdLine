#ifndef _cmdsi5351_h_
#define _cmdsi5351_h_


#ifdef __cplusplus
extern "C" {
#endif
 
#include "console.h"
#include "i2c.h"
#include "si5351.h"

class CmdSi5351 : public ConsoleCommand{
    Console *console;
    i2cbus_t m_i2c;
    Si5351 si5351;
public:
    void init(void *params) { console = static_cast<Console*>(params); }

    char execute(int argc, char **argv);
    void help(void);
    
    CmdSi5351 () : ConsoleCommand("si5351") {  m_i2c = {NULL, 0, 0}; }
};


#ifdef __cplusplus
}
#endif

#endif