/**
 * 
 * 
 *      BLUEBOARD board.h for console 
 * 
 * 
 * */

#ifndef _board_h_
#define _board_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <blueboard.h>
#include <lcd.h>
#include <timer.h>
#include <stdout.h>
#include <debug.h>

#define BOARD_Init BB_Init
#define SW_Reset BB_SW_Reset
#define DelayMs CLOCK_DelayMs
#define GetTicks CLOCK_GetTicks
#define ElapsedTicks CLOCK_ElapsedTicks

#define DBG_PIN         11
#define DBG_PIN_HIGH    LPC_GPIO2->FIOSET = (1 << DBG_PIN)
#define DBG_PIN_LOW     LPC_GPIO2->FIOCLR = (1 << DBG_PIN)
#define DBG_PIN_INIT \
        LPC_GPIO2->FIODIR |= (1 << DBG_PIN); \
        LPC_PINCON->PINSEL4 &= ~(3<<22);  /* P2.11 (used as GPIO) */    \


void App(void);

extern StdOut vcp;

#ifdef __cplusplus
}
#endif
#endif

