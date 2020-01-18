#include "board.h"
#include "cmdpwm.h"


void CmdPwm::help(void){
    vcom->printf("Usage: pwm [option] \n\n");  
    vcom->printf("\t -r, run pwm (50%)\n");
    vcom->printf("\t -d duty, duty cycle 0-100 \n");
    vcom->printf("\t -s, disable pwm\n");
    vcom->printf("\t -t, enable pwm\n");
}


void CmdPwm::start(void){
        vcom->printf("1kHz square wave on P2.0 (TXD1) using pwm\n");
        PWM_Init(1000);
	    PWM_Enable(1);
	    PWM_Set(1, 50);
        running = ON;
}

char CmdPwm::execute(void *ptr){
    char *p1, *p2;
    uint16_t arg;
    
    // Check is has parameters
    p1 = strtok_s((char*)ptr, ' ', COMMAND_MAX_LEN - 10, &p2);

    // if not show help
    if( p1 == NULL){
        help();
        return CMD_OK;
    }

    // get first option
    arg = ((uint8_t*)ptr)[0] << 8;
    arg |= ((uint8_t*)(ptr))[1];

    // test first option
    switch(arg){
        case '-' *256 + 's':
             PWM_Disable(1);
            break;

        case '-' *256 + 't':
             PWM_Enable(1);
            break;

        case '-' *256 + 'r':
            start();
            break;

        case '-' *256 + 'd':
            arg = yatoi(p2);
            if(arg >= 0 && arg < 101){
                if (running == OFF){
                    start();
                }
                PWM_Set(1, arg);
            }
            break;

        default:
            break;
    }
    return CMD_OK;
}
