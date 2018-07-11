#include <common.h>
#include <display.h>

//#include "usbserial.h"
#include "vcom.h"
#include "command.h"
#include "cmdbase.h"

void abort(void){}
char baseHelp(void *);

CmdLine baseCommands[] =
{
	{"help",baseHelp},
	{"echo", echo},
	//{"gpio",gpioCmd},
	//{"spi",spiCmd},	
	//{"avr",avrCmd},
	{"capture",captureTest},
	//{"match",matchTest},
};

char baseHelp(void *ptr){
	vcom->printf("\nAvalilable commands:\n\r");

	for(uint8_t i = 0; i < sizeof(baseCommands)/sizeof(CmdLine); i++){
		vcom->printf("\t%s\n\r", (char*)baseCommands[i].name);
	}

	return CMD_OK;	
}

int main()
{
    char line[10],i, res;
	
    CLOCK_Init(72);
	CLOCK_InitUSBCLK();

    DISPLAY_Init(ON);
	//LCD_Rotation(LCD_LANDSCAPE);
	LCD_Bkl(ON);

    vcom->init();

	while(1)
	{
		do{
			line[0] = '\0';
			vcom->puts("\rLPC BUS: ");
			i = vcom->getLine(line, sizeof(line));
			vcom->puts("\n\r");
		}while(!i);			

        res = cmdExecute(line, baseCommands, sizeof(baseCommands)/sizeof(CmdLine));

        if (res == CMD_NOT_FOUND){
            vcom->printf("Command not found\n");
        }else if(res == CMD_BAD_PARAM){ 
            vcom->printf("Bad parameter \n");
        }	
	}	
	return 0;
}
