#ifndef _board_h_
#define _board_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "main.h"
#include "stm32f1xx.h"
#include <fifo.h>
#include "pwm.h"
#include "spi.h"
#include "rng.h"
#include "serial.h"
#include "uart.h"
#include "gpio.h"
#include "gpio_stm32f1xx.h"
#include "tone_stm32f1xx.h"
#include "dma_stm32f1xx.h"

/**
 * HW symbols
 * */
#define LED1_PIN                PC_13
#define LED1_INIT               GPIO_Config(LED1_PIN, GPO_LS);
#define LED1_TOGGLE             GPIO_Toggle(LED1_PIN)
#define LED1_ON                 GPIO_Write(LED1_PIN, GPIO_PIN_SET)
#define LED1_OFF                GPIO_Write(LED1_PIN, GPIO_PIN_RESET)

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

/**
 * Function prototypes
 *
 * PB10 SCL
 * PB11 SDA
 * */
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

/**
 * UART
 * */

void BOARD_LCD_Init(void);
void SW_Reset(void);

uint16_t spiExchange(uint8_t *buffer, uint16_t len, uint32_t timeout);

#ifdef __cplusplus
}
#endif

#endif