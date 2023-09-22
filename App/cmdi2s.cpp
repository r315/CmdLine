#include "board.h"
#include "cmdi2s.h"

static i2sbus_t i2s;


void CmdI2s::help(void){
    console->println("Usage: i2s <option> [params] \n");    
    console->println("options:");
    console->println("  init <sample rate> <bits sample>,\n"
                       "                    sample rate: 16000 to 96000\n"
                       "                    bits sample: 8, 16 1n 32");
    console->println("  start,     Start FS and data output");
    console->println("  stop,      Stop FS and data output");
    console->printchar('\n');
}

char CmdI2s::execute(int argc, char **argv){
    int32_t val1, val2;

    if(argc < 1){
        help();
        return CMD_OK;
    }

    if(xstrcmp("init", (const char*)argv[1]) == 0){
        if(ia2i(argv[2], &val1)){
            if(ia2i(argv[3], &val2)){
                i2s.sample_rate = val1;
                i2s.data_size = val2;
                i2s.bus = I2S_BUS0;
                i2s.channels = 2;
                i2s.mode = I2S_TX_MASTER | I2S_RX_MASTER;
                I2S_Init(&i2s);
                return CMD_OK;
            }
        }
    }

    if(xstrcmp("start", (const char*)argv[1]) == 0){
        I2S_Start(&i2s);
        LPC_I2S->TXFIFO = 0x12345678;
        return CMD_OK;
    }

    if(xstrcmp("stop", (const char*)argv[1]) == 0){
        I2S_Stop(&i2s);
        return CMD_OK;
    }

    if(xstrcmp("loopback", (const char*)argv[1]) == 0){
        
        for (int i = 0; i < 512; i++ ){
	        i2s.txbuffer[i] = i;
	        i2s.rxbuffer[i] = 0;
        }

        LPC_I2S->IRQ = (8 << 16) | (1 << 8) | IRQ_RX_IRQ_EN;

        I2S_Start(&i2s);

        for (int i = 0; i < 512; i++ ){
	        while (((LPC_I2S->STATE >> 16) & 0xFF) == 8);
	        LPC_I2S->TXFIFO = i2s.txbuffer[i++];
        }
        
        /* Wait for RX and TX complete before comparison */
        //FIX: wridx should be restricted to this module while ( i2s.wridx < (512 - 8) );

        I2S_Stop(&i2s);
        /* Validate TX and RX buffer */
        for (int i=1; i< 512; i++ ){
	        if ( i2s.rxbuffer[i] != i2s.txbuffer[i-1] ){
	            console->printf("Audio loopback fail at index %d\n", i);
                break;
	        }
        }        
        
        return CMD_OK;
    }
    
  
    return CMD_BAD_PARAM;
}