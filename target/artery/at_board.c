#include <sys/times.h>
#include "board.h"
#include "spi.h"
#include "serial.h"
#include "drvlcd.h"
#include "stimer.h"

drvlcdspi_t lcd0;
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

void BOARD_LCD_Init(void){
    lcd0.spidev.bus = SPI_BUS3;
    lcd0.w = 128;
    lcd0.h = 160;
    lcd0.cs = LCD_CS;
    lcd0.cd = LCD_CD;
    lcd0.bkl = LCD_BKL;
    lcd0.rst = 255;

    LCD_Bkl(0);
    GPIO_Config(LCD_BKL, GPO_MS);
    GPIO_Config(LCD_CD, GPO_MS);
    GPIO_Config(LCD_CS, GPO_MS);

    LCD_Init(&lcd0);
}


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
