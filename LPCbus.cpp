#include <common.h>
#include <display.h>

#include "usbserial.h"
#include "vcom.h"
#include "command.h"

void abort(void){}

char showHelp(void *);
char echo(void *);
char captureTest(void *ptr);
char matchTest(void *ptr);

CmdLine lpcbusCommands[] =
{
	{"help",showHelp},
	{"echo", echo},
	//{"gpio",gpioCmd},
	//{"spi",spiCmd},	
	//{"avr",avrCmd},
	{"capture",captureTest},
	//{"match",matchTest},
};

char echo(void *ptr){
	vcom->printf("%s\n", (char*)ptr);
	return CMD_OK;	
}


void capCb(void *ptr){
	static uint32_t count = 0;
	if(count == 0){
		count = *((uint32_t*)ptr);
	}else{
		count = *((uint32_t*)ptr) - count - 1;
		vcom->printf("T = %uus\n", count);
		TIM_Stop(LPC_TIM3);
		//fifo_put(&rxfifo, '\n');
		count = 0;
	}
}

char captureTest(void *ptr){
	vcom->printf("1kHz square wave on P2.0 (TXD1) using pwm\n");
	vcom->printf("Period measure test on pin P0.23 (AD0.0)\n");

	PWM_Init(1000);
	PWM_Enable(1);
	PWM_Set(1, 50);
	
	TIMER_CAP_Init(LPC_TIM3, 0, CAP_FE, capCb);

	return CMD_OK;	
}

void matchCb(void *ptr){
	LPC_GPIO2->FIOPIN ^= (1<<0);
}

char matchTest(void *ptr){
	vcom->printf("1kHz square wave on P2.0 (TXD1) using interrupt\n");
	PINSEL_P2_0(P2_0_GPIO);
	PINDIR_P2_0(GPIO_OUTPUT);

	TIMER_Match_Init(LPC_TIM3, 0, 500, matchCb);

	return CMD_OK;	
}

char showHelp(void *ptr){
	vcom->printf("\nAvalilable commands:\n\r");

	for(uint8_t i = 0; i < sizeof(lpcbusCommands)/sizeof(CmdLine); i++){
		vcom->printf("\t%s\n\r", (char*)lpcbusCommands[i].name);
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
			i = vcom->gets_echo(line);
			vcom->puts("\n\r");
		}while(!i);			

        res = cmdProcess(line, lpcbusCommands, sizeof(lpcbusCommands)/sizeof(CmdLine));

        if (res == CMD_NOT_FOUND){
            vcom->printf("Command not found\n");
        }else if(res == CMD_BAD_PARAM){ 
            vcom->printf("Bad parameter \n");
        }	
	}	
	return 0;
}
