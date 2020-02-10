
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdout.h>

#include "board.h"
#include "fifo.h"


#define VCOM_FIFO_SIZE 512

static fifo_t txfifo;
static fifo_t rxfifo;
static uint8_t rx;

void vc_init(void){
    rxfifo.size = VCOM_FIFO_SIZE;
	txfifo.size = VCOM_FIFO_SIZE;
	fifo_init(&txfifo);
	fifo_init(&rxfifo);
    fifo_flush(&txfifo);
	fifo_flush(&rxfifo);

    HAL_UART_Receive_IT(&huart2, &rx, 1);    
}   

//------------------------------------------
void vc_putchar(char c){
    HAL_UART_Transmit(&huart2, (uint8_t*)&c, 1, 1000);
}

void vc_puts(const char* str){
    //uint32_t len = strlen(str);
	//HAL_UART_Transmit_DMA(&huart2, (uint8_t*)str, len);
	while(*str)
		vc_putchar(*str++);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
	if(huart == &huart2){
		fifo_put(&rxfifo, rx);
		HAL_UART_Receive_IT(&huart2, &rx, 1);
		return;
	}

	((Array*)(aux_uart.user_ctx))->empty = 0;
	HAL_UART_Receive_IT(&huart1, ((Array*)(aux_uart.user_ctx))->data, ((Array*)(aux_uart.user_ctx))->len);
}
// ------------------------------------------
uint8_t vc_kbhit(void){
    return fifo_avail(&rxfifo);
}

char vc_getchar(void){
    char c;
    while(!fifo_get(&rxfifo, (uint8_t*)&c));
    return c;
}

uint8_t vc_getCharNonBlocking(char *c){
   return fifo_get(&rxfifo, (uint8_t*)c);
}

StdOut vcp = {
    .init = vc_init,
    .xgetchar = vc_getchar,
    .xputchar = vc_putchar,
    .xputs = vc_puts,
    .getCharNonBlocking = vc_getCharNonBlocking,
    .kbhit = vc_kbhit
};

/**
 *
 */
void aux_init(void){
	HAL_UART_Receive_IT(&huart1, ((Array*)(aux_uart.user_ctx))->data, ((Array*)(aux_uart.user_ctx))->len);
	((Array*)(aux_uart.user_ctx))->empty = 1;
}

void aux_putchar(char c){
	HAL_UART_Transmit(&huart1, (uint8_t*)&c, 1, 1000);
}

void aux_puts(const char* str){
	while(*str)
		aux_putchar(*str++);
}

char aux_getchar(void){
	uint8_t rx;
	HAL_UART_Receive(&huart1, &rx, 1, 1000);
	return (char)rx;
}

uint8_t aux_getCharNonBlocking(char *c){
	return 0;
}

uint8_t aux_kbhit(void){
	return !((Array*)(aux_uart.user_ctx))->empty;
}

StdOut aux_uart = {
	.init = aux_init,
	.xgetchar = aux_getchar,
	.xputchar = aux_putchar,
	.xputs = aux_puts,
	.getCharNonBlocking = aux_getCharNonBlocking,
	.kbhit = aux_kbhit
};
