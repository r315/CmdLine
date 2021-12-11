#include "board.h"
#include "cmdi2s.h"

static i2sbus_t i2s;

extern volatile uint8_t *I2STXBuffer, *I2SRXBuffer;
extern volatile uint32_t I2SReadLength;
extern volatile uint32_t I2SWriteLength;
extern volatile uint32_t I2SRXDone, I2STXDone;
extern volatile uint32_t I2SDMA0Done, I2SDMA1Done;


void CmdI2s::help(void){

}

char CmdI2s::execute(void *ptr){
    int32_t val1, val2;
    char *argv[4] = {0};
    int argc;

    argc = strToArray((char*)ptr, argv);

    if(argc < 1){
        help();
        return CMD_OK;
    }

    if(xstrcmp("init", (const char*)argv[0]) == 0){
        if(yatoi(argv[1], &val1)){
            if(yatoi(argv[2], &val2)){
                i2s.sample_rate = val1;
                i2s.data_size = val2;
                i2s.bus = I2S_BUS0;
                I2S_Init(&i2s);
                return CMD_OK;
            }
        }
    }

    if(xstrcmp("loopback", (const char*)argv[0]) == 0){
        /* Configure temp register before reading */
        for (int i = 0; i < 512; i++ )	/* clear buffer */
        {
	        I2STXBuffer[i] = i;
	        I2SRXBuffer[i] = 0;
        }
        
        I2SWriteLength = I2SReadLength = I2SRXDone = I2STXDone = 0;

        I2S_Start();

        LPC_I2S->I2SIRQ = (8 << 16) | (1 << 8) | I2S_IRQ_RX_EN;

        while ( I2SWriteLength < 512 ){
	        while (((LPC_I2S->I2SSTATE >> 16) & 0xFF) == 8);
	        LPC_I2S->I2STXFIFO = I2STXBuffer[I2SWriteLength++];
        }

        I2STXDone = 1;
        /* Wait for RX and TX complete before comparison */
        while ( !I2SRXDone || !I2STXDone );

        /* Validate TX and RX buffer */
        for (int i=1; i< 512; i++ ){
	        if ( I2SRXBuffer[i] != I2STXBuffer[i-1] ){
	            console->print("Audio loopback fail");
                break;
	        }
        }
        
        I2S_Stop();
        
        return CMD_OK;
    }
    
  
    return CMD_BAD_PARAM;
}