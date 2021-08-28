#include "board.h"

int main()
{    

    CLOCK_Init(100);
	CLOCK_InitUSBCLK();

    BOARD_Init();    

    DISPLAY_Init(1);
	LCD_Rotation(LCD_LANDSCAPE);
	LCD_Bkl(1);

    DBG_PIN_INIT;

    App();

	return 0;
}
