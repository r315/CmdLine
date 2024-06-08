#include "board.h"
#include "serial.h"
#include "usbd_cdc_if.h"

#define UART_FUNCTION_NAME(a, b) UART##a##_##b
#define HANDLER_NAME(a) BOARD_SERIAL##a##_HANDLER

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

serialport_t BOARD_SERIAL_HANDLERS;

/**
 * Uart0/1/3
 * */
UART_FUNCTIONS(0)

/**
 * virtual com port
 * */
static void SERIAL4_Init(void){
    serialbus_t *serial = &BOARD_SERIAL4_HANDLER.port;
    
	fifo_init(&serial->txfifo);
	fifo_init(&serial->rxfifo);
    
    DelayMs(1500);
    fifo_flush(&serial->txfifo);
	fifo_flush(&serial->rxfifo);

    USBSERIAL_Init(&serial->txfifo, &serial->rxfifo);
}

static int SERIAL4_WriteBytes(const uint8_t *data, int len){
    uint32_t retries = 1000;
	while(retries--){
		if(	CDC_Transmit_FS((uint8_t *)data, len) == USBD_OK)
			return len;
	}
    return 0;
}

static int SERIAL4_Write(uint8_t c){
    SERIAL4_WriteBytes(&c, 1);
    return c;
}

static int SERIAL4_Read(void){
    serialbus_t *serial = &BOARD_SERIAL4_HANDLER.port;
    char c;
    while(!fifo_get(&serial->rxfifo, (uint8_t*)&c));
    return c;
}

static int SERIAL4_ReadBytes(uint8_t *dst, int len)
{
    serialbus_t *serial = &BOARD_SERIAL4_HANDLER.port;
    int count = len;

    while(count--) {
        while(!fifo_get(&serial->rxfifo, dst)); 
        dst++; 
    }

    return len;
}

static inline int SERIAL4_Available(void){
    serialbus_t *serial = &BOARD_SERIAL4_HANDLER.port;
    return fifo_avail(&serial->rxfifo);
}

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
            hserial->port.bus = UART_BUS1;
            break;

        case SERIAL4:
            hserial->serial.write = SERIAL4_Write;
            hserial->serial.writeBytes = SERIAL4_WriteBytes;
            hserial->serial.read = SERIAL4_Read;
            hserial->serial.readBytes = SERIAL4_ReadBytes;
            hserial->serial.available = SERIAL4_Available;
            SERIAL4_Init();

        default:
            return;
    }

    serialbus_t *port = &hserial->port;
    
    port->speed = SERIAL_CONFIG_GET_SPEED(config);
    port->parity = SERIAL_CONFIG_GET_PARITY(config);
    port->stopbit = SERIAL_CONFIG_GET_STOP(config);
    port->datalength = SERIAL_CONFIG_GET_DATA(config);
    
    UART_Init(&hserial->port);
}


void SERIAL_Init(void)
{
    SERIAL_Config(&BOARD_SERIAL0_HANDLER, SERIAL0 | SERIAL_DATA_8B | SERIAL_PARITY_NONE | SERIAL_STOP_1B | SERIAL_SPEED_115200);
    SERIAL_Config(&BOARD_SERIAL4_HANDLER, SERIAL4);
}

serialops_t *SERIAL_GetSerialOps(int32_t nr)
{
    switch(nr){
        case SERIAL0:
        case SERIAL1:
            return BOARD_SERIAL0;

        default:
            break;
    }
    
    return BOARD_SERIAL4;
}