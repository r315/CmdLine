#include "board.h"
#include "cmdrfinder.h"


void startRanging(void){
	aux_uart.xputchar(START_RANGING);
}

uint8_t readFrame(void){
	return aux_uart.kbhit();
}

uint8_t checkSum(uint8_t *buf){
uint8_t sum = 0;

	for(uint16_t i = 0; i < FRAME_SIZE - 1; i++, buf++){
		sum += *buf;
	}

	return !(sum - *buf);
}


char CmdRfinder::execute(void *ptr){
Array arr;
uint16_t distance;
uint8_t m = 0;

	arr.data = buf;
	arr.len = FRAME_SIZE;
	arr.empty = 1;

	help();

	aux_uart.user_ctx = &arr;
	aux_uart.init();


	while(!console->kbhit()){

		if(m == 0){
			startRanging();
			m = 1;
		}else{
			if(readFrame() && checkSum(buf)){
				distance = (buf[1] << 8) | buf[2];
				console->print("\r%dmm  ", distance);
			}
			m = 0;
		}
		DelayMs(100);
	}

	console->xgetchar();


	return CMD_OK;
}
