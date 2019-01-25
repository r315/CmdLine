/**
 * LPC1768 stk500 compatible programmer
 * 
 * the stk500 programmer is a port of https://github.com/texane/picisp
 *  
 * usage: avrdude -c stk500v1 -P /dev/ttyACM0 -p t13
 * */

#include <stdio.h>
#include "board.h"
#include "consoleout.h"
#include "stk500.h"
#include "types.h"
#include "spi_avr.h"
#include "cmdavr.h"
#include "cmdspi.h"

#include "debug.h"

#define NO_SERVICE 0

enum{
    WAITING_COMMAND,
};

typedef struct _Service{
    uint8_t state;
    stk500_error_t error;
    unsigned int isize;
    unsigned int osize;
    unsigned char do_timeout;
    unsigned char buf[STK500_BUF_MAX_SIZE];
}Service;

static ConsoleOut *serialport;
static Service stkService;

//------------------------------------------------------------
//
//------------------------------------------------------------
void spi_setup(void){
    // spiInit is called if spi is not initialized
}

int spi_start(void){ 
    return  avrProgrammingEnable(NO);
}

void spi_stop(void){
    AVR_RST1;
}

void spi_read_signature(uint8_t *buf){
    avrDeviceCode(buf);
}

void spi_chip_erase(void){
    avrChipErase();
}

uint16_t spi_read_program(uint16_t addr){
    return avrReadProgram(addr);
}

void spi_load_program_page(uint8_t addr, uint16_t value){
    avrLoadProgramPage(addr,value);
}

void spi_write_program_page(uint16_t addr){
    avrWriteProgramPage(addr);
}

void spi_write_lock_bits(uint8_t value){

}

void spi_write_fuse_bits(uint8_t value){

}

void spi_write_fuse_high_bits(uint8_t value){

}

void spi_write_extended_fuse_bits(uint8_t value){

}

uint8_t spi_read_fuse_bits(void){
return 0;
}

uint8_t spi_read_extended_fuse_bits(void){
return 0;
}

uint8_t spi_read_fuse_high_bits(void){
return 0;
}

void spi_read_calibration_bytes(uint8_t *buf){

}

uint8_t spi_read_eeprom(uint16_t addr){
    return 0;
}

void spi_write_eeprom(uint16_t addr, uint8_t value){

}

uint8_t spi_read_lock_bits(void){
    return 0;
}

uint8_t spi_write_read(uint8_t *buf){
   spiWrite(buf, AVR_INSTRUCTION_SIZE);
return buf[3];
}

void stk500_ServiceInit(ConsoleOut *sp){
    serialport = sp;    
    stkService.state = STK500_ERROR_SUCCESS;
    stkService.isize = 0;
    stkService.osize = 0;
    stk500_setup();
}

void serial_write(uint8_t * s, uint32_t len){
    while(len--)
        serialport->putchar(*(s++));
}

int read_uint8(uint8_t *c, uint8_t do_timeout){

#if !NO_SERVICE
    if(!serialport->kbhit())
        return -1;
    *c = (uint8_t)serialport->getchar();    
#else
uint32_t ticks;
int rc;
    if(do_timeout){
        ticks = GetTicks();
        while(ElapsedTicks(ticks) < 1000){
            rc = serialport->getchar();
            if(rc != EOF){
                *c = (uint8_t)rc;
                return 0;
            }
        }
    }else{
        *c = serialport->getc();
        return 0;
    }
    return -1;
#endif
return 0;
}


void stk500_Service(void){

#if !NO_SERVICE
    switch(stkService.state){

        
        case STK500_ERROR_MORE_DATA:
            if (read_uint8(stkService.buf + stkService.isize, 0) == -1){
                stk500_timeout(stkService.buf, stkService.isize, &stkService.osize);
                stkService.state = STK500_ERROR_SUCCESS;
                break;                
            }
            ++stkService.isize;
            stkService.state = stk500_process(stkService.buf, stkService.isize, &stkService.osize);
            break;

        case STK500_ERROR_SUCCESS:
        default:
            stkService.state = STK500_ERROR_MORE_DATA;
            stkService.isize = 0;
            stkService.osize = 0;            
            break;

    }
    
    if ((stkService.state == STK500_ERROR_SUCCESS) && (stkService.osize)){
	    serial_write(stkService.buf, stkService.osize);
    }
#else
    stk500_error_t error;
    unsigned int isize;
    unsigned int osize;
    unsigned char do_timeout;
    unsigned char buf[STK500_BUF_MAX_SIZE];

    stk500_setup();

    while (1)
    {
        /* process new command */

        error = STK500_ERROR_MORE_DATA;

        do_timeout = 0;

        isize = 0;
        osize = 0;

        while (error == STK500_ERROR_MORE_DATA){
            if (read_uint8(buf + isize, do_timeout) == -1){
                /* has timeouted */
                stk500_timeout(buf, isize, &osize);
                error = STK500_ERROR_SUCCESS;
                break;
            }

	        do_timeout = 1;

	        ++isize;

	        error = stk500_process(buf, isize, &osize);
	    }

        if ((error == STK500_ERROR_SUCCESS) && (osize))
	        serial_write(buf, osize);
    }
#endif
}
