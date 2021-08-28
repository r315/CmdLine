#include "board.h"
#include "cmdkeyfob.h"
#include "scapture.h"


void CmdKeyFob::printBitstream(bitstreamview_e view, uint16_t *bitstream, uint32_t bittime, uint32_t streamlen){

    switch(view){
        case BITSTREAM_RAW:
            for(uint32_t i = 0; i < streamlen; i++){            
                console->print(" %d%c", bitstream[i], (i%2) == 0 ? ',' : '\n');
            }
            console->print("\n");
            break;

        case BITSTREAM_WIDTH:            
            for(uint32_t i = 0; i < streamlen - 1; i++){
                int16_t diff = bitstream[i + 1] - bitstream[i];                
                console->print(" %d%c", (diff < 0) ? - diff : diff, (i < streamlen - 2) ? ',' : ' ');
            }
            console->print("\n");
            break;

        case BITSTREAM_DECODE:
            break;

        default:
            break;
    }

}

void CmdKeyFob::help(void){
    console->xputs("usage: keyfob <capture> <duration>\n");
}

char CmdKeyFob::execute(void *ptr){
    char *argv[4];
    int argc;
    int32_t val1;

    argc = strToArray((char*)ptr, argv);

    if(argc < 1){
        help();
        return CMD_BAD_PARAM;
    }

    if(strcmp((const char*)argv[0], "capture") == 0){     
        if(yatoi(argv[1], &val1)){
            SCAP_Init();
            SCAP_Start(capture_data, CAP_BUF_SIZE, val1);
            int remaining = SCAP_Wait();
            printBitstream(BITSTREAM_WIDTH, capture_data, 0, CAP_BUF_SIZE - remaining);
            return CMD_OK;
        }   
    }

    if(strcmp((const char*)argv[0], "start") == 0){      
        
        return CMD_OK;
    }

    return CMD_BAD_PARAM;
}