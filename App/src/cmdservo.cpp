#include "cmdservo.h"
#include "board.h"

void CmdServo::help(void){
    console->print("Usage: servo <900 - 2100> \n");
    console->print("Generate RC Servo signal on pin PB9\n"); 
}

char CmdServo::execute(int argc, char **argv){
int32_t curValue;

    if(!ia2i(argv[1], &curValue)){
        help(); 
        return CMD_BAD_PARAM;
    }

    SERVO_SetPulse(curValue);
    return CMD_OK;
}