#ifndef _usbserial_h_
#define _usbserial_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "type.h"

// data structure holding fifo operations
typedef struct _Fifo_ops{
    uint8_t (*put) (uint8_t data);
    uint8_t (*get) (uint8_t *dst);
    int (*free) (void);
    int (*available) (void);
}Fifo_ops;

// data structure for GET_LINE_CODING / SET_LINE_CODING class requests
typedef struct {
	U32		dwDTERate;
	U8		bCharFormat;
	U8		bParityType;
	U8		bDataBits;
} TLineCoding;

void USBSERIAL_Init(Fifo_ops *fifo_ops);

#ifdef __cplusplus
}
#endif
#endif