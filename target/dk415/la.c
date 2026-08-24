#include "la.h"
#include "at32f4xx.h"
#include "clock.h"
#include "dma.h"

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
static uint32_t samples_buffer[SAMPLER_MAX_SAMPLES];


/**
 * @brief
 * @param
 */
void sampler_init(void)
{
    CLOCK_Enable(TIMER_SAMPLER, ENABLE);
    TIMER_SAMPLER->DIV = (CLOCK_Get(CLOCK_CLK4) / SAMPLER_MAX_SAMPLE_RATE) - 1;
    TIMER_SAMPLER->DIE = TMR_DIE_UEVIE;
}

void sampler_set_samples(uint32_t n)
{
    if(n > SAMPLER_MAX_SAMPLES){
        n = SAMPLER_MAX_SAMPLES;
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
    if(rate > SAMPLER_MAX_SAMPLE_RATE){
        rate = SAMPLER_MAX_SAMPLE_RATE;
    }

    TIMER_SAMPLER->AR = SAMPLER_MAX_SAMPLE_RATE / rate;
}

uint8_t sampler_is_triggered(void)
{
    return 1;
}

void sampler_reset(void)
{
    TIMER_SAMPLER->CTRL1 = 0;
    /* Initialize the sampler. */
    sampler_state = SAMPLER_IDLE;
    trigger_armed = 0;
    sampler_set_samples(SAMPLER_MAX_SAMPLES);
    sampler_set_rate(1000);
}

void sampler_arm(void)
{
    TIMER_SAMPLER->CTRL1 = 0;
    TIMER_SAMPLER->CNT = 0;
    TIMER_SAMPLER->STS = 0;
    /* Update sampler state. */
    sampler_state = SAMPLER_ARMED;
}

void sampler_abort(void)
{

}

const uint32_t *sampler_get_samples(void)
{
    return samples_buffer;
}

uint32_t sampler_acquire_samples(void)
{
    switch (sampler_state) {
        /* Can start sampling. */
        case SAMPLER_ARMED: {
            uint32_t offset = samples_to_acquire;

            /* Skip if no trigger set. */
            if (!sampler_is_triggered() && trigger_armed) {
                break;
            }

            /* Take samples. */

            /* Start timer */
            TIMER_SAMPLER->CTRL1 = TMR_CTRL1_CNTEN;
            /* Clear timer interrupt flag. */
            TIMER_SAMPLER->STS = 0;
            /* Capture samples into the terminal buffer. */
            while(offset > 0) {
                offset--;
                samples_buffer[offset] = offset; //GPIOA->IPTDT;
                /* Wait for timer interrupt to trigger. */
                while ((TIMER_SAMPLER->STS & TMR_STS_UEVIF) == 0) {}
                TIMER_SAMPLER->STS = 0;
            }

            /* Stop timer  */
            TIMER_SAMPLER->CTRL1 = 0;
            /* Reset the analyzer state. */
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
