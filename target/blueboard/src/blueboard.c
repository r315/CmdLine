// File Name: blueboard.c
// Date:  	  6-10-2010
/* 
07-05-2013 void getSysFreq(void) -> int getSysFreq(void)
			removida a chamada lcdInit() no metodo blueboardInit()
TODO: inicializar clock usb
*/

#include "blueboard.h"
#include "button.h"
#include "spi.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BB_MAIN_SPI (&s_spi1)
#define SPI_DEFAULT_SPEED	8000000		// Speed for SD card

static spibus_t s_spi1;
//---------------------------------------------------
//	
//---------------------------------------------------
void BB_Init(void){
	LPC_GPIO1->FIODIR |= LED1|LED2;
	LPC_GPIO2->FIODIR |= LED3;

	//GPIO_Init(BOARD_ACCEL_CS_PIN, PIN_OUT_PP);
	GPIO_Init(BOARD_SDCARD_CS_PIN, PIN_OUT_PP);
	
	LED1_OFF;
	LED2_OFF;
	LED3_OFF;
	
	//BOARD_ACCEL_DESELECT;	
	BOARD_SDCARD_DESELECT;

	LCD_Init(NULL);
	BUTTON_Init(BUTTON_DEFAULT_HOLD_TIME);

    // SPI configuration for memory card
	BB_MAIN_SPI->bus = SPI_BUS0;
    BB_MAIN_SPI->freq = SPI_DEFAULT_SPEED;
    BB_MAIN_SPI->flags  = SPI_MODE0;
    SPI_Init(BB_MAIN_SPI);

	//ACC_Init();
}

void SW_Reset(void){
    NVIC_SystemReset();
}

void DelayMs(uint32_t ms){
    CLOCK_DelayMs(ms);
}

uint32_t GetTick(void){
    return CLOCK_GetTicks();
}

uint32_t ElapsedTicks(uint32_t start_ticks){
    return CLOCK_ElapsedTicks(start_ticks);
}
//---------------------------------------------------
//	
//---------------------------------------------------
void BB_ConfigClockOut(uint8_t en){
	if(en){
		LPC_SC->CLKOUTCFG = (1<<4)|/* CCLK/2 */ (1<<8);/* CLKOU_EN*/
		LPC_PINCON->PINSEL3 |= (1<<22);// P1.27 CLKOUT 
	}else{
		LPC_SC->CLKOUTCFG = (1<<4)|/* CCLK/2 */ (1<<8);/* CLKOU_EN*/
		LPC_PINCON->PINSEL3 &= ~(1<<22);
	}
}


//--------------------------------------------------
//
//--------------------------------------------------
void BB_SPI_Write(uint8_t *data, uint32_t count){
	SPI_Transfer(BB_MAIN_SPI, data, count);
}

void BB_SPI_WriteDMA(uint8_t *data, uint32_t count){
	SPI_TransferDMA(BB_MAIN_SPI, data, count);
}

uint16_t BB_SPI_Send(uint8_t data){
	return SPI_Xchg(BB_MAIN_SPI, &data);
}

void BB_SPI_WaitEOT(){
	SPI_WaitEOT(BB_MAIN_SPI);
}

void BB_SPI_SetFrequency(uint32_t freq){
    BB_MAIN_SPI->flags &= ~SPI_ENABLED;	 
    BB_MAIN_SPI->freq = freq;    
    SPI_Init(BB_MAIN_SPI);
}

spibus_t *BB_SPI_GetMain(void){
    return BB_MAIN_SPI;
}

#ifdef __cplusplus
}
#endif

