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

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern StdOut uart;
extern StdOut uart_aux;

// LED Pin PB3 (Arduino D13)
#define LED_ON      HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET)
#define LED_OFF     HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET)
#define LED_TOGGLE  HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin)

#define DelayMs(_X) HAL_Delay(_X)

/**
 * TFT stuff
 */
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

#define SPI_BLOCK_XFER
#include "st7735.h"

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
#define LCD_BKL_Pin
#define LCD_BKL_GPIO_Port

#define LCD_CS1             LCD_CS_GPIO_Port->BSRR = LCD_CS_Pin
#define LCD_CS0             LCD_CS_GPIO_Port->BRR = LCD_CS_Pin
#define LCD_CD1             LCD_CD_GPIO_Port->BSRR = LCD_CD_Pin
#define LCD_CD0             LCD_CD_GPIO_Port->BRR = LCD_CD_Pin
#define LCD_BKL1            //LCD_BKL_GPIO_Port->BSRR = LCD_BKL_Pin
#define LCD_BKL0            //LCD_BKL_GPIO_Port->BRR = LCD_BKL_Pin
#define LCD_RST1            //LCD_RST_GPIO_Port->BSRR = LCD_RST_Pin
#define LCD_RST0            //LCD_RST_GPIO_Port->BRR = LCD_RST_Pin

extern SPI_HandleTypeDef hspi1;

static inline void SPI_Send(uint8_t data){
	HAL_SPI_Transmit(&hspi1, &data, 1, 1000);
}

static inline void SPI_Read(uint8_t *dst, uint32_t len){
	HAL_SPI_Receive(&hspi1, dst, len, 1000);
}

static inline void SPI_Write(uint8_t *dst, uint32_t len){
	HAL_SPI_Transmit(&hspi1, dst, len, 1000);
}

#ifdef __cplusplus
}
#endif

#endif /* _board_h_ */
