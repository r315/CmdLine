#ifndef _board_h_
#define _board_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdout.h>
#include "main.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern StdOut vcp;
extern StdOut aux_uart;

#define DelayMs(_X) HAL_Delay(_X)

#ifdef __cplusplus
}
#endif

#endif /* _board_h_ */
