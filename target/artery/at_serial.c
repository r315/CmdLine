#include "board.h"
#include "serial.h"

#define UART_FUNCTION_NAME(a, b) UART##a##_##b
#define HANDLER_NAME(a) serial##a##_handler

#define UART_FUNCTIONS(N) \
static inline int UART_FUNCTION_NAME(N, Available)(void){ return UART_Available(&HANDLER_NAME(N).port); } \
static inline int UART_FUNCTION_NAME(N, read)(void){ uint8_t c; UART_Read(&HANDLER_NAME(N).port, &c, 1); return c; } \
static inline int UART_FUNCTION_NAME(N, readBytes)(uint8_t *buf, int len){ return UART_Read(&HANDLER_NAME(N).port, buf, len); } \
static inline int UART_FUNCTION_NAME(N, write)(uint8_t c){ return UART_Write(&HANDLER_NAME(N).port, &c, 1);  } \
static inline int UART_FUNCTION_NAME(N, writeBytes)(const uint8_t *buf, int len){ return UART_Write(&HANDLER_NAME(N).port, buf, len);  }

#define ASSIGN_UART_FUNCTIONS(I, N) \
I->serial.available = UART_FUNCTION_NAME(N, Available); \
I->serial.read = UART_FUNCTION_NAME(N, read); \
I->serial.readBytes = UART_FUNCTION_NAME(N, readBytes); \
I->serial.write = UART_FUNCTION_NAME(N, write); \
I->serial.writeBytes = UART_FUNCTION_NAME(N, writeBytes);

static serialport_t serial0_handler;

/**
 * Uart0/1/3
 * */
UART_FUNCTIONS(0)

/**
 * API
 * */
void SERIAL_Config(serialport_t *hserial, uint32_t config){

    if(hserial == NULL){
        return ;
    }

    switch(SERIAL_CONFIG_GET_NUM(config)){
        case SERIAL0:
            ASSIGN_UART_FUNCTIONS(hserial, 0);
            hserial->port.bus = UART_BUS0;
            break;

        default:
            return;
    }

    serialbus_t *port = &hserial->port;
    
    port->speed = SERIAL_CONFIG_GET_SPEED(config);
    port->parity = SERIAL_CONFIG_GET_PARITY(config);
    port->stopbit = SERIAL_CONFIG_GET_STOP(config);
    port->datalength = SERIAL_CONFIG_GET_DATA(config);
    
    UART_Init(&hserial->port);;
}

void SERIAL_Init(void)
{
    SERIAL_Config(&serial0_handler, SERIAL0 | SERIAL_DATA_8B | SERIAL_PARITY_NONE | SERIAL_STOP_1B | SERIAL_SPEED_115200);
}

serialops_t *SERIAL_GetSerialOps(int32_t nr)
{
    (void)nr;
    return &serial0_handler.serial;
}

int _write(int file, char *ptr, int len){
    (void)file;
    serial0_handler.serial.writeBytes((uint8_t*)ptr, len);
    return len;
}
