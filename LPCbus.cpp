#include <common.h>
#include <display.h>

#include "vcom.h"
#include "cmdecho.h"
#include "cmdpwm.h"

//void abort(void){}

int main()
{
    char line[COMMAND_MAX_LINE];
	uint8_t i;

	Vcom vcom1;
	Command cmd;
	
	Command cmd(&vcom1);	
	CmdEcho echo(&vcom1);
	CmdPwm pwm(&vcom1);
	
	cmd.add(&cmd);
	cmd.add(&echo);
	cmd.add(&pwm);

    CLOCK_Init(72);
	CLOCK_InitUSBCLK();

    DISPLAY_Init(ON);
	//LCD_Rotation(LCD_LANDSCAPE);
	LCD_Bkl(ON);

    vcom1.init();	

	while(1)
	{
		do{
			line[0] = '\0';
			vcom1.puts("\rLPC BUS: ");
			i = vcom1.getLine(line, sizeof(line));
			vcom1.puts("\n\r");
		}while(!i);		

		cmd.parse(line);
		memset(line,0 , COMMAND_MAX_LINE);
		
	}	
	return 0;
}
