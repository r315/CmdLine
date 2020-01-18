#ifndef _usbserial_h_
#define _usbserial_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "type.h"
#include "fifo.h"

// data structure for GET_LINE_CODING / SET_LINE_CODING class requests
typedef struct {
	U32		dwDTERate;
	U8		bCharFormat;
	U8		bParityType;
	U8		bDataBits;
} TLineCoding;

void USBSERIAL_Init(fifo_t *tx, fifo_t *rx);

#ifdef __cplusplus
}
#endif
#endif