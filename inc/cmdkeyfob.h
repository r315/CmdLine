#ifndef _cmdkeyfob_h_
#define _cmdkeyfob_h_


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "console.h"

#define CAP_BUF_SIZE 64

typedef enum {BITSTREAM_RAW = 0, BITSTREAM_WIDTH, BITSTREAM_DECODE} bitstreamview_e;

class CmdKeyFob : public ConsoleCommand{
    Console *console;
    uint16_t capture_data[CAP_BUF_SIZE];
    uint32_t capture_count;

public:
    void init(void *params) { 
        console = static_cast<Console*>(params);
        capture_count = 0;
    }

    char execute(void *ptr);
    void help(void);
    void printBitstream(bitstreamview_e view, uint16_t *bitstream, int16_t bittime, uint8_t idle_state, uint32_t streamlen);

    CmdKeyFob () : ConsoleCommand("keyfob") { }    
};


#ifdef __cplusplus
}
#endif

#endif