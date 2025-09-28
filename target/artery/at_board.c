#include <sys/times.h>
#include "board.h"
#include "spi.h"
#include "serial.h"
#include "drvlcd.h"
#include "stimer.h"
#include "tone.h"

#ifdef ENABLE_TFT_DISPLAY
static drvlcdspi_t lcd0;
#endif

#if 0
// Dedicated timer for stimer
static void appTimerInit(TMR_Type *tmr)
{
    IRQn_Type irq;
    RCC_ClockType clock;

    switch((uint32_t)tmr){
        case (uint32_t)TMR10:
            RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_TMR10, ENABLE);
            irq = TMR1_OV_TMR10_IRQn;
            break;
        case (uint32_t)TMR11:
            RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_TMR11, ENABLE);
            irq = TMR1_TRG_COM_TMR11_IRQn;
            break;
    }

    RCC_GetClocksFreq(&clock);

    tmr->CTRL1 = 0;
    // When DIV > 1, APB clock doubles
    tmr->DIV = (clock.APB2CLK_Freq/500000) - 1;
    tmr->AR = 1000;
    tmr->CNT = 0;
    tmr->DIE = TMR_DIE_UEVIE;
    NVIC_EnableIRQ(irq);
    tmr->CTRL1 = TMR_CTRL1_CNTEN;
}
//void TMR1_TRG_COM_TMR11_IRQHandler(void)
void TMR1_OV_TMR10_IRQHandler(void)
{
    uint16_t st = TMR10->STS;
    TMR10->STS = ~st;
    STIMER_Handler();
}
#endif

static void InitTimeBase(void)
{
	SysTick_Config((SystemCoreClock / 1000) - 1);
    //appTimerInit(TMR10);
}

#if (USE_TIMER_SYSTICK == 1)
#else
static volatile uint32_t ticms;
void SysTick_Handler(void){
    ticms++;
    STIMER_Handler();
}

void DelayMs(uint32_t ms){
    __IO uint32_t end = ticms + ms;
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
#endif

clock_t clock(void){
    return (clock_t)GetTick();
}

#ifdef ENABLE_TFT_DISPLAY
void BOARD_LCD_Init(void)
{
    lcd0.spidev.bus = SPI_BUS0;
    lcd0.spidev.freq = SPI_FREQ;
    lcd0.w = TFT_W;
    lcd0.h = TFT_H;
    lcd0.cs = LCD_CS;
    lcd0.cd = LCD_CD;
    lcd0.bkl = LCD_BKL;
    lcd0.rst = 255;

    SPI_Init(&lcd0.spidev);

    LCD_Bkl(0);
    GPIO_Config(LCD_BKL, GPO_MS);
    GPIO_Config(LCD_CD, GPO_MS);
    GPIO_Config(LCD_CS, GPO_MS);

    LCD_Init(&lcd0);
}
#endif

void BOARD_Init(void)
{
	SystemInit();
	SystemCoreClockUpdate();

	InitTimeBase();

    RCC->APB2EN |= RCC_APB2EN_GPIOAEN;
    RCC->APB2EN |= RCC_APB2EN_GPIOBEN;

	LED1_PIN_INIT;

    #ifdef ENABLE_DIGITAL_AUDIO
    GPIO_Config(PA_7, GPIO_SPI1_SD);
    GPIO_Config(PA_4, GPIO_SPI1_WS);
    GPIO_Config(PA_5, GPIO_SPI1_CK);
    #endif

    SERIAL_Init();
    #ifdef ENABLE_TFT_DISPLAY
    BOARD_LCD_Init();
    #endif

    #ifdef ENABLE_TONE
    TONE_Init();
    #endif
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

#ifdef ENABLE_TONE
enum tone_e TONE_Init(void)
{

    tone_pwm_init_t init = {
        .tmr = TMR1,
        .ch = 1 - 1,
        .pin = PA_8,
        .pin_idle = 0
    };

    //RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_AFIO, ENABLE);
    //AFIO->MAP4 = (1 << 8);

    return TONE_PwmInit(&init);
}
#endif /* ENABLE_TONE */

#ifdef ENABLE_PWM
void BOARD_PWM_Init(pwmchip_t *pwmchip)
{
    pwmchip->chip = 3; // Timer3
    PWM_Init(pwmchip);

    GPIO_Config(PB_0, GPO_HS_AF);
    GPIO_Config(PB_1, GPO_HS_AF);
}
#endif