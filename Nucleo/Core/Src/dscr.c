/**
 * Digital Signal capture and replay
 * */
#include "board.h"
#include "dscr.h"
#include "dma.h"

#define TIM_CR2_MMS_COMPARE_PULSE   (3U << TIM_CR2_MMS_Pos)
#define TIM_CCMR1_CC1S_TI1          (1U << TIM_CCMR1_CC1S_Pos)
#define TIM_CCER_TI1FP1_BOTH        (10U)
#define TIM_SMCR_SMS_TRIGGER_MODE   (6U << TIM_SMCR_SMS_Pos)
#define TIM_CCMR2_OC4M_TOGGLE       (3U << TIM_CCMR2_OC4M_Pos)

#define CAP_TIM                     TIM1
#define CAP_TIM_IRQn                TIM1_CC_IRQn
#define CAP_TIM_ENABLE              \
    __HAL_RCC_TIM1_CLK_ENABLE();    \
    __HAL_RCC_TIM1_FORCE_RESET();   \
    __HAL_RCC_TIM1_RELEASE_RESET();

#define TIMEOUT_TIM                 TIM15
#define TIMEOUT_TIM_IRQn            TIM1_BRK_TIM15_IRQn
#define TIMEOUT_TIM_ENABLE          \
    __HAL_RCC_TIM15_CLK_ENABLE();   \
    __HAL_RCC_TIM15_FORCE_RESET();  \
    __HAL_RCC_TIM15_RELEASE_RESET();

#define CAP_DMA_IRQn                DMA1_Channel2_IRQn
#define REP_DMA_IRQn                DMA1_Channel4_IRQn

typedef struct {
    TIM_TypeDef *tim;
    dmactrl_t dma;
    void (*cb)(uint32_t ncap);
}dscr_t;

static dscr_t dsc, dsr;

void DSCR_Init(void){
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    CAP_TIM_ENABLE;
    TIMEOUT_TIM_ENABLE;

    dsc.tim = CAP_TIM;
    dsr.tim = CAP_TIM;
    
    dsc.tim->PSC = (SystemCoreClock / 1000000) - 1;     // Count us
    dsc.tim->ARR = 65535;                               // Max count 65535us
    dsc.tim->CR2 = TIM_CR2_MMS_COMPARE_PULSE;           // Configure master mode, valid for TIM1 and TIM15
    dsc.tim->CCMR1 = TIM_CCMR1_CC1S_TI1;                // Configure channel 1 capture mode for TI1 input
    dsc.tim->CCMR2 = TIM_CCMR2_OC4M_TOGGLE;             // Configure channel 4 toggle OC4
    dsc.tim->CCER = TIM_CCER_TI1FP1_BOTH;               // Capture on both edges
    dsc.tim->DIER = TIM_DIER_CC1DE | TIM_DIER_CC4DE;    // Enable DMA Request
    dsc.tim->BDTR = TIM_BDTR_MOE;                       // Set main output enable

    TIMEOUT_TIM->CR1 = TIM_CR1_OPM;                     // Enable one pulse mode
    TIMEOUT_TIM->PSC = (SystemCoreClock / 1000) - 1;    // Count ms
    TIMEOUT_TIM->ARR = 65535;                           // Count to max
    TIMEOUT_TIM->DIER = TIM_DIER_UIE;                   // Generate interrupt on update event

    /** GPIO Configuration
    PA8 (Pin D9)    <------ TIM1_CH1
    PA11 (Pin D10)  ------> TIM1_CH4 (open drain)
    */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    DMA_Request(&dsc.dma, DMA1_TIM1_CH1);
    DMA_Channel_TypeDef *dma_channel = (DMA_Channel_TypeDef*)dsc.dma.channel;

    dma_channel->CCR =
            DMA_CCR_MSIZE_0 |                           // 16bit Dst size
			DMA_CCR_PSIZE_0 |                           // 16bit src size
			DMA_CCR_TCIE;                               // Enable Transfer Complete interrupt
    dma_channel->CPAR = (uint32_t)&CAP_TIM->CCR1;       // Peripheral source
    dma_channel->CCR |= DMA_CCR_MINC;                   // Enable memory increment

    DMA_Request(&dsr.dma, DMA1_TIM1_CH4);
    dma_channel = (DMA_Channel_TypeDef*)dsr.dma.channel;

    dma_channel->CCR =
            DMA_CCR_MSIZE_0 |                           // 16bit Dst size
			DMA_CCR_PSIZE_0 |                           // 16bit src size
            DMA_CCR_DIR |                               // Transfer memory to peripheral
			DMA_CCR_TCIE;                               // Enable Transfer Complete interrupt
    dma_channel->CPAR = (uint32_t)&CAP_TIM->CCR4;       // Peripheral source
    dma_channel->CCR |= DMA_CCR_MINC;                   // Enable memory increment
    
    HAL_NVIC_EnableIRQ(CAP_DMA_IRQn);
    HAL_NVIC_EnableIRQ(REP_DMA_IRQn);
    HAL_NVIC_EnableIRQ(TIMEOUT_TIM_IRQn);
    //HAL_NVIC_EnableIRQ(CAP_TIM_IRQn);
}

/**
 * @brief Starts a capture
 * 
 * \param dst       : Pointer for destination of captured values
 * \param size      : Maximum buffer size
 * \param duration  : Timeout of capture in ms, capture will end after this time from 1st capture
 *  */
void DSCR_StartCapture(uint16_t *dst, uint32_t size, uint32_t duration, void(*cb)(uint32_t)){
    DMA_Channel_TypeDef *dma_channel = (DMA_Channel_TypeDef*)dsc.dma.channel;

    DSCR_StopCapture();

    dma_channel->CMAR = (uint32_t)dst;
    dma_channel->CNDTR = size;
    dma_channel->CCR |= DMA_CCR_EN;         // Enable DMA Channel
    
    TIMEOUT_TIM->ARR = duration - 1;        // Configure max count as timeout
    TIMEOUT_TIM->EGR = TIM_EGR_UG;          // Reset timer and update registers
    TIMEOUT_TIM->SR &= ~TIM_SR_UIF;         // Ignore initial update event
    TIMEOUT_TIM->SMCR = TIM_SMCR_SMS_TRIGGER_MODE;      // Configure slave mode, valid for TIM1 and TIM15

    dsc.cb = cb;
    
    dsc.tim->EGR = TIM_EGR_UG;              // Reset timer and update registers
    //dsc.tim->DIER |= TIM_DIER_CC1IE;        // Enable capture interrupt, when not in master/slave timers
    dsc.tim->CR1 |= TIM_CR1_CEN;            // Start capture timer
    dsc.tim->CCER |= TIM_CCER_CC1E;         // Enable channel 1 capture
}

/**
 * @brief Waits for a capture to end
 * 
 * \retval : number of remaining capture values
 *  */
uint32_t DSCR_WaitCapture(void){
    while(dsc.tim->CR1 & TIM_CR1_CEN);
    return ((DMA_Channel_TypeDef*)dsc.dma.channel)->CNDTR;
}

/**
 * @brief Stop current capture by disabling timer and dma
 * 
 *  */
void DSCR_StopCapture(void){
    dsc.tim->CCER &= ~TIM_CCER_CC1E;        // Disable channel 1
    dsc.tim->CR1 &= ~TIM_CR1_CEN;           // Stop capture timer
    ((DMA_Channel_TypeDef*)
    dsc.dma.channel)->CCR &= ~DMA_CCR_EN;   // Disable DMA
}

/**
 * @brief Reproduces a signal from buffer
 * 
 * \param src : Pointer to source buffer
 * \param size : size of buffer
 * */
void DSCR_Replay(uint16_t *src, uint32_t size){
    DMA_Channel_TypeDef *dma_channel = (DMA_Channel_TypeDef*)dsr.dma.channel;
    
    if(size == 0){
        return;
    }    

    dsr.tim->CCER &= ~TIM_CCER_CC4E;        // Disable channel 4 capture
    dsr.tim->CR1 &= ~TIM_CR1_CEN;           // Stop capture timer

    dma_channel->CCR &= ~DMA_CCR_EN;        // Disable DMA
    dma_channel->CMAR = (uint32_t)src;
    dma_channel->CNDTR = size;
    dma_channel->CCR |= DMA_CCR_EN;         // Enable DMA Channel
    
    TIMEOUT_TIM->SMCR = 0;                  // Disable slave mode, valid for TIM1 and TIM15

    dsr.tim->EGR = TIM_EGR_UG | TIM_EGR_CC4G;// Reset timer and update registers
    dsr.tim->CCER |= TIM_CCER_CC4E;         // Enable channel 4
    dsr.tim->CR1 |= TIM_CR1_CEN;            // Start capture timer
    LED_ON;
}

/**
 * @brief Static callback to application with remaning captures
 * */
static void DSCR_CallBack(void){
    if(dsc.cb != NULL){
        // Call callback with remaining captures
        dsc.cb(((DMA_Channel_TypeDef*)dsc.dma.channel)->CNDTR);
    }
}
/** 
 * Interrupt Handlers
 * */

// Required for TIM2 and TIM15, this handler is called to on first 
// capture to enable TIM15 (Timeout).
// When using TIM1, TIM15 is started by master/slave mode.
void DSCR_TimHandler(void){
    uint16_t st = CAP_TIM->SR;
    //CC1IF when read is never set because DMA clears it, however
    // UIF is set on first capture
    if(st & TIM_SR_UIF){
        CAP_TIM->DIER &= ~TIM_DIER_CC1IE;   // Disable update interrupt
        TIMEOUT_TIM->CR1 |= TIM_CR1_CEN;    // Start timeout timer
        LED_ON;
    }
    CAP_TIM->SR ^= st;
}

/**
 * Called by DMA EOT interrupt
 * */
void DSCR_CaptureHandler(void){
    DSCR_StopCapture();
    DSCR_CallBack();
}

/**
 * Called by DMA EOT interrupt
 * */
void DSCR_ReplayHandler(void){
    CLEAR_BIT(dsr.tim->CR1, TIM_CR1_CEN);      // Stop timer
    CLEAR_BIT(dsc.tim->CCER,TIM_CCER_CC4E);    // Disable channel 4
    ((DMA_Channel_TypeDef*)
    dsr.dma.channel)->CCR &= ~DMA_CCR_EN;      // Disable DMA
    LED_OFF;
}

/**
 * Called by tim ug event interrupt
 * */
void DSCR_TimeoutHandler(void){
    if((TIMEOUT_TIM->SR & TIM_SR_UIF) != 0){
        DSCR_StopCapture();
        TIMEOUT_TIM->SR &= ~TIM_SR_UIF;     // Clear update event flag
        LED_OFF;
        DSCR_CallBack();
    }
}