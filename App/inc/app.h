#ifndef _app_h_
#define _app_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct _Array{
	uint8_t *data;
	uint8_t len;
	uint8_t empty;
}Array;

void App(void);

int16_t map(int16_t x, int16_t in_min, int16_t in_max, int16_t out_min, int16_t out_max);

#ifdef __cplusplus
}
#endif

#endif