#include <common.h>
#include <display.h>

#include "usbserial.h"
#include "vcom.h"


void abort(void){

}

int main()
{
    char i;//line[10],i;
    CLOCK_Init(72);
	CLOCK_InitUSBCLK();


    DISPLAY_Init(ON);
	//LCD_Rotation(LCD_LANDSCAPE);
	LCD_Bkl(ON);
    
    vcom->init();

	while(1)
	{
		
			//line[0] = '\0';
			//vcom1.puts("\rLPC BUS: ");
			//vcom1.gets_echo(line);
			//vcom1.puts("\n\r");
        i = vcom->getc();
        DISPLAY_printf("%X ", i);
		
	}	
	return 0;
}
