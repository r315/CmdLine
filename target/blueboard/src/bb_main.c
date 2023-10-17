#include "board.h"
#include "clock_lpc17xx.h"
#include "tim_lpc17xx.h"
#include "app.h"
#include "gpio.h"

void App(void);


static spibus_t spi_aux = {
    .bus = BOARD_SPI_AUX_BUS,
    .freq = 500000,
    .flags  = SPI_MODE0
};

spibus_t *BOARD_GetSpiMain(void){
    return BB_SPI_GetMain();
}

spibus_t *BOARD_GetSpiAux(void){
    return &spi_aux;
}

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
    BB_Init();

    CLOCK_InitUSBCLK();
    SERIAL_Init();   

    TIM_InitMatch(LPC_TIM0);    // Use TIMER0 as timebase for generic timer

    
    //LCD_SetOrientation(LCD_LANDSCAPE);
    
    DBG_PIN_INIT;

    App();

	return 0;
}
