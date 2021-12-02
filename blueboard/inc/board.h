#ifndef _board_h_
#define _board_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "blueboard.h"
#include "lcd.h"
#include "timer.h"
#include "stdout.h"
#include "debug.h"
#include "pwm.h"
#include "serial.h"
#include "spi.h"
#include "uart.h"


#define SW_Reset                BB_SW_Reset

#define DBG_PIN         11
#define DBG_PIN_HIGH    LPC_GPIO2->FIOSET = (1 << DBG_PIN)
#define DBG_PIN_LOW     LPC_GPIO2->FIOCLR = (1 << DBG_PIN)
#define DBG_PIN_INIT \
        LPC_GPIO2->FIODIR |= (1 << DBG_PIN); \
        LPC_PINCON->PINSEL4 &= ~(3<<22);  /* P2.11 (used as GPIO) */

/**
 * Serial
 * */
#define BOARD_SERIAL_HANDLERS           BOARD_SERIAL0_HANDLER, BOARD_SERIAL1_HANDLER, BOARD_SERIAL3_HANDLER, BOARD_SERIAL4_HANDLER
#define BOARD_SERIAL0                   (&BOARD_SERIAL0_HANDLER.out)
#define BOARD_SERIAL1                   (&BOARD_SERIAL1_HANDLER.out)
#define BOARD_SERIAL4                   (&BOARD_SERIAL4_HANDLER.out)
#define BOARD_STDIO                     BOARD_SERIAL4

extern serialhandler_t BOARD_SERIAL_HANDLERS;
/**
 * SPI
 * */
#define BOARD_SPIDEV_HANDLER spi1
#define BOARD_SPIDEV    (&BOARD_SPIDEV_HANDLER)
extern spibus_t BOARD_SPIDEV_HANDLER;

#ifdef __cplusplus
}
#endif
#endif

