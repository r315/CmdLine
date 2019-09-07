
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdout.h>

#include "board.h"
#include "fifo.h"


#define VCOM_FIFO_SIZE 512

static fifo_t txfifo;
static fifo_t rxfifo;
extern UART_HandleTypeDef huart2;
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
	//HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
	fifo_put(&rxfifo, rx);
    HAL_UART_Receive_IT(&huart2, &rx, 1);
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
