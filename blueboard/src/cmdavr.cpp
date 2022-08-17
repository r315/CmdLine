#include "board.h"
#include "cmdavr.h"
#include "cmdspi.h"
#include "debug.h"
#include "timer.h"

#define READ_PROGRAM_PAGE_L "\x20\x00\x00\x00"
#define READ_PROGRAM_PAGE_H "\x28\x00\x00\x00"

#define DEVICE_CODE0_CMD    "\x30\x00\x00\x00"
#define DEVICE_CODE1_CMD    "\x30\x00\x01\x00"
#define DEVICE_CODE2_CMD    "\x30\x00\x02\x00"

#define LOAD_PROGRAM_PAGE_L "\x40\x00\x00\x00"
#define LOAD_PROGRAM_PAGE_H "\x48\x00\x00\x00"
#define WRITE_PROGRAM_PAGE  "\x4C\x00\x00\x00"

#define READ_FUSE_L         "\x50\x00\x00\x00"
#define READ_FUSE_H         "\x58\x08\x00\x00"

#define WRITE_FUSE_L        "\xAC\xA0\x00\x00"
#define WRITE_FUSE_H        "\xAC\xA8\x00\x00"
#define CHIP_ERASE          "\xAC\x80\x00\x00"
#define DEVICE_PROG_ENABLE  "\xAC\x53\x00\x00"

#define POLL_RDY            "\xF0\x00\x00\x00"

#define AVR_ENABLE_RETRIES  2

#define AVR_PROGRAMMING_ACTIVE    (1<<0)
 

enum{
    AVR_RESPONSE_OK = 0,
    AVR_RESPONSE_FAIL = -1
};

typedef struct _AvrDevice{
    uint8_t data[AVR_INSTRUCTION_SIZE];
    uint8_t status;
}AvrDevice;

volatile uint32_t tbit;
/*
uint8_t instruction[AVR_INSTRUCTION_SIZE];

SpiBuffer serial_instruction = {
    .len = AVR_INSTRUCTION_SIZE,
    .data = instruction,
};
*/

static AvrDevice s_avr_device;

static inline void avrWaitReady(void){
uint32_t timeout = 0x100000;
    while(timeout--){
        
        memcpy(s_avr_device.data, POLL_RDY, AVR_INSTRUCTION_SIZE);
        BOARD_SPI_Write(s_avr_device.data, AVR_INSTRUCTION_SIZE);
        if( !(s_avr_device.data[3] & 0x01)){
            return;
        }                
    }
}
/****************************************************************************
 * Debug wire is a serial protocol used on AVR chips for debuging
 *                               
 * RESET     _______           _________   _   _   _   _   _______________
 *                  |         |         | | | | | | | | | |
 *                  |_________|         |_| |_| |_| |_| |_|
 * 
 *                  |<-150us->|         [      0x55       ]
 * 
 * Note: debugWire only available if fuse bit DWEN is programmed
 * 
 * If the DWEN bit is programmed the protocol is enabled and 
 * starts upon a reset by sending a sync byte 0x55. This byte is used
 * to determine the baudrate by measuring each pulse length. 
 * Current implementation only measures the duration of the first low pule 
 * after the 150us initial pulse. The measured duration is set as bit time reference 
 * and used for communication.
 * 
 * */

// Called when a falling edge is detected on reset pin (P0.23 for blue board)
void autoBaudCb(uint32_t capValue){
    static uint32_t delta = 0;
	if(delta == 0){
		delta = capValue;
	}else{
		delta = capValue - delta - 1;
		
		TIM_Reset(LPC_TIM3);
        tbit = delta - 2;
		delta = 0;
	}
}

// Called at every bit time
void bitTime(void){
    tbit = 1;
}

// TODO: This can be optimized to use timer callback to send bits
// and avoid blocking calls
// This has to be tested due tos changes on timer API
void avrSend_dW(uint8_t data){

    TIMER_SetInterval(bitTime, (tbit >> 1));
    AVR_RST0;

	for(uint8_t i = 0; i < 8; i++){
        TIM_Restart(LPC_TIM3);
        tbit = 0;

        while(!tbit){

        }

		if(data & (0x1<<i)){
			 AVR_RST1;
		}else{
			 AVR_RST0;
		}		
	}

    TIM_Restart(LPC_TIM3);
    tbit = 0;
    while(!tbit);
    AVR_RST1;
}

// TODO: Optimize to use generic timer API and perform testing
char avrDisable_dW(void){
    uint32_t autobaud;

    AVR_RST1; DelayMs(5);
    AVR_RST0; DelayMs(5);
    TIM_InitCapture(LPC_TIM3);
    TIM_Capture(LPC_TIM3, 0, 1, autoBaudCb);  // 1: Capture on falling edge
    tbit = 0;
    AVR_RSTZ; DelayMs(50); // Skip 150us initial pulse
    AVR_RST1;
    AVR_RSTY;    
    while(!tbit);
#if 0
    count = AVR_DW_SYNC_TIMEOUT;
    while(!tbit && count)
    {
        DelayMs(5);
        count--;
    }

    if(!count)
    {
        VCOM_printf("Unable to sync debugWire\n");
        return 0;
    }

    if(tbit < 10)
    {
        VCOM_printf("Unable to preform autobaud\n");
        return 0;
    }
#endif
    autobaud = tbit;
    TIM_InitMatch(LPC_TIM3);
    //VCOM_printf("Tbit = %dus\n",tbit/2);
    TIMER_SetInterval(bitTime, tbit * 7);
    TIM_Restart(LPC_TIM3);
    tbit = 0;
    while(!tbit);
    tbit = autobaud;
    AVR_RSTY;
    avrSend_dW(0x06);
    TIM_Reset(LPC_TIM3);
    return 0;
}

/**
* Program mode is entered if the device echoes the second 
 * sent byte. On this action the programming mode is enable 
 * and the reset line is left at low state. This should execute 
 * prior to any operation.
 * */
char avrProgrammingEnable(uint8_t trydW){

    if(s_avr_device.status & AVR_PROGRAMMING_ACTIVE)
    {
       return AVR_RESPONSE_OK; 
    }

    memcpy(s_avr_device.data, DEVICE_PROG_ENABLE, AVR_INSTRUCTION_SIZE);

    BOARD_SPI_SetFrequency(DEFAULT_AVR_SPI_FREQ);
    
    AVR_RSTY;

    for (uint8_t i = 0; i < AVR_ENABLE_RETRIES; i++){
        AVR_RST1;
        DelayMs(2);
        AVR_RST0;
        DelayMs(20);
        BOARD_SPI_Write(s_avr_device.data, AVR_INSTRUCTION_SIZE);
        if(s_avr_device.data[2] == 0x53){
            s_avr_device.status |= AVR_PROGRAMMING_ACTIVE;
            return AVR_RESPONSE_OK;
        }

        // TODO: If fail, try using lower SCK
        if(i == AVR_ENABLE_RETRIES) 
            break;
        
        memcpy(s_avr_device.data, DEVICE_PROG_ENABLE, AVR_INSTRUCTION_SIZE);

        if(trydW) 
            avrDisable_dW();       
    }
    AVR_RST1;
    return AVR_RESPONSE_FAIL; 
}

/**
 * Clear programming active flag and release reset pin (HIGH state)
 * */
void avrProgrammingDisable(void){
    s_avr_device.status &= ~(AVR_PROGRAMMING_ACTIVE);
    AVR_RST1;
}

/**
 * if present, place device signature on the given buffer
 * */
void avrDeviceCode(uint8_t *buf){

    if( avrProgrammingEnable(NO) != AVR_RESPONSE_OK){
        *((int32_t*)buf) = AVR_RESPONSE_FAIL;
        return;
    }

    memcpy(s_avr_device.data, DEVICE_CODE0_CMD, AVR_INSTRUCTION_SIZE);
    BOARD_SPI_Write(s_avr_device.data, AVR_INSTRUCTION_SIZE);
    buf[2] = s_avr_device.data[3];

    memcpy(s_avr_device.data, DEVICE_CODE1_CMD, AVR_INSTRUCTION_SIZE);
    BOARD_SPI_Write(s_avr_device.data, AVR_INSTRUCTION_SIZE);
    buf[1] = s_avr_device.data[3];

    memcpy(s_avr_device.data, DEVICE_CODE2_CMD, AVR_INSTRUCTION_SIZE);
    BOARD_SPI_Write(s_avr_device.data, AVR_INSTRUCTION_SIZE);
    buf[0] = s_avr_device.data[3];

    buf[3] = 0;
}


void avrWriteFuses(uint8_t lh, uint8_t fuses){

    if( avrProgrammingEnable(NO) != AVR_RESPONSE_OK){
        return;
    }

    if (lh == 1){
        memcpy(s_avr_device.data, WRITE_FUSE_H, AVR_INSTRUCTION_SIZE);
    }else{
        memcpy(s_avr_device.data, WRITE_FUSE_L, AVR_INSTRUCTION_SIZE);
    }   

    s_avr_device.data[3] = fuses;
    

    BOARD_SPI_Write(s_avr_device.data, AVR_INSTRUCTION_SIZE);

    avrWaitReady();
}

uint32_t avrReadFuses(void){
uint32_t fuses;

    if( avrProgrammingEnable(NO) != AVR_RESPONSE_OK){
        return -1;
    }

    memcpy(s_avr_device.data, READ_FUSE_L, AVR_INSTRUCTION_SIZE);
    BOARD_SPI_Write(s_avr_device.data, AVR_INSTRUCTION_SIZE);
    fuses = s_avr_device.data[3];

    memcpy(s_avr_device.data, READ_FUSE_H, AVR_INSTRUCTION_SIZE);
    BOARD_SPI_Write(s_avr_device.data, AVR_INSTRUCTION_SIZE);
    fuses |= s_avr_device.data[3] << 8;    

    return fuses;
}

void avrChipErase(void){

    if( avrProgrammingEnable(NO) != AVR_RESPONSE_OK){
        return;
    }

    memcpy(s_avr_device.data, CHIP_ERASE, AVR_INSTRUCTION_SIZE);

    BOARD_SPI_Write(s_avr_device.data, AVR_INSTRUCTION_SIZE);

    avrWaitReady();
}

uint16_t avrReadProgram(uint16_t addr){
uint16_t value = 0;

    if( avrProgrammingEnable(NO) != AVR_RESPONSE_OK){
        return value;
    }

    /* read high byte */
    memcpy(s_avr_device.data, READ_PROGRAM_PAGE_H, AVR_INSTRUCTION_SIZE);
    s_avr_device.data[1] = HIGH_BYTE(addr);
    s_avr_device.data[2] = LOW_BYTE(addr);
    BOARD_SPI_Write(s_avr_device.data, AVR_INSTRUCTION_SIZE);
    value = s_avr_device.data[3];

    value <<= 8;

    /* read low byte */
    memcpy(s_avr_device.data, READ_PROGRAM_PAGE_L, AVR_INSTRUCTION_SIZE);
    s_avr_device.data[1] = HIGH_BYTE(addr);
    s_avr_device.data[2] = LOW_BYTE(addr);
    BOARD_SPI_Write(s_avr_device.data, AVR_INSTRUCTION_SIZE);
    value |= s_avr_device.data[3];    

    return value;
}

void avrLoadProgramPage(uint8_t addr, uint16_t value){

    if( avrProgrammingEnable(NO) != AVR_RESPONSE_OK){
        return;
    }

    /* load low byte */
    memcpy(s_avr_device.data, LOAD_PROGRAM_PAGE_L, AVR_INSTRUCTION_SIZE);
    s_avr_device.data[2] = addr;
    s_avr_device.data[3] = LOW_BYTE(value);
    BOARD_SPI_Write(s_avr_device.data, AVR_INSTRUCTION_SIZE);
    
    /* load high byte */
    memcpy(s_avr_device.data, LOAD_PROGRAM_PAGE_H, AVR_INSTRUCTION_SIZE);
    s_avr_device.data[2] = addr;
    s_avr_device.data[3] = HIGH_BYTE(value);
    BOARD_SPI_Write(s_avr_device.data, AVR_INSTRUCTION_SIZE);
}

void avrWriteProgramPage(uint16_t addr){ 

    if( avrProgrammingEnable(NO) != AVR_RESPONSE_OK){
        return;
    }  

    memcpy(s_avr_device.data, WRITE_PROGRAM_PAGE, AVR_INSTRUCTION_SIZE);
    s_avr_device.data[1] = HIGH_BYTE(addr);
    s_avr_device.data[2] = LOW_BYTE(addr);
    BOARD_SPI_Write(s_avr_device.data, AVR_INSTRUCTION_SIZE);

    avrWaitReady();
}


//--------------------------------------
//
//--------------------------------------
void CmdAvr::help(void){
    console->print("Usage: avr [option] \n\n");  
    console->print("\t -s, s_avr_device signature\n");
    console->print("\t -e, Erase device\n");
    console->print("\t -f, Read fuses\n");
    console->print("\t -p, <dw> programming mode, try debug wire \n");
    console->putString("  Avr Pins\n"
                    "\tSCK   P0.7\n"
                    "\tMISO  P0.8\n"
                    "\tMOSI  P0.9\n"
                    "\tRST   P0.24=>P0.23\n"); 

}

char CmdAvr::execute(void *ptr){
int32_t signature, aux;
char *p1;

	p1 = (char*)ptr;

	// check parameters
    if( p1 == NULL || *p1 == '\0'){
        help();
        return CMD_OK;
    }

    s_avr_device.status = 0;

	// parse options
	while(*p1 != '\0'){
		if( isNextWord(&p1,"-s")){			
            avrDeviceCode((uint8_t*)&signature);
            if(signature == AVR_RESPONSE_FAIL){
                console->print("fail to enable programming\n");
            }else{
                console->print("s_avr_device signature 0x%X\n", signature);
            }
		    //busnum = nextInt(&p1);
		}else if( isNextWord(&p1,"-p")){			
            nextInt(&p1, &aux);
            if(aux) console->print("Using debug wire, pulse P0.23 to GNG to stop autobaud\n");
            avrProgrammingEnable(aux);
            //slave = nextHex(&p1);
		}else if( isNextWord(&p1,"-e")){			
            avrChipErase();
            //op = I2C_READ;
		}else if( isNextWord(&p1,"-f")){			
            avrFuses(p1);
            //op = I2C_WRITE;       
        }else{
			p1 = nextWord(p1);
		}
	}

    return CMD_OK;
}



char CmdAvr::avrFuses(void *ptr){
uint8_t lh;
uint32_t fuses;

    lh = nextChar((char**)&ptr); 

    if( lh == 'l'){
        lh = 0;
    }else if( lh == 'h')
        lh = 1;
   
    

    if(!nextHex((char**)&ptr, &fuses))
        lh = 255;

    if(lh == 0 || lh == 1){
        avrWriteFuses(lh,fuses);
    }  

    fuses = avrReadFuses();
    console->print("Fuses: H=%X L=%X\n", (fuses >> 8) & 255, fuses & 255 );

    return CMD_OK;
}
