#ifndef _board_h_
#define _board_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "blueboard.h"
#include "lcd.h"
#include "stdout.h"
#include "debug.h"
#include "pwm.h"
#include "serial.h"
#include "spi.h"
#include "uart.h"
#include "rng.h"
#include "i2s.h"
#include "button.h"


#define BOARD_Init              BB_Init
#define SW_Reset                BB_SW_Reset
#define BOARD_SPI_Write         BB_SPI_Write
#define BOARD_SPI_SetFrequency  BB_SPI_SetFrequency
#define BOARD_SPI_AUX_BUS       SPI_BUS2

#define DBG_PIN                 11
#define DBG_PIN_HIGH            LPC_GPIO2->FIOSET = (1 << DBG_PIN)
#define DBG_PIN_LOW             LPC_GPIO2->FIOCLR = (1 << DBG_PIN)
#define DBG_PIN_INIT            LPC_GPIO2->FIODIR |= (1 << DBG_PIN); \
                                LPC_PINCON->PINSEL4 &= ~(3<<22);  /* P2.11 (used as GPIO) */

#define BOARD_CARD_ACTIVE       LED1_ON
#define BOARD_CARD_NOT_ACTIVE   LED1_OFF


void BOARD_SERIAL_Init();
serialhandler_t *BOARD_GetSerial(void);
serialhandler_t *BOARD_GetSerialAux(void);
void BOARD_LCD_Init(void);
void BOARD_LCD_WriteArea(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *data);
void BOARD_LCD_Scroll(uint16_t sc);
void BOARD_SPI_InitAux(void);
spibus_t *BOARD_GetSpiAux(void);
spibus_t *BOARD_GetSpiMain(void);

#ifdef __cplusplus
}
#endif
#endif

