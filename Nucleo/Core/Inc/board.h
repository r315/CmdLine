#ifndef _board_h_
#define _board_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdout.h>
#include "main.h"
#include "pwm.h"
#include "lcd.h"
#include "spi.h"
#include "rng.h"
#include "serial.h"
#include "uart.h"
#include "dma.h"

#include "dma_stm32l4xx.h"

/**
 * General macros
 * */
static inline void DelayMs(uint32_t ms) { HAL_Delay(ms); }
static inline uint32_t GetTick(){ return HAL_GetTick(); }

/**
 * Serial header
 * */
#define BOARD_SERIAL_HANDLERS   BOARD_SERIAL0_HANDLER, BOARD_SERIAL1_HANDLER
#define BOARD_SERIAL0           (&BOARD_SERIAL0_HANDLER.out)
#define BOARD_SERIAL1           (&BOARD_SERIAL1_HANDLER.out)
#define BOARD_STDIO             BOARD_SERIAL0

extern serialhandler_t BOARD_SERIAL_HANDLERS;

/**
 * LED and GPIO's
 *  LED Pin PB3 (Arduino D13)
 * */
#define LED_ON      HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET)
#define LED_OFF     HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET)
#define LED1_TOGGLE  HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin)

#define BOARD_GPIO_Init(_P, _IO, _M) // TODO as in bluepill

/**
 * TFT stuff
 */
#define TFT_SPIDEV      BOARD_SPIDEV

#if 0
#define TFT_W 80
#define TFT_H 160  // 162 on GRAM

#define TFT_OFFSET_SOURCE	26
#define TFT_OFFSET_GATE		1
#define TFT_BGR_FILTER
#else
#define TFT_W 128
#define TFT_H 160
#endif

#define LCD_CK_Pin          GPIO_PIN_1
#define LCD_CK_GPIO_Port    GPIOA
#define LCD_CS_Pin          GPIO_PIN_4
#define LCD_CS_GPIO_Port    GPIOA
#define LCD_CD_Pin          GPIO_PIN_6
#define LCD_CD_GPIO_Port    GPIOA
#define LCD_DI_Pin          GPIO_PIN_7
#define LCD_DI_GPIO_Port    GPIOA
#define LCD_RST_Pin
#define LCD_RST_GPIO_Port
#define LCD_BKL_Pin         GPIO_PIN_4
#define LCD_BKL_GPIO_Port   GPIOB

#define LCD_CS1             LCD_CS_GPIO_Port->BSRR = LCD_CS_Pin
#define LCD_CS0             LCD_CS_GPIO_Port->BRR = LCD_CS_Pin
#define LCD_CD1             LCD_CD_GPIO_Port->BSRR = LCD_CD_Pin
#define LCD_CD0             LCD_CD_GPIO_Port->BRR = LCD_CD_Pin
#define LCD_BKL1            LCD_BKL_GPIO_Port->BSRR = LCD_BKL_Pin
#define LCD_BKL0            LCD_BKL_GPIO_Port->BRR = LCD_BKL_Pin
#define LCD_RST1            //LCD_RST_GPIO_Port->BSRR = LCD_RST_Pin
#define LCD_RST0            //LCD_RST_GPIO_Port->BRR = LCD_RST_Pin


/**
 * SPI Header
 * */
#define BOARD_SPIDEV_HANDLER    spi1
#define BOARD_SPIDEV            (&BOARD_SPIDEV_HANDLER)
extern spibus_t BOARD_SPIDEV_HANDLER;


#ifdef __cplusplus
}
#endif

#endif /* _board_h_ */
