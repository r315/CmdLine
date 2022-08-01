#include "board.h"
#include "cmdrst.h"

void CmdRst::help(void){
}

char CmdRst::execute(int argc, char **argv){
    NVIC_SystemReset();
    return CMD_OK;
}