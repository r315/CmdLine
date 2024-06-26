#ifndef _cmdi2s_h_
#define _cmdi2s_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "console.h"
#include "i2s.h"

class CmdI2s : public ConsoleCommand{
    Console *console;

public:
    void init(void *params) { console = static_cast<Console*>(params); }

    char execute(int argc, char **argv);
    void help(void);

    CmdI2s () : ConsoleCommand("i2s") { }
    
private:
    i2sbus_t m_i2s;
};


#ifdef __cplusplus
}
#endif

#endif