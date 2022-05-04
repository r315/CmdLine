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

extern StdOut *userio;

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

    BOARD_GPIO_Init(BOARD_SPI_PORT, BOARD_SPI_DO_PIN, PIN_OUT_AF | PIN_OUT_50MHZ);
    BOARD_GPIO_Init(BOARD_SPI_PORT, BOARD_SPI_DI_PIN, PIN_OUT_AF | PIN_OUT_50MHZ);
    BOARD_GPIO_Init(BOARD_SPI_PORT, BOARD_SPI_CK_PIN, PIN_OUT_AF | PIN_OUT_50MHZ);
    BOARD_GPIO_Init(BOARD_SPI_PORT, BOARD_SPI_CS_PIN, PIN_OUT_2MHZ);

    console->print("FLASHROM OK");

    while(done == FALSE){
        char c = userio->xgetchar();

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
                BOARD_SPI_CS_LOW;
                break;

            case XFER:                
                c = (char)BOARD_SPI_Transfer(c, SPI_XFER_TIMEOUT);
                buf[idx++] = (uint8_t)c;
                
                if(idx == xfer_count){
                    BOARD_SPI_CS_HIGH;
                    for(idx = 0; idx < xfer_count; idx++){
                        BOARD_STDIO->xputchar(buf[idx]);  
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