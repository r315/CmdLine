#include "board.h"
#include "cmdavr.h"
#include "cmdspi.h"
#include "debug.h"

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

#define AVR_PROGRAMMING_ACTIVE  (1<<0)

#define AVR_DISABLE_RESET               \
            DelayMs(100);               \
            AVR_RST_PIN_PORT->FIOSET = AVR_RST_PIN;


#define AVR_RSTZ AVR_RST_PIN_PORT->FIODIR &= ~AVR_RST_PIN  
#define AVR_RSTY AVR_RST_PIN_PORT->FIODIR |= AVR_RST_PIN  

enum{
    AVR_RESPONSE_OK = 0,
    AVR_RESPONSE_FAIL = -1
};


Vcom *_vcom;

volatile uint32_t tbit;

uint8_t instruction[AVR_INSTRUCTION_SIZE];
SpiBuffer serial_instruction = {
    .len = AVR_INSTRUCTION_SIZE,
    .data = instruction,
};


static inline void avrWaitReady(void){
uint32_t timeout = 0x100000;
    while(timeout--){
        serial_instruction.len = AVR_INSTRUCTION_SIZE; 
        memcpy(serial_instruction.data, POLL_RDY, AVR_INSTRUCTION_SIZE);
        spiWriteBuffer(&serial_instruction);
        if( !(serial_instruction.data[3] & 0x01)){
            return;
        }                
    }
}

// Called when a falling edge is detected on P0.23 
void autoBaudCb(void *ptr){
    static uint32_t count = 0;
	if(count == 0){
		count = *((uint32_t*)ptr);
	}else{
		count = *((uint32_t*)ptr) - count - 1;
		
		TIM_Stop(LPC_TIM3);
        tbit = count - 2;
		count = 0;
	}
}

// Called each time the timer expires
void bitTime(void *ptr){
    tbit = 1;
}

void avrSend_dW(uint8_t data){

    TIMER_Match_Init(LPC_TIM3, 0, (tbit >> 1) , bitTime);
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


char avrDisable_dW(void){
uint32_t autobaud;
    AVR_RST0;
    TIMER_CAP_Init(LPC_TIM3, 0, CAP_FE, autoBaudCb);
    tbit = 0;
    AVR_RSTZ;
    AVR_RST1;
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
    //VCOM_printf("Tbit = %dus\n",tbit/2);
    TIMER_Match_Init(LPC_TIM3, 0, tbit * 7, bitTime);
    TIM_Restart(LPC_TIM3);
    tbit = 0;
    while(!tbit);
    tbit = autobaud;
    AVR_RSTY;
    avrSend_dW(0x06);
    TIM_Stop(LPC_TIM3);
    return 0;
}

char avrProgrammingEnable(uint8_t trydW){

    serial_instruction.len = AVR_INSTRUCTION_SIZE;

    memcpy(serial_instruction.data, DEVICE_PROG_ENABLE, AVR_INSTRUCTION_SIZE);
    
    AVR_RSTY;

    for (uint8_t i = 0; i < AVR_ENABLE_RETRIES; i++){
        AVR_RST1;
        DelayMs(2);
        AVR_RST0;
        DelayMs(20);
        spiWriteBuffer(&serial_instruction);
        if(serial_instruction.data[2] == 0x53){
            return AVR_RESPONSE_OK;
        }

        if(i == AVR_ENABLE_RETRIES) 
            break;
        
        memcpy(serial_instruction.data, DEVICE_PROG_ENABLE, AVR_INSTRUCTION_SIZE);
        if(trydW) 
            avrDisable_dW();       
    }
    AVR_RST1;
    return AVR_RESPONSE_FAIL; 
}

void avrDeviceCode(uint8_t *buf){

    serial_instruction.len = AVR_INSTRUCTION_SIZE; 

    memcpy(serial_instruction.data, DEVICE_CODE0_CMD, AVR_INSTRUCTION_SIZE);
    spiWriteBuffer(&serial_instruction);
    buf[0] = serial_instruction.data[3] << 16;

    memcpy(serial_instruction.data, DEVICE_CODE1_CMD, AVR_INSTRUCTION_SIZE);
    spiWriteBuffer(&serial_instruction);
    buf[1] = serial_instruction.data[3] << 8;

    memcpy(serial_instruction.data, DEVICE_CODE2_CMD, AVR_INSTRUCTION_SIZE);
    spiWriteBuffer(&serial_instruction);
    buf[2] = serial_instruction.data[3];
}


void avrWriteFuses(uint8_t lh, uint8_t fuses){

    if (lh == 1){
        memcpy(serial_instruction.data, WRITE_FUSE_H, AVR_INSTRUCTION_SIZE);
    }else{
        memcpy(serial_instruction.data, WRITE_FUSE_L, AVR_INSTRUCTION_SIZE);
    }   

    serial_instruction.data[3] = fuses;
    serial_instruction.len = AVR_INSTRUCTION_SIZE;

    spiWriteBuffer(&serial_instruction);

    avrWaitReady();
}

uint32_t avrReadFuses(void){
uint32_t fuses;

    serial_instruction.len = AVR_INSTRUCTION_SIZE;  

    memcpy(serial_instruction.data, READ_FUSE_L, AVR_INSTRUCTION_SIZE);
    spiWriteBuffer(&serial_instruction);
    fuses = serial_instruction.data[3];

    memcpy(serial_instruction.data, READ_FUSE_H, AVR_INSTRUCTION_SIZE);
    spiWriteBuffer(&serial_instruction);
    fuses |= serial_instruction.data[3] << 8;    

    return fuses;
}



void avrChipErase(void){

    serial_instruction.len = AVR_INSTRUCTION_SIZE;
    memcpy(serial_instruction.data, CHIP_ERASE, AVR_INSTRUCTION_SIZE);

    spiWriteBuffer(&serial_instruction);

    avrWaitReady();
}

uint16_t avrReadProgram(uint16_t addr){
uint16_t value = 0;

    serial_instruction.len = AVR_INSTRUCTION_SIZE;

    /* read high byte */
    memcpy(serial_instruction.data, READ_PROGRAM_PAGE_H, AVR_INSTRUCTION_SIZE);
    serial_instruction.data[1] = HIGH_BYTE(addr);
    serial_instruction.data[2] = LOW_BYTE(addr);
    spiWriteBuffer(&serial_instruction);
    value = serial_instruction.data[3];

    value <<= 8;

    /* read low byte */
    memcpy(serial_instruction.data, READ_PROGRAM_PAGE_L, AVR_INSTRUCTION_SIZE);
    serial_instruction.data[1] = HIGH_BYTE(addr);
    serial_instruction.data[2] = LOW_BYTE(addr);
    spiWriteBuffer(&serial_instruction);
    value |= serial_instruction.data[3];    

    return value;
}

void avrLoadProgramPage(uint8_t addr, uint16_t value){

    serial_instruction.len = AVR_INSTRUCTION_SIZE;

    /* load low byte */
    memcpy(serial_instruction.data, LOAD_PROGRAM_PAGE_L, AVR_INSTRUCTION_SIZE);
    serial_instruction.data[2] = addr;
    serial_instruction.data[3] = LOW_BYTE(value);
    spiWriteBuffer(&serial_instruction);
    
    /* load high byte */
    memcpy(serial_instruction.data, LOAD_PROGRAM_PAGE_H, AVR_INSTRUCTION_SIZE);
    serial_instruction.data[2] = addr;
    serial_instruction.data[3] = HIGH_BYTE(value);
    spiWriteBuffer(&serial_instruction);
}

void avrWriteProgramPage(uint16_t addr){

    serial_instruction.len = AVR_INSTRUCTION_SIZE;

    memcpy(serial_instruction.data, WRITE_PROGRAM_PAGE, AVR_INSTRUCTION_SIZE);
    serial_instruction.data[1] = HIGH_BYTE(addr);
    serial_instruction.data[2] = LOW_BYTE(addr);
    spiWriteBuffer(&serial_instruction);

    avrWaitReady();
}


//--------------------------------------
//
//--------------------------------------
void CmdAvr::help(void){
    vcom->printf("Usage: avr [option] \n\n");  
    vcom->printf("\t -s, Device signature\n");
    vcom->printf("\t -e, Erase device\n");
    vcom->printf("\t -f, Read fuses\n");
    vcom->printf("\t -p, programming mode\n");
    vcom->puts("  Avr Pins\n"
                    "\tSCK   P0.7\n"
                    "\tMISO  P0.8\n"
                    "\tMOSI  P0.9\n"
                    "\tRST   P0.24=>P0.23\n"); 

}

char CmdAvr::execute(void *ptr){
int32_t signature;
char *p1;

	p1 = (char*)ptr;

    //_vcom = this->vcom;

	// check parameters
    if( p1 == NULL || *p1 == '\0'){
        help();
        return CMD_OK;
    }

	// parse options
	while(*p1 != '\0'){
		if( !xstrcmp(p1,"-s")){
			p1 = nextParameter(p1);
            avrDeviceCode((uint8_t*)&signature);
            if(signature == AVR_RESPONSE_FAIL){
                vcom->printf("fail to enable programming\n");
            }else{
                vcom->printf("Device signature 0x%X\n", signature);
            }
		    //busnum = nextInt(&p1);
		}else if( !xstrcmp(p1,"-p")){
			p1 = nextParameter(p1);
            avrProgrammingEnable(YES);
            //slave = nextHex(&p1);
		}else if( !xstrcmp(p1,"-e")){
			p1 = nextParameter(p1);
            avrChipErase();
            //op = I2C_READ;
		}else if( !xstrcmp(p1,"-f")){
			p1 = nextParameter(p1);
            avrFuses(p1);
            //op = I2C_WRITE;
        }else if( !xstrcmp(p1,"-s")){
			p1 = nextParameter(p1);
            //op = I2C_SCAN;
        }else{
			p1 = nextParameter(p1);
		}
	}  

   /* if( avrProgrammingEnable(YES) == AVR_RESPONSE_FAIL) {
        return AVR_RESPONSE_FAIL;
    }*/
 

    return CMD_OK;
}



char CmdAvr::avrFuses(void *ptr){
uint8_t lh;
int fuses;

    lh = nextChar((char**)&ptr); 

    if( lh == 'l'){
        lh = 0;
    }else if( lh == 'h')
        lh = 1;
   
    fuses = nextHex((char**)&ptr);

    if(fuses == -1)
        lh = 255;

    if(lh == 0 || lh == 1){
        avrWriteFuses(lh,fuses);
    }  

    fuses = avrReadFuses();
    vcom->printf("Fuses: H=%X L=%X\n", (fuses >> 8) & 255, fuses & 255 );

    return CMD_OK;
}
