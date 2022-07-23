#include "board.h"
#include "app.h"

void App(void);


void BOARD_LCD_Init(){
    
}

void BOARD_LCD_WriteArea(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *data){
	LCD_WriteArea(x, y, w, h, data);
}

void BOARD_LCD_Scroll(uint16_t sc){
	LCD_Scroll(sc);
}

int main()
{
    CLOCK_Init(100);
	CLOCK_InitUSBCLK();

    TIM_InitMatch(LPC_TIM0);    // Use TIMER0 as timebase for generic timer

    BOARD_Init();
    BOARD_SERIAL_Init();
    DISPLAY_Init(0);
    LCD_Rotation(LCD_LANDSCAPE);
    LCD_Bkl(1);
    DBG_PIN_INIT;

    App();

	return 0;
}
