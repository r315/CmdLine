#ifndef _LA_H_
#define _LA_H_

#include <stdint.h>

/**
 * DMA operates a maximum of 25MHz with
 * CPU overclocked to 250MHz.
 * DMA requires some clocks to perform transference
 * thus it is not possible to have a transference on
 * every clock of timer when it only divides sclk by 1.
 *
 * However dividing sclk by 10, gives at least 10 clocks
 * for DMA. It could be fewer cycles but DS does not specify
 * this parameter
 *
 */
#define SAMPLER_DMA

#define SAMPLER_NUM_OF_CHANNELS     8
#define SAMPLER_MEMORY_SIZE         0x4000

/**
 * The highest sample rate in Hz.
 */
#ifdef SAMPLER_DMA
#define SAMPLER_MAX_SAMPLE_RATE     25000000UL
#else
#define SAMPLER_MAX_SAMPLE_RATE     1000000UL
#endif

#define TIMER_SAMPLER               TMR2

#ifdef __cplusplus
extern "C" {
#endif

void sampler_init(void);
void sampler_set_samples(uint32_t n);
void sampler_set_triggers(uint32_t mask);
void sampler_set_rate(uint32_t n);
void sampler_reset(void);
void sampler_arm(void);
void sampler_abort(void);
uint8_t sampler_is_triggered(void);
uint32_t sampler_acquire_samples(void);
const uint8_t *sampler_get_samples(void);

#ifdef __cplusplus
}
#endif

#endif