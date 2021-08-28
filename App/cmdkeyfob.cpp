#include <stdlib.h>
#include "board.h"
#include "cmdkeyfob.h"
#include "scapture.h"


void CmdKeyFob::printBitstream(bitstreamview_e view, uint16_t *bitstream, int16_t bittime, uint8_t idle_state, uint32_t streamlen){

    if(streamlen == 0){
        return;
    }

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
                console->print(" %d%c", (diff < 0) ? -diff : diff, (i%2) == 0 ? ',' : '\n');
            }
            console->print("\n");
            break;

        case BITSTREAM_DECODE:
        {
            if(bittime == 0){
                bittime = bitstream[1] - bitstream[0];                
                for(uint32_t i = 0; i < streamlen - 1; i++){
                    int16_t diff = bitstream[i + 1] - bitstream[i];
                    if(abs(diff) < abs(bittime)){
                        bittime = diff;
                    }
                }
                // TODO: Frames are separated by pulse larger than 10mm,
                // this can be used to identify frames
                console->print("Using bit time = %dus\n", bittime);
            }

            uint16_t bit_count = 0;
            for(uint32_t i = 0; i < streamlen - 1; i++){
                if(i%64 == 0){
                    console->xputchar('\n');
                }

                int16_t diff = bitstream[i + 1] - bitstream[i];
                if(diff < 0 ){
                    diff = -diff;
                }

                if(diff < bittime){
                    console->print("\nError: bit time > pulse width\n");
                    return;
                }

                uint8_t bits = diff / bittime;
                idle_state = !idle_state;

                bit_count += bits;
                
                while(bits--){
                    console->print("%d", idle_state);
                }                
            }
            console->print("\n%d bits decoded\n", bit_count);     
            break;
        }

        default:
            break;
    }

}

void CmdKeyFob::help(void){
    console->xputs("usage: keyfob <option> <parameters>\n\n");
    console->xputs("options:\n");
    console->xputs("  capture <time>, start capture with a maximum time [ms]\n");
    console->xputs("  decode <bit time> [format], decode a capture\n");
    console->xputs("      <bit time>    bit time [us] for decode, 0 find and use smallest pulse width\n");
    console->xputs("      [format]      not specified use bit time for decode\n");
    console->xputs("                    raw, capture value from timer\n");
    console->xputs("                    width, pulse width [us]\n");
    console->xputchar('\n');
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
            capture_count = CAP_BUF_SIZE - SCAP_Wait();
            console->print("Got %d pulses\n", capture_count - 1);
            return CMD_OK;
        }   
    }

    if(strcmp((const char*)argv[0], "decode") == 0){        
        if(yatoi(argv[1], &val1)){  // get bit time
            bitstreamview_e format;
            if(strcmp((const char*)argv[2], "raw") == 0){
                format = BITSTREAM_RAW;
            }else if(strcmp((const char*)argv[2], "width") == 0){
                format = BITSTREAM_WIDTH;
            }else{
                format = BITSTREAM_DECODE;
            }
            printBitstream(format, capture_data, val1, 1, capture_count);
            return CMD_OK;
        }
    }

    return CMD_BAD_PARAM;
}