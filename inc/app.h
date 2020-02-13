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

#ifdef __cplusplus
}
#endif

#endif