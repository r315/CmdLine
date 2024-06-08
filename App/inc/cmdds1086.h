#ifndef _cmdds1086_h_
#define _cmdds1086_h_


#ifdef __cplusplus
extern "C" {
#endif
 
#include "console.h"
#include "i2c.h"
#include "ds1086.h"

class CmdDS1086 : public ConsoleCommand{
    Console *console;
    i2cbus_t m_i2c;
    DS1086 ds1086;
public:
    void init(void *params) { console = static_cast<Console*>(params); }

    char execute(int argc, char **argv);
    void help(void);
    
    CmdDS1086 () : ConsoleCommand("ds1086") {  m_i2c = {NULL, 0, 0}; }
};


#ifdef __cplusplus
}
#endif

#endif