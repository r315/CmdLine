#ifndef _cmdrfinder_h_
#define _cmdrfinder_h_


#ifdef __cplusplus
extern "C" {
#endif

#include <console.h>

#define FRAME_SIZE		4 // 0XFF + H_DATA + L_DATA + SUM
#define START_RANGING	0x55

class CmdRfinder : public ConsoleCommand{
	Console *console;
	uint8_t buf[FRAME_SIZE];
public:
    CmdRfinder () : ConsoleCommand("rfinder") {}
    void init(void *params) { console = static_cast<Console*>(params); }

    char execute(int argc, char **argv);
    void help(void){
        //console->print("Usage: rfinder <> [option] \n\n");
        console->print("** Range finder **  \n\n");
        console->print("Pin assignment: \n");
        console->print("\t PA9: TX\n\tPA10: RX\n\n");
    }
};

#ifdef __cplusplus
}
#endif

#endif
