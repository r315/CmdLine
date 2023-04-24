#ifndef _cmd_encoder_h_
#define _cmd_encoder_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "console.h"

class CmdEncoder : public ConsoleCommand{
    Console *console;
public:
    void init(void *params) { console = static_cast<Console*>(params); }
    CmdEncoder () : ConsoleCommand("encoder") { }
    char execute(int argc, char **argv);
    void help(void);
};


#ifdef __cplusplus
}
#endif

#endif
