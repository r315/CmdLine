#ifndef _cmdecho_h_
#define _cmdecho_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "console.h"

class CmdEcho : public ConsoleCommand{
    Console *console;

public:
    void init(void *params) {
		console = static_cast<Console*>(params);
	}

    char execute(void *ptr){
        console->print("%s\n", (char*)ptr);
        return CMD_OK;
    }
    void help(void){}

    CmdEcho () : ConsoleCommand("echo") { }    
};


#ifdef __cplusplus
}
#endif

#endif