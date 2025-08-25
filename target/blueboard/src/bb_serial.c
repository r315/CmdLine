#include "board.h"
#include "serial.h"
#include "usbserial.h"


#define UART_FUNCTION_NAME(a, b) UART##a##_##b
#define HANDLER_NAME(a) hs##a

#define UART_FUNCTIONS(N) \
static inline int UART_FUNCTION_NAME(N, Available)(void){ return UART_Available(&HANDLER_NAME(N).port); } \
static inline int UART_FUNCTION_NAME(N, readchar)(void){ char c; UART_Read(&HANDLER_NAME(N).port, (uint8_t*)&c, 1); return c; } \
static inline int UART_FUNCTION_NAME(N, read)(char *buf, int len){ return UART_Read(&HANDLER_NAME(N).port, (uint8_t*)buf, len); } \
static inline int UART_FUNCTION_NAME(N, writechar)(char c){ return UART_Write(&HANDLER_NAME(N).port, (uint8_t*)&c, 1);  } \
static inline int UART_FUNCTION_NAME(N, write)(const char *buf, int len){ return UART_Write(&HANDLER_NAME(N).port, (const uint8_t*)buf, len);  }

#define ASSIGN_UART_FUNCTIONS(I, N) \
I->serial.available = UART_FUNCTION_NAME(N, Available); \
I->serial.readchar = UART_FUNCTION_NAME(N, readchar); \
I->serial.read = UART_FUNCTION_NAME(N, read); \
I->serial.writechar = UART_FUNCTION_NAME(N, writechar); \
I->serial.write = UART_FUNCTION_NAME(N, write);

#define BOARD_SERIAL0                   (&hs0.out)
#define BOARD_SERIAL1                   (&hs1.out)
#define BOARD_SERIAL3                   (&hs1.out)
#define BOARD_SERIAL4                   (&hs4.out)

static serialport_t hs0, hs1, hs3, hs4;

/**
 * Uart0/1/3
 * */
UART_FUNCTIONS(0)
UART_FUNCTIONS(1)
UART_FUNCTIONS(3)

#if 0 // TODO: Fix
/**
 * virtual com port
 * */
static void SERIAL4_Init(void){
    serialbus_t *serial = &hs4.port;

	fifo_init(&serial->txfifo);
	fifo_init(&serial->rxfifo);

	USBSERIAL_Init(&serial->txfifo, &serial->rxfifo);

    DelayMs(1500);
    fifo_flush(&serial->txfifo);
	fifo_flush(&serial->rxfifo);
}

static int SERIAL4_WriteChar(char c){
    serialbus_t *serial = &hs4.port;
    while(!fifo_put(&serial->txfifo, c));
    return c;
}

static int SERIAL4_Write(const char *str, int len){
    serialbus_t *serial = &hs4.port;
    int count = len;

    while(count--){
        while(!fifo_put(&serial->txfifo, *str));
        str++;
    }

    return len;
}

static int SERIAL4_ReadChar(void){
    serialbus_t *serial = &hs4.port;
    uint8_t c;
    while(!fifo_get(&serial->rxfifo, &c));
    return c;
}

static int SERIAL4_Read(char *dst, int len)
{
    serialbus_t *serial = &hs4.port;
    int count = len;

    while(count--) {
        while(!fifo_get(&serial->rxfifo, (uint8_t*)dst));
        dst++;
    }

    return len;
}

static int SERIAL4_Available(void){
    serialbus_t *serial = &hs4.port;
    return fifo_avail(&serial->rxfifo);
}
#endif
/**
 * API
 * */
void SERIAL_Config(serialport_t *hserial, uint32_t config){

    if(hserial == NULL){
        return ;
    }

    switch(SERIAL_CONFIG_GET_NUM(config)){
        case SERIAL0: // Conflicts with LCD on BB
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
#if 0 // TODO: Fix, create internal data structor for usb
        case SERIAL4:
            hserial->serial.available = SERIAL4_Available;
            hserial->serial.writechar = SERIAL4_WriteChar;
            hserial->serial.write = SERIAL4_Write;
            hserial->serial.readchar = SERIAL4_ReadChar;
            hserial->serial.read = SERIAL4_Read;
            SERIAL4_Init();
#endif
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

void SERIAL_Init(void){
    //SERIAL_Config(&hs0, SERIAL0 | SERIAL_DATA_8B | SERIAL_PARITY_NONE | SERIAL_STOP_1B | SERIAL_SPEED_115200);
    SERIAL_Config(&hs1, SERIAL1 | SERIAL_DATA_8B | SERIAL_PARITY_NONE | SERIAL_STOP_1B | SERIAL_SPEED_115200);
    SERIAL_Config(&hs3, SERIAL3 | SERIAL_DATA_8B | SERIAL_PARITY_NONE | SERIAL_STOP_1B | SERIAL_SPEED_115200);
    SERIAL_Config(&hs4, SERIAL4);
}

serialops_t *SERIAL_GetSerialOps(int32_t nr){

    switch(nr){
        case SERIAL1:
            return &hs1.serial;
        case SERIAL3:
            return &hs3.serial;
        default:
            break;
    }

    return &hs4.serial;
}

serialbus_t *SERIAL_GetSerialBus(int32_t nr){
    return &hs3.port;
}

int _write(int file, char *ptr, int len)
{
    (void)file;

	UART_Write(&hs1.port, (uint8_t*)ptr, len);

	return len;
}
