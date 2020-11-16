#include "board.h"
#include "cmdflashrom.h"
#include "spi_flash.h"

typedef enum {
    IDLE = 0,
    HDR,
    XFER
}spistate_e;

#define LOW     0
#define HIGH    1

void CmdFlashRom::help(void){ 
    console->print("flashrom\n");
    console->print("q  exits flashrom mode\n");
    console->print("Usage: flashrom -p bluepill_spi:dev=/dev/ttyACM0 --flash-size");
}

char CmdFlashRom::execute(void *ptr){
    uint8_t done = FALSE;
    uint8_t xfer_count = 0;
    spistate_e state = IDLE;
    uint8_t buf[16];
    uint16_t idx = 0;

    console->print("FLASHROM OK");

    while(done == FALSE){
        char c = vcp.xgetchar();

        switch (state){
            case IDLE:            
                switch(c){
                    case 'q': 
                        done = TRUE;
                        break;
                    case ':':                        
                        state = HDR;
                        break;
                    default:
                        break;
                }
                break;            

            case HDR:
                xfer_count = c;
                idx = 0;
                state = XFER;
                SPI_SetCS(LOW);
                break;

            case XFER:                
                SPI_Transfer((uint8_t*)&c, SPI_XFER_TIMEOUT);
                buf[idx++] = (uint8_t)c;
                
                if(idx == xfer_count){
                    SPI_SetCS(HIGH);
                    for(idx = 0; idx < xfer_count; idx++){
                        vcp.xputchar(buf[idx]);  
                    }
                    state = IDLE;   
                }                
                break;

            default:
                break;

        }
    }

    return CMD_OK;
}