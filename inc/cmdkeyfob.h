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

public:
    void init(void *params) { console = static_cast<Console*>(params);}

    char execute(void *ptr);
    void help(void);
    void printBitstream(bitstreamview_e view, uint16_t *bitstream, uint32_t bittime, uint32_t streamlen);

    CmdKeyFob () : ConsoleCommand("keyfob") { }    
};


#ifdef __cplusplus
}
#endif

#endif