
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

#define FRAME_RATE      14000
#define PWM_MIN_PULSE   1000
#define PWM_MAX_PULSE   2000
#define SBUS_BAUDRATE   100000
#define UART_NUMBER     0

typedef struct _Sbus{
    uint8_t start;
    uint8_t data[21];
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



static SbusFrame sframe;
static SbusUart sbusuart;

void sendFrame(void *frame){
    UART_Send((Uart*)&sbusuart, (uint8_t*)&sframe, sizeof(SbusFrame));
}

void CmdSbus::Flags(void){
    console->print("CH7: %s\n", (sframe.flags & (1 << 7)) ? "ON" : "OFF" );  
    console->print("CH8: %s\n", (sframe.flags & (1 << 6)) ? "ON" : "OFF" );
    console->print("Frame lost: %s\n", (sframe.flags & (1 << 5)) ? "YES" : "NO" );  
    console->print("Failsafe: %s\n", (sframe.flags & (1 << 4)) ? "ON" : "OFF" );
    console->putc('\n');
}


void CmdSbus::help(void){
    console->print("Usage: sbus <ch> [value] [-f]\n\n");  
    console->print("\t<ch>, Channel number 1-16\n");
    console->print("\t[value], 1000-2000\n");
}


char CmdSbus::execute(void *ptr){
char *p1, channel = 255, flags;
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
		sbusuart.state = RUNNING;
		TIMER_Periodic(LPC_TIM3, 0, FRAME_RATE, sendFrame, &sframe);
	}	

	while (*p1 != '\0') {
		if (!xstrcmp(p1, "-f")) {
			p1 = nextParameter(p1);
            if (nextInt(&p1, &aux)) {
                flags = aux;	
		    }else
            {
                Flags();
                return CMD_OK;
            }            
		}else if (nextInt(&p1, &aux)){
            if(channel == 255){
                channel = aux;
            }else if(pulse == 0) {
                pulse = aux;
            }
        }else{
            p1 = nextParameter(p1);
        }   
    }

	if (channel == -1 || pulse < PWM_MIN_PULSE || pulse > PWM_MAX_PULSE) {
		return CMD_BAD_PARAM;
	}

    sframe.flags = flags;
    sframe.data[0] = pulse;
    sframe.data[1] = pulse>>8;
    sframe.start = 0x12;
    //UART_Send((Uart*)&sbusuart, (uint8_t*)&sframe, sizeof(SbusFrame));

	return CMD_OK;
}