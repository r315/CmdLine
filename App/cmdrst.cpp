#include "board.h"
#include "cmdrst.h"

void CmdRst::help(void){
}

char CmdRst::execute(void *ptr){
    NVIC_SystemReset();
    return CMD_OK;
}