#include "cmdmsensor.h"
#include "board.h"
#include "timer.h"

#define SENSOR_TIMEOUT_NORMAL       2
#define SENSOR_TIMEOUT_SLEEP1       32
#define SENSOR_TIMEOUT_SLEEP2       320

#define SENSOR_WRITE_REG             0x80

#define SENSOR_SDIO_PIN             (1 << 0)
#define SENSOR_SCLK_PIN             (1 << 1)
#define SENSOR_SCLK_LOW             LPC_GPIO0->FIOCLR = SENSOR_SCLK_PIN
#define SENSOR_SCLK_HIGH            LPC_GPIO0->FIOSET = SENSOR_SCLK_PIN
#define SENSOR_SDIO_LOW             LPC_GPIO0->FIOCLR = SENSOR_SDIO_PIN
#define SENSOR_SDIO_HIGH            LPC_GPIO0->FIOSET = SENSOR_SDIO_PIN

#define SENSOR_SDIO_OUT             LPC_GPIO0->FIODIR |= SENSOR_SDIO_PIN
#define SENSOR_SDIO_IN              LPC_GPIO0->FIODIR &= ~SENSOR_SDIO_PIN

#define SENSOR_SDIO_STATE           !!(LPC_GPIO0->FIOPIN & SENSOR_SDIO_PIN)

static uint8_t timeout;

static void set_timeout(){
    timeout = 1;
}

static void sensor_timer_init(void){
    PCONP_RIT_ENABLE;
    LPC_RIT->RICTRL = 0;
    CLOCK_SetPCLK(PCLK_RIT, PCLK_4);
    LPC_RIT->RICOMPVAL = (CLOCK_GetPCLK(PCLK_RIT) / 1000000UL) - 1;
}

static void delay_us(uint32_t us){
    LPC_RIT->RICOUNTER = 0;
    LPC_RIT->RICTRL = RIT_RICTRL_RITEN | RIT_RICTRL_RITENCLR;

    while(us){
        if(LPC_RIT->RICTRL & RIT_RICTRL_RITINT){
            LPC_RIT->RICTRL = RIT_RICTRL_RITEN | RIT_RICTRL_RITINT | RIT_RICTRL_RITENCLR;
            us--;
        }
    }
    
    LPC_RIT->RICTRL = 0;
}

static void sensor_addr(int8_t reg){
    uint8_t mask = 0x80;

    SENSOR_SDIO_OUT;
    
    do{
        SENSOR_SCLK_LOW;
        if(reg & mask)SENSOR_SDIO_HIGH;
        else SENSOR_SDIO_LOW;
        delay_us(4);        
        SENSOR_SCLK_HIGH;
        delay_us(4);
    }while((mask = (mask >> 1)));    
}

static uint8_t sensor_read(uint8_t reg, uint8_t *pdata){
    uint8_t data, bit;
    sensor_addr(reg & ~SENSOR_WRITE_REG); 

    SENSOR_SDIO_IN;
    data = 0;
    bit = 8;

    do{
        data <<= 1;
        SENSOR_SCLK_LOW;
        delay_us(4);
        data |= SENSOR_SDIO_STATE;
        SENSOR_SCLK_HIGH;
        delay_us(4);
    }while(--bit);

    *pdata = data;
    return 1;
}

static uint8_t sensor_write(uint8_t reg, uint8_t *pdata){
    uint8_t mask = 0x80;

    sensor_addr(reg | SENSOR_WRITE_REG);

    do{
        SENSOR_SCLK_LOW;
        delay_us(4);
        if(*pdata & mask)SENSOR_SDIO_HIGH;
        else SENSOR_SDIO_LOW;
        SENSOR_SCLK_HIGH;
        delay_us(4);
    }while((mask = (mask >> 1))); 

    return 1;
}

static void sensor_sync(uint16_t timeout){
    SENSOR_SCLK_LOW;
    DelayMs(1); // tresync
    SENSOR_SCLK_HIGH;
    DelayMs(timeout);
}

void CmdMSensor::help(void){
    console->print("PAN3204 Mouse sensor driver\n");
    console->print("Usage:msensor <init|read|write|sync|dump|track>\n\n");
}

char CmdMSensor::execute(int argc, char **argv){
    uint32_t val;
    uint8_t data;

    if(xstrcmp("init", (const char*)argv[1]) == 0){
        GPIO_Init(P0_0, PIN_OUT_PP);
        GPIO_Init(P0_1, PIN_OUT_PP);
        sensor_timer_init();
        sensor_sync(SENSOR_TIMEOUT_NORMAL);
        return CMD_OK;
    }else if(xstrcmp("read", (const char*)argv[1]) == 0){
        if(hatoi(argv[2], (uint32_t*)&val)){
            if(sensor_read(val, &data)){
                console->print("Reg[%x]: %x\n", val, data);
                return CMD_OK;
            }
        }
    }else if(xstrcmp("write", (const char*)argv[1]) == 0){
        if(hatoi(argv[2], (uint32_t*)&val)){
            uint8_t reg = val;
            if(hatoi(argv[3], (uint32_t*)&val)){
                data = val;
                sensor_write(reg, &data);
                return CMD_OK;                
            }
        }
    }else if(xstrcmp("sync", (const char*)argv[1]) == 0){
        sensor_sync(SENSOR_TIMEOUT_NORMAL);
        return CMD_OK;
    }else if(xstrcmp("dump", (const char*)argv[1]) == 0){
        for(uint8_t i = 0; i < 32; i++){
            sensor_read(i, &data);
            DelayMs(1);
            console->print("Reg[%x]: %x\n", i, data);            
        }
        return CMD_OK;
    }else if(xstrcmp("track", (const char*)argv[1]) == 0){
        uint32_t tid = TIMER_SetInterval(set_timeout, 100000);
        timeout = 0;
        #if 0
        for(uint8_t i = 0; i < 32; i++){
            console->print("%02X ",i);
        }
        console->putChar('\n');
        #endif

        while(console->getCharNonBlocking((char*)&data) == 0){
            
            if(timeout){
                #if 1
                int8_t x, y;
                sensor_read(2, &data);
                if(data & 0x80){
                    sensor_read(3, (uint8_t*)&x);
                    sensor_read(4, (uint8_t*)&y);
                    console->print("\r x = %d, y = %d       \r", x, y);
                }
                #else
                for(uint8_t i = 0; i < 32; i++){
                    sensor_read(i, &data);
                    console->print("%02X ",data);
                }
                console->putChar('\r');
                #endif
                timeout = 0;
            }
        }
        TIMER_ClearInterval(tid);
        return CMD_OK;
    }
    return CMD_BAD_PARAM;
}
