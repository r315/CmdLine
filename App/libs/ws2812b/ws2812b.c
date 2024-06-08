#include <string.h>
#include <stdio.h>
#include "gpio.h"
#include "dma.h"
#include "board.h"

#ifdef STM32L412xx
#include "stm32l4xx_hal.h"
#define DBG_PIN_TOGGLE
#elif defined(AT32F415CBT7)
#include "at32f4xx.h"
#include "gpio_at32f4xx.h"
#include "dma_at32f4xx.h"

#define DBG_PIN_TOGGLE     LED1_TOGGLE
#endif
//#include "write.h"

#define WS2812_BIT_FREQ     800000UL  // 800kHz
#define WS2812_T0H          2500000UL // 1/400ns => 2.5Mhz
#define WS2812_T1H          (WS2812_T0H >> 1)

#define WS2811_BIT_FREQ     400000UL  // 400kHz
#define WS2811_T0H          3000000UL // 1/333ns => 3Mhz
#define WS2811_T1H          1500000UL // 1/666ns => 1.5Mhz

#define RGBLED_BIT_FREQ     WS2812_BIT_FREQ
#define RGBLED_T0H          WS2812_T0H
#define RGBLED_T1H          WS2812_T1H

#define RGBLED_FRAME_SIZE   24

static uint16_t t0h, t1h;
static dmatype_t dma;
static uint16_t  *pledframe, ledframe[RGBLED_FRAME_SIZE * 2]; // Using double buffering
static uint8_t *leddata;
static uint8_t nleds;

/**
 * @brief Prepare single led data by converting 24bit
 *        color pulse width time. data is converted
 *        into GRB frame format
 * 
 * @param dst output pulse width buffer 
 * @param src 24bit color in format RGB888
 */
static void ws2812_prepare_data(uint16_t *dst, uint8_t *src)
{
    // shift to GRB
    uint32_t grbdata = (src[1] << 16) | (src[2] << 8) | src[0];

    uint32_t mask = 1 << (RGBLED_FRAME_SIZE - 1);
    
    for(uint32_t j=0; j < RGBLED_FRAME_SIZE; j++, mask >>= 1)
    {
        dst[j] = (grbdata & mask) ? t1h : t0h; 
    }
}

/**
 * @brief Callback for half-transfer and end of transfer of DMA
 * 
 */
static void ws2812_eot(void)
{
    DBG_PIN_TOGGLE;
    
    if(--nleds == 0){
        #if defined(AT32F415CBT7)
        DMA_Cancel(&dma);                      // last led has received all bits, stop DMA
        TMR5->CC2 = 0;                         // Force low to force reset
        TMR5->CTRL1 &= ~TMR_CTRL1_CNTEN;       // stop timer also
        #endif
    }else{
        ws2812_prepare_data(pledframe, (nleds > 1) ? leddata : 0);
        pledframe = (pledframe == ledframe) ?  // swap buffer
                    ledframe + RGBLED_FRAME_SIZE:  ledframe;
        leddata += 3; // move to next led data
    }
}

void ws2812_init() {

    #ifdef STM32L412xx

    #elif defined(AT32F415CBT7)
    GPIO_Config(PA_1, GPO_MS_AF);
    #endif  

    #if defined(AT32F415CBT7)
    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR5, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1PERIPH_TMR5, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1PERIPH_TMR5, DISABLE);

    t0h = (SystemCoreClock / RGBLED_T0H) - 1;
    t1h = (SystemCoreClock / RGBLED_T1H) - 1;

    TMR5->CTRL1 = TMR_CTRL1_CLKDIV_1 | TMR_CTRL1_ARPEN;
    TMR5->DIV = 0;
    TMR5->AR = (SystemCoreClock / RGBLED_BIT_FREQ) - 1; // setup PWM frequency
    TMR5->CC2 = 0;                  // force low state on pin
    TMR5->CCM1 = TMR_CCM1_OC2MODE_2 | TMR_CCM1_OC2MODE_1; // PWM A mode
    TMR5->CCE = TMR_CCE_C2EN;       // Enable CC2
    TMR5->DIE = TMR_DIE_UEVDE;      // update event triggers DMA transfer

    dma.dst = (void*)&TMR5->CC2;    // CC2 as dst for PA1 pin
    dma.dsize = DMA_CCR_PSIZE_16;   // transferring 16bit
    dma.src = (void*)ledframe;      // ledframe buffer is the source
    dma.ssize = DMA_CCR_PSIZE_16;
    dma.dir = DMA_DIR_M2P;
    dma.single = 0;                 // Circular transfer
    dma.eot = ws2812_eot;           // ht/eof callback
    
    DMA_Config(&dma, DMA2_REQ_TIM5_UP); // Configure DMA for update event

    #elif defined(STM32L412xx)
    dma.eot = ws2812_eot;
    #endif
    memset(ledframe, 0, sizeof(ledframe)); // clears led frame
}

/**
 * @brief Write led data into leds
 * 
 * @param count Number of led's to be updated
 * @param data  RGB888 data buffer for led's
 */
void ws2812_write(uint32_t count,  uint8_t *data)
{
    if(count == 0)
        return;

    leddata = data;
    
    ws2812_prepare_data(ledframe, data);     // prepare data of first led
    leddata += 3;                            // set pointer to second led data

    if(count > 1){
        ws2812_prepare_data(ledframe + RGBLED_FRAME_SIZE, 
                            leddata);        // prepare data of second led
        leddata += 3;                        // set pointer to 3rd led data
    }

    nleds = count;                           // save number of led's to update

    pledframe = ledframe;                    // set pointer for first half, this half should be written 
                                             // on first interrupt (half-transfer)

    DBG_PIN_TOGGLE;
#if defined(AT32F415CBT7)
    DMA_Channel_Type *stream = (DMA_Channel_Type*)dma.stream;

    stream->TCNT = RGBLED_FRAME_SIZE * 2;  // Setup double buffer
    TMR5->STS = TMR_STS_UEVIF;             // Set update event flag, this avoids DMA transfer upon DMA enable
    stream->CHCTRL |=   DMA_CHCTRL1_HTIE | // Interrupt on half buffer to prepare next led data
                        DMA_CHCTRL1_CHEN;  // enable DMA
    TMR5->EVEG = TMR_EVEG_UEVG;            // Force update event, this clears counter and requests a DMA transfer
    TMR5->CTRL1 |= TMR_CTRL1_CNTEN;        // Enable count
#elif defined(STM32L412xx)
#endif
}