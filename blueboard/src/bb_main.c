#include "board.h"
#include "app.h"

void App(void);

spibus_t BOARD_SPIDEV_HANDLER;

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

    BOARD_Init();

    //SERIAL_Config(&BOARD_SERIAL0_HANDLER, SERIAL0);
    SERIAL_Config(&BOARD_SERIAL1_HANDLER, SERIAL1 | SERIAL_DATA_8B | SERIAL_PARITY_NONE | SERIAL_STOP_1B | SERIAL_SPEED_115200);
    SERIAL_Config(&BOARD_SERIAL3_HANDLER, SERIAL3 | SERIAL_DATA_8B | SERIAL_PARITY_NONE | SERIAL_STOP_1B | SERIAL_SPEED_115200);
    SERIAL_Config(&BOARD_SERIAL4_HANDLER, SERIAL4);

    DISPLAY_Init(1);
	LCD_Rotation(LCD_LANDSCAPE);
	LCD_Bkl(1);

    DBG_PIN_INIT;

    App();

	return 0;
}
