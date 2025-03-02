#ifndef _board_h_
#define _board_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "blueboard.h"
#include "ili9328.h"
#include "debug.h"
#include "pwm.h"
#include "serial.h"
#include "spi.h"
#include "uart.h"
#include "rng.h"
#include "i2s.h"
#include "button.h"
#include "gpio.h"

#define BOARD_Init              BB_Init
#define BOARD_SPI_Write         BB_SPI_Write
#define BOARD_SPI_SetFrequency  BB_SPI_SetFrequency

#define DBG_PIN                 26
#define DBG_PIN_HIGH            LPC_GPIO3->FIOSET = (1 << DBG_PIN)
#define DBG_PIN_LOW             LPC_GPIO3->FIOCLR = (1 << DBG_PIN)
#define DBG_PIN_INIT            LPC_GPIO3->FIODIR |= (1 << DBG_PIN); \
                                LPC_PINCON->PINSEL7 &= ~(3 << 20);  /* P3.26 (used as GPIO) */

void BOARD_LCD_Init(void);
inline spibus_t *BOARD_GetSpiAux(void){return &spibus_1;}
inline spibus_t *BOARD_GetSpiMain(void){return &spibus_sd;}

#ifdef __cplusplus
}
#endif
#endif

