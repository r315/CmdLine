#ifndef _cmdhelp_h_
#define _cmdhelp_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "console.h"

class CmdHelp : public ConsoleCommand {
	Console *console;
public:
    CmdHelp() : ConsoleCommand("help") {}	
	void init(void *params) { console = static_cast<Console*>(params); }

	void help(void) {
		console->print("Available commands:\n\n");
		
		for (uint8_t i = 0; i < console->getCmdListSize(); i++) {			
				console->printf("\t%s\n", console->getCmdIndexed(i)->getName());
		}
		console->printchar('\n');
	}

	char execute(int argc, char **argv) {
		help();
		return CMD_OK;
	}	
};


#ifdef __cplusplus
}
#endif

#endif