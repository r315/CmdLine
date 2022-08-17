#include "board.h"
#include "cmdplayer.h"
//#define MINIMP3_ONLY_MP3
//#define MINIMP3_IMPLEMENTATION
//#include "minimp3.h"
#include "i2s.h"

//static mp3dec_t s_mp3d;
static i2sbus_t s_i2s;
static uint32_t s_audio_buf[1024] __aligned(1024);
static uint32_t s_test_buf[1024] __aligned(1024);

//#include "alarm_32.c"

static void *s_aud_ptr;
static uint32_t s_aud_len = 0;
static bool s_mono;

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
}

/**
 * @brief Plays memory buffer
 * 
 * @param buf   : Pointer to samples buffer
 * @param len   : Number of samples in buffer 
 */
void CmdPlayer::playBuffer(uint32_t *buf, uint32_t len){
    s_aud_ptr = buf;
    s_aud_len = len;
    if(s_i2s.txcp == NULL){
        s_i2s.txcp = audio_cb;
        // clear buffer, oh well the two first callbacks will fill it.
        memset32(s_audio_buf, 0, sizeof(s_audio_buf) / 4);
        I2S_Start(&s_i2s);
    }
}

void CmdPlayer::help(void){
    
}

char CmdPlayer::execute(int argc, char **argv){

    if(xstrcmp("test", (const char*)argv[1]) == 0){
        audio_init();
        memset32(s_test_buf, 0xAAAAAAAA, sizeof(s_test_buf)/8);
        memset32(s_test_buf  + sizeof(s_test_buf)/8, 0xBBBBBBBB, sizeof(s_test_buf)/8);
        playBuffer((uint32_t*)s_test_buf, sizeof(s_test_buf) / 4);
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
    }

    return CMD_BAD_PARAM;
}
