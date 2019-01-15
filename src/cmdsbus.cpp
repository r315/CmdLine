
/**
 * The protocol is 25 Byte long and is send every 14ms (analog mode) or 7ms (highspeed mode).
 * One Byte = 1 startbit + 8 databit + 1 paritybit + 2 stopbit (8E2), baudrate = 100'000 bit/s
 * The highest bit is send first. The logic is inverted (Level High = 1)
 *
 * [startbyte] [data1] [data2] .... [data22] [flags][endbyte]
 *
 * startbyte = 11110000b (0xF0)
 * 
 * data 1-22 = [ch1, 11bit][ch2, 11bit] .... [ch16, 11bit] (ch# = 0 bis 2047)
 * channel 1 uses 8 bits from data1 and 3 bits from data2
 * channel 2 uses last 5 bits from data2 and 6 bits from data3  etc.
 *
 * flags:
 * bit7 = ch17 = digital channel (0x80)
 * bit6 = ch18 = digital channel (0x40)
 * bit5 = Frame lost, equivalent red LED on receiver (0x20)
 * bit4 = failsafe activated (0x10)
 * bit3 = n/a
 * bit2 = n/a
 * bit1 = n/a
 * bit0 = n/a
 * 
 * endbyte = 00000000b
 * */

#include "board.h"
#include "cmdsbus.h"

#define FRAME_RATE   14000

typedef struct _Sbus{
    uint8_t start;
    uint8_t data[21];
    uint8_t flags;
    uint8_t end;
}SbusFrame;

static SbusFrame sframe;
static uint8_t init = 0;

void sendFrame(void *frame){
    LED1_TOGGLE;
}


void CmdSbus::help(void){
    vcom->printf("Usage: sbus [option] \n\n");  
    vcom->printf("\t -c <nr>, Display channel nr value \n");
    vcom->printf("\t -c <nr> <value>, Set channel nr value \n");
}


char CmdSbus::execute(void *ptr){
char *p1;

	p1 = (char*)ptr;

    if( p1 == NULL || *p1 == '\0'){
        help();
        return CMD_OK;
    }

    if(init == 0){
        TIMER_Periodic(LPC_TIM3, 0,FRAME_RATE , sendFrame, &sframe);
        init = 1;
    }

    while(*p1 != '\0'){
        if( !xstrcmp(p1,"-c")){
            p1 = nextParameter(p1);
		    int32_t val = nextInt(&p1);
            
		}else if( !xstrcmp(p1,"-p")){
			p1 = nextParameter(p1);
            
		}else if( !xstrcmp(p1,"-e")){
			p1 = nextParameter(p1);
           
		}else if( !xstrcmp(p1,"-f")){
			p1 = nextParameter(p1);
            
        }else{
			p1 = nextParameter(p1);
		}
    }

    return CMD_OK;
}