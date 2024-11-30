#include "board.h"
#include "clock_lpc17xx.h"
#include "tim_lpc17xx.h"
#include "app.h"


void BOARD_LCD_Init(){
    BB_LCD_Init();
}

int main()
{
    BOARD_Init();

    CLOCK_InitUSBCLK();
    SERIAL_Init();

    TIM_InitMatch(LPC_TIM0);    // Use TIMER0 as timebase for generic timer


    //LCD_SetOrientation(LCD_LANDSCAPE);

    DBG_PIN_INIT;

    App();

	return 0;
}
