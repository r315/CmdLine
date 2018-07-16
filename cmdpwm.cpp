#include "cmdpwm.h"


void CmdPwm::help(void){
    vcom->printf("Usage: pwm [option] \n\n");  
    vcom->printf("\t -s, start pwm\n");
    vcom->printf("\t -d duty, duty cycle 0-100 \n");
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
    
   
    p1 = strtok_s((char*)ptr, ' ', COMMAND_MAX_LINE - 10, &p2);

    if( p1 == NULL){
        help();
        return CMD_OK;
    }


    arg = *((uint8_t*)ptr) << 8;
    arg |= *((uint8_t*)(ptr+1));

    switch(arg){
        case '-' *256 + 's':
            start();
            break;

         case '-' *256 + 'd':
            arg = yatoi(p2);
            if(arg >= 0 && arg < 101){
                if (running == OFF){
                    start();
                }
                PWM_Set(1, 50);
            }
            break;

        default:
            break;
    }
    return CMD_OK;
}