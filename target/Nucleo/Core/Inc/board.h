#ifndef _board_h_
#define _board_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdout.h>
#include "main.h"
#include "pwm.h"
#include "drvlcd.h"
#include "spi.h"
#include "rng.h"
#include "serial.h"
#include "uart.h"
#include "dma.h"
#include "st7735.h"
#include "dma_stm32l4xx.h"
#include "gpio_stm32l4xx.h"

/**
 * General macros
 * */


/**
 * LED and GPIO's
 *  LED Pin PB3 (Arduino D13)
 * */
#define LED_ON          HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET)
#define LED_OFF         HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET)
#define LED1_TOGGLE     HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin)

/**
 * TFT stuff
 */
#define LCD_SCK_PIN         PA_1
#define LCD_DI_PIN          PA_7
#define LCD_CS_PIN          PA_4
#define LCD_CD_PIN          PA_6
#define LCD_RST_PIN         -1
#define LCD_BKL_PIN         PB_4

#define I2C_MAX_ITF         3
#define I2C_IF0 	        0 //PB6->SCL, PB7<->SDA

/**
 * SPI Header
 * */
#define BOARD_SPIDEV_HANDLER    spi1
#define BOARD_SPIDEV            (&BOARD_SPIDEV_HANDLER)
extern spibus_t BOARD_SPIDEV_HANDLER;

void BOARD_Init(void);
void BOARD_LCD_Init(void);
void SW_Reset(void);
void DelayMs(uint32_t ms);
uint32_t GetTick(void);

#ifdef __cplusplus
}
#endif

#endif /* _board_h_ */
