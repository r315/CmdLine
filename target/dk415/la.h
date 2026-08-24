#ifndef _LA_H_
#define _LA_H_

#include <stdint.h>

#define SAMPLER_NUM_OF_CHANNELS     8
#define SAMPLER_MAX_SAMPLES         0x1000
#define TIMER_SAMPLER               TMR10

/**
 * How much memory is allocated for samples, in samples.
 */
#define SAMPLER_MEMORY_SIZE         0x1000

/**
 * The highest sample rate in Hz.
 */
#define SAMPLER_MAX_SAMPLE_RATE     1000000UL


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
const uint32_t *sampler_get_samples(void);

#ifdef __cplusplus
}
#endif

#endif