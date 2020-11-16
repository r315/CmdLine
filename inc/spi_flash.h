#ifndef _spi_flash_h_
#define _spi_flash_h_

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MFD_ID          0x90
#define JEDEC_ID        0x9F


/* PM25LV512 */
#define WREN            0x06
#define WRDI            0x04
#define RDSR            0x05
#define WRSR            0x01
#define READ            0x03
#define FAST_READ       0x0B
#define PG_PROG         0x02
#define SECTOR_ERASE    0xD7
#define BLOCK_ERASE     0xD8
#define CHIP_ERASE      0xC7
#define RDID            0xAB
#define PM25LV512_ID    0x9D7B
/* Status Register */
/* | WPEN |  X  |  X  |  X  | BP1 | BP0 | WEN | RDY | */
#define SR_WPEN         (1<<7)
#define SR_BP1          (1<<3)
#define SR_BP0          (1<<2)
#define SR_WEN          (1<<1)
#define SR_RDY          (1<<0)

uint32_t flashDevice_ID(void);
uint32_t flashJEDEC_ID(void);
uint32_t flashMFD_ID(void);

uint8_t flashReadStatus(void);
void flashWriteStatus(uint8_t st);

uint32_t flashRead(uint32_t addr, uint8_t *dst, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif