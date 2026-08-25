#include "la.h"
#include "at32f4xx.h"
#include "clock.h"
#include "dma.h"
#include "dma_at32f4xx.h"

/**
 * Sampler states.
 */
typedef enum {
    /** Sampler is not armed, no acquisition is in progress. */
    SAMPLER_IDLE = 0,
    /** Sampler is either ready for acquisition or is currently acquiring. */
    SAMPLER_ARMED
} sump_sampler_state_t;


/**
 * The current state of the sampler.
 */
static sump_sampler_state_t sampler_state = SAMPLER_IDLE;
static uint32_t samples_to_acquire;
static uint8_t trigger_armed;
static uint8_t samples_buffer[SAMPLER_MEMORY_SIZE];
#ifdef SAMPLER_DMA
static dmatype_t sampler_dma;
static volatile uint32_t done;
static void sampler_eos_handler(void)
{
    done = 1;
}
#endif
/**
 * @brief
 * @param
 */
void sampler_init(void)
{
    CLOCK_Enable(TIMER_SAMPLER, ENABLE);
    TIMER_SAMPLER->DIV = 0;
    // Sample time given by AR division
    TIMER_SAMPLER->AR = (CLOCK_Get(CLOCK_CLK4) / SAMPLER_MAX_SAMPLE_RATE) - 1;
    // Trigger DMA on UP event
    TIMER_SAMPLER->DIE = TMR_DIE_UEVDE;
    // Plus mode
    TIMER_SAMPLER->CTRL1 = TMR_CTRL1_PMEN;

    GPIOA->CTRLL = 0x44444444;
#ifdef SAMPLER_DMA
    sampler_dma.dir = DMA_DIR_P2M;
    sampler_dma.src = (void*)&GPIOA->IPTDT;
    sampler_dma.ssize = DMA_DATA_SIZE_8;
    sampler_dma.dst = (void*)samples_buffer;
    sampler_dma.dsize = DMA_DATA_SIZE_8;
    sampler_dma.single = 1;
    sampler_dma.eot = sampler_eos_handler;
    DMA_Config(&sampler_dma, DMA1_REQ_TIM2_UP);
#endif
    sampler_set_samples(SAMPLER_MEMORY_SIZE);
    sampler_set_rate(200000);
}

void sampler_reset(void)
{
    /* Stop timer */
    TIMER_SAMPLER->CTRL1 &= ~TMR_CTRL1_CNTEN;
    /* Reset counter */
    TIMER_SAMPLER->CNT = 0;
#ifdef SAMPLER_DMA
    DMA_Cancel(&sampler_dma);
#endif
    /* Initialize the sampler. */
    sampler_state = SAMPLER_IDLE;
    trigger_armed = 0;
}

void sampler_set_samples(uint32_t n)
{
    if(n > SAMPLER_MEMORY_SIZE){
        n = SAMPLER_MEMORY_SIZE;
    }

    samples_to_acquire = n;
}

void sampler_set_triggers(uint32_t mask)
{
    (void)mask;
    trigger_armed = 1;
}

void sampler_set_rate(uint32_t rate)
{
    uint32_t ck_int;

    if(rate > SAMPLER_MAX_SAMPLE_RATE){
        rate = SAMPLER_MAX_SAMPLE_RATE;
    }

    ck_int = CLOCK_Get(CLOCK_CLK4);

    if(rate < ck_int)
        rate = (ck_int / rate) - 1;
    else
        rate = 1;

    TIMER_SAMPLER->AR = rate;
}

uint8_t sampler_is_triggered(void)
{
    return 1;
}


void sampler_arm(void)
{
#ifdef SAMPLER_DMA
    sampler_dma.len = samples_to_acquire;
    DMA_Start(&sampler_dma);
#endif
    TIMER_SAMPLER->STS = 0;
    /* Update sampler state. */
    sampler_state = SAMPLER_ARMED;
}

void sampler_abort(void)
{

}

const uint8_t *sampler_get_samples(void)
{
    #if 0
    uint32_t len = samples_to_acquire;
    // Expand 8 channels to 32 channels sample
    uint8_t *last_sample = ((uint8_t*)samples_buffer) + len;
    uint32_t *psample = samples_buffer + SAMPLER_MEMORY_SIZE;

    while(len--){
        psample--;
        last_sample--;
        *psample = (uint32_t)(*last_sample);
    }
    #endif
    return samples_buffer;
}

uint32_t sampler_acquire_samples(void)
{
    switch (sampler_state) {
        /* Can start sampling. */
        case SAMPLER_ARMED: {
            /* Skip if no trigger set. */
            if (!sampler_is_triggered() && trigger_armed) {
                break;
            }

            /* Take samples. */

            /* Start timer */
            TIMER_SAMPLER->CTRL1 |= TMR_CTRL1_CNTEN;
        #ifdef SAMPLER_DMA
            done = 0;
            while(!done) {
                ;
            }
        #else
            uint32_t offset = samples_to_acquire;
            uint8_t *psample = (uint8_t*)samples_buffer;
            /* Clear timer interrupt flag. */
            TIMER_SAMPLER->STS = 0;
            /* Capture samples into buffer. */
            while(offset > 0) {
                offset--;
                psample[offset] = GPIOA->IPTDT;
                /* Wait for timer interrupt to trigger. */
                while ((TIMER_SAMPLER->STS & TMR_STS_UEVIF) == 0) {}
                TIMER_SAMPLER->STS = 0;
            }
        #endif
            /* Stop and reset the sampler state. */
            sampler_reset();
            /* Acquisition complete. */
            return samples_to_acquire;
        }

        case SAMPLER_IDLE:
        default:
            /* Nothing to do. */
        break;
    }

    /* Acquisition was not performed. */
    return 0;
}
