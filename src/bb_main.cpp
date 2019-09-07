#include "board.h"

int main()
{    

    CLOCK_Init(72);
	CLOCK_InitUSBCLK();

    BOARD_Init();    

    DISPLAY_Init(ON);
	LCD_Rotation(LCD_LANDSCAPE);
	LCD_Bkl(ON);

    App();

	return 0;
}
