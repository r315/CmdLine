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

/**
 * HW symbols
 * */

#define DBG_LED_TOGGLE  HAL_GPIO_TogglePin(DBG_GPIO_Port, DBG_Pin)
#define DBG_LED_ON      HAL_GPIO_WritePin(DBG_GPIO_Port, DBG_Pin, GPIO_PIN_SET)
#define DBG_LED_OFF     HAL_GPIO_WritePin(DBG_GPIO_Port, DBG_Pin, GPIO_PIN_RESET)
#define LED_TOGGLE      DBG_LED_TOGGLE

#define GetTicks HAL_GetTick
#define DelayMs(d) HAL_Delay(d)

static inline uint32_t ElapsedTicks(uint32_t start_ticks){ 
    uint32_t current = GetTicks(); 
    return (current > start_ticks) ? current - start_ticks : 0xFFFFFFFF - start_ticks + current;
}

/** 
 * Global variables
 * */
extern I2C_HandleTypeDef hi2c2;
extern TIM_HandleTypeDef htim4;
extern StdOut vcom;

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

#define PWM_OUT_VOLTAGE     1       // Channel that controls output voltage
#define PWM_OUT_CURRENT     2       // Channel that controls output current
#define PWM_RESOLUTION      10UL
#define PWM_MAX_VALUE       (1<<PWM_RESOLUTION)
#define PWM_MIN_VALUE       5
#define PWM_MAX_CH          4

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
 * ADC is triggered by TIM3 TRGO and performs dual 
 * simultaneous convertion. it converts 4 channels and transfers 
 * the result to memory using DMA 
 * 
 * \param ms    Time between convertions
 ************************************************************ */
void ADC_Init(uint16_t);
void ADC_Stop(void);

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
#define SERVO_PORT  GPIOB
#define SERVO_PIN   9
/**
 * @brief start servo pulse, 1000-200us pulse every 20ms on pin PB9
 * */
void SERVO_Init(void);

/**
 * @brief set servo pulse width
 * @param pulse : pulse width, 900 - 2100
 * */
void SERVO_SetPulse(uint16_t pulse);

extern StdOut uart;
extern StdOut vcp;
extern StdOut uart_aux;
 
/* ************************************************************
 * SPI API
 * ************************************************************ */
#define SPI_XFER_TIMEOUT    1000

void SPI_Init(void);
uint32_t SPI_Read(uint8_t *dst, uint32_t size);
uint32_t SPI_Write(uint8_t *src, uint32_t size);
void SPI_SetCS(uint8_t cs);
uint8_t SPI_Transfer(uint8_t *data, uint32_t timeout);

 
#ifdef __cplusplus
}
#endif

#endif