#include "board.h"


void USART1_IRQHandler(void){
    UART_IRQHandler(&BOARD_SERIAL0_HANDLER.port);
}

void USART2_IRQHandler(void){
    //UART_IRQHandler(&huart2);
}

void USART3_IRQHandler(void){
    //UART_IRQHandler(&huart3);
}