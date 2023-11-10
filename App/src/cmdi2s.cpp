#include "board.h"
#include "cmdi2s.h"
#include "i2s.h"

#define I2S_BUFFER_SIZE     16
static uint16_t tx_buf[I2S_BUFFER_SIZE];
//static uint16_t rx_buf[I2S_BUFFER_SIZE];

void CmdI2s::help(void){
    console->println("Usage: i2s <option> [params] \n");    
    console->println("options:");
    console->println("  init <sample rate> <bits/slot size> <protocol>,");
    console->println("             sample rate: 16000 to 96000");
    console->println("             bits: 0:16/16, 1:16/32, 2:24/32, 3:32/32");
    console->println("             protocol: 0: Philips, 1:MSB, 2:LSB, 3:PCM");
    console->println("  start,     Start FS and data output");
    console->println("  stop,      Stop FS and data output");
    console->printchar('\n');
}

char CmdI2s::execute(int argc, char **argv){
    int32_t val1, val2, val3;

    if(argc < 2){
        help();
        return CMD_OK;
    }

    if(xstrcmp("init", (const char*)argv[1]) == 0){
        if(ia2i(argv[2], &val1)){
            if(ia2i(argv[3], &val2)){
                if(ia2i(argv[4], &val3)){
                    m_i2s.sample_rate = val1;
                    m_i2s.data_size = (i2sdsize_t)val2; //3-0
                    m_i2s.bus = I2S_BUS0;
                    m_i2s.channels = 2;
                    m_i2s.mode = I2S_MASTER_TX | I2S_MASTER_RX | I2S_EN_TX | I2S_EN_RX | ((val3 &3) << 5);
                    m_i2s.tx_buf_len = I2S_BUFFER_SIZE;
                    m_i2s.txbuffer = (uint32_t*)tx_buf;
                    I2S_Init(&m_i2s);
                    return CMD_OK;
                }
            }
        }
    }

    if(xstrcmp("start", (const char*)argv[1]) == 0){
        I2S_Start(&m_i2s);
        for(int i = 0; i < I2S_BUFFER_SIZE; i++){
            m_i2s.txbuffer[i] = i;
        }
        return CMD_OK;
    }

    if(xstrcmp("stop", (const char*)argv[1]) == 0){
        I2S_Stop(&m_i2s);
        return CMD_OK;
    }

    if(xstrcmp("loopback", (const char*)argv[1]) == 0){
        
        for (int i = 0; i < 512; i++ ){
	        m_i2s.txbuffer[i] = i;
	        m_i2s.rxbuffer[i] = 0;
        }        

        I2S_Start(&m_i2s);
        
        /* Wait for RX and TX complete before comparison */
        //FIX: wridx should be restricted to this module while ( m_i2s.wridx < (512 - 8) );

        I2S_Stop(&m_i2s);
        /* Validate TX and RX buffer */
        for (int i=1; i< 512; i++ ){
	        if ( m_i2s.rxbuffer[i] != m_i2s.txbuffer[i-1] ){
	            console->printf("Audio loopback fail at index %d\n", i);
                break;
	        }
        }        
        
        return CMD_OK;
    }
  
    return CMD_BAD_PARAM;
}