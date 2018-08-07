#include <common.h>
#include <display.h>

#include "vcom.h"
#include "cmdecho.h"
#include "cmdpwm.h"
#include "cmdgpio.h"
#include "cmdled.h"
#include "cmdmem.h"
#include "cmdi2c.h"
#include "cmdspi.h"
#include "cmdavr.h"

//void abort(void){}

int main()
{
    char line[COMMAND_MAX_LINE];
	uint8_t i;

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
	
	cmd.add(&cmd);
	cmd.add(&echo);
	cmd.add(&pwm);
	cmd.add(&gpio);
	cmd.add(&led);
	cmd.add(&mem);
	cmd.add(&i2c);
    cmd.add(&spi);
    cmd.add(&avr);

    CLOCK_Init(72);
	CLOCK_InitUSBCLK();

    DISPLAY_Init(ON);
	//LCD_Rotation(LCD_LANDSCAPE);
	LCD_Bkl(ON);

    vcom1.init();

	while(1)
	{
		memset(line,0 , COMMAND_MAX_LINE);
		
		do{
			vcom1.printf("LPC BUS: ");
			i = vcom1.getLine(line, sizeof(line));
			vcom1.putc('\n');
		}while(!i);		

		cmd.parse(line);
		
	}	
	return 0;
}
