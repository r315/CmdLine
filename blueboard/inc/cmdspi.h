#ifndef _cmdspi_h_
#define _cmdspi_h_

#define SPI_INIT (1<<0)

#define SPI_IF LPC_SSP1

typedef struct _SpiBuffer{
    uint8_t len;
    uint8_t *data;
}SpiBuffer;


#ifdef __cplusplus
extern "C" {
#endif

#include "console.h"

class CmdSpi : public ConsoleCommand{
    Console *console;
public:
    CmdSpi () : ConsoleCommand("spi") { }
    void init(void *params){console = static_cast<Console*>(params);}
    char execute(void *ptr);
    void help(void);
};

void spiWriteBuffer(SpiBuffer *buf);
void spiWrite(uint8_t *data, uint32_t len);
void spiSetFrequency(uint32_t freq);

#ifdef __cplusplus
}
#endif

#endif
