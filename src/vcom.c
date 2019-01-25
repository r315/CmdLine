
#include <stdint.h>
#include <stdio.h>
#include <consoleout.h>

#include "usbserial.h"
#include "strfunc.h"
#include "fifo.h"

#define VCOM_FIFO_SIZE 512

static fifo_t txfifo;
static fifo_t rxfifo;

void vc_init(void){
    rxfifo.size = VCOM_FIFO_SIZE;
	txfifo.size = VCOM_FIFO_SIZE;

	fifo_init(&txfifo);
	fifo_init(&rxfifo);
	USBSERIAL_Init(&txfifo, &rxfifo);
}   

void vc_putchar(char c){
    while(!fifo_put(&txfifo, c));
}

void vc_puts(const char* str){
    while(*str)
        vc_putchar(*str++);
}

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

ConsoleOut vcom = {
    .init = vc_init,
    .getchar = vc_getchar,
    .putchar = vc_putchar,
    .puts = vc_puts,
    .getCharNonBlocking = vc_getCharNonBlocking,
    .kbhit = vc_kbhit
};
