
#include "board.h"
#include "buzzer.h"

#include <console.h>
#include "cmdecho.h"
#include "cmdhelp.h"
#include "cmdmem.h"
#include "cmdrfinder.h"
#include "cmdadc.h"
#include "cmdrst.h"
#include "cmdservo.h"
#include "cmdspiflash.h"
#include "cmdflashrom.h"
#include "cmdbuz.h"
#include "cmdversion.h"
#include "cmdkeyfob.h"
#include "cmdpwm.h"
#include "cmdtft.h"
#include "cmdencoder.h"


StdOut *userio = BOARD_STDIO;

#if defined (BOARD_BLUEBOARD)
#include "cmdspi.h"
#include "cmdavr.h"
#include "cmdreset.h"
#include "cmdsd.h"
#include "cmdsbus.h"
#include "cmdi2s.h"

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
uint8_t curSelected = selectedOption(opt, nopt);

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

#ifdef ENABLE_DEBUG
extern "C" void dbg_putc(char c){
    userio->xputchar(c);
}
#endif

extern "C" void App(void){
    
    uint8_t mode;
    Console console;

    //userio->init();    
    console.init(userio, "Blueboard>");    
    
	CmdEcho echo;
    CmdHelp help;
    CmdSpi spi;
    CmdAvr avr;
    CmdSbus sbus;
    CmdReset rst;
    CmdSd sd;
	CmdPwm pwm;
	CmdVersion ver;
	//CmdGpio gpio();
	//CmdMem mem();
	//CmdI2c i2c();
    //CmdAwg awg();
	CmdTft tft;
	CmdI2s i2s;

    console.addCommand(&help);
    console.addCommand(&echo);
    console.addCommand(&spi);
    console.addCommand(&avr);
    console.addCommand(&sbus);
    console.addCommand(&rst);
    console.addCommand(&sd);	
	//console.addCommand(&echo);
	console.addCommand(&pwm);
	//console.addCommand(&gpio);
	//console.addCommand(&led);
	//console.addCommand(&mem);
	//console.addCommand(&i2c);
    //console.addCommand(&spi);
    //console.addCommand(&avr);
    //console.addCommand(&test);
    //console.addCommand(&awg);
	console.addCommand(&ver);
	console.addCommand(&tft);
	console.addCommand(&i2s);

    stk500_ServiceInit(userio);
  
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

#elif defined (BOARD_NUCLEO_L412KB)
extern "C"
void App(void){
	
	Console con;
	CmdHelp help;
	CmdMem mem;
	CmdBuz buz;
	CmdVersion ver;
	CmdKeyFob keyfob;
	CmdPwm pwm;
	CmdTft tft;

	con.init(userio,"nucleo>");
	con.addCommand(&help);
	con.addCommand(&ver);
	con.addCommand(&mem);
	con.addCommand(&buz);
	con.addCommand(&keyfob);
	con.addCommand(&pwm);
	con.addCommand(&tft);

	con.cls();
	con.print("Freq: %dMHz\n", SystemCoreClock / 1000000);

	LED_OFF;
	buzInit();
	buzSetLevel(50);
	buzPlayRtttl("rtttl_14:d=16,o=6,b=180:c,e,g");

	while(1){
		con.process();
	}
}
#elif defined (BOARD_BLUEPILL)
void App(void){

	Console con;
	CmdEcho echo;
	CmdHelp help;
	CmdMem mem;
	CmdRfinder rfinder;
	CmdAdc adc;
	CmdRst rst;
	CmdServo servo;
	CmdSpiFlash spiflash;
	CmdFlashRom flashrom;
	CmdTft tft;
	CmdEncoder enc;

	userio->init();
	con.init(userio,"bluepill>");
	con.addCommand(&echo);
	con.addCommand(&help);
	con.addCommand(&mem);
	con.addCommand(&rfinder);
	con.addCommand(&adc);
	con.addCommand(&rst);
	con.addCommand(&servo);
	con.addCommand(&spiflash);
	con.addCommand(&flashrom);
	con.addCommand(&tft);
	con.addCommand(&enc);

	while(1){
		 //HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
		 //con.print("Test\n");
		con.process();
		 HAL_Delay(100);
	}
}
#endif
