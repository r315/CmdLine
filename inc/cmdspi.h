#ifndef _cmdspi_h_
#define _cmdspi_h_

#define SPI_INIT (1<<0)

#define SPI_IF LPC_SSP1

typedef struct _SpiBuffer{
    uint8_t len;
    uint8_t *data;
}SpiBuffer;


char spiCmd(void *);
char spiHelp(void *);
char spiWrite(void *);
char spiRead(void *);
char spiWriteBuffer(SpiBuffer *);
#endif
