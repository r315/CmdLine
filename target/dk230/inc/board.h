#ifndef _board_h_
#define _board_h_


#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include "gd32e23x_gpio.h"

#define LED1_PIN_INIT  \
        rcu_periph_clock_enable(RCU_GPIOA); \
        gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_8);

#define LED1_OFF    gpio_bit_write(GPIOA, GPIO_PIN_8, SET)
#define LED1_ON     gpio_bit_write(GPIOA, GPIO_PIN_8, RESET)
#define LED1_TOGGLE gpio_bit_toggle(GPIOA, GPIO_PIN_8)

#define I2C_MAX_ITF     2

void board_init(void);
void DelayMs(uint32_t ms);
uint32_t ElapsedTicks(uint32_t start_ticks);
uint32_t GetTick(void);
void SW_Reset(void);
void __debugbreak(void);

void board_config_output(uint32_t frequency);
int32_t board_trim_irc(int8_t adj);

#ifdef __cplusplus
}
#endif

#endif