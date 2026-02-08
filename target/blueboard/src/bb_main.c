#include "board.h"
#include "clock_lpc17xx.h"
#include "tim_lpc17xx.h"
#include "app.h"

#if defined(ENABLE_DISPLAY)
void BOARD_LCD_Init(){
    BB_LCD_Init();
}
#endif

#ifdef ENABLE_PWM
void BOARD_PWM_Init(pwmchip_t *pwmchip)
{
    #warning "PWM init not implemented for blueboard"
    /* pwmchip->chip = 3;
    PWM_Init(pwmchip);

    GPIO_Config(PB_0, GPO_HS_AF);
    GPIO_Config(PB_1, GPO_HS_AF); */
}
#endif


int main()
{
    BOARD_Init();

    CLOCK_InitUSBCLK();
    SERIAL_Init();

    TIM_InitMatch(LPC_TIM0);    // Use TIMER0 as timebase for generic timer


    //LCD_SetOrientation(LCD_LANDSCAPE);

    DBG_PIN_INIT;

    App();

	return 0;
}
