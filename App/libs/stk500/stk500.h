/*
** Made by fabien le mentec <texane@gmail.com>
**
** Started on  Sun Nov 15 12:35:04 2009 texane
** Last update Tue Nov 17 18:17:46 2009 texane
*/

#ifndef STK500_H_INCLUDED
#define STK500_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "serial.h"

#define STK500_BUF_MAX_SIZE 512

typedef enum
{
    STK500_ERROR_SUCCESS = 0,
    STK500_ERROR_MORE_DATA,
    STK500_ERROR_INVALID_CMD,
    STK500_ERROR_FAILURE,
    STK500_ERROR_MAX,
} stk500_error_t;

void stk500_setup(serialops_t *sp);
void stk500_loop(void);
stk500_error_t stk500_process(uint8_t *, uint32_t, uint32_t *);
void stk500_timeout(uint8_t *, uint32_t, uint32_t *);

#ifdef __cplusplus
}
#endif

#endif /* ! STK500_H_INCLUDED */
