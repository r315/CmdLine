
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
#include <fifo.h>

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
#define SBUS_FRAME_DATA_SIZE     ((SBUS_NUM_CHANNELS * SBUS_FRAME_CHANNEL_BITS) / SBUS_FRAME_DATA_BITS)

#define FLAGS                   sbus.flags
#define SET_FLAG(x)             FLAGS |= x
#define CLR_FLAG(x)             FLAGS &= ~(x) 
#define FLAG_SET(x)             ((FLAGS & x) != 0)
#define FLAG_CLR(x)             ((FLAGS & x) == 0)

#define NEW_DATA_FLAG           (1<<0)

#define ARM_CHANNEL         4
#define ARM_VALUE           992
#define DISARM_VALUE        128

#define THROTLE_CHANNEL     2

enum SbusState {NOT_RUNNING, RUNNING};

typedef struct sbusframe{
    uint8_t header;
    uint8_t data[SBUS_FRAME_DATA_SIZE];
    uint8_t flags;
    uint8_t footer;
}sbusframe_t;

typedef struct sbus{
    uart_t uart;
    sbusframe_t frame;
    sbusframe_t next_frame;
    SbusState state = NOT_RUNNING;
    volatile uint32_t flags = 0;
}sbus_t;

volatile uint32_t new_frame; 
static sbus_t sbus;
static fifo_t rxdata;

void extractChannelData(uint16_t *data, sbusframe_t *frame){
uint8_t idx = 0;
uint8_t mask = (1<<0), bit;
    for (uint32_t ch = 0; ch < SBUS_NUM_CHANNELS; ch++){
        uint32_t ch_data = 0;
        for (uint8_t i = 0; i < SBUS_FRAME_CHANNEL_BITS; i++){
            bit = ((frame->data[idx] & mask) != 0) ? 1 : 0;
            mask <<= 1;
            if(mask == 0){
                idx++;
                mask = (1<<0);
            }
            ch_data |= (bit << i);
        }
        data[ch] = ch_data;        
    }
}

void printSbusFrame(sbusframe_t *frame){
//uint16_t data[SBUS_NUM_CHANNELS];
	//extractChannelData(data, frame);
    //for (uint32_t ch = 0; ch < SBUS_NUM_CHANNELS; ch++){       
    //    DBG_PRINT("CH[%u] = %u\n", ch, data[ch]);
    //}
    for (uint8_t i = 0; i < 25; i++)
    {
          DBG_PRINT("Byte[%u] = %u\n", i, ((uint8_t*)frame)[i]);
    }
    
}

void sendFrame(void *frame){

    // check if frame was updated
    if(FLAG_SET(NEW_DATA_FLAG)){
        memcpy(&sbus.frame, &sbus.next_frame, sizeof(sbusframe_t));
        FLAG_CLR(NEW_DATA_FLAG);
    }
    UART_Send(&sbus.uart, (uint8_t*)&sbus.frame, sizeof(sbusframe_t));
/*
    uint8_t *byte = (uint8_t*)&sbus.frame;
    for(uint32_t i = 0; i < sizeof(sbusframe_t); i++, byte++){
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
*/
    uint32_t available = fifo_avail(&rxdata);
    // Check if there is data available
    if(available > 0){
        do{ 
            // Empty fifo until we find a frame start           
            uint8_t dummy = fifo_peek(&rxdata);
            if(dummy == SBUS_FRAME_START){
                break;
            }
            
            fifo_get(&rxdata, &dummy);
            available--;
        }while(available > 0);

        // At this point either fifo is empty, or head is pointing to a frame start
        // or a frame was received
        if(available >= sizeof(sbusframe_t)){
            DBG_PIN_HIGH;
            // A frame is available, get data
            sbusframe_t tmp_frame;
            for (uint32_t i = 0; i < sizeof(sbusframe_t); i++){
                fifo_get(&rxdata, (uint8_t*)&tmp_frame + i);
            }
            printSbusFrame(&tmp_frame);            
            DBG_PIN_LOW;
        }
    }
}

RAM_CODE void receiveCB(uint32_t data){
    if(data & 0xFFFF0000){
        DBG_PRINT("Data lost\n");
        //fifo_flush(&rxdata);
        return;
    }
    //DBG_PRINT("%x\n",data);
    fifo_put(&rxdata, data);
}

/**
 * @brief change the value of a given channel in a given sbus frame
 * 
 * Note for data organization
 *          channel.bit
 * data[0]: | 1.7 | 1.6 | 1.5 | 1.4 | 1.3 | 1.2 | 1.1 | 1.0 |
 * data[1]: | 2.4 | 2.3 | 2.2 | 2.1 | 2.0 | 1.10| 1.9 | 1.8 |
 * data[2]: | 3.1 | 3.0 | 2.10| 2.9 | 2.8 | 2.7 | 2.6 | 2.5 | 
 * data[3]: ...
 * 
 * @param frame     :pointer to sbus frame
 * @param channel   : channel to be updated
 * @param value     : new value for channel
 * */
void updateChannel(sbusframe_t *frame, uint8_t channel, uint16_t value){
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
    console->print("CH7: %s\n", (sbus.frame.flags & (1 << 7)) ? "ON" : "OFF" );  
    console->print("CH8: %s\n", (sbus.frame.flags & (1 << 6)) ? "ON" : "OFF" );
    console->print("Frame lost: %s\n", (sbus.frame.flags & (1 << 5)) ? "YES" : "NO" );  
    console->print("Failsafe: %s\n", (sbus.frame.flags & (1 << 4)) ? "ON" : "OFF" );
    console->xputchar('\n');
}


void CmdSbus::help(void){
    console->print("Usage: sbus <ch> [value] [-f <flags>]\n\n");  
    console->print("\t<ch>, Channel number 1-16\n");
    console->print("\tarm,  1: arm, other disarm\n");
    console->print("\t[value], 1000-2000\n");
    console->print("\t-f, <flags> 0-255\n");
    console->print("\treceive, receive mode\n");
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

	if (sbus.state == NOT_RUNNING) {
        fifo_init(&rxdata);
        sbus.uart.baudrate = SBUS_BAUDRATE;
        UART_Init(&sbus.uart, UART_NUMBER);
        LPC_GPIO2->FIODIR |= (1 << 1);
		sbus.state = RUNNING;
        sbus.next_frame.header = SBUS_FRAME_START;
        sbus.next_frame.footer = 0;
        for(int i = 0; i < SBUS_NUM_CHANNELS; i++){
            updateChannel(&sbus.next_frame, i, PWM_CENTER_PULSE);
            //sbus.next_frame.data[i] = i;
        }
        //updateChannel(&sbus.next_frame, THROTLE_CHANNEL, PWM_MIN_PULSE);
        SET_FLAG(NEW_DATA_FLAG);
		TIMER_Periodic(LPC_TIM3, 0, FRAME_RATE, sendFrame, &sbus.frame);
	}	

	while (*p1 != '\0') {
		if (isNextWord(&p1, "-f")) {			
            if (nextInt(&p1, &aux)) {
                sbus.next_frame.flags = aux;	
		    }else
            {
                Flags();
                return CMD_OK;
            }
		}else if(isNextWord(&p1, "arm")){            
            if (nextInt(&p1, &aux)) {
                updateChannel(&sbus.next_frame, ARM_CHANNEL, (aux == 1) ? ARM_VALUE : DISARM_VALUE);
                updateChannel(&sbus.next_frame, THROTLE_CHANNEL, PWM_MIN_PULSE);
                console->print("armed\n");
                break;
            }else{
                updateChannel(&sbus.next_frame, ARM_CHANNEL, DISARM_VALUE);
                console->print("disarmed\n");
                break;
            }
        }else if (nextInt(&p1, &aux)){
            if(channel == 255){
                channel = aux;
            }else if(pulse == 0) {
                pulse = aux;
            }
        }else if(isNextWord(&p1, "receive")){
            sbus.uart.rxcb = receiveCB;
            UART_ReceiveIT(&sbus.uart, NULL, 0);
            break;
        }else{
            p1 = nextWord(p1);
        }   
    }

	if (channel == -1){ // || pulse < PWM_MIN_PULSE || pulse > PWM_MAX_PULSE) {
		return CMD_BAD_PARAM;
	}

    if(channel != 255){
        updateChannel(&sbus.next_frame, channel, pulse);
        SET_FLAG(NEW_DATA_FLAG);
    }

	return CMD_OK;
}