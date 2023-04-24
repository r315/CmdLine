#include <stdio.h>
#include "board.h"
#include "app.h"

void BOARD_LCD_Init(void){
}

int main(void)
{
    BOARD_Init();

    //LCD_SetOrientation(LCD_REVERSE_LANDSCAPE);
    
    //LIB2D_Init();
    //LIB2D_Print("CPU %uMHz\n", SystemCoreClock/1000000);

    App();
    
    while (1)
    {
        LED1_TOGGLE;
        DelayMs(200);
    }

    return 0;
}
