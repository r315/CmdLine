#ifndef _board_h_
#define _board_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdout.h>
#include <stdint.h>
#include "main.h"
#include "stm32f1xx.h"
#include <fifo.h>
#include "pwm.h"
#include "spi.h"
#include "rng.h"
#include "serial.h"
#include "uart.h"
#include "st7735.h"
#include "gpio.h"
#include "gpio_stm32f1xx.h"

/**
 * HW symbols
 * */

#define DBG_LED_TOGGLE          GPIO_Toggle(PC_13)
#define DBG_LED_ON              GPIO_Write(PC_13, GPIO_PIN_SET)
#define DBG_LED_OFF             GPIO_Write(PC_13, GPIO_PIN_RESET)
#define LED_TOGGLE              DBG_LED_TOGGLE
#define LED1_TOGGLE             LED_TOGGLE

/* 
 *
 * */
void BOARD_Init(void);
uint32_t GetTick(void);
uint32_t ElapsedTicks(uint32_t start_ticks);
void DelayMs(uint32_t ms);

/** 
 * Global variables
 * */
extern I2C_HandleTypeDef hi2c2;
extern TIM_HandleTypeDef htim4;

/**
 * Function prototypes
 * 
 * PB10 SCL
 * PB11 SDA
 * */
void setInterval(void(*cb)(), uint32_t ms);
void i2cCfgDMA(uint8_t *src, uint16_t size);
void i2cSendDMA(uint8_t address, uint8_t *data, uint16_t size);

/**
* Vile hack to reenumerate, physically _drag_ d+ low.
* (need at least 2.5us to trigger usb disconnect)
* */
static inline void reenumerate_usb(void){
    USB->CNTR = USB_CNTR_PDWN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIOA->CRH = (GPIOA->CRH & ~(0x0F << 16)) | (2 << 16);
    GPIOA->BRR |= GPIO_PIN_12;
    //for (unsigned int i = 0; i < 100000; i++)
	//	__asm__("nop");
}

/**
 * PWM
 * 
 * PB0 PWM_VOUT
 * PB1 PWM_IOUT
 * PWM_ILOAD
 * PWM_VLOAD
 */
#define PWM_RESOLUTION          10UL
#define PWM_MAX_VALUE           (1<<PWM_RESOLUTION)
#define PWM_MIN_VALUE           5
#define PWM_MAX_CH              4

/**
 * ADC
 *
 * Analog Pins
 * VOUT PA0
 * IOUT PA1 
 * VLOAD PA2
 * ILOAD PA3
 * */

/* ***********************************************************
 * Configure callback for end of transfer of ADC convertions
 * 
 * \param  cb    call back function void cb(uin16_t *adc_convertions);
 * \return none
 ************************************************************ */
void ADC_SetCallBack(void (*)(uint16_t*));

/* ***********************************************************
 * Get the last performed convertions
 * Not thread safe
 * \param  none
 * \return uint16_t *last_adc_convertions 
 ************************************************************ */
uint16_t *ADC_LastConvertion(void);

/* ************************************************************
 * Servo API
 * ************************************************************ */
#define SERVO_PORT              GPIOB
#define SERVO_PIN               9
/**
 * @brief start servo pulse, 1000-200us pulse every 20ms on pin PB9
 * */
void SERVO_Init(void);

/**
 * @brief set servo pulse width
 * @param pulse : pulse width, 900 - 2100
 * */
void SERVO_SetPulse(uint16_t pulse);
 
/* ************************************************************
 * SPI API
 *
 * 
 * ************************************************************ */
#define SPI_XFER_TIMEOUT        1000

#define LCD_DI_PIN              PB_15
#define LCD_SCK_PIN             PB_13
#define LCD_CS_PIN              PB_12
#define LCD_CD_PIN              PB_14
#define LCD_RST_PIN             -1
#define LCD_BKL_PIN             PB_10

#define BOARD_SPI_DO_PIN        PB_15
#define BOARD_SPI_DI_PIN        PB_14
#define BOARD_SPI_CK_PIN        PB_13
#define BOARD_SPI_CS_PIN        PB_12

#define BOARD_SPI_CS_LOW        GPIOB->BRR = (1 << 12) 
#define BOARD_SPI_CS_HIGH       GPIOB->BSRR = (1 << 12)

#define BOARD_SPIDEV_HANDLER    spi2
#define BOARD_SPIDEV            (&BOARD_SPIDEV_HANDLER)

extern spibus_t BOARD_SPIDEV_HANDLER;

void BOARD_SPI_Init(void);
void BOARD_SPI_SetCS(uint8_t cs);
uint16_t BOARD_SPI_Transfer(uint16_t data, uint32_t timeout);
uint32_t BOARD_SPI_Write(uint8_t *src, uint32_t size);
uint32_t BOARD_SPI_Read(uint8_t *dst, uint32_t size);

/**
 * UART
 * */
#define BOARD_SERIAL_HANDLERS   BOARD_SERIAL0_HANDLER, BOARD_SERIAL4_HANDLER
#define BOARD_SERIAL0           (&BOARD_SERIAL0_HANDLER.serial)
#define BOARD_SERIAL1           BOARD_SERIAL0
#define BOARD_SERIAL4           (&BOARD_SERIAL4_HANDLER.serial)
#define BOARD_STDIO             BOARD_SERIAL4

extern serialport_t BOARD_SERIAL_HANDLERS;

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

#define LCD_CK_Pin          GPIO_PIN_13
#define LCD_CK_GPIO_Port    GPIOB
#define LCD_CD_Pin          GPIO_PIN_14
#define LCD_CD_GPIO_Port    GPIOB
#define LCD_DI_Pin          GPIO_PIN_15
#define LCD_DI_GPIO_Port    GPIOB
#define LCD_RST_Pin
#define LCD_RST_GPIO_Port
#define LCD_BKL_Pin         10
#define LCD_BKL_GPIO_Port   GPIOB

#define LCD_CS0             BOARD_SPI_CS_LOW
#define LCD_CS1             BOARD_SPI_CS_HIGH
#define LCD_CD0             LCD_CD_GPIO_Port->BRR = LCD_CD_Pin
#define LCD_CD1             LCD_CD_GPIO_Port->BSRR = LCD_CD_Pin
#define LCD_BKL0            LCD_BKL_GPIO_Port->BRR = (1 << LCD_BKL_Pin)
#define LCD_BKL1            LCD_BKL_GPIO_Port->BSRR = (1 << LCD_BKL_Pin)
#define LCD_RST0            //LCD_RST_GPIO_Port->BRR = LCD_RST_Pin
#define LCD_RST1            //LCD_RST_GPIO_Port->BSRR = LCD_RST_Pin

#define TFT_SPIDEV      BOARD_SPIDEV

void BOARD_LCD_Init(void);
void BOARD_LCD_WriteArea(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *data);
void BOARD_LCD_Scroll(uint16_t sc);
void SW_Reset(void);

#ifdef __cplusplus
}
#endif

#endif