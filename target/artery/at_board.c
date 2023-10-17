#include <sys/times.h>
#include "board.h"
#include "spi.h"
#include "serial.h"
#include "drvlcd.h"

drvlcdspi_t lcd0;

static void InitTimeBase(void){
#if (USE_TIMER_SYSTICK == 1)
	LPC_SC->PCONP |= SC_PCONP_PCTIM3;
	LPC_SC->PCLKSEL1 &= ~(3 << 14);
    LPC_SC->PCLKSEL1 |= (PCLK_1 << 14);
	LPC_TIM3->TCR = TIM_TCR_CRST;
	LPC_TIM3->CCR = 0;				// Timer mode
	LPC_TIM3->PR = (SystemCoreClock / 1000 - 1);
	LPC_TIM3->TCR = TIM_TCR_CEN;
#else
	SysTick_Config((SystemCoreClock / 1000) - 1); // config 1000us
#endif
}

#if (USE_TIMER_SYSTICK == 1)
#else
static volatile uint32_t ticms;
void SysTick_Handler(void){
    ticms++;
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
    lcd0.spidev.bus = SPI_BUS1;
    lcd0.w = 128;
    lcd0.h = 160;
    lcd0.cs = LCD_CS;
    lcd0.cd = LCD_CD;
    lcd0.bkl = LCD_BKL;
    lcd0.rst = 255;

    // To use PB3 as SPI2 sclk, SPI2 has to be remapped
    RCC->APB2EN |= RCC_APB2EN_AFIOEN;
    AFIO->MAP = AFIO_MAP_SWJTAG_CONF_JTAGDISABLE;
    AFIO->MAP5 = AFIO_MAP5_SPI2_GRMP;

    GPIO_Config(LCD_SCLK, GPIO_SPI2_SCK);
    GPIO_Config(LCD_DI, GPIO_SPI2_MOSI);
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

    //spibus.bus = SPI_BUS0;
    //spibus.freq = SPI_FREQ;
    //spibus.flags = SPI_HW_CS;
    //SPI_Init(&spibus);

	LED1_PIN_INIT;

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
