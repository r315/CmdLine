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
#include "tone_at32f4xx.h"
#include "pwm.h"

// TODO add ifdef to select proper include
#ifdef BOARD_PWRKT
//#include "ili9341.h"
//#include "st7735.h"
#include "st7789.h"
#endif


#define SET_BIT(REG, BIT)       ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)     ((REG) &= ~(BIT))

#ifdef BOARD_415DK
#define TFT_DRV_ST7735
#define LED_PIN         PA_8
#define LED1_PIN_INIT   GPIO_Config(LED_PIN, GPO_LS);

#define LED1_OFF        GPIO_Write(LED_PIN, GPIO_PIN_HIGH)
#define LED1_ON         GPIO_Write(LED_PIN, GPIO_PIN_LOW)
#define LED1_TOGGLE     GPIO_Toggle(LED_PIN)

#define I2C_MAX_ITF     4

#define SPI_FREQ        18000 //kHz

/**
* @brief Lcd Pin configuration:
**/
#define LCD_CS      PA_4
#define LCD_CD      PA_6
#define LCD_RST     -1
#define LCD_BKL     PA_3
#define LCD_SCLK    PA_5
#define LCD_DI      PA_7
#elif defined(BOARD_PWRKT)

#define SPI_FREQ        18000 //kHz
#define SPI_BUSX        SPI_BUS1
#define SPI_CS_PIN      PB_12
#define I2C_MAX_ITF     4

#define TFT_DRV_ST7789

#define LED_PIN         PA_8
#define LED1_PIN_INIT   GPIO_Config(LED_PIN, GPO_LS);

/**
* @brief Lcd Pin configuration:
**/
#define LCD_CS      SPI_CS_PIN
#define LCD_CD      PA_15
#define LCD_RST     -1
#define LCD_BKL     PB_2
#define LCD_SCLK    PB_13
#define LCD_DI      PB_15
#else
#error "No board defined"
#endif

#ifdef ENABLE_TFT_DISPLAY

#ifdef TFT_DRV_ST7735
#define TFT_W   128
#define TFT_H   160
#elif defined(TFT_DRV_ILI9341)
#define TFT_W   240
#define TFT_H   320
#elif defined(TFT_DRV_ST7789)
#define TFT_W   240
#define TFT_H   240
#else
#error "No display driver specified"
#endif

//extern const drvlcd_t *drvlcd;
#endif /* ENABLE_TFT_DISPLAY */


void BOARD_Init(void);
void BOARD_LCD_Init(void);
void BOARD_PWM_Init(pwmchip_t *pwmchip);

void DelayMs(uint32_t ms);
uint32_t ElapsedTicks(uint32_t start_ticks);
uint32_t GetTick(void);
void SW_Reset(void);
void __debugbreak(void);

#ifdef __cplusplus
}
#endif

#endif