#ifndef _flash_dev_qspi_h_
#define _flash_dev_qspi_h_

#include <stdint.h>
#include "flash_dev.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FLASH_DEV_SPI_REMS          0x90    // Read Electronic Manufacturer Signature
#define FLASH_DEV_SPI_RDID          0x9F    // Read Device Identification
#define FLASH_DEV_SPI_WREN          0x06
#define FLASH_DEV_SPI_WRDI          0x04
#define FLASH_DEV_SPI_RDSR          0x05
#define FLASH_DEV_SPI_WRSR          0x01
#define FLASH_DEV_SPI_READ          0x03
#define FLASH_DEV_SPI_READ_FAST     0x0B
#define FLASH_DEV_SPI_PAGE_PROG     0x02
#define FLASH_DEV_SPI_BE_4K         0x20    // Block erase 4k
#define FLASH_DEV_SPI_BE_32K        0x52
#define FLASH_DEV_SPI_BE_64K        0xD8
#define FLASH_DEV_SPI_CE            0xC7    // Chip erase


#define FLASH_DEV_SPI_SR_SRP0       (1<<7)
#define FLASH_DEV_SPI_SR_BP4        (1<<6)
#define FLASH_DEV_SPI_SR_BP3        (1<<5)
#define FLASH_DEV_SPI_SR_BP2        (1<<4)
#define FLASH_DEV_SPI_SR_BP1        (1<<3)
#define FLASH_DEV_SPI_SR_BP0        (1<<2)
#define FLASH_DEV_SPI_SR_WEN        (1<<1)
#define FLASH_DEV_SPI_SR_BSY        (1<<0)

flashdev_res_t flashDevQspiReadId(flashdev_t *fdev, uint8_t *id);
flashdev_res_t flashDevQspiRead(flashdev_t *fdev, uint8_t *buffer, uint32_t addr, uint32_t len);
flashdev_res_t flashDevQspiWrite(flashdev_t *fdev, const uint8_t *buffer, uint32_t addr, uint32_t len);
flashdev_res_t flashDevQspiErase(flashdev_t *fdev, uint32_t addr, uint32_t size);

flashdev_res_t flashDevQspiSingleCmd(flashdev_t *fdev, uint8_t cmd);

#ifdef __cplusplus
}
#endif

#endif