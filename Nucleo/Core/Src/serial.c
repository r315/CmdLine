
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdout.h>

#include "board.h"
#include "fifo.h"
#include "app.h"

#define VC_USART		USART2

extern UART_HandleTypeDef huart1;
static USART_TypeDef *usart;

static fifo_t txfifo;
static fifo_t rxfifo;

static void vc_init(void){
	fifo_init(&txfifo);
	fifo_init(&rxfifo);
    fifo_flush(&txfifo);
	fifo_flush(&rxfifo);

	usart = VC_USART;

	usart->CR1 |= USART_CR1_RXNEIE;

	HAL_NVIC_EnableIRQ(USART2_IRQn);
}   

static void vc_putchar(char c){
    fifo_put(&txfifo, (uint8_t)c);
	SET_BIT(usart->CR1, USART_CR1_TXEIE);
}

static void vc_puts(const char* str){
    while(*str){
		fifo_put(&txfifo, *(uint8_t*)str++);
	}
	SET_BIT(usart->CR1, USART_CR1_TXEIE);
}

static uint8_t vc_kbhit(void){
    return fifo_avail(&rxfifo);
}

static char vc_getchar(void){
    char c;
    while(!fifo_get(&rxfifo, (uint8_t*)&c));
    return c;
}

static uint8_t vc_getCharNonBlocking(char *c){
   return fifo_get(&rxfifo, (uint8_t*)c);
}

StdOut uart = {
    .init = vc_init,
    .xgetchar = vc_getchar,
    .xputchar = vc_putchar,
    .xputs = vc_puts,
    .getCharNonBlocking = vc_getCharNonBlocking,
    .kbhit = vc_kbhit
};

void SERIAL_IRQHandler(void){
	
	uint32_t isrflags = usart->ISR;
	uint32_t cr1its = usart->CR1;

	/* If no error occurs */
	uint32_t errorflags = (isrflags & (uint32_t)(USART_ISR_PE |
			USART_ISR_FE | USART_ISR_ORE | USART_ISR_NE | USART_ISR_RTOF));

	if (errorflags == 0U){
		if (((isrflags & USART_ISR_RXNE) != 0U)	&& ((cr1its & USART_CR1_RXNEIE) != 0U))	{
			fifo_put(&rxfifo, (uint8_t)READ_REG(usart->RDR));
		}

		if (((isrflags & USART_ISR_TXE) != 0U)	&& ((cr1its & USART_CR1_TXEIE) != 0U)){
			if(fifo_get(&txfifo, (uint8_t*)&usart->TDR) == 0U){
				/* No data transmitted, disable TXE interrupt */
				CLEAR_BIT(usart->CR1, USART_CR1_TXEIE);
			}
		}
	}

	//((Array*)(uart_aux.user_ctx))->empty = 0;
	//HAL_UART_Receive_IT(&huart1, ((Array*)(uart_aux.user_ctx))->data, ((Array*)(uart_aux.user_ctx))->len);
}

/**
 * Auxiliary serial port
 */
static void aux_init(void){
	HAL_UART_Receive_IT(&huart1, ((Array*)(uart_aux.user_ctx))->data, ((Array*)(uart_aux.user_ctx))->len);
	((Array*)(uart_aux.user_ctx))->empty = 1;
}

static void aux_putchar(char c){
	HAL_UART_Transmit(&huart1, (uint8_t*)&c, 1, 1000);
}

static void aux_puts(const char* str){
	while(*str)
		aux_putchar(*str++);
}

static char aux_getchar(void){
	uint8_t rx;
	HAL_UART_Receive(&huart1, &rx, 1, 1000);
	return (char)rx;
}

static uint8_t aux_getCharNonBlocking(char *c){
	return 0;
}

static uint8_t aux_kbhit(void){
	return !((Array*)(uart_aux.user_ctx))->empty;
}

StdOut uart_aux = {
	.init = aux_init,
	.xgetchar = aux_getchar,
	.xputchar = aux_putchar,
	.xputs = aux_puts,
	.getCharNonBlocking = aux_getCharNonBlocking,
	.kbhit = aux_kbhit
};
