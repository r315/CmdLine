#ifndef _cmdversion_h_
#define _cmdversion_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "console.h"

#include "version.h"

class CmdVersion : public ConsoleCommand{
    Console *console;

public:
    void init(void *params) {
		console = static_cast<Console*>(params);
	}

    char execute(int argc, char **argv){
        //console->print("Version %d.%d.%d %s\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_STRING);
        console->print("%s\n", VERSION_STRING);
        return CMD_OK;
    }

    void help(void){}

    CmdVersion () : ConsoleCommand("version") { }    
};


#ifdef __cplusplus
}
#endif

#endif