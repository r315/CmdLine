#ifndef _tft_h_
#define _tft_h_

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tftdrv{
    void (*init)(void *spi);
    void (*clear)(uint16_t color);
    void (*writeArea)(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *data);
    void (*fillRect)(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
    void (*rect)(uint16_t x, uint16_t y,  uint16_t w, uint16_t h, uint16_t color);
    void (*hLine)(uint16_t x, uint16_t y, uint16_t width, uint16_t color);
    void (*vLine)(uint16_t x, uint16_t y, uint16_t height, uint16_t color);
    void (*line)(uint16_t x1, uint16_t y1,  uint16_t x2, uint16_t y2, uint16_t color);
    void (*pixel)(uint16_t x, uint16_t y, uint16_t color);
    void (*scroll)(uint16_t sc);
    void (*rotation)(uint8_t m);
    uint16_t (*width)(void);
    uint16_t (*height)(void);
    void (*backlight)(uint8_t level);
    void (*copy)(uint16_t sx, uint16_t sy, uint16_t sw, uint16_t sh, uint16_t dx, uint16_t dy, uint16_t dw, uint16_t dh);
}tftdrv_t;


#ifdef __cplusplus
}
#endif

#endif
