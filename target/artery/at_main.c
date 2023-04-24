#include <stdio.h>
#include "board.h"

int main(void)
{
    BOARD_Init();

    //LCD_SetOrientation(LCD_REVERSE_LANDSCAPE);
    
    //LIB2D_Init();
    //LIB2D_Print("CPU %uMHz\n", SystemCoreClock/1000000);

    printf("Hello\n");
    puts("put string");
    
    while (1)
    {
        LED1_TOGGLE;
        DelayMs(200);
    }

    return 0;
}
