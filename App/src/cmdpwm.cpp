#include "board.h"
#include "pwm.h"
#include "cmdpwm.h"

void CmdPwm::init(void *params)
{
    console = static_cast<Console*>(params);
    pwmchip.frequency = 60;
    BOARD_PWM_Init(&pwmchip);
}

void CmdPwm::help(void){
    console->println("Usage: pwm [freq|duty|period|pulse|enable|pol] \n");
    console->println("  freq [value],       Get/Set value 16 - 250000 [Hz]");
    console->println("  duty <ch> [value],  Get/Set value 0 - 100");
    console->println("  period [period],    Get/Set period 0 - 66535");
    console->println("  pulse <ch> [value], Get/Set 0 < value < time");
    console->println("  enable <ch> [0|1],  Get/set channel enable");
    console->println("  pol <ch> [0|1],     channel polarization");
}

char CmdPwm::execute(int argc, char **argv){
    int32_t val1, val2;

    if(argc == 1){
        help();
        return CMD_OK;
    }

    if(!xstrcmp("freq", (const char*)argv[1])){
        if(ia2i(argv[2], &val1)){
            console->printf("Number of ticks: %d\n", PWM_FrequencySet(&pwmchip, (uint32_t)val1));
        }else{
            console->printf("PWM Frequecy: %dHz\n", pwmchip.frequency);
        }
        return CMD_OK;
    }

    if(!xstrcmp("duty", (const char*)argv[1])){
        if(ia2i(argv[2], &val1)){
            if(ia2i(argv[3], &val2)){
                PWM_DutySet(&pwmchip, (uint8_t)val1, (uint8_t)val2);
            }else{
                console->printf("PWM[%d]: %d\n", val1, PWM_DutyGet(&pwmchip, val1));
            }
            return CMD_OK;
        }
    }

    if(!xstrcmp("period", (const char*)argv[1])){
        if(ia2i(argv[2], &val1)){
            PWM_PeriodSet(&pwmchip, (uint32_t)val1);
        }else{
            console->printf("PWM period: %dticks\n", PWM_PeriodGet(&pwmchip));
        }
        return CMD_OK;
    }

    if(!xstrcmp("pulse", (const char*)argv[1])){
        if(ia2i(argv[2], &val1)){
            if(ia2i(argv[3], &val2)){
                PWM_PulseSet(&pwmchip, (uint8_t)val1, (uint32_t)val2);
            }else{
                console->printf("PWM[%d]: %d\n", val1, PWM_PulseGet(&pwmchip, val1));
            }
            return CMD_OK;
        }
    }

    if(!xstrcmp("enable", (const char*)argv[1])){
        if(ia2i(argv[2], &val1)){
            if(ia2i(argv[3], &val2)){
                PWM_Enable(&pwmchip, (uint8_t)val1, (enum pwmpstate)val2);
            }
            return CMD_OK;
        }
    }

    if(xstrcmp("pol", (const char*)argv[1]) == 0){
        if(ia2i(argv[2], &val1)){
            if(ia2i(argv[3], &val2)){
                PWM_Polarity(&pwmchip, val1, val2);
                return CMD_OK;
            }
        }
    }

    return CMD_BAD_PARAM;
}
