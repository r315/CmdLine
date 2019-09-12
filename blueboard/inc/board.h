#ifndef _board_h_
#define _board_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <blueboard.h>
#include <libemb.h>
#include <gpio.h>
#include <ili9328.h>
#include <lcd.h>
#include <timer.h>
#include <stdout.h>

#define BOARD_Init BB_Init
#define SW_Reset BB_SW_Reset
#define DelayMs CLOCK_DelayMs
#define GetTicks CLOCK_GetTicks
#define ElapsedTicks CLOCK_ElapsedTicks

void App(void);

extern StdOut vcp;

#ifdef __cplusplus
}
#endif


#endif

