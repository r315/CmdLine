#include <console.h>
#include "board.h"
#include "cmdecho.h"
#include "cmdhelp.h"
#include "cmdmem.h"

#if defined(__BB__)
//#include "vcom.h"
#include <console.h>
#include "cmdhelp.h"
#include "cmdecho.h"
#include "cmdspi.h"
#include "cmdavr.h"
#include "cmdreset.h"
#include "cmdsd.h"
/*#include "cmdpwm.h"
#include "cmdgpio.h"
#include "cmdled.h"
#include "cmdmem.h"
#include "cmdi2c.h"
#include "cmdtest.h"
#include "cmdawg.h"
*/
#include "cmdsbus.h"

#else

#endif

extern StdOut vcp;

#if defined(__BB__)

enum {
    MODE_LPCBUS = 0,
    MODE_STK500
};

//void abort(void){}
void stk500_ServiceInit(StdOut *sp);
void stk500_Service(void);
void lpcBus_Service(void);

typedef struct _option{
	const char *name;
	uint8_t selected;
}option_type;


option_type options[] = {
	{"LPC Bus",TRUE},
	{"stk500", FALSE},
};


void printOptions(option_type *opt, uint8_t nopt){
	DISPLAY_Goto(0,0);
	for(uint8_t i = 0; i < nopt; i++){
		if(opt[i].selected) DISPLAY_SetColors(WHITE, YELLOW);
		else DISPLAY_SetColors(WHITE, BLUE);
		DISPLAY_printf("%s\n", opt[i].name);
	}
}

uint8_t selectedOption(option_type *opt, uint8_t nopt){
uint8_t i;

	for(i = 0; i < nopt; i++){
		if(opt[i].selected)
			break;
	}

	return i;
}

uint8_t checkOptions(option_type *opt, uint8_t nopt){
uint8_t curSelected = selectedOption(opt, nopt);;

	if(BUTTON_Read() == BUTTON_PRESSED){
        switch(BUTTON_GetValue()){
            case BUTTON_UP:
                if(curSelected > 0){
                    opt[curSelected].selected = FALSE;
                    curSelected--;
                    opt[curSelected].selected = TRUE;
                }
                break;
            case BUTTON_DOWN:
                if(curSelected < nopt - 1){
				    opt[curSelected].selected = FALSE;
			    	curSelected++;
				    opt[curSelected].selected = TRUE;
			    }
                break;

        }        
		printOptions(opt, nopt);
	}
    return curSelected;
}


void App(void){
    
    uint8_t mode;
    Console console;

    vcp.init();    
    console.init(&vcp, "LPC BUS>");    
    
	CmdEcho echo;
    CmdHelp help;
    CmdSpi spi;
    CmdAvr avr;
    //CmdSbus sbus;
    CmdReset rst;
    CmdSd sd;

    console.addCommand(&help);
    console.addCommand(&echo);
    console.addCommand(&spi);
    console.addCommand(&avr);
    //console.addCommand(&sbus);
    console.addCommand(&rst);
    console.addCommand(&sd);

	/*CmdPwm pwm();
	CmdGpio gpio();
	CmdLed led();
	CmdMem mem();
	CmdI2c i2c();
    CmdTest test();
    CmdAwg awg();
    CmdSbus sbus();*/
	
	//console.addCommand(&echo);
	/*console.addCommand(&pwm);
	console.addCommand(&gpio);
	console.addCommand(&led);
	console.addCommand(&mem);
	console.addCommand(&i2c);
    console.addCommand(&spi);
    console.addCommand(&avr);
    console.addCommand(&test);
    console.addCommand(&awg);
    console.addCommand(&sbus);
    */   

    stk500_ServiceInit(&vcp);
  
	LCD_Clear(BLACK);

    printOptions(options, sizeof(options) / sizeof(option_type));
    
	while(1)
	{	
		
		do{
            mode = checkOptions(options, sizeof(options) / sizeof(option_type));
			switch(mode){
                case MODE_LPCBUS:
                    console.process();
                    break;
                case MODE_STK500:
                    stk500_Service();
                    break;
            }
                      
		}while(TRUE);		

		
	}	
}

#elif defined(__NUCLEO_L412KB__)
void App(void){

	//uint8_t data[10];

	Console con;
	CmdEcho echo;
	CmdHelp help;
	CmdMem mem;
	
	vcp.init();
	con.init(&vcp,"nucleo>");
	con.addCommand(&echo);
	con.addCommand(&help);
	con.addCommand(&mem);


	while(1){
		 //HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
		 //con.print("Test\n");
		con.process();
		 HAL_Delay(100);
	}
}
#endif
