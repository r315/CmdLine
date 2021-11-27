#include "board.h"
#include "app.h"

void App(void);

int main()
{    

    CLOCK_Init(100);
	CLOCK_InitUSBCLK();

    BOARD_Init();

    BOARD_SERIAL4_HANDLER.port.bus = SERIAL4;
    SERIAL_Config(&BOARD_SERIAL4_HANDLER);

    BOARD_SERIAL0_HANDLER.port.bus = SERIAL0;
    SERIAL_Config(&BOARD_SERIAL0_HANDLER);

    DISPLAY_Init(1);
	LCD_Rotation(LCD_LANDSCAPE);
	LCD_Bkl(1);

    DBG_PIN_INIT;

    App();

	return 0;
}
