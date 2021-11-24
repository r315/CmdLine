
#include "board.h"
#include "usbd_cdc_if.h"
#include <stdout.h>


void BOARD_Init(void){
    BOARD_SPIDEV->bus = SPI_BUS1;
    BOARD_SPIDEV->freq = 1000000;
    BOARD_SPIDEV->cfg = SPI_SW_CS;

    SPI_Init(BOARD_SPIDEV);
    BOARD_GPIO_Init(BOARD_SPI_PORT, BOARD_SPI_DO_PIN, PIN_OUT_AF | PIN_OUT_50MHZ);
    BOARD_GPIO_Init(BOARD_SPI_PORT, BOARD_SPI_DI_PIN, PIN_OUT_AF | PIN_OUT_50MHZ);
    BOARD_GPIO_Init(BOARD_SPI_PORT, BOARD_SPI_CK_PIN, PIN_OUT_AF | PIN_OUT_50MHZ);
    BOARD_GPIO_Init(BOARD_SPI_PORT, BOARD_SPI_CS_PIN, PIN_OUT_2MHZ);

    SERVO_Init();
}

/**
 * @brief Configure GPIO pin
 * @param port : pin port
 * @param pin : pin number 0 - 15
 * @param mode : Output
 *                  GPO_2MHZ
 *                  GPO_10MHZ
 *                  GPO_50MHZ
 *                  GPO_AF | GPO_xxMHZ
 *                  GPO_AF_OD | GPO_xxMHZ
 *               Input
 *                  GPI_FLOAT
 *                  GPI_ANALOG
 *                  GPI_PD
 *                  GPI_PU
 * */
void BOARD_GPIO_Init(GPIO_TypeDef *port, uint8_t pin, uint8_t mode) {
    
    if(mode == PIN_IN_PD){
        port->BRR = (1 << pin);
    }else if(mode == PIN_IN_PU){
        port->BSRR = (1 << pin);
    }    

    mode &= 0x0f;

    if(pin <  8){ 
        port->CRL = (port->CRL & ~(15 << (pin << 2))) | (mode << (pin << 2));
    }else{ 
        port->CRH = (port->CRH & ~(15 << ((pin - 8) << 2))) | (mode << ((pin - 8) << 2)); 
    }
}


void setInterval(void(*cb)(), uint32_t ms){
    // start loop, timer is configures on startup
    // and call is made on interupt handler
    HAL_TIM_Base_Start_IT(&htim4);
}


/**
 * PWM Driver
 * 
 * PWM1 - PA6
 * PWM2 - PA7
 * PWM3 - PB0
 * PWM4 - PB1
 * 
 * \param initial - startup duty for each channel
 * 
 * */
void PWM_Init(uint32_t period){

     /* Configure Timer 3 */
    RCC->APB1ENR  |= RCC_APB1ENR_TIM3EN;
    RCC->APB1RSTR |= RCC_APB1ENR_TIM3EN;
    RCC->APB1RSTR &= ~RCC_APB1ENR_TIM3EN;

    TIM3->CR1 = TIM_CR1_ARPE;
    TIM3->CCMR1 = (6<<TIM_CCMR1_OC2M_Pos) | (6<<TIM_CCMR1_OC1M_Pos) | (TIM_CCMR1_OC2PE) | (TIM_CCMR1_OC1PE);  // PWM Mode 1
    TIM3->CCMR2 = (6<<TIM_CCMR2_OC4M_Pos) | (6<<TIM_CCMR2_OC3M_Pos) | (TIM_CCMR2_OC4PE) | (TIM_CCMR2_OC3PE); 
    TIM3->PSC = 1; // Timer3 freq = SystemClock / 2

    TIM3->ARR = (1<<PWM_RESOLUTION) - 1;
    
    TIM3->CR1 |= TIM_CR1_CEN;     // Start pwm before enable outputs

    GPIOA->CRL &= ~(0xFF << 24);
    GPIOA->CRL |= (0xAA << 24);   // PA7-6 AFO-PP
    GPIOB->CRL &= ~(0xFF << 0);
    GPIOB->CRL |= (0xAA << 0);    // PB1-0 AFO-PP
}

/**
 * @brief Set new pwm value for the given channel
 * 
 * \param ch [in] :      Channel to be configured
 *                       PWM_1 ... PWM_6
 * \param duty [in] :    Duty cycle 0 - 100
 * */
void PWM_Set(uint8_t ch, uint8_t duty){

     if(ch > PWM_MAX_CH)
        return;

     if(duty > 100)
        duty = 100;

    uint32_t cmp = TIM3->ARR;
    cmp *= duty;
    cmp /= 100;

    uint32_t *ccr = (uint32_t*)&TIM3->CCR1;

    ccr[ch - 1] = cmp;    
}

/**
 * @brief Get pwm duty from channel
 * 
 * \param ch [in] :  Channel to be configured
 *                      PWM_1 ... PWM_6
 * \retval duty :    Duty cycle 0 - 100
 * */
uint8_t PWM_Get(uint8_t ch){

    if(ch > PWM_MAX_CH)
        return 0;
    
    uint32_t *ccr = (uint32_t*)&TIM3->CCR1;

    uint32_t cmp = ccr[ch - 1] * 100;    
    
    return cmp / TIM3->ARR;
}

/**
 * PA6   ------> TIM1_CH1
 * PA7   ------> TIM1_CH2
 * PB0   ------> TIM1_CH3
 * PB1   ------> TIM1_CH4
 * */
static void PWM_CfgGpio(uint8_t ch, uint8_t enable){
    enable = (enable == 0) ? PIN_IN_ANALOG :  PIN_OUT_AF | PIN_OUT_2MHZ;

    switch(ch){
        case PWM_1:
            BOARD_GPIO_Init(GPIOA, GPIO_PIN_6, enable);
            break;

        case PWM_2:
            BOARD_GPIO_Init(GPIOA, GPIO_PIN_7, enable);
            break;

        case PWM_3:
            BOARD_GPIO_Init(GPIOB, GPIO_PIN_0, enable);
            break;

        case PWM_4:
            BOARD_GPIO_Init(GPIOB, GPIO_PIN_1, enable);
            break;

        default:
            break;
    }
}

/**
 * @brief Enables PWM channel and configure GPIO pin to PWM function
 * 
 * \param ch [in] Channel to be configured
 *                  PWM_1
 *                  ---
 *                  PWM_6
 * 
 * PWM pins start on PA8
 * */
void PWM_Enable(uint8_t channel){
    if(channel > PWM_MAX_CH)
        return;

    uint8_t shift = (channel - 1) << 2;
    TIM3->CCER |= (TIM_CCER_CC1E << shift);

    PWM_CfgGpio(channel, 1);
}

/**
 * @brief Disables PWM channel and configure GPIO pin to default
 * 
 * \param ch [in] Channel to be configured
 *                 PWM_1 ... PWM_6
 * */
void PWM_Disable(uint8_t channel){
    if(channel > PWM_MAX_CH)
        return;

    uint8_t shift = (channel - 1) << 2;
    TIM3->CCER &= ~(TIM_CCER_CC1E << shift);

    PWM_CfgGpio(channel, 0);
}

/**
 * @brief Generate RC Servo signal on pin PB9
 * */
void SERVO_Init(void){
     /* Configure Timer 3, same as pwm*/
    RCC->APB1ENR  |= RCC_APB1ENR_TIM3EN;
    RCC->APB1RSTR |= RCC_APB1ENR_TIM3EN;
    RCC->APB1RSTR &= ~RCC_APB1ENR_TIM3EN;

    TIM3->CR1 = TIM_CR1_ARPE;       // Enable auto-reload preload
    //TIM3->CCMR1 = (6<<TIM_CCMR1_OC2M_Pos) | (6<<TIM_CCMR1_OC1M_Pos) | (TIM_CCMR1_OC2PE) | (TIM_CCMR1_OC1PE);  // PWM Mode 1
    
    TIM3->CCER = TIM_CCER_CC1E; // Enable channel 1
    TIM3->PSC = (SystemCoreClock/1000000) - 1;

    TIM3->ARR = 20000 - 1;
    TIM3->CCR1 = 1500;
    
    TIM3->DIER |= TIM_DIER_CC1IE | TIM_DIER_UIE;

    NVIC_EnableIRQ(TIM3_IRQn);

    TIM3->CR1 |= TIM_CR1_CEN;     // Start pwm before enable outputs

    BOARD_GPIO_Init(SERVO_PORT, SERVO_PIN, PIN_OUT_2MHZ);    
}

/**
 * @brief Set servo pulse
 * @param pulse : pulse width in us 1000 - 2000
 * */
void SERVO_SetPulse(uint16_t pulse){
    if(pulse < 900 || pulse > 2100){
        return;
    }

    TIM3->CCR1 = pulse;
}

void SERVO_Stop(void){

    TIM3->CR1 = 0;

    NVIC_DisableIRQ(TIM3_IRQn);
    
    BOARD_GPIO_Init(SERVO_PORT, SERVO_PIN, PIN_IN_FLOAT);
}

uint16_t SERVO_Get(void){
    return 0;
}

void TIM3_IRQHandler(void){

    if(TIM3->SR & TIM_SR_CC1IF){
        SERVO_PORT->BRR = (1 << SERVO_PIN);
        TIM3->SR &= ~(TIM_SR_CC1IF); 
    }

    if(TIM3->SR & TIM_SR_UIF){
        SERVO_PORT->BSRR = (1 << SERVO_PIN);
        TIM3->SR &= ~(TIM_SR_UIF); 
    }
}

/**
 * virtual com port stuff
 * */
#define VCOM_FIFO_SIZE 512

static fifo_t rxfifo;

static void vc_init(void){
    rxfifo.size = VCOM_FIFO_SIZE;
	fifo_init(&rxfifo);
	fifo_flush(&rxfifo);
}


// called from cdc interface
void vc_put(uint8_t *c){
    fifo_put(&rxfifo, *c);
}

static uint8_t vc_getCharNonBlocking(char *c){    
    return fifo_get(&rxfifo, (uint8_t*)c);
}

static char vc_getchar(void){
    char c;
    while(fifo_avail(&rxfifo) == 0);
    fifo_get(&rxfifo, (uint8_t*)&c);
    return c;
}

uint8_t vc_kbhit(void){
    return fifo_avail(&rxfifo);
}

static void putAndRetry(uint8_t *data, uint16_t len){
uint32_t retries = 1000;
	while(retries--){
		if(	CDC_Transmit_FS(data, len) == USBD_OK)
			break;
	}
}

static void vc_putchar(char c){
	putAndRetry((uint8_t*)&c, 1);
}

static void vc_puts(const char *s){
uint16_t len = 0;
	
	while( *((const char*)(s + len)) != '\0'){
		len++;	
	}
	putAndRetry((uint8_t*)s, len);
}

StdOut vcp = {
    .init = vc_init,
    .xgetchar = vc_getchar,
    .xputchar = vc_putchar,
    .xputs = vc_puts,
    .getCharNonBlocking = vc_getCharNonBlocking,
    .kbhit = vc_kbhit
};

StdOut uart_aux;

/**
 * I2C DMA driver
 * 
 * This driver was implemented in a way that reduce the most setup steps much as possible,
 * for that the dma is configured once and then all transfers start by the I2C
 * */
void i2cCfgDMA(uint8_t *src, uint16_t size){
    // Configure DMA1 CH4 to handle i2c transmission
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;    
    DMA1_Channel4->CCR =    DMA_CCR_TCIE | // Transfer complete interrupt
                            DMA_CCR_DIR  | // Read From memory
                            DMA_CCR_CIRC |
                            DMA_CCR_MINC;  // Increment memory address
    DMA1_Channel4->CNDTR = size;
    DMA1_Channel4->CPAR = (uint32_t)&I2C2->DR;
    DMA1_Channel4->CMAR = (uint32_t)src;
    DMA1_Channel4->CCR |= DMA_CCR_EN;
    
    // Configure I2C2 transfer
    RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;    
    I2C2->CCR = 45;           // Freq 400Khz
    I2C2->CR1 = I2C_CR1_PE | I2C_CR1_ACK;
    I2C2->CR2 = 0x24;
    I2C2->CR2 |= I2C_CR2_DMAEN;
    HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 5, 0); // Highest priority
    HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
} 

/**
 * Due to the implementation of the driver, the parameters
 * data and size are ignored
 * */
void i2cSendDMA(uint8_t address, uint8_t *data, uint16_t size){
uint32_t n;

    while(I2C2->SR2 & I2C_SR2_BUSY); // wait for any transfer to end

    I2C2->CR1 |= I2C_CR1_START;     // Send start condition

    n = 1000;
    while(!(I2C2->SR1 & I2C_SR1_SB)){ // wait for master mode
        if(--n == 0)
            return;
    }    
    
    I2C2->CR2 |= I2C_CR2_DMAEN;         // enable DMA
    I2C2->DR = address;                 
    while(!(I2C2->SR1 & I2C_SR1_ADDR)){ // wait for slave acknowledge       
        if(--n == 0)            
            return;

        if(I2C2->SR1 & I2C_SR1_AF){
            I2C2->SR1 &= ~(I2C_SR1_AF);
            I2C2->CR1 |= I2C_CR1_STOP;  
            return;      
        }
    }
    n = I2C2->SR2; // Dummy read for crearing flags
    //while(I2C2->SR2 & I2C_SR2_BUSY);   
}

/**
 * ADC Driver
 * */

#define ADC_TRIGGER_CLOCK 500000UL
#define ADC_CR1_DUALMOD_FAST_INTERLEAVED     (7 << 16)
#define ADC_CR1_DUALMOD_SIMULTANEOUS         (1 << 16)
#define ADC_SAMPLES 4
#define ADC_SQR1_L_(len)                     (len << 20)
#define ADC_SQR3_SQ1_(ch)                    (ch << 0)
#define ADC_SQR3_SQ2_(ch)                    (ch << 5)
#define ADC_SQR3_SQ3_(ch)                    (ch << 10)
#define ADC_SQR3_SQ4_(ch)                    (ch << 15)
#define ADC_SMPR2_SMP0_(cy)                    (cy << 0)
#define ADC_SMPR2_SMP1_(cy)                    (cy << 3)
#define ADC_SMPR2_SMP2_(cy)                    (cy << 6)
#define ADC_SMPR2_SMP3_(cy)                    (cy << 9)

static void (*eotcb)(uint16_t*);
static uint16_t adcres[ADC_SAMPLES];

/* ***********************************************************
 * ADC is triggered by TIM2 TRGO and performs dual 
 * simultaneous convertion on regular simultaneous mode. 
 * It performs the conversion of 4 channels and transfers 
 * the result to memory using DMA
 * At the end of convertion, optionaly a callback function 
 * may be invoked
 * 
 * \param ms    Time between convertions
 ************************************************************ */

void ADC_Init(uint16_t ms){

    /* Configure DMA Channel1*/
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;           // Enable DMA1

    DMA1_Channel1->CPAR = (uint32_t)&ADC1->DR;  // Source address ADC1 Data Register
    DMA1_Channel1->CMAR = (uint32_t)adcres;     // Destination address memory
    DMA1_Channel1->CNDTR = 2; //ADC_SAMPLES;
    DMA1_Channel1->CCR =    DMA_CCR_PL |        // Highest priority
                            DMA_CCR_MSIZE_1 |   // 32bit Dst size
                            DMA_CCR_PSIZE_1 |   // 32bit src size
                            DMA_CCR_MINC |      // increment memory pointer after transference
                            DMA_CCR_CIRC |      // Circular mode
                            DMA_CCR_TCIE;       // Enable end of transfer interrupt    
    DMA1_Channel1->CCR |=   DMA_CCR_EN;

     /* Configure Timer 2 */
    RCC->APB1ENR  |= RCC_APB1ENR_TIM2EN;    // Enable Timer 2
    RCC->APB1RSTR |= RCC_APB1ENR_TIM2EN;    // Reset timer registers
    RCC->APB1RSTR &= ~RCC_APB1ENR_TIM2EN;

    TIM2->CCMR1 = (3<<TIM_CCMR1_OC2M_Pos);  // Toggle OC2REF on match
    TIM2->CCER = TIM_CCER_CC2E;             // Enable compare output for CCR2
    TIM2->PSC = (SystemCoreClock/ADC_TRIGGER_CLOCK) - 1; // 500kHz clock

    TIM2->ARR = (ms * (SystemCoreClock/ADC_TRIGGER_CLOCK)) - 1;
    TIM2->CCR2 = TIM2->ARR;

#if defined (OUTPUT_ADC_TRIGGER)
    /* Configure TIM3_CH1 Output PA6 */
    GPIOA->CRL &= ~(0x0F << 24); // Clear mode and configuration
    GPIOA->CRL |= (0x0A << 24);  // Output 2MHz, Alternative function push-pull
#endif

    /* Configure ADC 1 */
    RCC->APB2ENR  |= RCC_APB2ENR_ADC1EN;     // Enable Adc1
    RCC->APB2RSTR |= RCC_APB2ENR_ADC1EN;
    RCC->APB2RSTR &= ~RCC_APB2ENR_ADC1EN;

    ADC1->CR2 = ADC_CR2_ADON;               // Turn on ADC1
    ADC1->CR2 |=
            ADC_CR2_EXTTRIG  |              // Only the rising edge of external signal can start the conversion
            //ADC_CR2_EXTSEL_2 |              // 0b100 Select TIM3_TRGO as Trigger source
            ADC_CR2_EXTSEL_1 |              // 0b011 Select TIM2_CC2 Event
            ADC_CR2_EXTSEL_0 |              // 
            ADC_CR2_DMA;                    // Enable DMA Request

    ADC1->CR1 = ADC_CR1_DUALMOD_SIMULTANEOUS |
                ADC_CR1_SCAN;               // Scan throu all channels on sequence

    ADC1->SQR1 = ADC_SQR1_L_(1);            // Two channels on sequence
    ADC1->SQR3 = ADC_SQR3_SQ1_(0) |         // First convertion CH0, second CH2 
                 ADC_SQR3_SQ2_(2);

    ADC1->SMPR2 = ADC_SMPR2_SMP0_(4) |      // CH0 and CH2 sample time, 41.5 cycles
                  ADC_SMPR2_SMP2_(4);
                
    /* Configure ADC 2 */
    RCC->APB2ENR  |= RCC_APB2ENR_ADC2EN;    // Enable Adc2
    RCC->APB2RSTR |= RCC_APB2ENR_ADC2EN;
    RCC->APB2RSTR &= ~RCC_APB2ENR_ADC2EN;

    ADC2->CR2 = ADC_CR2_EXTTRIG  |
    		    ADC_CR2_EXTSEL_2 |          // Select SWSTART for dual convertion
				ADC_CR2_EXTSEL_1 |
    		    ADC_CR2_EXTSEL_0 |
				ADC_CR2_ADON;

    ADC2->SQR1 = ADC_SQR1_L_(1);            // Two channels on sequence
    ADC2->SQR3 = ADC_SQR3_SQ1_(1) |         // first convertion CH1, second CH3 
                 ADC_SQR3_SQ2_(3);

    ADC2->SMPR2 = ADC_SMPR2_SMP1_(4) |      // CH1 and CH3 sample time, 41.5 cycles
                  ADC_SMPR2_SMP3_(4);

    ADC2->CR1 = ADC_CR1_SCAN;

    /* Configure GPIOA Pins */
    GPIOA->CRL &= ~0xFFFF;                

    NVIC_SetPriority(DMA1_Channel1_IRQn, 0); // Highest priority
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);

    eotcb = NULL;
    TIM2->CR1 |= TIM_CR1_CEN;
}


uint16_t *ADC_LastConvertion(void){
    return adcres;
}

void ADC_Stop(void){
  TIM2->CR1 &= ~TIM_CR1_CEN;  
}

/**
 * DMA transfer interrupt handler
 * */
void DMA1_Channel1_IRQHandler(void){

    if(DMA1->ISR & DMA_ISR_TCIF1){
        ADC1->SR = 0;                    // Clear ADC1 Flags
        ADC2->SR = 0;
            if(eotcb != NULL)
        eotcb(adcres);
    }

    DMA1->IFCR = DMA_IFCR_CGIF1;    // Clear DMA Flags TODO: ADD DMA Error handling ?
}


void ADC_SetCallBack(void (*cb)(uint16_t*)){
    eotcb = cb;
}


/**
 * SPI API
 * This board uses SPI2
 * 
 * Pins:
 * PB12 -> CS
 * PB13 -> SCLK
 * PB14 <- MISO
 * PB15 -> MOSI
 * 
 * */
#define DMA_CCR_PL_Medium   (1<<12)
#define DMA_CCR_MSIZE_8     (0<<10)
#define DMA_CCR_MSIZE_16    (1<<10)
#define DMA_CCR_MSIZE_32    (2<<10)
#define DMA_CCR_PSIZE_16    (1<<8)
#define DMA_CCR_PSIZE_8     (0<<8)

uint16_t BOARD_SPI_Transfer(uint16_t data, uint32_t timeout){
    return SPI_Single_Transfer(BOARD_SPIDEV, data);
}

uint32_t BOARD_SPI_Read(uint8_t *dst, uint32_t size){
    if(size == 0 || dst == NULL){
        return 0;
    }
    
    for (uint32_t i = 0; i < size; i++, dst++){
        *dst = SPI_Single_Transfer(BOARD_SPIDEV, 0xFF);
    }

    return size;
}

uint32_t BOARD_SPI_Write(uint8_t *src, uint32_t size){
    SPI_Write(BOARD_SPIDEV, src, size);
    return size;
}

uint32_t RNG_Get(void){
    return 0;
}