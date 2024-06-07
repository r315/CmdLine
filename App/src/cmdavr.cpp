#include "cmdavr.h"
#include "avr_if.h"
#include "stk500.h"

#define DEFAULT_AVR_SPI_FREQ        100UL   //kHz

//--------------------------------------
//
//--------------------------------------
void CmdAvr::help(void){
    console->print("Usage: avr [option] \n\n");
    console->print("\tinit <spibus> <rst pin number>, ");
    console->print("spi bus num and pin number for reset\n");
    console->print("\t-s, read device signature\n");
    console->print("\t-f [l/h] <value>, Read fuses\n");
    console->print("\t-e, Erase device\n");
    console->print("\tstk, Enter stk500 mode\n");
}

char CmdAvr::execute(int argc, char **argv){
    int32_t param1, param2;

    if(argc < 2){
        help();
        return CMD_OK;
    }
    
    if (!xstrcmp("init", argv[1]))
    {
        if (ia2i(argv[2], &param1)){
            if (ia2i(argv[3], &param2)){
                avrspibus.bus = param1;
                avrspibus.freq = DEFAULT_AVR_SPI_FREQ;
                avrspibus.eot_cb = NULL;
                avrspibus.flags = SPI_IDLE;
                avr_if_Init(&avrspibus, param2);
                return CMD_OK;
            }
        }
    }

    if (!xstrcmp("-s", argv[1])){
        uint32_t signature;
        avrDeviceCode((uint8_t*)&signature);
        if((int)signature == AVR_RESPONSE_FAIL){
            console->print("fail to enable programming\n");
        }else{
            console->printf("Signature 0x%X (%s)\n", signature, avrDeviceName(signature));      
        }
        return CMD_OK;
    }

    if (!xstrcmp("-f", argv[1])){
        uint8_t lh;

        // Check low/high fuse
        lh = (argv[2][0] == 'h') ? 1 : 0;
                
        if(ha2i(argv[3], (uint32_t*)&param1)){
            avrWriteFuses(lh, param1);
        }

        param1 = avrReadFuses();
        console->printf("Fuses: H=%X L=%X\n", (param1 >> 8) & 255, param1 & 255);

        return CMD_OK;
    }

    if (!xstrcmp("-s", argv[1])){
        avrChipErase();
        return CMD_OK;
    }

    #ifdef ENABLE_AVR_DW
    if(!strcmp("dw", argv[1])){
        if(ha2i(argv[2], (uint32_t*)&param1)){
            return CMD_OK;
        }
    }
    #endif

     if(!strcmp("stk", argv[1])){
        if(ha2i(argv[2], (uint32_t*)&param1)){
            stk500_setup(SERIAL_GetSerialOps(-1));
            stk500_loop();
            return CMD_OK;
        }
    }

    return CMD_BAD_PARAM;
}
