#include "cmdIr.h"
#include "stimer.h"
#include "rc6.h"
#include <debug.h>

static uint8_t rc6_tr;
static stimer_t ir_tx_timer;
static stimer_t ir_rx_timer;
static uint16_t ir_tx_buf[44];
static uint16_t ir_rx_buf[64];
static uint8_t ir_repeat_count;
static uint8_t ir_tx_buf_len;
static uint8_t ir_rx_buf_len;


static uint32_t ir_repeat(stimer_t *timer)
{
    (void)timer;
    if(--ir_repeat_count){
        IR_Transmit(ir_tx_buf, ir_tx_buf_len);
    }else{
        IR_Receive(ir_rx_buf, sizeof(ir_rx_buf) >> 1);
    }
    return 0;
}

static uint32_t ir_capture(stimer_t *timer)
{
    (void)timer;
    #if 0
    dbg_printf("\nEdges: %d\n", ir_rx_buf_len);
    for(uint8_t i = 0; i < ir_rx_buf_len; i++){
        dbg_printf("%u ", ir_rx_buf[i]);
    }
    #else
    dbg_printf("\nNumber of marks: %d\n", ir_rx_buf_len - 1);
    for(uint8_t i = 0; i < ir_rx_buf_len - 1; i++){
        ir_rx_buf[i] = ir_rx_buf[i + 1] - ir_rx_buf[i];
        dbg_printf("%u, ", ir_rx_buf[i]);
    }
    #endif
    dbg_printf("\n");

    return 0;
}

static void ir_eot_handler(void)
{
    STIMER_SetInterval(&ir_tx_timer, 80);
    STIMER_Start(&ir_tx_timer);
}

static void ir_eor_handler(uint16_t length)
{
    ir_rx_buf_len = length;
    STIMER_SetInterval(&ir_rx_timer, 1);
    STIMER_Start(&ir_rx_timer);
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
        #ifdef ENABLE_IR_DUPLEX
        IR_Init(IR_MODE_DUPLEX);
        IR_SetTxCallback(ir_eot_handler);
        IR_SetRxCallback(ir_eor_handler);
        STIMER_SetCallback(&ir_tx_timer, ir_repeat);
        STIMER_SetCallback(&ir_rx_timer, ir_capture);
        STIMER_Config(&ir_tx_timer);
        STIMER_Config(&ir_rx_timer);
        STIMER_Stop(&ir_tx_timer);
        STIMER_Stop(&ir_rx_timer);
        IR_Receive(ir_rx_buf, sizeof(ir_rx_buf) >> 1);
        #else
        if(!xstrcmp("tx", argv[2])){
            IR_Init(IR_MODE_TX);
            IR_SetTxCallback(ir_eot_handler);
        }else{
            IR_Init(IR_MODE_RX);
            IR_SetRxCallback(ir_eor_handler);
        }
        #endif
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
            ir_repeat_count = 4;
            ir_tx_buf_len = RC6_Frame(ir_tx_buf, rc6_tr++, 0, code);
            IR_CancelReceive();
            IR_Transmit(ir_tx_buf, ir_tx_buf_len);
            return CMD_OK;
        }
    }

    if(!xstrcmp("capture", argv[1])){
        IR_Receive(ir_rx_buf, sizeof(ir_rx_buf) >> 1);
        return CMD_OK;
    }

    if(!xstrcmp("replay", argv[1])){
        for(uint8_t i = 0; i < ir_rx_buf_len-1; i++){
            ir_tx_buf[i] = ir_rx_buf[i + 1] - ir_rx_buf[i];

            if(ir_tx_buf[i] == 0){
                // Abort on invalid capture
                return CMD_OK;
            }

            if(!(i&1))
                ir_tx_buf[i] = IR_MARK_ON(ir_tx_buf[i]);
        }
        ir_tx_buf_len = ir_rx_buf_len - 1;
        ir_repeat_count = 5;
        IR_CancelReceive();
        IR_Transmit(ir_tx_buf, ir_tx_buf_len);
        return CMD_OK;
    }

    return CMD_BAD_PARAM;
}



