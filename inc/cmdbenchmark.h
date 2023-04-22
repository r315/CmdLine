#ifndef _cmdbenchmark_h_
#define _cmdbenchmark_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "console.h"

class CmdBenchmark : public ConsoleCommand {
	Console *console;
public:
    CmdBenchmark() : ConsoleCommand("benchmark") {}	
	void init(void *params) { console = static_cast<Console*>(params); }

	void help(void);
	char execute(int argc, char **argv);
};


#ifdef __cplusplus
}
#endif

#endif