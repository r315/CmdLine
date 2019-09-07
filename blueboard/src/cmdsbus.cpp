
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

//static StdOut uart;

#define FRAME_RATE          14000
#define PWM_MIN_PULSE       128  // 960
#define PWM_CENTER_PULSE    992  // 1500
#define PWM_MAX_PULSE       1793 // 2000
#define SBUS_BAUDRATE       100000
#define UART_NUMBER         0

#define SBUS_NUM_CHANNELS       16
#define SBUS_FRAME_START        0x0F
#define SBUS_FRAME_CHANNEL_BITS 11
#define SBUS_FRAME_DATA_BITS    8
#define SBUS_FRAME_DATA_NUM     ((SBUS_NUM_CHANNELS * SBUS_FRAME_CHANNEL_BITS) / SBUS_FRAME_DATA_BITS)

#define ARM_CHANNEL         4
#define ARM_VALUE           992
#define DISARM_VALUE        128

#define THROTLE_CHANNEL     2

volatile uint32_t updating; 

typedef struct _Sbus{
    uint8_t start;
    uint8_t data[SBUS_FRAME_DATA_NUM];
    uint8_t flags;
    uint8_t end;
}SbusFrame;

enum SbusState {NOT_RUNNING, RUNNING};

typedef struct _SbusUart{
    void *dev;
    uint32_t baudrate;
    uint8_t bits;       // 3-0: Number of bits, 4: Configure DMA
    SbusState state = NOT_RUNNING;
}SbusUart;

static SbusFrame sframe, next_sframe;
static SbusUart sbusuart;

void sendFrame(void *frame){

    if(!updating){
        memcpy(&sframe, &next_sframe, sizeof(SbusFrame));
    }
    UART_Send((Uart*)&sbusuart, (uint8_t*)&sframe, sizeof(SbusFrame));

    uint8_t *byte = (uint8_t*)&sframe;
    for(uint32_t i = 0; i < sizeof(SbusFrame); i++, byte++){
        uint8_t data = *byte;
        for(int j = 0; j < 8; j++){
            if(data & (0x80 >> j))
                LPC_GPIO2->FIOSET = (1 << 1);
            else
                LPC_GPIO2->FIOCLR = (1 << 1);
               for(int z = 0; z < 50; z++)
                    asm("nop");
        }
    }
}

void updateChannel(SbusFrame *frame, uint8_t channel, uint16_t value){
uint8_t chDataIdx = ((channel * SBUS_FRAME_CHANNEL_BITS) / SBUS_FRAME_DATA_BITS);
uint8_t mask = (1 << (channel * SBUS_FRAME_CHANNEL_BITS) % SBUS_FRAME_DATA_BITS);
//uint8_t mask = (1 << (channel * SBUS_FRAME_CHANNEL_BITS) % SBUS_FRAME_DATA_BITS);
uint8_t i,b;

    for(i = 0, b = 0; i < SBUS_FRAME_CHANNEL_BITS; i++, b++){
        if(value & (1 << b)){
            frame->data[chDataIdx] |= mask;            
        }else{
            frame->data[chDataIdx] &= ~(mask);
        }
        // check if was the last bit of data
        if( mask == 0x80){
            // move to next data index
            chDataIdx++;
            mask = 1;
        }else{
            mask <<= 1;
        }
    }
}

void CmdSbus::Flags(void){
    console->print("CH7: %s\n", (sframe.flags & (1 << 7)) ? "ON" : "OFF" );  
    console->print("CH8: %s\n", (sframe.flags & (1 << 6)) ? "ON" : "OFF" );
    console->print("Frame lost: %s\n", (sframe.flags & (1 << 5)) ? "YES" : "NO" );  
    console->print("Failsafe: %s\n", (sframe.flags & (1 << 4)) ? "ON" : "OFF" );
    console->xputchar('\n');
}


void CmdSbus::help(void){
    console->print("Usage: sbus <ch> [value] [-f]\n\n");  
    console->print("\t<ch>, Channel number 1-16\n");
    console->print("\tarm,  1: arm, other disarm\n");
    console->print("\t[value], 1000-2000\n");
}


char CmdSbus::execute(void *ptr){
char *p1, channel = 255;
uint16_t pulse = 0;
int32_t aux;

	p1 = (char*)ptr;

	if (p1 == NULL || *p1 == '\0') {
		help();
		return CMD_OK;
	}

	if (sbusuart.state == NOT_RUNNING) {
        sbusuart.baudrate = SBUS_BAUDRATE;
        UART_Init((Uart*)&sbusuart, UART_NUMBER);
        LPC_GPIO2->FIODIR |= (1 << 1);
		sbusuart.state = RUNNING;
        next_sframe.start = SBUS_FRAME_START;
        for(int i = 0; i < SBUS_NUM_CHANNELS; i++){
            updateChannel(&next_sframe, i, PWM_CENTER_PULSE);
        }
        updateChannel(&next_sframe, THROTLE_CHANNEL, PWM_MIN_PULSE);
		TIMER_Periodic(LPC_TIM3, 0, FRAME_RATE, sendFrame, &sframe);
	}	

	while (*p1 != '\0') {
		if (isNextWord(&p1, "-f")) {			
            if (nextInt(&p1, &aux)) {
                next_sframe.flags = aux;	
		    }else
            {
                Flags();
                return CMD_OK;
            }
		}else if(isNextWord(&p1, "arm")){            
            if (nextInt(&p1, &aux)) {
                updateChannel(&next_sframe, ARM_CHANNEL, (aux == 1) ? ARM_VALUE : DISARM_VALUE);
                updateChannel(&next_sframe, THROTLE_CHANNEL, PWM_MIN_PULSE);
                console->print("armed\n");
            }else{
                updateChannel(&next_sframe, ARM_CHANNEL, DISARM_VALUE);
                console->print("disarmed\n");     
            }
        }else if (nextInt(&p1, &aux)){
            if(channel == 255){
                channel = aux;
            }else if(pulse == 0) {
                pulse = aux;
            }
        }else{
            p1 = nextWord(p1);
        }   
    }

	if (channel == -1){ // || pulse < PWM_MIN_PULSE || pulse > PWM_MAX_PULSE) {
		return CMD_BAD_PARAM;
	}

    updating = 1;
   
    updateChannel(&next_sframe, channel, pulse);
    updating = 0;

	return CMD_OK;
}