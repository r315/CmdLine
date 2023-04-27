#include <stdlib.h>
#include "board.h"
#include "cmdkeyfob.h"
#include "dscr.h"

static CmdKeyFob *kfob;

static uint16_t med_btn[] = {
    400,200,750,250,800,150,700,600,400,550,350,600,400,250,650,300,700,600,300,650,350,600,350,600,350,10000
};

//static uint16_t small_btn[] = { };

static void capCallBack(uint32_t ncaps){
    kfob->m_cap_count = CAP_BUF_SIZE - ncaps;
}
/*
static void memcpy16(void *dst, void *src, uint32_t cnt){
    uint16_t *p1 = (uint16_t*)dst;
    uint16_t *p2 = (uint16_t*)src;
    while(cnt--){
        *p1++ = *p2++;
    }
}
*/
void CmdKeyFob::printBitstream(bitstreamview_e view, uint16_t *bitstream, uint32_t len, int16_t bittime, uint8_t idle_state){

    if(len == 0){
        return;
    }
    
    //console->print("\nData of %d edges\n", len);

    switch(view){
        case BITSTREAM_RAW:
            for(uint32_t i = 0; i < len; i++){            
                console->print(" %d%c", bitstream[i], (i%2) == 0 ? ',' : '\n');
            }
            console->print("\n");
            break;

        case BITSTREAM_WIDTH:
            console->print("Pulses duration in us :");
        case BITSTREAM_JSON:
            console->print("[");
            for(uint32_t i = 0; i < len - 1; i++){
                int16_t diff = bitstream[i + 1] - bitstream[i];
                diff = (diff < 0) ? -diff : diff;
                console->print("%d%s", diff, (i < len - 2) ? ", " : "]\r\n");
            }
            break;

        case BITSTREAM_DECODE:
        {
            if(bittime == 0){
                bittime = bitstream[1] - bitstream[0];                
                for(uint32_t i = 0; i < len - 1; i++){
                    int16_t diff = bitstream[i + 1] - bitstream[i];
                    if(abs(diff) < abs(bittime)){
                        bittime = diff;
                    }
                }
                // TODO: Frames are separated by pulse larger than 10ms,
                // this can be used to identify frames
                console->print("Using bit time = %dus\n", bittime);
            }

            uint16_t bit_count = 0;
            for(uint32_t i = 0; i < len - 1; i++){
                if(i%64 == 0){
                    console->putChar('\n');
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

        case SERIAL_SCOPE:
            for(uint32_t i = 0; i < len - 1; i++){
                int16_t diff = bitstream[i + 1] - bitstream[i];
                diff = (diff < 0) ? -diff : diff;

                diff /= 10;
                
                while(diff){
                    console->print("%d\r\n", i%2 * 100);
                    diff--;
                }
            }
            break;

        default:
            break;
    }

}

void CmdKeyFob::help(void){
    console->putString("usage: keyfob <capture|decode> <time|bit time> [raw|width|single]\n");
    console->putString("options:");
    console->putString("  capture <time> [single], start capture with a maximum time [ms]");
    console->putString("      [single]      - do single capture");
    console->putString("  decode <bit time> [format], decode a capture");
    console->putString("      <bit time>    bit time [us] for decode, 0 find and use smallest pulse width");
    console->putString("      [format]      - not specified use bit time for decode");
    console->putString("                    - \"raw\", capture value from timer");
    console->putString("                    - \"width\", pulse width [us]");
    console->putChar('\n');
}

char CmdKeyFob::execute(int argc, char **argv){
    int32_t val1;

    if(kfob == NULL){
        kfob = static_cast<CmdKeyFob*>(this);
    }

    if(m_init == false){
        DSCR_Init();
        memset16(m_buf, 0, CAP_BUF_SIZE);
        m_init = true;
    }

    if(argc < 2){
        help();
        return CMD_BAD_PARAM;
    }

    if(strcmp((const char*)argv[1], "capture") == 0){
        if(m_capturing == true){
            DSCR_StopCapture();
            m_capturing = false;
        }

        bool single_cap = false;

        if(strcmp((const char*)argv[3], "single") == 0){
            single_cap = true;
        }

        if(ia2i(argv[1], &val1)){
            char c;
            do{
                if(m_capturing == false){
                    DSCR_StartCapture(m_buf, CAP_BUF_SIZE, val1, capCallBack);
                    m_capturing = true;
                    m_cap_count = 0;
                }

                if(m_cap_count > 0){
                    printBitstream(BITSTREAM_JSON, m_buf, m_cap_count, 0, 1);
                    m_capturing = false;
                    if(single_cap){
                        break;
                    }
                }
                console->getCharNonBlocking(&c);
            }while(c != '\n');
            return CMD_OK;
        }   
    }

    if(strcmp((const char*)argv[1], "decode") == 0){        
        if(ia2i(argv[2], &val1)){  // get bit time
            bitstreamview_e format;
            if(strcmp((const char*)argv[3], "raw") == 0){
                format = BITSTREAM_RAW;
            }else if(strcmp((const char*)argv[3], "width") == 0){
                format = BITSTREAM_WIDTH;
            }else{
                format = BITSTREAM_DECODE;
            }
            printBitstream(format, m_buf, m_cap_count, val1, 1);
            return CMD_OK;
        }
    }

    if(strcmp((const char*)argv[1], "stop") == 0){         
        DSCR_StopCapture();
        m_capturing = false;    
        return CMD_OK;
    }

    if(strcmp((const char*)argv[1], "replay") == 0){
        DSCR_Replay(m_buf, m_cap_count);
        return CMD_OK;
    }

    if(strcmp((const char*)argv[1], "push") == 0){
        if(ia2i(argv[2], &val1)){  // get bit time
            switch(val1){
                case 1:
                case 3:
                default: 
                    break;

                case 2:
                    uint16_t pulse = 0;
                    uint16_t repeat = 10;
                   //uint16_t *ptr = m_buf + m_cap_count;

                    for(m_cap_count = 0; m_cap_count < (sizeof(med_btn)/2) * repeat; m_cap_count++){
                        pulse += med_btn[m_cap_count % (sizeof(med_btn)/2)];
                        m_buf[m_cap_count] = pulse;
                    }

                    break;
                    
            }
            DSCR_Replay(m_buf, m_cap_count);
            return CMD_OK;
        }
    }

    return CMD_BAD_PARAM;
}