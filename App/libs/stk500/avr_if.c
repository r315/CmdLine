
#include <stdio.h>
#include <string.h>

#include "board.h"
#include "stk500.h"
#include "avr_if.h"

#define READ_PROGRAM_PAGE_L "\x20\x00\x00\x00"
#define READ_PROGRAM_PAGE_H "\x28\x00\x00\x00"

#define DEVICE_CODE0_CMD    "\x30\x00\x00\x00"
#define DEVICE_CODE1_CMD    "\x30\x00\x01\x00"
#define DEVICE_CODE2_CMD    "\x30\x00\x02\x00"

#define LOAD_PROGRAM_PAGE_L "\x40\x00\x00\x00"
#define LOAD_PROGRAM_PAGE_H "\x48\x00\x00\x00"
#define WRITE_PROGRAM_PAGE  "\x4C\x00\x00\x00"

#define READ_FUSE_L         "\x50\x00\x00\x00"
#define READ_FUSE_H         "\x58\x08\x00\x00"

#define WRITE_FUSE_L        "\xAC\xA0\x00\x00"
#define WRITE_FUSE_H        "\xAC\xA8\x00\x00"
#define CHIP_ERASE          "\xAC\x80\x00\x00"
#define DEVICE_PROG_ENABLE  "\xAC\x53\x00\x00"

#define POLL_RDY            "\xF0\x00\x00\x00"

#define AVR_INSTRUCTION_SIZE    4
#define AVR_PROGRAMMING_ACTIVE  (1<<0)
#define AVR_ENABLE_RETRIES      2
#define AVR_DW_SYNC_TIMEOUT     10

#define YES 1
#define NO 0

#define LOW_BYTE(W)             (uint8_t)(((W) >> 0) & 0xff)
#define HIGH_BYTE(W)            (uint8_t)(((W) >> 8) & 0xff)

#define AVR_RST0                GPIO_Write(rst_pin, GPIO_PIN_LOW)
#define AVR_RST1                GPIO_Write(rst_pin, GPIO_PIN_HIGH)
#define AVR_RSTZ                GPIO_Config(rst_pin, GPI_FLOAT)
#define AVR_RSTY                GPIO_Config(rst_pin, GPO_PP)

// #define ENABLE_AVR_DW

typedef struct avrdevice{
    uint8_t data[AVR_INSTRUCTION_SIZE];
    uint8_t status;
}avrdevice_t;

static avrdevice_t s_avr_device;

static spibus_t *avrspibus;
static uint32_t rst_pin;

/**
 * @brief 
 * 
 * @return int 
 */
int avrWaitReady(void){
    uint32_t timeout = 0x100000;
    
    while(timeout--){        
        memcpy(s_avr_device.data, POLL_RDY, AVR_INSTRUCTION_SIZE);
        SPI_Transfer(avrspibus, s_avr_device.data, AVR_INSTRUCTION_SIZE);
        if( !(s_avr_device.data[3] & 0x01)){
            return AVR_RESPONSE_OK;
        }                
    }

    return AVR_RESPONSE_TIMEOUT;
}

/**
 * @brief Enter program mode.
 * program mode is enabled if the device echoes second sent byte.
 * Reset signal must be kepted low for following commands.
 * 
 * @param en        Enable/Disable program mode
 * @param trydW     Try debug wire interface
 * @return uint8_t 
 */
uint8_t avrProgrammingEnable(uint8_t en, uint8_t trydW){

    if(!en){
        /**
        * Clear programming active flag and release reset pin (HIGH state)
        * */
        s_avr_device.status &= ~(AVR_PROGRAMMING_ACTIVE);
        AVR_RST1;
        return AVR_RESPONSE_OK; 
    }

    if(s_avr_device.status & AVR_PROGRAMMING_ACTIVE)
    {
       return AVR_RESPONSE_OK; 
    }

    memcpy(s_avr_device.data, DEVICE_PROG_ENABLE, AVR_INSTRUCTION_SIZE);
    
    AVR_RST1;
    AVR_RSTY;

    for (uint8_t i = 0; i < AVR_ENABLE_RETRIES; i++){
        AVR_RST1;
        DelayMs(2);
        AVR_RST0;
        DelayMs(20);
        SPI_Transfer(avrspibus, s_avr_device.data, AVR_INSTRUCTION_SIZE);
        if(s_avr_device.data[2] == 0x53){
            s_avr_device.status |= AVR_PROGRAMMING_ACTIVE;
            return AVR_RESPONSE_OK;
        }

        // TODO: If fail, try using lower SCK
        if(i == AVR_ENABLE_RETRIES) 
            break;
        
        memcpy(s_avr_device.data, DEVICE_PROG_ENABLE, AVR_INSTRUCTION_SIZE);
        #ifdef ENABLE_AVR_DW
        if(trydW) 
            avrDisable_dW();
        #endif       
    }
    
    AVR_RST1;

    return AVR_RESPONSE_FAIL; 
}

/**
 * @brief Tryes to read device signature
 * 
 * @param buf 
 */
void avrDeviceCode(uint8_t *buf){

    if(avrProgrammingEnable(YES, NO) != AVR_RESPONSE_OK){
        *((int32_t*)buf) = AVR_RESPONSE_FAIL;
        return;
    }

    memcpy(s_avr_device.data, DEVICE_CODE0_CMD, AVR_INSTRUCTION_SIZE);
    SPI_Transfer(avrspibus, s_avr_device.data, AVR_INSTRUCTION_SIZE);
    buf[2] = s_avr_device.data[3];

    memcpy(s_avr_device.data, DEVICE_CODE1_CMD, AVR_INSTRUCTION_SIZE);
    SPI_Transfer(avrspibus, s_avr_device.data, AVR_INSTRUCTION_SIZE);
    buf[1] = s_avr_device.data[3];

    memcpy(s_avr_device.data, DEVICE_CODE2_CMD, AVR_INSTRUCTION_SIZE);
    SPI_Transfer(avrspibus, s_avr_device.data, AVR_INSTRUCTION_SIZE);
    buf[0] = s_avr_device.data[3];

    buf[3] = 0;
}

/**
 * @brief Write fuse
 * 
 * @param lh 
 * @param fuses 
 */
void avrWriteFuses(uint8_t lh, uint8_t fuses)
{

    if( avrProgrammingEnable(YES, NO) != AVR_RESPONSE_OK){
        return;
    }

    if (lh == 1){
        memcpy(s_avr_device.data, WRITE_FUSE_H, AVR_INSTRUCTION_SIZE);
    }else{
        memcpy(s_avr_device.data, WRITE_FUSE_L, AVR_INSTRUCTION_SIZE);
    }   

    s_avr_device.data[3] = fuses;
    
    SPI_Transfer(avrspibus, s_avr_device.data, AVR_INSTRUCTION_SIZE);

    avrWaitReady();
}

/**
 * @brief Read fuses
 * 
 * @return uint32_t 
 */
uint32_t avrReadFuses(void)
{
    uint32_t fuses;

    if( avrProgrammingEnable(YES, NO) != AVR_RESPONSE_OK){
        return -1;
    }

    memcpy(s_avr_device.data, READ_FUSE_L, AVR_INSTRUCTION_SIZE);
    SPI_Transfer(avrspibus, s_avr_device.data, AVR_INSTRUCTION_SIZE);
    fuses = s_avr_device.data[3];

    memcpy(s_avr_device.data, READ_FUSE_H, AVR_INSTRUCTION_SIZE);
    SPI_Transfer(avrspibus, s_avr_device.data, AVR_INSTRUCTION_SIZE);
    fuses |= s_avr_device.data[3] << 8;    

    return fuses;
}

/**
 * @brief Chip erase
 * 
 */
void avrChipErase(void){

    if( avrProgrammingEnable(YES, NO) != AVR_RESPONSE_OK){
        return;
    }

    memcpy(s_avr_device.data, CHIP_ERASE, AVR_INSTRUCTION_SIZE);

    SPI_Transfer(avrspibus, s_avr_device.data, AVR_INSTRUCTION_SIZE);

    avrWaitReady();
}

/**
 * @brief Read program memory
 * 
 * @param addr 
 * @return uint16_t 
 */
uint16_t avrReadProgram(uint16_t addr){
    uint16_t value = 0;

    if( avrProgrammingEnable(YES, NO) != AVR_RESPONSE_OK){
        return value;
    }

    /* read high byte */
    memcpy(s_avr_device.data, READ_PROGRAM_PAGE_H, AVR_INSTRUCTION_SIZE);
    s_avr_device.data[1] = HIGH_BYTE(addr);
    s_avr_device.data[2] = LOW_BYTE(addr);
    SPI_Transfer(avrspibus, s_avr_device.data, AVR_INSTRUCTION_SIZE);
    value = s_avr_device.data[3];

    value <<= 8;

    /* read low byte */
    memcpy(s_avr_device.data, READ_PROGRAM_PAGE_L, AVR_INSTRUCTION_SIZE);
    s_avr_device.data[1] = HIGH_BYTE(addr);
    s_avr_device.data[2] = LOW_BYTE(addr);
    SPI_Transfer(avrspibus, s_avr_device.data, AVR_INSTRUCTION_SIZE);
    value |= s_avr_device.data[3];    

    return value;
}

/**
 * @brief load data to program memory
 * 
 * @param addr 
 * @param value 
 */
void avrLoadProgramPage(uint8_t addr, uint16_t value){

    if( avrProgrammingEnable(YES, NO) != AVR_RESPONSE_OK){
        return;
    }

    /* load low byte */
    memcpy(s_avr_device.data, LOAD_PROGRAM_PAGE_L, AVR_INSTRUCTION_SIZE);
    s_avr_device.data[2] = addr;
    s_avr_device.data[3] = LOW_BYTE(value);
    SPI_Transfer(avrspibus, s_avr_device.data, AVR_INSTRUCTION_SIZE);
    
    /* load high byte */
    memcpy(s_avr_device.data, LOAD_PROGRAM_PAGE_H, AVR_INSTRUCTION_SIZE);
    s_avr_device.data[2] = addr;
    s_avr_device.data[3] = HIGH_BYTE(value);
    SPI_Transfer(avrspibus, s_avr_device.data, AVR_INSTRUCTION_SIZE);
}

/**
 * @brief write to program memory
 * 
 * @param addr 
 */
void avrWriteProgramPage(uint16_t addr){ 

    if( avrProgrammingEnable(YES, NO) != AVR_RESPONSE_OK){
        return;
    }  

    memcpy(s_avr_device.data, WRITE_PROGRAM_PAGE, AVR_INSTRUCTION_SIZE);
    s_avr_device.data[1] = HIGH_BYTE(addr);
    s_avr_device.data[2] = LOW_BYTE(addr);
    SPI_Transfer(avrspibus, s_avr_device.data, AVR_INSTRUCTION_SIZE);

    avrWaitReady();
}

/**
 * @brief Get the Device Name object
 * 
 * @param sig 
 * @return const char* 
 */
const char *avrDeviceName(uint32_t sig)
{
    struct avrdevname{
        uint32_t sig;
        const char *name;
    };

    struct avrdevname avrnames[] = {
        {0x1E9007, "ATtiny13A"},
        {0x1e9702, "ATMEG128"}
    };

    for (int i = 0; i < 2; i++)
    {
        if(avrnames[i].sig == sig){
            return avrnames[i].name;
        }
    }

    return "Unkonw";
}


#ifdef ENABLE_AVR_DW
volatile uint32_t tbit;

extern uint32_t TIMER_SetInterval(void (*func)(void), uint32_t us);

// Called at every bit time
void bitTime(void){
    tbit = 1;
}

/****************************************************************************
 * Debug wire is a serial protocol used on AVR chips for debuging
 *                               
 * RESET     _______           _________   _   _   _   _   _______________
 *                  |         |         | | | | | | | | | |
 *                  |_________|         |_| |_| |_| |_| |_|
 * 
 *                  |<-150us->|         [      0x55       ]
 * 
 * Note: debugWire only available if fuse bit DWEN is programmed
 * 
 * If the DWEN bit is programmed the protocol is enabled and 
 * starts upon a reset by sending a sync byte 0x55. This byte is used
 * to determine the baudrate by measuring each pulse length. 
 * Current implementation only measures the duration of the first low pule 
 * after the 150us initial pulse. The measured duration is set as bit time reference 
 * and used for communication.
 * 
 * https://www.ruemohr.org/docs/debugwire.html
 * 
 * */

/**
 * @brief Called when a falling edge is detected on reset pin (P0.23 for blueboard)
 * 
 * @param capValue 
 */
static void autoBaudCb(uint32_t capValue){
    static uint32_t delta = 0;
	if(delta == 0){
		delta = capValue;
	}else{
		delta = capValue - delta - 1;
		
		TIM_Reset(LPC_TIM3);
        tbit = delta - 2;
		delta = 0;
	}
}

/**
 * @brief Sends byt using debug wire
 * TODO: This can be optimized to use timer callback to send bits
 * and avoid blocking calls
 * This has to be tested due tos changes on timer API
 * 
 * @param data 
 */
static void avrSend_dW(uint8_t data){

    TIMER_SetInterval(bitTime, (tbit >> 1));
    LPC_GPIO0->FIOCLR = (1<<24); //AVR_RST0;

	for(uint8_t i = 0; i < 8; i++){
        TIM_Restart(LPC_TIM3);
        tbit = 0;

        while(!tbit){

        }

		if(data & (0x1<<i)){
			LPC_GPIO0->FIOSET = (1<<24); //AVR_RST1;
		}else{
			LPC_GPIO0->FIOCLR = (1<<24); //AVR_RST0;
		}		
	}

    TIM_Restart(LPC_TIM3);
    tbit = 0;
    while(!tbit);
    LPC_GPIO0->FIOSET = (1<<24); //AVR_RST1;
}

/**
 * @brief Disables debug wire interface.
 * Only implemented for blueboard, too lazy
 * to port for other chips
 * 
 * Only works with P0_24 connected to RST and P00_23
 * 
 * @return uint8_t 
 */
uint8_t avrDisable_dW(void){
    uint32_t autobaud;

    LPC_GPIO0->FIOSET = (1<<24); //AVR_RST1; 
    DelayMs(5);
    LPC_GPIO0->FIOCLR = (1<<24); //AVR_RST0; 
    DelayMs(5);
    TIM_InitCapture(LPC_TIM3);
    TIM_Capture(LPC_TIM3, 0, 1, autoBaudCb);  // 1: Capture on falling edge
    tbit = 0;
    LPC_GPIO0->FIODIR &= ~(1<<24); //AVR_RSTZ; 
    DelayMs(50); // Skip 150us initial pulse
    LPC_GPIO0->FIOSET = (1<<24); //AVR_RST1;
    LPC_GPIO0->FIODIR |= (1<<24); //AVR_RSTY;
    while(!tbit);
#if 0
    count = AVR_DW_SYNC_TIMEOUT;
    while(!tbit && count)
    {
        DelayMs(5);
        count--;
    }

    if(!count)
    {
        VCOM_printf("Unable to sync debugWire\n");
        return 0;
    }

    if(tbit < 10)
    {
        VCOM_printf("Unable to preform autobaud\n");
        return 0;
    }
#endif
    autobaud = tbit;
    TIM_InitMatch(LPC_TIM3);
    TIMER_SetInterval(bitTime, tbit * 7);
    TIM_Restart(LPC_TIM3);
    tbit = 0;
    while(!tbit);
    tbit = autobaud;
    LPC_GPIO0->FIODIR |= (1<<24); //AVR_RSTY;
    avrSend_dW(0x06);
    TIM_Reset(LPC_TIM3);
    return 0;
}
#endif

//------------------------------------------------------------
//
//------------------------------------------------------------
void spi_write_lock_bits(uint8_t value)
{
}

void spi_write_fuse_bits(uint8_t value)
{
}

void spi_write_fuse_high_bits(uint8_t value)
{
}

void spi_write_extended_fuse_bits(uint8_t value)
{
}

uint8_t spi_read_fuse_bits(void)
{
    return 0;
}

uint8_t spi_read_extended_fuse_bits(void)
{
    return 0;
}

uint8_t spi_read_fuse_high_bits(void)
{
    return 0;
}

void spi_read_calibration_bytes(uint8_t *buf)
{
}

uint8_t spi_read_eeprom(uint16_t addr)
{
    return 0;
}

void spi_write_eeprom(uint16_t addr, uint8_t value)
{
}

uint8_t spi_read_lock_bits(void)
{
    return 0;
}

uint8_t spi_write_read(uint8_t *buf)
{
    SPI_Transfer(avrspibus, buf, AVR_INSTRUCTION_SIZE);
    return buf[3];
}

void avr_if_Init(spibus_t *spi, uint32_t pin)
{ 
    avrspibus = spi;    
    rst_pin = pin;

    SPI_Init(avrspibus);
}
