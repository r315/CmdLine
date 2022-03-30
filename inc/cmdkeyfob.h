#ifndef _cmdkeyfob_h_
#define _cmdkeyfob_h_


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "console.h"

#define CAP_BUF_SIZE 512

typedef enum {BITSTREAM_RAW = 0, BITSTREAM_WIDTH, BITSTREAM_DECODE, SERIAL_SCOPE, BITSTREAM_JSON} bitstreamview_e;

class CmdKeyFob : public ConsoleCommand{
    Console *console;
    bool m_init;
    bool m_capturing;
    uint16_t m_buf[CAP_BUF_SIZE];

public:
    uint32_t m_cap_count;

    void init(void *params) { 
        console = static_cast<Console*>(params);
        m_cap_count = 0;
        m_init = false;
        m_capturing = false;
    }

    char execute(void *ptr);
    void help(void);
    void printBitstream(bitstreamview_e view, uint16_t *bitstream, uint32_t len, int16_t bittime, uint8_t idle_state);

    CmdKeyFob () : ConsoleCommand("keyfob") { }    
};


#ifdef __cplusplus
}
#endif

#endif