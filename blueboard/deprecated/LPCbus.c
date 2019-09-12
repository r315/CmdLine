#include <common.h>
#include "vcom.h"
#include "strfunctions.h"
#include "command.h"
#include "cmdgpio.h"
#include "cmdspi.h"
#include "cmdavr.h"




char showHelp(void *);
char echo(void *);
char captureTest(void *ptr);
char matchTest(void *ptr);

CmdLine lpcbusCommands[] =
{
	{"help",showHelp},
	{"echo", echo},
	{"gpio",gpioCmd},
	{"spi",spiCmd},	
	{"avr",avrCmd},
	{"capture",captureTest},
	{"match",matchTest},
};

char echo(void *ptr){
	VCOM_printf("%s\n", (char*)ptr);
	return CMD_OK;	
}


void capCb(void *ptr){
	static uint32_t count = 0;
	if(count == 0){
		count = *((uint32_t*)ptr);
	}else{
		count = *((uint32_t*)ptr) - count - 1;
		VCOM_printf("T = %uus\n", count);
		TIM_Stop(LPC_TIM3);
		//fifo_put(&rxfifo, '\n');
		count = 0;
	}
}

char captureTest(void *ptr){
	VCOM_printf("1kHz square wave on P2.0 using pwm\n");
	VCOM_printf("Period measure test on pin P0.23\n");

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
	VCOM_printf("1kHz square wave on P2.0 using interrupt\n");
	PINSEL_P2_0(P2_0_GPIO);
	PINDIR_P2_0(GPIO_OUTPUT);

	TIMER_Match_Init(LPC_TIM3, 0, 500, matchCb);

	return CMD_OK;	
}

char showHelp(void *ptr){
	VCOM_printf("\nAvalilable commands:\n\r");

	for(int i = 0; i < sizeof(lpcbusCommands)/sizeof(CmdLine); i++){
		VCOM_printf("\t%s\n\r", (char*)lpcbusCommands[i].name);
	}

	return CMD_OK;	
}



int main()
{
char line[CMD_MAX_LINE];
uint8_t i = 0;

	CLOCK_Init(72);
	CLOCK_InitUSBCLK();

	DISPLAY_Init(ON);
	//LCD_Rotation(LCD_LANDSCAPE);
	LCD_Bkl(ON);

	DISPLAY_printf("Pi = %f\n", 3.141592);

	usbSerialInit();
	
	VCOM_puts("echo");
	VCOM_gets_echo(line);
	VCOM_flush();

	while(1)
	{
		do{
			line[0] = '\0';
			VCOM_puts("\rLPC BUS: ");
			i = VCOM_gets_echo(line);
			VCOM_puts("\n\r");
		}while(!i);
	
		cmdProcess(line, lpcbusCommands, sizeof(lpcbusCommands)/sizeof(CmdLine));		
	}	
	return 0;
}
