
#include <stdint.h>
#include <stdio.h>
#include "board.h"
#include "app.h"

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
    board_init();

    App();

    while(1)
    {
    }
}

