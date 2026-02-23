#include <stdio.h>
#include "qspi.h"
#include "spiflash/flash_dev.h"
#include "spiflash/flash_dev_spi.h"
#include "spiflash/flash_dev_qspi.h"

#define GD25LQ16_REMS   0xC814
#define GD25LQ16_RDID   0xC86015
#define GD25LQ16_tCE    20000

#define GD25LQ16_VOLATILE_SR_CMD    0x50

static qspires_t gd25lq16_StatusRead(flashdev_t *fdev, uint8_t srcmd, uint8_t *status)
{
    qspitrf_t trf = {0};

    trf.inst = srcmd;
    trf.op = 1;      // Read
    trf.admode = 0;  // No address
    trf.abmode = 0;  // No alternate bytes
    trf.imode = 1;   // Single line
    trf.dmode = 1;   // Single line
    trf.len = 1;     // Response 1 byte
    trf.dummy = 0;   // No dummy cycles
    trf.buffer = status;

    return QSPI_Command(fdev->bus, &trf);
}

static qspires_t gd25lq16_StatusWrite(flashdev_t *fdev, uint8_t *status)
{
    int res;
    qspitrf_t trf = {0};

#ifdef GD25LQ16_VOLATILE_SR_CMD
    if ((res = flashDevQspiSingleCmd(fdev, GD25LQ16_VOLATILE_SR_CMD)) != FLASH_DEV_OK)
#else
    if ((res = flashDevQspiSingleCmd(fdev, FLASH_DEV_SPI_WREN)) != QSPI_OK)
#endif
    {
        return res;
    }

    trf.inst = FLASH_DEV_SPI_WRSR;
    trf.op = 0;      // Write
    trf.imode = 1;   // Single line
    trf.admode = 0;  // No address
    trf.abmode = 0;  // No alternate bytes
    trf.dmode = 1;   // Single line
    trf.len = 2;     // 2 bytes
    trf.dummy = 0;   // No dummy cycles
    trf.buffer = status;

    return QSPI_Command(fdev->bus, &trf);
}

static qspires_t gd25lq16_QuadEn(flashdev_t *fdev, uint8_t en)
{
    uint8_t sr[2];
    qspires_t res;

    // Read SR 2
    if ((res = gd25lq16_StatusRead(fdev, FLASH_DEV_SPI_RDSR2, &sr[1])) != QSPI_OK){
        return res;
    }
    // Check if already in requested state
    if (!!(sr[1] & FLASH_DEV_SPI_SR_QE) == en){
        return QSPI_OK;
    }
    // Set QE bit
    sr[1] = en ? sr[1] | FLASH_DEV_SPI_SR_QE : sr[1] & ~(FLASH_DEV_SPI_SR_QE);
    // Append SR 1
    if ((res = gd25lq16_StatusRead(fdev, FLASH_DEV_SPI_RDSR, sr)) != QSPI_OK){
        return res;
    }
    // Write SR1 and SR2
    return gd25lq16_StatusWrite(fdev, (uint8_t*)&sr);
}

static qspires_t gd25lq16_MappedRead(flashdev_t *fdev)
{
    qspitrf_t trf = {0};

    trf.inst = FLASH_DEV_SPI_READ_QUAD,
    trf.op = 3;      // Mapped
    trf.imode = 1;   // Single line
    trf.admode = 3;  // Quad
    trf.abmode = 3;  // Quad
    trf.dmode = 3;   // Quad
    trf.absize = 0;  // 8bit
    trf.adsize = 2;  // 24bit address
    trf.dummy = 4;   // 4 dummy cycles

    return QSPI_MapMode(fdev->bus, &trf);
}

static flashdev_res_t gd25lq16_Init(flashdev_t *fdev)
{
    flashDevQspiSingleCmd(fdev, FLASH_DEV_SPI_RST_EN);
    flashDevQspiSingleCmd(fdev, FLASH_DEV_SPI_RESET);
    gd25lq16_QuadEn(fdev, 1);
    return gd25lq16_MappedRead(fdev) == QSPI_OK ? FLASH_DEV_OK : FLASH_DEV_ERROR;
}

const flashdev_info_t gd25lq16 =
{
    .name = "GD25LQ16",
    .mid = GD25LQ16_RDID,
    .size = 0x00200000, /* 2MB byte */
    .pagesize = 256,
    .sectorsize = 0x1000,
    .blocksize = 0x10000,
    .itf = FLASH_IF_QSPI | FLASH_IF_SPI,
    .tce = GD25LQ16_tCE,
    .ops = {
        .init = gd25lq16_Init,
        .erase = NULL,
        .read = NULL,
        .write = NULL,
        .readId = NULL,
        .command = NULL
    }
};
