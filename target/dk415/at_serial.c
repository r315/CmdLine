#include "board.h"
#include "serial.h"
#include "stdinout.h"

#define UART_FUNCTION_NAME(a, b) UART##a##_##b
#define HANDLER_NAME(a) hs##a.bus

#define UART_FUNCTIONS(N) \
static inline int UART_FUNCTION_NAME(N, Available)(void){ return UART_Available(&HANDLER_NAME(N)); } \
static inline int UART_FUNCTION_NAME(N, readchar)(void){ char c; UART_Read(&HANDLER_NAME(N), (uint8_t*)&c, 1); return c; } \
static inline int UART_FUNCTION_NAME(N, read)(char *buf, int len){ return UART_Read(&HANDLER_NAME(N), (uint8_t*)buf, len); } \
static inline int UART_FUNCTION_NAME(N, writechar)(char c){ return UART_Write(&HANDLER_NAME(N), (uint8_t*)&c, 1);  } \
static inline int UART_FUNCTION_NAME(N, write)(const char *buf, int len){ return UART_Write(&HANDLER_NAME(N), (const uint8_t*)buf, len);  }

#define ASSIGN_SERIAL_OPS(S, N) \
    S.available = UART_FUNCTION_NAME(N, Available); \
    S.readchar = UART_FUNCTION_NAME(N, readchar); \
    S.read = UART_FUNCTION_NAME(N, read); \
    S.writechar = UART_FUNCTION_NAME(N, writechar); \
    S.write = UART_FUNCTION_NAME(N, write);

static serialport_t hs0;

stdinout_t *host_serial;

/**
 * Uart0/1/3
 * */
UART_FUNCTIONS(0)


extern void usb_pwrkt_init(void);
extern void usb_pwrkt_connect(void);
extern void usb_pwrkt_disconnect(void);

extern int usb_pwrkt_available(void);
extern char usb_pwrkt_peek(int idx);
extern int usb_pwrkt_receive(char *buffer, int length);
extern int usb_pwrkt_send(const char *data, int length);

extern int usb_pwrkt_is_connected(void);
extern void usb_pwrkt_handler(void);

#ifdef BOARD_PWRKT
void USBOTG_IRQHandler(void)
{
    usb_pwrkt_handler();
}
#endif
/**
 * API
 * */
void SERIAL_Config(serialport_t *hserial, int32_t nr, uint32_t config){

    if(hserial == NULL){
        return ;
    }

    #ifdef BOARD_PWRKT
    (void)nr;

    hserial->ops.available = usb_pwrkt_available;
    hserial->ops.read = usb_pwrkt_receive;
    hserial->ops.write = usb_pwrkt_send;
    usb_pwrkt_init();

    #else
    switch(nr){
        case SERIAL0:
            ASSIGN_SERIAL_OPS(hserial->ops, 0);
            hserial->bus.bus = UART_BUS0;
            host_serial = (stdinout_t*)&hserial->ops;
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
    #endif
}

void SERIAL_Init(void)
{
#ifdef BOARD_PWRKT
    SERIAL_Config(&hs0, SERIAL0, 0);
#else
    SERIAL_Config(&hs0, SERIAL0, SERIAL_DATA_8B | SERIAL_PARITY_NONE | SERIAL_STOP_1B | SERIAL_SPEED_115200);

    GPIO_Config(PA_9, GPIO_USART1_TX);
    GPIO_Config(PA_10, GPIO_USART1_RX);
#endif
    //stdinout = (stdinout_t*)&serial0_handler.ops;
}

serialops_t *SERIAL_GetSerialOps(int32_t nr)
{
    (void)nr;
    return &hs0.ops;
}

int _write(int file, char *ptr, int len)
{
    (void)file;
	return hs0.ops.write((const char*)ptr, len);
}
