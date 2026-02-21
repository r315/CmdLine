#include "board.h"
#include "serial.h"
#include "usbd_cdc_if.h"

#define UART_FUNCTION_NAME(a, b) UART##a##_##b
#define HANDLER_NAME(a) hs##a.bus

#define UART_FUNCTIONS(N) \
static inline int UART_FUNCTION_NAME(N, Available)(void){ return UART_Available(&HANDLER_NAME(N)); } \
static inline int UART_FUNCTION_NAME(N, readchar)(void){ char c; UART_Read(&HANDLER_NAME(N), (uint8_t*)&c, 1); return c; } \
static inline int UART_FUNCTION_NAME(N, read)(char *buf, int len){ return UART_Read(&HANDLER_NAME(N), (uint8_t*)buf, len); } \
static inline int UART_FUNCTION_NAME(N, writechar)(char c){ return UART_Write(&HANDLER_NAME(N), (uint8_t*)&c, 1);  } \
static inline int UART_FUNCTION_NAME(N, write)(const char *buf, int len){ return UART_Write(&HANDLER_NAME(N), (const uint8_t*)buf, len);  }

#define ASSIGN_UART_FUNCTIONS(I, N) \
I->ops.available = UART_FUNCTION_NAME(N, Available); \
I->ops.readchar = UART_FUNCTION_NAME(N, readchar); \
I->ops.read = UART_FUNCTION_NAME(N, read); \
I->ops.writechar = UART_FUNCTION_NAME(N, writechar); \
I->ops.write = UART_FUNCTION_NAME(N, write);

static serialport_t hs0;

/**
 * Uart0/1/3
 * */
UART_FUNCTIONS(0)
#if 0 // TODO: Fix, create internal structure for usb
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

static int SERIAL4_Write(const char *data, int len){
    uint32_t retries = 1000;
	while(retries--){
		if(	CDC_Transmit_FS((uint8_t *)data, len) == USBD_OK)
			return len;
	}
    return 0;
}

static int SERIAL4_WriteChar(char c){
    SERIAL4_Write(&c, 1);
    return c;
}

static int SERIAL4_ReadChar(void){
    serialbus_t *serial = &BOARD_SERIAL4_HANDLER.port;
    char c;
    while(!fifo_get(&serial->rxfifo, (uint8_t*)&c));
    return c;
}

static int SERIAL4_Read(char *dst, int len)
{
    serialbus_t *serial = &BOARD_SERIAL4_HANDLER.port;
    int count = len;

    while(count--) {
        while(!fifo_get(&serial->rxfifo, (uint8_t*)dst));
        dst++;
    }

    return len;
}

static inline int SERIAL4_Available(void){
    serialbus_t *serial = &BOARD_SERIAL4_HANDLER.port;
    return fifo_avail(&serial->rxfifo);
}
#endif
/**
 * API
 * */
void SERIAL_Config(serialport_t *hserial, int32_t nr, uint32_t config){

    if(hserial == NULL){
        return ;
    }

    switch(nr){
        case SERIAL0:
            ASSIGN_UART_FUNCTIONS(hserial, 0);
            hserial->bus.bus = UART_BUS0;
            break;
#if 0 // TODO: Fix
        case SERIAL4:
            hserial->serial.writechar = SERIAL4_WriteChar;
            hserial->serial.write = SERIAL4_Write;
            hserial->serial.readchar = SERIAL4_ReadChar;
            hserial->serial.read = SERIAL4_Read;
            hserial->serial.available = SERIAL4_Available;
            SERIAL4_Init();
#endif
        default:
            return;
    }

    serialbus_t *serialbus = &hserial->bus;

    serialbus->speed = SERIAL_CONFIG_GET_SPEED(config);
    serialbus->parity = SERIAL_CONFIG_GET_PARITY(config);
    serialbus->stopbit = SERIAL_CONFIG_GET_STOP(config);
    serialbus->datalength = SERIAL_CONFIG_GET_DATA(config);

    UART_Init(serialbus);
}


void SERIAL_Init(void)
{
    SERIAL_Config(&hs0, SERIAL0,  SERIAL_DATA_8B | SERIAL_PARITY_NONE | SERIAL_STOP_1B | SERIAL_SPEED_115200);
    //SERIAL_Config(NULL, SERIAL4, 0);
}

serialops_t *SERIAL_GetSerialOps(int32_t nr)
{
    /* switch(nr){
        case SERIAL0:
        case SERIAL1:
            return ;

        default:
            break;
    } */

    return &hs0.ops;
}