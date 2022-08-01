
#include "board.h"
#include "cmdadc.h"
#include "adc.h"

#define ADC_DONE    (1 << 0)
#define ADC_INIT    (1 << 2)

static uint16_t adcvalues[4];
static volatile uint8_t adc_flags = 0;
static adctype_t s_adc;

extern "C" void adcEoc(void) {
    adc_flags |= ADC_DONE;
}

void CmdAdc::help(void){

}

char CmdAdc::execute(void *ptr) {
    char *argv[4];
    int argc;
    uint8_t quit = 0;

    if(!(adc_flags & ADC_INIT)){
        s_adc.buf = adcvalues;
        s_adc.len = 4;
        s_adc.ch = ADC_CH3 | ADC_CH2 | ADC_CH1 | ADC_CH0;
        ADC_Init(&s_adc);
        adc_flags = ADC_INIT;
    }
    
    argc = strToArray((char*)ptr, argv);

    if(xstrcmp("start", (const char*)argv[0]) == 0) {
        s_adc.eoc = adcEoc;
        s_adc.ch = ADC_CH3 | ADC_CH2 | ADC_CH1 | ADC_CH0;
        ADC_Start(&s_adc);
        DelayMs(10);
        ADC_Stop(&s_adc);
        console->print("AN0: %u\n", adcvalues[0]);
        console->print("AN1: %u\n", adcvalues[1]);
        console->print("AN2: %u\n", adcvalues[2]);
        console->print("AN3: %u\n\n", adcvalues[3]);
    }

    if(xstrcmp("single", (const char*)argv[0]) == 0) {
        s_adc.eoc = NULL;
        s_adc.ch = ADC_CH0;
        console->print("AN0: %u\n", ADC_ConvertSingle(&s_adc));
    }

#if 0
    while(quit == 0 ){
        char c;
        if(console->getCharNonBlocking(&c)){
            if(c == 0x1B){
                quit = 1;
            }
        }
        if(adc_flags & ADC_DONE){
            console->print("AN0: %u\n", adcvalues[0]);
            console->print("AN1: %u\n", adcvalues[1]);
            console->print("AN2: %u\n", adcvalues[2]);
            console->print("AN3: %u\n\n", adcvalues[3]);
            adc_flags = ADC_INIT;
        }
    }
#endif
    //ADC_Stop();

    return CMD_OK;
}