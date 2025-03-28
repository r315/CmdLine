#ifndef _board_h_
#define _board_h_


#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include "at32f4xx.h"
#include "spi.h"
#include "gpio.h"
#include "gpio_at32f4xx.h"
#include "ili9341.h"
//#include "st7735.h"
//#include "st7789.h"

#define SET_BIT(REG, BIT)       ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)     ((REG) &= ~(BIT))

#ifndef BOARD_415DK
#define BOARD_415DK
#endif

#ifdef ENABLE_TFT_DISPLAY
#ifdef TFT_ST7735S
#define TFT_W   128
#define TFT_H   160
#define SCREEN_OFFSET_X     0
#define SCREEN_OFFSET_Y     0
#elif defined(TFT_ILI9341)
#define TFT_W   240
#define TFT_H   320
#else
#error "No display driver specified"
#endif
/**
* @brief Lcd Pin configuration:
**/
#define LCD_CS      PA_4 //PB_12
#define LCD_CD      PA_6 //PB_13
#define LCD_RST     -1
#define LCD_BKL     PA_3 //PB_14
#define LCD_SCLK    PA_5 //PB_3
#define LCD_DI      PA_7 //PB_5
extern const drvlcd_t *drvlcd;
#endif /* ENABLE_TFT_DISPLAY */

#define LED_PIN         PA_8
#define LED1_PIN_INIT   GPIO_Config(LED_PIN, GPO_LS);

#define LED1_OFF        GPIO_Write(LED_PIN, GPIO_PIN_HIGH)
#define LED1_ON         GPIO_Write(LED_PIN, GPIO_PIN_LOW)
#define LED1_TOGGLE     GPIO_Toggle(LED_PIN)

#define I2C_MAX_ITF     4

#define SPI_FREQ        18000 //kHz
extern spibus_t spibus;

void BOARD_Init(void);
void BOARD_LCD_Init(void);
void DelayMs(uint32_t ms);
uint32_t ElapsedTicks(uint32_t start_ticks);
uint32_t GetTick(void);
void SW_Reset(void);
void __debugbreak(void);

#ifdef __cplusplus
}
#endif

#endif