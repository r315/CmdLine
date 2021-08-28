#ifndef _board_h_
#define _board_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdout.h>
#include "main.h"
#include "pwm.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern StdOut uart;
extern StdOut uart_aux;

// LED Pin PB3 (Arduino D13)
#define LED_ON      HAL_GPIO_Write(LD3_GPIO_Port, LD3_Pin, GPIO_SET)
#define LED_OFF     HAL_GPIO_Write(LD3_GPIO_Port, LD3_Pin, GPIO_RESET)

#define DelayMs(_X) HAL_Delay(_X)


#ifdef __cplusplus
}
#endif

#endif /* _board_h_ */
