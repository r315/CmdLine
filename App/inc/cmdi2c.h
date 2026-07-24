#ifndef _cmdi2c_h_
#define _cmdi2c_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "console.h"
#include "i2c.h"

class CmdI2c : public ConsoleCommand{
    Console *console;
    void printAsc(uint8_t *buf, int count);
public:
    void init(void *params) {
        console = static_cast<Console*>(params);
    }

    char execute(int argc, char **argv);
    void help(void);

    CmdI2c () : ConsoleCommand("i2c") { }
};

#ifdef __cplusplus
}
#endif

#endif
