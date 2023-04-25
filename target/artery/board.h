#ifndef _board_h_
#define _board_h_


#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include "at32f4xx.h"
#include "gpio.h"
#include "gpio_at32f4xx.h"
//#include "ili9341.h"
//#include "st7735.h"
#include "st7789.h"

#define SET_BIT(REG, BIT)       ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)     ((REG) &= ~(BIT))

#define BOARD_ARTERY

#define SPI_FREQ    18000 //kHz

#define LCD_IO_SET(port, pinmask) port->BSRE = pinmask
#define LCD_IO_RESET(port, pinmask) port->BRE = pinmask

#ifdef TFT_ST7735S
#define TFT_W   128
#define TFT_H   160
#define SCREEN_OFFSET_X     0
#define SCREEN_OFFSET_Y     0
#elif defined(TFT_ILI9341)
#define SCREEN_OFFSET_X     80
#define SCREEN_OFFSET_Y     48
#else
#define SCREEN_OFFSET_X     ((TFT_W - SCREEN_W) / 2)
#define SCREEN_OFFSET_Y     ((TFT_H - SCREEN_H) / 2)
#endif
/**
* @brief Lcd Pin configuration:
*       PA4  CS
*       PB1  RST
*       PB10 C'/D
*       PA7  SI
*       PA5  SCK
*       PB11 Backlight enable
*       PA6  SO
**/
#define LCD_CS   PA_4
#define LCD_CD   PB_10
#define LCD_RST  PB_1
#define LCD_BKL  PB_11

#define LCD_CS0  LCD_IO_RESET(GPIOA, 1 << 4)
#define LCD_CS1  LCD_IO_SET(GPIOA, 1 << 4)
#define LCD_CD0  LCD_IO_RESET(GPIOB, 1 << 10)
#define LCD_CD1  LCD_IO_SET(GPIOB, 1 << 10)
#define LCD_RST0 LCD_IO_RESET(GPIOB, 1 << 1)
#define LCD_RST1 LCD_IO_SET(GPIOB, 1 << 1)
#define LCD_BKL0 LCD_IO_RESET(GPIOB, 1 << 11)
#define LCD_BKL1 LCD_IO_SET(GPIOB, 1 << 11)

#define LCD_PIN_INIT \
    GPIO_Config(LCD_BKL, GPIO_OUTPUT); \
    GPIO_Config(LCD_RST, GPIO_OUTPUT); \
    GPIO_Config(LCD_CD, GPIO_OUTPUT);  \
    GPIO_Config(LCD_CS, GPIO_OUTPUT); 

/**
 * @brief Button pins
 * */
#define BUTTON_LEFT  	(1<<15)
#define BUTTON_RIGHT 	(1<<13)
#define BUTTON_CENTER   (1<<14)
#define BUTTON_A        BUTTON_CENTER

#define BUTTON_LEFT2	(1<<9)
#define BUTTON_RIGHT2	(1<<12)

#define BUTTON_HW_READ  (~GPIOB->IPTDT & BUTTON_MASK)
#define BUTTON_MASK     (BUTTON_LEFT | BUTTON_RIGHT | BUTTON_A | BUTTON_LEFT2 | BUTTON_RIGHT2)

#define LED_PIN         PB_3
#define LED1_PIN_INIT \
    GPIO_Config(LED_PIN, GPIO_OUTPUT); \
    RCC->APB2EN |= RCC_APB2EN_AFIOEN; /* Remap Jtag pins */ \
    AFIO->MAP = AFIO_MAP_SWJTAG_CONF_JTAGDISABLE;

#define LED1_OFF        GPIO_Write(LED_PIN, GPIO_PIN_HIGH)
#define LED1_ON         GPIO_Write(LED_PIN, GPIO_PIN_LOW)
#define LED1_TOGGLE     GPIO_Toggle(LED_PIN)

#define DBG_PIN_INIT    LED1_PIN_INIT
#define DBG_PIN_TOGGLE  LED1_TOGGLE

//enum {false = 0, true, OFF = false, ON = true};

void BOARD_Init(void);
void DelayMs(uint32_t ms);
uint32_t ElapsedTicks(uint32_t start_ticks);
uint32_t GetTick(void);
void SW_Reset(void);
void __debugbreak(void);

void BOARD_LCD_Init(void);
#ifdef __cplusplus
}
#endif

#endif