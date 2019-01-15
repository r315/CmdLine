#include "board.h"

#include "vcom.h"
#include "cmdecho.h"
#include "cmdpwm.h"
#include "cmdgpio.h"
#include "cmdled.h"
#include "cmdmem.h"
#include "cmdi2c.h"
#include "cmdspi.h"
#include "cmdavr.h"
#include "cmdtest.h"
#include "cmdawg.h"
#include "cmdsbus.h"


enum {
    MODE_LPCBUS = 0,
    MODE_STK500
};

//void abort(void){}
void stk500_ServiceInit(Vcom *vc);
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

void lpcBus_Service(Vcom *vcom, Command *cmd){
static char line[COMMAND_MAX_LEN];
static uint8_t lineLen;

    if(vcom->getLineNonBlocking(line, &lineLen, COMMAND_MAX_LEN)){
        vcom->putc('\n');
        cmd->parse(line);
        lineLen = 0;
		vcom->printf("LPC BUS: ");
        memset(line,0 , COMMAND_MAX_LEN);
    }
}

int main()
{
    
	uint8_t mode;

	Vcom vcom1;	

    Command cmd(&vcom1);	
	CmdEcho echo(&vcom1);
	CmdPwm pwm(&vcom1);
	CmdGpio gpio(&vcom1);
	CmdLed led(&vcom1);
	CmdMem mem(&vcom1);
	CmdI2c i2c(&vcom1);
    CmdSpi spi(&vcom1);
    CmdAvr avr(&vcom1);
    CmdTest test(&vcom1);
    CmdAwg awg(&vcom1);
    CmdSbus sbus(&vcom1);
	
	cmd.add(&cmd);
	cmd.add(&echo);
	cmd.add(&pwm);
	cmd.add(&gpio);
	cmd.add(&led);
	cmd.add(&mem);
	cmd.add(&i2c);
    cmd.add(&spi);
    cmd.add(&avr);
    cmd.add(&test);
    cmd.add(&awg);
    cmd.add(&sbus);

    CLOCK_Init(72);
	CLOCK_InitUSBCLK();

    BOARD_Init();

    DISPLAY_Init(ON);
	LCD_Rotation(LCD_LANDSCAPE);
	LCD_Bkl(ON);

    vcom1.init();

    stk500_ServiceInit(&vcom1);
  
	LCD_Clear(BLACK);

    printOptions(options, sizeof(options) / sizeof(option_type));

	while(1)
	{	
		
		do{
            mode = checkOptions(options, sizeof(options) / sizeof(option_type));
			switch(mode){
                case MODE_LPCBUS:
                    lpcBus_Service(&vcom1, &cmd);
                    break;
                case MODE_STK500:
                    stk500_Service();
                    break;
            }
                      
		}while(TRUE);		

		
	}	
	return 0;
}
