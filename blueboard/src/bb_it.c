#include <stdint.h>
#include "board.h"


void UART0_IRQHandler(void){
	UART_IRQHandler(&BOARD_SERIAL0_HANDLER.port);
}

void UART1_IRQHandler(void){
	UART_IRQHandler(&BOARD_SERIAL1_HANDLER.port);
}

void UART2_IRQHandler(void){
	//UARTx_IRQHandler(&BOARD_SERIAL2_HANDLER);
}

void UART3_IRQHandler(void){
	UART_IRQHandler(&BOARD_SERIAL3_HANDLER.port);
}

void I2S_IRQHandler(void){
	I2S_DMA_IRQHandler(NULL);	
}

void DMA_IRQHandler(void){
	I2S_DMA_IRQHandler(NULL);
}