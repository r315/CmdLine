#include "board.h"
#include "serial.h"

#define UART_FUNCTION_NAME(a, b) UART##a##_##b
#define HANDLER_NAME(a) serial##a##_handler

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


static serialhandler_t serial0_handler, serial1_handler;

/**
 * Uart0/1/3
 * */
UART_FUNCTIONS(0)
UART_FUNCTIONS(1)

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
            hserial->port.bus = UART_BUS2;
            break;

		case SERIAL1:
            ASSIGN_UART_FUNCTIONS(hserial, 1);
            hserial->port.bus = UART_BUS1;
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

void SERIAL_Init(void)
{
    SERIAL_Config(&serial0_handler, SERIAL0 | SERIAL_DATA_8B | SERIAL_PARITY_NONE | SERIAL_STOP_1B | SERIAL_SPEED_115200);
    SERIAL_Config(&serial1_handler, SERIAL1 | SERIAL_DATA_8B | SERIAL_PARITY_NONE | SERIAL_STOP_1B | SERIAL_SPEED_115200);
}

stdout_t *SERIAL_GetStdout(uint8_t nr)
{
    return nr == SERIAL0 ? &serial0_handler.out :  &serial1_handler.out;
}