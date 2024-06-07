

/*
** Made by fabien le mentec <texane@gmail.com>
**
** Started on  Sat Nov 14 17:18:24 2009 texane
** Last update Tue Nov 17 18:14:57 2009 texane
*/

// https://github.com/texane/picisp/blob/master/pic18f/src/main.c

#ifndef SPI_H_INCLUDED
#define SPI_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "spi.h"

enum avresp{
    AVR_RESPONSE_OK = 0,
    AVR_RESPONSE_FAIL = -1,
    AVR_RESPONSE_TIMEOUT
};


uint8_t avrProgrammingEnable(uint8_t en, uint8_t trydw);
void avrLoadProgramPage(uint8_t addr, uint16_t value);
uint16_t avrReadProgram(uint16_t addr);
void avrWriteProgramPage(uint16_t addr);
void avrDeviceCode(uint8_t *buf);
void avrWriteLockBits(uint8_t);
void avrChipErase(void);
void avrWriteFuses(uint8_t lh, uint8_t fuses);
uint32_t avrReadFuses(void);
void avr_if_Init(spibus_t *spi, uint32_t pin);
const char *avrDeviceName(uint32_t sig);


    uint8_t spi_read_eeprom(uint16_t);
    void spi_write_eeprom(uint16_t, uint8_t);
    uint8_t spi_read_lock_bits(void);
    void spi_write_fuse_bits(uint8_t);
    void spi_write_fuse_high_bits(uint8_t);
    void spi_write_extended_fuse_bits(uint8_t);
    uint8_t spi_read_fuse_bits(void);
    uint8_t spi_read_extended_fuse_bits(void);
    uint8_t spi_read_fuse_high_bits(void);
    void spi_read_calibration_bytes(uint8_t *);
    uint8_t spi_write_read(uint8_t *);

#ifdef __cplusplus
}
#endif

#endif /* ! SPI_H_INCLUDED */