#include "cmdbenchmark.h"
#include "dhry.h"
extern uint32_t SystemCoreClock;

void CmdBenchmark::help(void){

}

char CmdBenchmark::execute(int argc, char **argv){
    float res = dhry(1000000) / 1757.0f;

    if(res){
        console->print("%.2f DMIPS/MHz\n",  res / (SystemCoreClock / 1000000));
    }

    return CMD_OK;
}