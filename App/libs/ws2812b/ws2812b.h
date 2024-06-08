#ifndef __WS2812_H__
#define __WS2812_H__

#ifdef __cplusplus
extern "C" {
#endif

void ws2812_init();
void ws2812_write(uint32_t, uint8_t*);

#ifdef __cplusplus
}
#endif

#endif 