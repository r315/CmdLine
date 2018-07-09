#include <common.h>
#include <display.h>

#include "usbserial.h"
#include "vcom.h"

static Vcom vcom1;

uint8_t vcom1_fifo_put(uint8_t data){
    return vcom1.fifo_put(&vcom1.rxfifo, data);
}

uint8_t vcom1_fifo_get(uint8_t *dst){
    return  vcom1.fifo_get(&vcom1.txfifo, dst);
}

int vcom1_fifo_free(void){
    return vcom1.fifo_free(&vcom1.rxfifo);
}

int vcom1_fifo_available(void){
    return vcom1.fifo_avail(&vcom1.txfifo);
}

Fifo_ops vcom1_fifo_ops = {
    .put = vcom1_fifo_put,
    .get = vcom1_fifo_get,
    .free = vcom1_fifo_free,
    .available = vcom1_fifo_available,
};

void abort(void){

}

int main()
{
    char line[10],i;
    CLOCK_Init(72);
	CLOCK_InitUSBCLK();

    vcom1.init();
    USBSERIAL_Init(&vcom1_fifo_ops);

    DISPLAY_Init(ON);
	//LCD_Rotation(LCD_LANDSCAPE);
	LCD_Bkl(ON);

	while(1)
	{
		
			//line[0] = '\0';
			//vcom1.puts("\rLPC BUS: ");
			//vcom1.gets_echo(line);
			//vcom1.puts("\n\r");
        i = vcom1.getc();
        DISPLAY_printf("%X ", i);
		
	}	
	return 0;
}
