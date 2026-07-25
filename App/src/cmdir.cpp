#include "cmdIr.h"
#include "stimer.h"
#include "rc6.h"

static stimer_t rc6_timer;
static uint16_t rc6_buf[44];
static uint8_t rc6_tr;
static uint8_t rc6_count;


static uint32_t rc6_repeat(stimer_t *timer)
{
    (void)timer;
    if(--rc6_count){
        IR_Transmit(rc6_buf, 44);
    }
    return 0;
}

static void ir_eot_handler(void)
{
    STIMER_Start(&rc6_timer);
}

void CmdIr::help(void)
{
    console->println("Usage: ir <init|pulse|rc6>");
    console->println("\tstart <pin number(hex)>");
    console->println("\tpulse <duration>, Modulated pulse 270us to 5000us");
    console->println("\trc6 <code>,       Transmit key code in RC6");
}

char CmdIr::execute(int argc, char **argv)
{
    if(argc == 1){
        help();
        return CMD_OK;
    }

    if(!xstrcmp("init", argv[1])){
        IR_Init();
        return CMD_OK;
    }

    if(!xstrcmp("pulse", argv[1])){
        int32_t duration;

        if(ia2i(argv[2], &duration)){
            IR_Pulse(duration);
            return CMD_OK;
        }

    }

    if(!xstrcmp("rc6", argv[1])){
        uint32_t code;
        if(ha2u(argv[2], &code)){
            uint8_t len = RC6_Frame(rc6_buf, rc6_tr++, 0, code);
            rc6_count = 4;
            rc6_timer.interval = 85;
            rc6_timer.callback = rc6_repeat;
            STIMER_Config(&rc6_timer);
            IR_SetEotHandler(ir_eot_handler);
            IR_Transmit(rc6_buf, len);
            return CMD_OK;
        }
    }

    return CMD_BAD_PARAM;
}



