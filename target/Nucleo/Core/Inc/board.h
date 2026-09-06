#ifndef _board_h_
#define _board_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "main.h"
#include "pwm.h"
#include "drvlcd.h"
#include "spi.h"
#include "qspi.h"
#include "rng.h"
#include "serial.h"
#include "uart.h"
#include "dma.h"
#include "tone.h"
#include "gpio.h"
#include "st7735.h"
#include "dma_stm32l4xx.h"
#include "gpio_stm32l4xx.h"
#include "spiflash/flash_dev.h"
#include "stdinout.h"

/**
 * General macros
 * */
/*
 NUCLEO L412KB

    +---UUUU---+
    |          |
 PA9|D1     VIN|
PA10|D0     GND|
 RST|nRST  nRST|
    |GND    +5V|
PA12|D2      A7|PA2
 PB0|D3      A6|PA7
 PB7|D4      A5|PA6
 PB6|D5      A4|PA5
 PB1|D6      A3|PA4
PC14|D7      A2|PA3
PC15|D8      A1|PA1
 PA8|D9      A0|PA0
PA11|D10   AREF|
 PB5|D11   +3V3|
 PB4|D12    D13|PB3 (LED)
    +----------+

 */
/**
 * LED and GPIO's
 * */
#define LED1_ON          HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET)
#define LED1_OFF         HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET)
#define LED1_TOGGLE      HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin)

/**
 * TFT stuff
 */
#define LCD_SCK_PIN         PA_1  // A1
#define LCD_DI_PIN          PA_7  // A6
#define LCD_CS_PIN          PA_4  // A3
#define LCD_CD_PIN          PA_6  // A5
#define LCD_RST_PIN         -1
#define LCD_BKL_PIN         PB_4  // D12

#define I2C_MAX_ITF         3
#define I2C_IF0 	        0 //PB6->SCL, PB7<->SDA

void BOARD_Init(void);
void BOARD_LCD_Init(void);
void BOARD_PWM_Init(pwmchip_t *pwmchip);

void SW_Reset(void);
void DelayMs(uint32_t ms);
uint32_t GetTick(void);

uint32_t i2cWrite(uint8_t device, const uint8_t *data, uint16_t len);
uint32_t i2cRead(uint8_t device, uint8_t *data, uint16_t len);

flashdev_res_t flashReadId(uint32_t *id);
flashdev_res_t flashRead(uint8_t *pbuffer, uint32_t addr, uint16_t len);
flashdev_res_t flashWrite(uint8_t *pbuffer, uint32_t addr, uint16_t len);
flashdev_res_t flashErase(uint32_t addr, enum flashdev_blk_sz size);

#ifdef ENABLE_I2C
extern i2cbus_t board_i2cbus;
#endif

extern stdinout_t *host_serial;

#ifdef __cplusplus
}
#endif

#endif /* _board_h_ */
