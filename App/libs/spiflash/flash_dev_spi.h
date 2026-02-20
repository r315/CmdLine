#ifndef _flash_dev_spih_
#define _flash_dev_spi_h_

#include "stdint.h"
#include "spi.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FLASH_DEV_SPI_MFD       0x90
#define FLASH_DEV_SPI_JEDEC     0x9F
#define FLASH_DEV_SPI_WREN      0x06
#define FLASH_DEV_SPI_WRDI      0x04
#define FLASH_DEV_SPI_RDSR      0x05
#define FLASH_DEV_SPI_WRSR      0x01
#define FLASH_DEV_SPI_READ      0x03
#define FLASH_DEV_SPI_READ_FAST 0x0B
#define FLASH_DEV_SPI_PAGE_PROG 0x02
#define FLASH_DEV_SPI_SE        0xD7
#define FLASH_DEV_SPI_BE        0xD8
#define FLASH_DEV_SPI_CE        0xC7


#define FLASH_DEV_SPI_SR_WPEN         (1<<7)
#define FLASH_DEV_SPI_SR_BP1          (1<<3)
#define FLASH_DEV_SPI_SR_BP0          (1<<2)
#define FLASH_DEV_SPI_SR_WEN          (1<<1)
#define FLASH_DEV_SPI_SR_RDY          (1<<0)



#ifdef __cplusplus
}
#endif

#endif