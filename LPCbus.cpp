extern "C" {
#include <common.h>    
}
#include "usbserial.h"

#include "vcom.h"

static Vcom vcom1;

uint8_t fifo_put(uint8_t data){
    return vcom1.fifo_put(&vcom1.rxfifo, data);
}

uint8_t fifo_get(uint8_t *dst){
    return  vcom1.fifo_get(&vcom1.txfifo, dst);
}

int fifo_free(void){
    return vcom1.fifo_free(&vcom1.rxfifo);
}

int fifo_available(void){
    return vcom1.fifo_avail(&vcom1.txfifo);
}

Fifo_ops fops = {
    .put = fifo_put,
    .get = fifo_get,
    .free = fifo_free,
    .available = fifo_available,
};

void abort(void){

}

int main()
{
    char line[10],i;
    CLOCK_Init(72);
	CLOCK_InitUSBCLK();

    vcom1.init();
    USBSERIAL_Init(&fops);

	while(1)
	{
		do{
			line[0] = '\0';
			vcom1.puts("\rLPC BUS: ");
			vcom1.gets_echo(line);
			vcom1.puts("\n\r");
		}while(!i);
	}	
	return 0;
}
