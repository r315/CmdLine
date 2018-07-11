
#include <common.h>

#include "command.h"
#include "vcom.h"
#include "cmdbase.h"

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

