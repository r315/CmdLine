#include "board.h"
#include "cmdplayer.h"
//#define MINIMP3_ONLY_MP3
//#define MINIMP3_IMPLEMENTATION
//#include "minimp3.h"
#include "i2s.h"
#include "pff.h"

//static mp3dec_t s_mp3d;
static i2sbus_t s_i2s;
static uint32_t s_audio_buf[1024] __aligned(1024);
static uint8_t s_temp_buf[4096] __aligned(1024);
extern FATFS sdcard;
//#include "alarm_32.c"

static void *s_aud_ptr;
static uint32_t s_aud_len = 0;
static bool s_mono;
static bool s_file;

static void audio_cb(uint32_t *stream, uint32_t len){
    DBG_PIN_HIGH;
    if(s_aud_len == 0){
        // Ensures that subsequent half-buffers are filled with zero
        // after playback ended
        memset32(stream, 0, len);
    }else{
        for(uint32_t i = 0; i < len; i++, s_aud_len--){
            if(s_aud_len == 0){
                // Fills remaining with zeros
                memset32(stream, 0, len - i);
                break;
            }
            
            if(s_mono){
                uint16_t sample = *(uint16_t*)s_aud_ptr;
                sample += 0x8000;
                *stream++ = (uint16_t)(sample >> 4);
                s_aud_ptr = (uint16_t*)s_aud_ptr + 1;
            }else{
                *stream++ = *(uint32_t*)s_aud_ptr;
                s_aud_ptr = (uint32_t*)s_aud_ptr + 1;
            }
        }

        if(s_file){
            s_file = false;
        }
    }
    DBG_PIN_LOW;
}

static void audio_init(void){
    s_i2s.bus = I2S_BUS2;
    s_i2s.sample_rate = 32000;
    s_i2s.channels = 2;
    s_i2s.data_size = 16;
    s_i2s.mode = I2S_TX_EN_MASTER;
    s_i2s.txbuffer = (uint32_t*)s_audio_buf;
    s_i2s.rxbuffer = NULL;
    s_i2s.tx_buf_len = sizeof(s_audio_buf) / 4;
    s_i2s.rx_buf_len = 0;
    s_i2s.txcp = NULL;
    s_i2s.rxcp = NULL;
    I2S_Init(&s_i2s);
    s_mono = false;
    s_file = false;
}

/**
 * @brief Plays memory buffer
 * 
 * @param buf   : Pointer to samples buffer
 * @param len   : Number of samples in buffer 
 */
void CmdPlayer::playBuffer(void *buf, uint32_t len){
    s_aud_ptr = buf;
    s_aud_len = len;
    if(s_i2s.txcp == NULL){
        s_i2s.txcp = audio_cb;
        // clear buffer, oh well the two first callbacks will fill it.
        memset32(s_audio_buf, 0, sizeof(s_audio_buf) / 4);
        I2S_Start(&s_i2s);
    }
}

void CmdPlayer::stop(void){
    s_aud_len = 0;   
}

void CmdPlayer::help(void){
    
}

/**
 * @brief Plays raw data 16bit 32kHz
 * 
 */
void CmdPlayer::rawFile(){
    uint32_t br;
    char c;
    uint8_t *ptr = s_temp_buf + (512 * 2);
    pf_read(s_temp_buf, 512 * 2 * 2 , (UINT*)&br); // Two buffers of 512 samples, 16 bit each
    playBuffer(s_temp_buf, sdcard.fsize / 2); // 16bit samples
    do{
        if(!s_file){
            s_file = true;
            uint32_t count = (s_aud_len < 512) ? s_aud_len * 2 : 1024;
            if(ptr < s_temp_buf + (512 * 2)){
                ptr = s_temp_buf + (512 * 2);
                s_aud_ptr = (uint32_t*)&s_temp_buf[0];
            }else{
                ptr = s_temp_buf;
                s_aud_ptr = (uint32_t*)(s_temp_buf + (512 * 2));
            }
            pf_read(ptr, count, (UINT*)&br);                   
        }
    }while(!console->getCharNonBlocking(&c) && s_aud_len); 
}

char CmdPlayer::execute(int argc, char **argv){

    if(xstrcmp("init", (const char*)argv[1]) == 0){
        audio_init();
        //memset32(s_test_buf, 0xAAAAAAAA, sizeof(s_test_buf)/8);
        //memset32(s_test_buf  + sizeof(s_test_buf)/8, 0xBBBBBBBB, sizeof(s_test_buf)/8);
        playBuffer(NULL, 0);
        return CMD_OK;
    }else if(xstrcmp("stop", (const char*)argv[1]) == 0){
        I2S_Stop(&s_i2s);
        return CMD_OK;
    }else if(xstrcmp("mute", (const char*)argv[1]) == 0){
        I2S_Mute(&s_i2s, !s_i2s.mute);
        return CMD_OK;
    }else if(xstrcmp("pause", (const char*)argv[1]) == 0){
        //s_mono = true;
        //playBuffer((uint32_t*)alarm_32, sizeof(alarm_32) / 2);
        return CMD_OK;
    }else if(xstrcmp("file", (const char*)argv[1]) == 0){
        if(pf_open (argv[2]) == FR_OK){
            const char *ext = chrinstr(argv[2], '.');
            if(xstrcmp(".raw", ext) == 0){
                console->print("Size: %d \n", sdcard.fsize);
                s_file = true;
                s_mono = true;
                rawFile();
            }else if(xstrcmp(".wav", ext) == 0){
                uint32_t br;
                pf_read(s_temp_buf, 44 , (UINT*)&br); // skip header
                s_file = true;
                s_mono = true;
                rawFile();
            }
            stop();
        }else{
            console->print("can't open file: %s \n", argv[2]);
        }
        return CMD_OK;
    }

    return CMD_BAD_PARAM;
}
