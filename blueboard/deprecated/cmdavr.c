#include <LPC17xx.h>
#include <string.h>
#include "strfunctions.h"
#include "vcom.h"
#include "command.h"
#include "cmdspi.h"
#include "cmdavr.h"

#define DEVICE_CODE0_CMD    {0x30, 0x00, 0x00, 0x00}
#define DEVICE_CODE1_CMD    {0x30, 0x00, 0x01, 0x00}
#define DEVICE_CODE2_CMD    {0x30, 0x00, 0x02, 0x00}

#define READ_FUSE_L         {0x50, 0x00, 0x00, 0x00}
#define READ_FUSE_H         {0x58, 0x00, 0x00, 0x00}
#define WRITE_FUSE_L        {0xAC, 0xA0, 0x00, 0x00}
#define WRITE_FUSE_H        {0xAC, 0xA8, 0x00, 0x00}
#define CHIP_ERASE          {0xAC, 0x80, 0x00, 0x00}
#define POLL_RDY            {0xF0, 0x00, 0x00, 0x00}

#define DEVICE_PROG_ENABLE {0xAC, 0x53, 0x00, 0x00}
#define AVR_ENABLE_RETRIES 2

#define AVR_PROGRAMMING_ACTIVE  (1<<0)

#define AVR_RST_PIN_PORT       LPC_GPIO0
#define AVR_RST_PIN            (1<<24)

#define AVR_DISABLE_RESET               \
            DelayMs(100);               \
            AVR_RST_PIN_PORT->FIOSET = AVR_RST_PIN; \
            devicestatus = 0;           \

#define AVR_RST0 AVR_RST_PIN_PORT->FIOCLR = AVR_RST_PIN
#define AVR_RST1 AVR_RST_PIN_PORT->FIOSET = AVR_RST_PIN
#define AVR_RSTZ AVR_RST_PIN_PORT->FIODIR &= ~AVR_RST_PIN  
#define AVR_RSTY AVR_RST_PIN_PORT->FIODIR |= AVR_RST_PIN  

#define AVR_DW_SYNC_TIMEOUT 10

static uint8_t devicestatus = 0;


CmdLine avrCommands[] =
{
	{"help",avrHelp},
	{"enable",avrEnable},
    {"test",avrTest},
    {"fuses",avrFuses},
    {"erase",avrErase},
};

volatile uint32_t tbit;

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


char avrProgrammingEnable(void){
uint8_t data[] = DEVICE_PROG_ENABLE;
SpiBuffer spibuf;
    
    spibuf.len = sizeof(data);  
    spibuf.data = data;

    if(devicestatus & AVR_PROGRAMMING_ACTIVE)
        return 0;
    
    AVR_RSTY;

    for (uint8_t i = 0; i < AVR_ENABLE_RETRIES; i++){
        AVR_RST0;
        DelayMs(50);
        spiWriteBuffer(&spibuf);
        if(spibuf.data[2] == 0x53){
            return 0;
            devicestatus |= AVR_PROGRAMMING_ACTIVE;
        }

        if(i == AVR_ENABLE_RETRIES) 
            break;
        
        memcpy(spibuf.data, (uint8_t [])DEVICE_PROG_ENABLE, 4);
        VCOM_printf("\nNo device detected, Disabling debugWire..\n");
        avrDisable_dW();
        DelayMs(2);
    }
    VCOM_printf("fail to enable programming\n");
    return -1; 
}

uint32_t avrDeviceCode(void){
uint8_t data[] = DEVICE_CODE0_CMD;
SpiBuffer spibuf;  
uint32_t dcode;

    spibuf.len = sizeof(data); 
    spibuf.data = data;

    spiWriteBuffer(&spibuf);
    dcode = spibuf.data[3] << 16;
    memcpy(spibuf.data, (uint8_t [])DEVICE_CODE1_CMD, 4);
    spiWriteBuffer(&spibuf);
    dcode |= spibuf.data[3] << 8;
    memcpy(spibuf.data, (uint8_t [])DEVICE_CODE2_CMD, 4);
    spiWriteBuffer(&spibuf);
    dcode |= spibuf.data[3];  

    return dcode;
}

void avrWriteFuses(uint8_t lh, uint8_t fuses){
uint8_t data [4];
SpiBuffer spibuf;  

    spibuf.len = sizeof(data); 
    spibuf.data = data;

    if (lh == 1){
        memcpy(spibuf.data, (uint8_t [])WRITE_FUSE_H, 4);
    }else{
        memcpy(spibuf.data, (uint8_t [])WRITE_FUSE_L, 4);
    }   

    data[3] = fuses;
    spibuf.data = data;
    spibuf.len = sizeof(data); 

    spiWriteBuffer(&spibuf);
}

uint32_t avrReadFuses(void){
uint8_t data[] = READ_FUSE_L;
SpiBuffer spibuf;  
uint32_t fuses;

    spibuf.len = sizeof(data); 
    spibuf.data = data;

    spiWriteBuffer(&spibuf);
    fuses = spibuf.data[3];
    memcpy(spibuf.data, (uint8_t [])READ_FUSE_H, 4);
    spiWriteBuffer(&spibuf);
    fuses |= spibuf.data[3] << 8;    

    return fuses;
}

uint8_t avrPollRdy(void){
uint8_t data[] = POLL_RDY;
SpiBuffer spibuf;
    spibuf.len = sizeof(data); 
    spibuf.data = data;
    spiWriteBuffer(&spibuf);
    return spibuf.data[3] & 0x01;
}

char avrHelp(void *ptr){
	VCOM_printf("\n\ravr commands:\n\r");
	VCOM_printf("\nenable,\tsend program enable command\n\r");
	VCOM_printf("\n\n\n\r");
	return CMD_OK;
}

char avrTest(void *ptr){
 
    return CMD_OK;
}

char avrErase(void *ptr){
uint8_t data[] = CHIP_ERASE;
SpiBuffer spibuf;  

    spibuf.len = sizeof(data); 
    spibuf.data = data;

    if( avrProgrammingEnable() ) {        
        AVR_DISABLE_RESET;
        return CMD_OK;
    }

    spiWriteBuffer(&spibuf);

    while(avrPollRdy());
    AVR_DISABLE_RESET;

    return CMD_OK;
}


char avrFuses(void *ptr){
uint8_t lh;
int fuses;

    if( avrProgrammingEnable() ) {        
        AVR_DISABLE_RESET;
        return CMD_OK;
    }

    lh = cmdNextChar((char**)&ptr); //*((uint8_t*)ptr);  

    if( lh == 'l'){
        lh = 0;
    }else if( lh == 'h')
        lh = 1;
   
    fuses = cmdNextHex((char**)&ptr);

    if(fuses == -1)
        lh = 255;

    if(lh == 0 || lh == 1){
        avrWriteFuses(lh,fuses);
        while(avrPollRdy());
        AVR_DISABLE_RESET;
    }  

    if( avrProgrammingEnable() ) {        
        AVR_DISABLE_RESET;
        return CMD_OK;
    }  

    fuses = avrReadFuses();
    VCOM_printf("Fuses: H=%X L=%X\n", (fuses >> 8) & 255, fuses & 255 );
    AVR_DISABLE_RESET;
    return CMD_OK;
}

char avrEnable(void *ptr){
    if( avrProgrammingEnable() ) {
        AVR_DISABLE_RESET;
        return CMD_OK;
    }

    VCOM_printf("Device signature 0x%X\n", avrDeviceCode());
   
    AVR_DISABLE_RESET;
    return CMD_OK;
}

char avrCmd(void *ptr){
    if(cmdProcess(ptr, avrCommands, sizeof(avrCommands)/sizeof(CmdLine)) == CMD_NOT_FOUND){
        avrHelp(NULL);
    }
    return CMD_OK;
}


