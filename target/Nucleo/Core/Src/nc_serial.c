#include "board.h"
#include "serial.h"

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

static serialport_t hs0, hs1;

/**
 * Uart0/1/3
 * */
UART_FUNCTIONS(0)
UART_FUNCTIONS(1)

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
            hserial->bus.bus = UART_BUS2;
            break;

		case SERIAL1:
            ASSIGN_UART_FUNCTIONS(hserial, 1);
            hserial->bus.bus = UART_BUS1;
            break;

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
    SERIAL_Config(&hs0, SERIAL0, SERIAL_DATA_8B | SERIAL_PARITY_NONE | SERIAL_STOP_1B | SERIAL_SPEED_115200);
    SERIAL_Config(&hs1, SERIAL1, SERIAL_DATA_8B | SERIAL_PARITY_NONE | SERIAL_STOP_1B | SERIAL_SPEED_115200);
}

serialops_t *SERIAL_GetSerialOps(int32_t nr)
{
    if(nr == -1 || nr == SERIAL0){
        return &hs0.ops;
    }

    return &hs1.ops;
}

int _write(int file, char *ptr, int len)
{
    (void)file;
	return hs0.ops.write((const char*)ptr, len);
}

int _read(int file, char *ptr, int len)
{
    (void)file;
    return hs0.ops.read(ptr, len);
}