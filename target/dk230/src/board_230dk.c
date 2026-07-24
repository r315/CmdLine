#include <stdint.h>
#include "board.h"
#include "serial.h"
#include "i2c.h"
#include "system_gd32e23x.h"

static volatile uint32_t ticms;

#ifdef ENABLE_I2C
i2cbus_t board_i2cbus;
#endif

void SysTick_Handler(void){
    ticms++;
    //LED1_TOGGLE;
}

void DelayMs(uint32_t ms){
    volatile uint32_t end = ticms + ms;
    while (ticms < end){ }
}

uint32_t ElapsedTicks(uint32_t start_ticks){
	int32_t delta = GetTick() - start_ticks;
    return (delta < 0) ? -delta : delta;
}

inline uint32_t GetTick(void)
{
    return ticms;
}

void board_init(void)
{

    LED1_PIN_INIT;

    //SystemInit();

    system_clock_config();

	SystemCoreClockUpdate();

	SysTick_Config(SystemCoreClock / 1000);

    SERIAL_Init();

    //I2C_Init(I2C_BUS0);
}

void SW_Reset(void){
    NVIC_SystemReset();
}

void __debugbreak(void){
	 asm volatile
    (
        "bkpt #01 \n"
    );
}

void system_clock_config(void)
{
    system_clock_72m_irc8m();
}

void board_config_output(uint32_t frequency)
{
    rcu_periph_clock_enable(RCU_TIMER2);
    uint32_t clock = rcu_clock_freq_get(CK_APB1);

    timer_deinit(TIMER2);
    timer_prescaler_config(TIMER2, (clock / 1000000UL) - 1, TIMER_PSC_RELOAD_UPDATE);
    timer_autoreload_value_config(TIMER2, frequency - 1);

    timer_channel_output_pulse_value_config(TIMER2, TIMER_CH_1, (frequency >> 1) - 1);
    timer_channel_output_mode_config(TIMER2, TIMER_CH_1, TIMER_OC_MODE_PWM0);
    timer_channel_output_state_config(TIMER2, TIMER_CH_1, TIMER_CCX_ENABLE);

    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_7);
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_7);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_7);

    timer_enable(TIMER2);
#if 0

    crm_periph_clock_enable(CRM_TMR5_PERIPH_CLOCK, TRUE);

    TMR5->ctrl1 = 0;
    TMR5->div = (clocks.apb1_freq / 500UL) - 1;
    TMR5->pr = 1000UL -1;
    TMR5->c1dt = 500 - 1;
    TMR5->cctrl_bit.c1en = 1;
    TMR5->cm1_output_bit.c1c = 0; // output
    TMR5->cm1_output_bit.c1octrl = 6; // PWM

    TMR5->ctrl1_bit.tmren = 1;

    GPIOA->cfglr = (GPIOA->cfglr & ~(0x0F << 0)) | (0x0A << 0); // PA0
#endif
}

#if 0
typedef struct {
    union{
        volatile uint32_t CTL0;
        struct {
            volatile uint32_t IRC8MEN : 1;
            volatile uint32_t IRC8MSTB : 1;
            volatile uint32_t rsvd0 : 1;
            volatile uint32_t IRC8MADJ : 5;
            volatile uint32_t IRC8MCALIB : 8;
            volatile uint32_t HXTALEN : 1;
            volatile uint32_t HXTALSTB : 1;
            volatile uint32_t HXTALBPS : 1;
            volatile uint32_t CLKMEN : 1;
            volatile uint32_t rsvd1 : 4;
            volatile uint32_t PLLEN : 1;
            volatile uint32_t PLLSTB : 1;
        };
    }CTL0_BIT;
    volatile uint32_t CFG0;
    volatile uint32_t INT;
    volatile uint32_t APB2RST;
    volatile uint32_t APB1RST;
    volatile uint32_t AHBEN;
    volatile uint32_t APB2EN;
    volatile uint32_t APB1EN;
    volatile uint32_t BDCTL;
    volatile uint32_t AHBRST;
    volatile uint32_t CFG1;
    volatile uint32_t CFG2;
    volatile uint32_t CTL1;
    uint32_t rsvd[0x100 - 0x34];
    volatile uint32_t VKEY;
    volatile uint32_t DSV;
}Rcu_Type;

#define RCU     ((Rcu_Type *) RCU_BASE)

int32_t board_trim_irc(int8_t adj)
{
    uint16_t irc8mcal = RCU->CTL0_BIT.IRC8MCALIB;
    uint8_t irc8madj = RCU->CTL0_BIT.IRC8MADJ;

    if(adj > 0){
        irc8madj += adj;

        if(irc8madj > 31){
            if(irc8mcal < 256) {
                irc8mcal++;
                RCU->CTL0_BIT.IRC8MCALIB = irc8mcal;
                irc8madj = 16;
            }else{
                goto exit;
            }
            RCU->CTL0_BIT.IRC8MADJ = irc8madj;
            goto exit;
        }

        RCU->CTL0_BIT.IRC8MADJ = irc8madj;
    }

    if(adj < 0){
        irc8madj += adj;

        if(irc8madj < 0){
            if(irc8mcal > 0) {
                irc8mcal--;
                RCU->CTL0_BIT.IRC8MCALIB = irc8mcal;
                irc8madj = 16;
            }else{
                goto exit;
            }
            RCU->CTL0_BIT.IRC8MADJ = irc8madj;
            goto exit;
        }

        RCU->CTL0_BIT.IRC8MADJ = irc8madj;
    }

exit:
    return irc8mcal << 8 | irc8madj;
}
#endif