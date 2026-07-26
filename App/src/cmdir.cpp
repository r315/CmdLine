#include "cmdIr.h"
#include "stimer.h"
#include "rc6.h"
#include <debug.h>

static uint8_t rc6_tr;
static stimer_t ir_timer;
static uint16_t ir_buf[44];
static uint8_t ir_repeat_count;
static uint8_t ir_buf_len;


static uint32_t ir_repeat(stimer_t *timer)
{
    (void)timer;
    if(--ir_repeat_count){
        IR_Transmit(ir_buf, ir_buf_len);
    }
    return 0;
}

static void ir_eot_handler(uint16_t unused)
{
    (void)unused;
    STIMER_Start(&ir_timer);
}
}

void CmdIr::help(void)
{
    console->println("Usage: ir <init|pulse|rc6>");
    console->println("\0init <mode>,      0: TX 1: RX");
    console->println("\tpulse <duration>, Modulated pulse > 400us");
    console->println("\trc6 <code>,       Transmit key code in RC6");
}

char CmdIr::execute(int argc, char **argv)
{
    if(argc == 1){
        help();
        return CMD_OK;
    }

    if(!xstrcmp("init", argv[1])){
        uint32_t mode;
        if(ha2u(argv[2], &mode)){
            IR_Init((enum ir_mode)mode);

            if(mode == IR_MODE_TX)
                IR_SetCallback(ir_eot_handler);
            else
                IR_SetCallback(ir_eor_handler);
        }
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
            ir_buf_len = RC6_Frame(ir_buf, rc6_tr++, 0, code);
            ir_repeat_count = 4;
            ir_timer.interval = 85;
            ir_timer.callback = ir_repeat;
            STIMER_Config(&ir_timer);
            IR_Transmit(ir_buf, ir_buf_len);
            return CMD_OK;
        }
    }

    return CMD_BAD_PARAM;
}



