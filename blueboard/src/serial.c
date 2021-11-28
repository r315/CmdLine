#include "board.h"
#include "serial.h"
#include "usbserial.h"

#define UART_FUNCTION_NAME(a, b) UART##a##_##b
#define HANDLER_NAME(a) BOARD_SERIAL##a##_HANDLER

#define UART_FUNCTIONS(N) \
static inline void UART_FUNCTION_NAME(N, Init)(void){ UART_Init(&HANDLER_NAME(N).port); } \
static inline void UART_FUNCTION_NAME(N, PutChar)(char c){ UART_PutChar(&HANDLER_NAME(N).port, c); } \
static inline void UART_FUNCTION_NAME(N, Puts)(const char* str){ UART_Puts(&HANDLER_NAME(N).port, str); } \
static inline char UART_FUNCTION_NAME(N, GetChar)(void){ return UART_GetChar(&HANDLER_NAME(N).port); } \
static inline uint8_t UART_FUNCTION_NAME(N, GetCharNonBlocking)(char *c){ return UART_GetCharNonBlocking(&HANDLER_NAME(N).port, c); } \
static inline uint8_t UART_FUNCTION_NAME(N, Kbhit)(void){ return UART_Kbhit(&HANDLER_NAME(N).port); }


#define ASSIGN_UART_FUNCTIONS(I, N) \
I->out.init = UART_FUNCTION_NAME(N, Init); \
I->out.xputchar = UART_FUNCTION_NAME(N, PutChar); \
I->out.xputs = UART_FUNCTION_NAME(N, Puts); \
I->out.xgetchar = UART_FUNCTION_NAME(N, GetChar); \
I->out.getCharNonBlocking = UART_FUNCTION_NAME(N, GetCharNonBlocking); \
I->out.kbhit = UART_FUNCTION_NAME(N, Kbhit)


serialhandler_t BOARD_SERIAL_HANDLERS;

/**
 * Uart0/1/3
 * */
UART_FUNCTIONS(0)
UART_FUNCTIONS(1)
UART_FUNCTIONS(3)

/**
 * virtual com port
 * */
static inline void SERIAL4_Init(void){
    serialbus_t *serial = &BOARD_SERIAL4_HANDLER.port;
    
	fifo_init(&serial->txfifo);
	fifo_init(&serial->rxfifo);

	USBSERIAL_Init(&serial->txfifo, &serial->rxfifo);
    
    DelayMs(1500);
    fifo_flush(&serial->txfifo);
	fifo_flush(&serial->rxfifo);
}

static inline void SERIAL4_PutChar(char c){
    serialbus_t *serial = &BOARD_SERIAL4_HANDLER.port;
    while(!fifo_put(&serial->txfifo, c));
}

static inline void SERIAL4_Puts(const char* str){
    serialbus_t *serial = &BOARD_SERIAL4_HANDLER.port;
    while(*str){
        while(!fifo_put(&serial->txfifo, *str));
        str++;
    }     
}

static inline char SERIAL4_GetChar(void){
    serialbus_t *serial = &BOARD_SERIAL4_HANDLER.port;
    char c;
    while(!fifo_get(&serial->rxfifo, (uint8_t*)&c));
    return c;
}

static inline uint8_t SERIAL4_GetCharNonBlocking(char *c){
    serialbus_t *serial = &BOARD_SERIAL4_HANDLER.port;
    return fifo_get(&serial->rxfifo, (uint8_t*)c);
}

static inline uint8_t SERIAL4_Kbhit(void){
    serialbus_t *serial = &BOARD_SERIAL4_HANDLER.port;
    return fifo_avail(&serial->rxfifo);
}

/**
 * API
 * */
void SERIAL_Config(serialhandler_t *hserial, uint32_t config){

    if(hserial == NULL){
        return ;
    }

    switch(SERIAL_CONFIG_GET_NUM(config)){
        case SERIAL0:
            ASSIGN_UART_FUNCTIONS(hserial, 0);
            hserial->port.bus = UART_BUS0;
            break;

        case SERIAL1:
            ASSIGN_UART_FUNCTIONS(hserial, 1);
            hserial->port.bus = UART_BUS1;
            break;

        case SERIAL2:
            break;

        case SERIAL3:
            ASSIGN_UART_FUNCTIONS(hserial, 3);
            hserial->port.bus = UART_BUS3;
            break;

        case SERIAL4:
            hserial->out.init = SERIAL4_Init;
            hserial->out.xputchar = SERIAL4_PutChar;
            hserial->out.xputs = SERIAL4_Puts;
            hserial->out.xgetchar = SERIAL4_GetChar;
            hserial->out.getCharNonBlocking = SERIAL4_GetCharNonBlocking;
            hserial->out.kbhit = SERIAL4_Kbhit;
            break;

        default:
            return;
    }

    serialbus_t *port = &hserial->port;
    
    port->speed = SERIAL_CONFIG_GET_SPEED(config);
    port->parity = SERIAL_CONFIG_GET_PARITY(config);
    port->stopbit = SERIAL_CONFIG_GET_STOP(config);
    port->datalength = SERIAL_CONFIG_GET_DATA(config);
    
    hserial->out.init();
}