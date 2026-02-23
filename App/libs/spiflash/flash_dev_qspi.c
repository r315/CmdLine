#include <string.h>
#include "flash_dev.h"
#include "flash_dev_spi.h"
#include "board.h"


flashdev_res_t flashDevQspiSingleCmd(flashdev_t *fdev, uint8_t cmd)
{
    qspitrf_t trf = {0};
    trf.inst = cmd;  // command
    trf.imode = 1;   // Instruction on single line

    return (QSPI_Command(fdev->bus, &trf) == QSPI_OK) ? FLASH_DEV_OK : FLASH_DEV_ERROR;
}

/**
 * @brief Generic read device identification
 * @param fdev
 * @param id id[0] = Manufacturer ID
 *           id[1] = Device ID
 *           id[2] = Second Manufacturer ID
 * @return operation status
 */
flashdev_res_t flashDevQspiReadId(flashdev_t *fdev, uint8_t *id)
{
    qspires_t res;
    qspitrf_t trf = {0};

    trf.inst = FLASH_DEV_SPI_RDID;
    trf.op = 1;      // Read
    trf.admode = 0;  // No address
    trf.abmode = 0;  // No alternate bytes
    trf.imode = 1;   // Single line
    trf.dmode = 1;   // Single line
    trf.len = 3;     // Response 3 bytes
    trf.dummy = 0;   // No dummy cycles
    trf.buffer = id;  // Buffer for read, buffer pointer is destroyed

    res = QSPI_Command(fdev->bus, &trf);
    // swap
    uint8_t tmp = id[0];
    id[0] = id[2];
    id[2] = tmp;

    return res == QSPI_OK ? FLASH_DEV_OK : FLASH_DEV_ERROR;
}