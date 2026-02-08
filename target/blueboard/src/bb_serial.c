#include "board.h"
#include "serial.h"
#include "usbserial.h"


#define UART_FUNCTION_NAME(a, b) UART##a##_##b
#define HANDLER_NAME(a) hs##a

#define UART_FUNCTIONS(N) \
static inline int UART_FUNCTION_NAME(N, Available)(void){ return UART_Available(&HANDLER_NAME(N).bus); } \
static inline int UART_FUNCTION_NAME(N, readchar)(void){ char c; UART_Read(&HANDLER_NAME(N).bus, (uint8_t*)&c, 1); return c; } \
static inline int UART_FUNCTION_NAME(N, read)(char *buf, int len){ return UART_Read(&HANDLER_NAME(N).bus, (uint8_t*)buf, len); } \
static inline int UART_FUNCTION_NAME(N, writechar)(char c){ return UART_Write(&HANDLER_NAME(N).bus, (uint8_t*)&c, 1);  } \
static inline int UART_FUNCTION_NAME(N, write)(const char *buf, int len){ return UART_Write(&HANDLER_NAME(N).bus, (const uint8_t*)buf, len);  }

#define ASSIGN_UART_FUNCTIONS(I, N) \
I->ops.available = UART_FUNCTION_NAME(N, Available); \
I->ops.readchar = UART_FUNCTION_NAME(N, readchar); \
I->ops.read = UART_FUNCTION_NAME(N, read); \
I->ops.writechar = UART_FUNCTION_NAME(N, writechar); \
I->ops.write = UART_FUNCTION_NAME(N, write);

static serialport_t hs0, hs1, hs3;
static fifo_t txfifo4, rxfifo4;
static serialops_t ops4;

/**
 * Uart0/1/3
 * */
UART_FUNCTIONS(0)
UART_FUNCTIONS(1)
UART_FUNCTIONS(3)

#if 1 // TODO: Fix
/**
 * virtual com port
 * */
static void SERIAL4_Init(void){
	fifo_init(&txfifo4);
	fifo_init(&rxfifo4);

	USBSERIAL_Init(&txfifo4, &rxfifo4);

    DelayMs(1500);
    fifo_flush(&txfifo4);
	fifo_flush(&rxfifo4);
}

static int SERIAL4_WriteChar(char c){
    while(!fifo_put(&txfifo4, c));
    return c;
}

static int SERIAL4_Write(const char *str, int len){
    int count = len;

    while(count--){
        while(!fifo_put(&txfifo4, *str));
        str++;
    }

    return len;
}

static int SERIAL4_ReadChar(void){
    uint8_t c;
    while(!fifo_get(&rxfifo4, &c));
    return c;
}

static int SERIAL4_Read(char *dst, int len)
{
    int count = len;

    while(count--) {
        while(!fifo_get(&rxfifo4, (uint8_t*)dst));
        dst++;
    }

    return len;
}

static int SERIAL4_Available(void){
    return fifo_avail(&rxfifo4);
}
#endif
/**
 * API
 * */
void SERIAL_Config(serialport_t *hserial, int32_t nr, uint32_t config){

    if(hserial == NULL && nr != SERIAL4){
        return ;
    }

    switch(nr){
        case SERIAL0: // Conflicts with LCD on BB
            ASSIGN_UART_FUNCTIONS(hserial, 0);
            hserial->bus.bus = UART_BUS0;
            break;

        case SERIAL1:
            ASSIGN_UART_FUNCTIONS(hserial, 1);
            hserial->bus.bus = UART_BUS1;
            break;

        case SERIAL2:
            break;

        case SERIAL3:
            ASSIGN_UART_FUNCTIONS(hserial, 3);
            hserial->bus.bus = UART_BUS3;
            break;

        case SERIAL4:
            ops4.available = SERIAL4_Available;
            ops4.writechar = SERIAL4_WriteChar;
            ops4.write = SERIAL4_Write;
            ops4.readchar = SERIAL4_ReadChar;
            ops4.read = SERIAL4_Read;
            SERIAL4_Init();

        default:
            return;
    }

    serialbus_t *bus = &hserial->bus;

    bus->speed = SERIAL_CONFIG_GET_SPEED(config);
    bus->parity = SERIAL_CONFIG_GET_PARITY(config);
    bus->stopbit = SERIAL_CONFIG_GET_STOP(config);
    bus->datalength = SERIAL_CONFIG_GET_DATA(config);

    UART_Init(bus);
}

void SERIAL_Init(void){
    //SERIAL_Config(&hs0, SERIAL0 | SERIAL_DATA_8B | SERIAL_PARITY_NONE | SERIAL_STOP_1B | SERIAL_SPEED_115200);
    SERIAL_Config(&hs1, SERIAL1, SERIAL_DATA_8B | SERIAL_PARITY_NONE | SERIAL_STOP_1B | SERIAL_SPEED_115200);
    SERIAL_Config(&hs3, SERIAL3, SERIAL_DATA_8B | SERIAL_PARITY_NONE | SERIAL_STOP_1B | SERIAL_SPEED_115200);
    SERIAL_Config(NULL, SERIAL4, 0);
}

serialops_t *SERIAL_GetSerialOps(int32_t nr){

    switch(nr){
        case SERIAL1:
            return &hs1.ops;
        case SERIAL3:
            return &hs3.ops;
        default:
            return &ops4;
    }
}

serialbus_t *SERIAL_GetSerialBus(int32_t nr){
    switch(nr){
        case SERIAL1: return &hs1.bus;
        case SERIAL3: return &hs3.bus;
    }
    return NULL;
}

int _write(int file, char *ptr, int len)
{
    (void)file;

	UART_Write(&hs1.bus, (uint8_t*)ptr, len);

	return len;
}
