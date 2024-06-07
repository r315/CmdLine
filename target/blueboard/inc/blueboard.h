/*************************************************
14-07-2011 alterado def de _osc para pll 

**************************************************/

#ifndef _blueboard_h_
#define _blueboard_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>

#ifndef BOARD_BLUEBOARD
#define BOARD_BLUEBOARD
#endif

#include "LPC17xx.h"
#include "lpc17xx_hal.h"
#include "spi.h"
#include "ili9328.h"

#define PLL48   0
#define PLL72   1
#define PLL80   2
#define PLL100  3

/* Watchdog */
#define WDMOD_WDEN    (1 << 0)
#define WDMOD_WDRESET (1 << 1)
#define WDCLKSEL_WDSEL_PCLK (1 << 0)

#define WDT_RELOAD          \
{                           \
    LPC_WDT->FEED = 0xAA; \
    LPC_WDT->FEED = 0x55; \
}

//-----------------------------------------------------
// Joystick pins P1.14, p1.15, p1.16, p1.17, p1.28
// see button.h
//-----------------------------------------------------
#define BUTTON_UP       (1<<15)
#define BUTTON_DOWN     (1<<17)
#define BUTTON_LEFT     (1<<16)
#define BUTTON_RIGHT    (1<<28)
#define BUTTON_A        (1<<14)
#define BUTTON_CENTER   BUTTON_A

#define BUTTON_HW_READ  (~(LPC_GPIO1->FIOPIN) & BUTTON_MASK)
#define BUTTON_HW_INIT  (LPC_GPIO1->FIODIR &= ~(BUTTON_MASK))

#define BUTTON_MASK     (BUTTON_UP | BUTTON_DOWN | BUTTON_LEFT | BUTTON_RIGHT | BUTTON_A )
//-----------------------------------------------------
//
//-----------------------------------------------------

#define LED1            (1<<29) //P1.29 (D8 BLUE)
#define LED1_ON         LPC_GPIO1->FIOSET = LED1
#define LED1_OFF        LPC_GPIO1->FIOCLR = LED1
#define LED1_TOGGLE     LPC_GPIO1->FIOPIN ^= LED1

#define LED2            (1<<18) //P1.18 (D7 RED)
#define LED2_OFF        LPC_GPIO1->FIOSET = LED2
#define LED2_ON         LPC_GPIO1->FIOCLR = LED2

#define LED3            (1<<9)  //P2.9 (D1 RED)
#define LED3_OFF        LPC_GPIO2->FIOSET = LED3
#define LED3_ON         LPC_GPIO2->FIOCLR = LED3

#define LEDS_INIT       \
        LPC_GPIO1->FIODIR |= LED1|LED2; \
	    LPC_GPIO2->FIODIR |= LED3; \
    	LED1_OFF;   \
	    LED2_OFF;   \
	    LED3_OFF;


#define ACCEL_CS_PIN    (1<<6)
#define SELECT_ACCEL    LPC_GPIO0->FIOCLR = ACCEL_CS_PIM
#define DESELECT_ACCEL  LPC_GPIO0->FIOSET = ACCEL_CS_PIN

#define BOARD_SDCARD_CS_PIN     16
#define BOARD_SDCARD_CS_MASK    (1<<BOARD_SDCARD_CS_PIN)
#define BOARD_SDCARD_SELECT     LPC_GPIO0->FIOCLR = BOARD_SDCARD_CS_MASK		/* MMC CS = L */
#define	BOARD_SDCARD_DESELECT	LPC_GPIO0->FIOSET = BOARD_SDCARD_CS_MASK		/* MMC CS = H */
#define	MMC_SEL                 !(LPC_GPIO0->FIOPIN & BOARD_SDCARD_CS_MASK)     /* MMC CS status (true:selected) */

#define  LCD_CS         (1<<10) //P1.10
#define  LCD_RS         (1<<9)  //P1.9
#define  LCD_WR         (1<<8)  //P1.8
#define  LCD_RD         (1<<4)  //P1.4
#define  LCD_LED        (1<<1)  //P1.1
#define  LCD_RST        (1<<0)  //P1.0

#define  LCD_CTRLPORT    	LPC_GPIO1
#define  LCD_DATAPORT    	LPC_GPIO0->FIOPIN0
#define  LCD_DATAPORTDIR 	LPC_GPIO0->FIODIR0

#define  LCDCS0         LCD_CTRLPORT->FIOCLR = LCD_CS;
#define  LCDCS1         LCD_CTRLPORT->FIOSET = LCD_CS; 
#define  LCDRS0         LCD_CTRLPORT->FIOCLR = LCD_RS;
#define  LCDRS1         LCD_CTRLPORT->FIOSET = LCD_RS;
#define  LCDWR0         LCD_CTRLPORT->FIOCLR = LCD_WR; //asm("nop");
#define  LCDWR1         LCD_CTRLPORT->FIOSET = LCD_WR;  
#define  LCDRD0         LCD_CTRLPORT->FIOCLR = LCD_RD;
#define  LCDRD1         LCD_CTRLPORT->FIOSET = LCD_RD; 
#define  LCDRST0        LCD_CTRLPORT->FIOCLR = LCD_RST;
#define  LCDRST1        LCD_CTRLPORT->FIOSET = LCD_RST; 
#define  LCD_BKL0       LCD_CTRLPORT->FIOCLR = LCD_LED;
#define  LCD_BKL1       LCD_CTRLPORT->FIOSET = LCD_LED;

#define LCD_IO_INIT                                                   \
    LCD_CTRLPORT->FIODIR |= LCD_CS|LCD_RS|LCD_WR|LCD_RD|LCD_LED|LCD_RST;  \
    LCD_DATAPORTDIR |= 0xFF;
    
#define RAM_FUNC        __attribute__ ((section(".ram_code")))
#define RAM_CODE        RAM_FUNC

//enum {false = 0, true, OFF = false, ON = true};
#define BOARD_SD_GET_SPI        (&spibus_sd)

extern spibus_t spibus_1, spibus_sd;

//-----------------------------------------------------
void BB_Init(void);
void SW_Reset(void);
void BB_ConfigClockOut(uint8_t en);
void BB_RitTimeBase_Init(void);
void BB_RitDelay(uint32_t ms);
uint32_t BB_RitTicks(void);

void BB_SPI_Write(uint8_t *data, uint32_t count);
void BB_SPI_WriteDMA(uint8_t *data, uint32_t count);
void BB_SPI_SetFrequency(uint32_t freq);

void DelayMs(uint32_t ms);
uint32_t ElapsedTicks(uint32_t start_ticks);
uint32_t GetTick(void);

#ifdef __cplusplus
}
#endif

#endif






