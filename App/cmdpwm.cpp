#include "board.h"
#include "cmdpwm.h"


void CmdPwm::help(void){
    console->putString("Usage: pwm <option> [params] \n");    
    console->putString("options:");
    console->putString("  start <period>,    Start pwm with period [us]");
    console->putString("  period <period>,   Change pwm frequency through period [us]");
    console->putString("  set <ch> <duty>,   Set duty(0 -100) for channel (1 - 6)");
    console->putString("  enable <ch>,       Enable channel (1 - 6)");
    console->putString("  disable <ch>,      Disable channel (1 - 6)");
    console->putString("  pol <ch> <pol>,    Polarity (1 - 0)\n");
}

void CmdPwm::start(uint32_t period){
    PWM_Init(period);
    ch_en |= (1 << 7);
}

void CmdPwm::enable(uint8_t ch){
    if( (ch_en & (1 << 7)) == 0){
        start(1000);
    }

    PWM_Enable(ch);
    ch_en |= (1 << ch);
}

void CmdPwm::disable(uint8_t ch){
    PWM_Disable(ch);
    ch_en &= ~(1 << ch);
}

char CmdPwm::execute(int argc, char **argv){
    int32_t val1, val2;

    if(argc == 1){
        help();
        return CMD_OK;
    }

    if(xstrcmp("start", (const char*)argv[1]) == 0){
        if(yatoi(argv[2], &val1)){
            start(val1);
            return CMD_OK;
        }
    }

    if(xstrcmp("period", (const char*)argv[1]) == 0){
        if(yatoi(argv[2], &val1)){
            PWM_Freq(val1);
            return CMD_OK;
        }
    }

    if(xstrcmp("enable", (const char*)argv[1]) == 0){
        if(yatoi(argv[2], &val1)){
            enable(val1);
            return CMD_OK;
        }
    }

    if(xstrcmp("disable", (const char*)argv[1]) == 0){
        if(yatoi(argv[2], &val1)){
            disable(val1);
            return CMD_OK;
        }
    }

    if(xstrcmp("set", (const char*)argv[1]) == 0){
        if(yatoi(argv[2], &val1)){
            if(yatoi(argv[2], &val2)){
                PWM_Set(val1, val2);
                return CMD_OK;
            }
        }
    }

    if(xstrcmp("get", (const char*)argv[1]) == 0){
        if(yatoi(argv[2], &val1)){            
            val2 = PWM_Get(val1);
            console->print("PWM%d = %d\n", val1, val2);
            return CMD_OK;            
        }
    }

    if(xstrcmp("pol", (const char*)argv[1]) == 0){
        if(yatoi(argv[2], &val1)){
            if(yatoi(argv[3], &val2)){
                PWM_Polarity(val1, val2);
                return CMD_OK;
            }
        }
    }
   
    return CMD_BAD_PARAM;
}
