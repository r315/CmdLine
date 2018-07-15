#include "cmdecho.h"

char CmdEcho::execute(void *ptr){
    vcom->printf("%s\n", (char*)ptr);
    return CMD_OK;
}