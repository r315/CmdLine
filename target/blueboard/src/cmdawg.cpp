
#include "board.h"
#include "cmdawg.h"

#define AWG_ENABLE      (1 << 0)
#define AWG_DAC_ON      (1 << 1)

const uint16_t sine100[] = {
0x0000, 0x0809, 0x100a, 0x17fb, 0x1fd4, 0x278d, 0x2f1e, 0x367f, 0x3da9, 0x4495,
0x4b3b, 0x5196, 0x579e, 0x5d4e, 0x629f, 0x678d, 0x6c12, 0x7029, 0x73d0, 0x7701,
0x79bb, 0x7bf9, 0x7dba, 0x7efc, 0x7fbe, 0x7fff, 0x7fbe, 0x7efc, 0x7dba, 0x7bf9,
0x79bb, 0x7701, 0x73d0, 0x7029, 0x6c12, 0x678d, 0x629f, 0x5d4e, 0x579e, 0x5196,
0x4b3b, 0x4495, 0x3da9, 0x367f, 0x2f1e, 0x278d, 0x1fd4, 0x17fb, 0x100a, 0x0809,
0x0000, 0xf7f6, 0xeff5, 0xe804, 0xe02b, 0xd872, 0xd0e1, 0xc980, 0xc256, 0xbb6a,
0xb4c4, 0xae69, 0xa861, 0xa2b1, 0x9d60, 0x9872, 0x93ed, 0x8fd6, 0x8c2f, 0x88fe,
0x8644, 0x8406, 0x8245, 0x8103, 0x8041, 0x8001, 0x8041, 0x8103, 0x8245, 0x8406,
0x8644, 0x88fe, 0x8c2f, 0x8fd6, 0x93ed, 0x9872, 0x9d60, 0xa2b1, 0xa861, 0xae69,
0xb4c4, 0xbb6a, 0xc256, 0xc980, 0xd0e1, 0xd872, 0xe02b, 0xe804, 0xeff5, 0xf7f6,
};

const uint16_t square [] = {
    0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF,
    0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF,
    0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF,
    0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF,
    0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF,
    0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF,
    0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF,
    0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF, 0x8000, 0x7FFF,
};

int16_t samples[256];

#define TEST_SIGNAL square

static void setupDac(dactype_t *dac, int16_t *data, uint32_t len){
    dac->buf = samples;
    dac->len = len;   

    /** Convert to unsigned, and clear bit 0
     * Setting bit 0 has the same effect as
     * setting BIAS bit 
     * */
    for (uint32_t i = 0; i < len; i++){
        samples[i] = (0x8000 + data[i]) & 0xFFC0; // clear lower 6 bits
    }        
}

void CmdAwg::help(void){
 	//vcom->printf("Usage: awg [option] \n\n");    
    console->print("usage: awg <option> <param>\n");
    console->print("    start <wave>\n");
    console->print("          wave : sine, square\n");
    console->print("    single : replay buffer once\n");
    console->print("    stop : ends signal\n");
    console->print("    write <16bit hex value> : write value directly to dac\n");
    console->print("    pclk : dac clock\n");
    console->print("    stop : ends signal\n");
    console->print("    rate <value> : 32 - 65535 dac clock divider\n");

    
}

char CmdAwg::execute(int argc, char **argv){

    if(!(flags & AWG_DAC_ON)){
        dacEnable(true);
        dac.rate = 100 - 1; // one sample/us
        setupDac(&dac, (int16_t*)TEST_SIGNAL, sizeof(TEST_SIGNAL) / sizeof(uint16_t));
    }    

    if(argc == 0){
        help();
    }

    if(xstrcmp("single", (const char*)argv[1]) == 0){
        dac.loop = 0;
        DAC_Config(&dac);
        DAC_Start(&dac);
    }

    if(xstrcmp("start", (const char*)argv[1]) == 0){
        if(xstrcmp("sine", (const char*)argv[2]) == 0){
            setupDac(&dac, (int16_t*)sine100, sizeof(sine100) / sizeof(uint16_t));
        }else if(xstrcmp("square", (const char*)argv[2]) == 0){
            setupDac(&dac, (int16_t*)square, sizeof(square) / sizeof(uint16_t));
        }else{
            return CMD_BAD_PARAM;
        }
       
        dac.loop = 1;
        DAC_Config(&dac);
        DAC_Start(&dac);
    }

    if(xstrcmp("stop", (const char*)argv[1]) == 0){
        DAC_Stop(&dac);
    }

    if(xstrcmp("write", (const char*)argv[1]) == 0){
        if(hatoi(argv[2], (uint32_t*)&argc)){ 
            DAC_Write(&dac, argc);
        }
    }
    
    if(xstrcmp("pclk", (const char*)argv[1]) == 0){
        console->print("PCLK: %d\n", CLOCK_GetPCLK(PCLK_DAC));
    }

    if(xstrcmp("rate", (const char*)argv[1]) == 0){
        if(ia2i(argv[2], (int32_t*)&argc)){ 
            DAC_UpdateRate(&dac, argc);
        }
    }

    return CMD_OK;
}

void CmdAwg::dacEnable(uint8_t en){
    if(en){
        DAC_Init(&dac);
        flags |= AWG_DAC_ON;
    }else{
        DAC_DeInit(&dac);
        flags &= ~AWG_DAC_ON;
    }
}
