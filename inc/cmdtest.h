#ifndef _cmdtest_h_
#define _cmdtest_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "command.h"

void LCD_Command(uint8_t ins) ;

class CmdTest : public Command{

public:
    CmdTest (Vcom *vc) : Command("test", vc) { }
    void help(void){ }
    
    char execute(void *ptr){
        char *p = (char*)ptr;
        uint16_t val; 

        //vcom->printf("Test: LCD ID: %X", LCD_ID());
        val = nextHex(&p);
        LCD_Command(LCD_GATE_SCAN_CTRL1);
	    LCD_Data(val);
        return CMD_OK; 
    }
};


#ifdef __cplusplus
}
#endif

#endif
