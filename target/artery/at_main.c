#include <stdio.h>
#include "board.h"
#include "app.h"
#include "lib2d.h"
#include "liblcd.h"

int main(void)
{
    BOARD_Init();

    App();
    
    while (1)
    {
        LED1_TOGGLE;
        DelayMs(200);
    }

    return 0;
}
