
#include "board.h"
#include "cmdadc.h"


uint16_t adcvalues[4];
volatile uint8_t flag = 0;

extern "C" void adccb(uint16_t *vals){
    *((uint64_t*)adcvalues) = *((uint64_t*)vals);
    flag = 1;
}

void CmdAdc::help(void){

}

char CmdAdc::execute(void *ptr){
uint8_t quit = 0;
    console->print("Starting adc\n\n");

    ADC_Init(200);
    ADC_SetCallBack(adccb);

    while(quit == 0 ){
        char c;
        if(vcp.getCharNonBlocking(&c)){
            if(c == 0x1B){
                quit = 1;
            }
        }
        if(flag){
            console->print("CH0: %u\n", adcvalues[0]);
            console->print("CH1: %u\n", adcvalues[1]);
            console->print("CH2: %u\n", adcvalues[2]);
            console->print("CH3: %u\n\n", adcvalues[3]);
            flag = 0;
        }
    }

    ADC_Stop();

    return CMD_OK;
}