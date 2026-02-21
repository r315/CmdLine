#include <stdio.h>
#include "spiflash/flash_dev.h"
#include "spiflash/flash_dev_spi.h"

#define AT25SF321B_REMS         0x1F15
#define AT25SF321B_RDID         0x1f8701

#define AT25SF321B_CMD_RDSR2    0x35
#define AT25SF321B_CMD_RDSR3    0x15
#define AT25SF321B_CMD_WRSR2    0x31
#define AT25SF321B_CMD_WRSR3    0x11
#define AT25SF321B_CMD_RDUID    0x4B

#define RENESSAS_SR1_SRP0       0x80
#define RENESSAS_SR1_BP4        0x40
#define RENESSAS_SR1_BP3        0x20
#define RENESSAS_SR1_BP2        0x10
#define RENESSAS_SR1_BP1        0x08
#define RENESSAS_SR1_BP0        0x04
#define RENESSAS_SR1_WEL        0x02
#define RENESSAS_SR1_RDY        0x01

#define RENESSAS_SR2_E_SU       0x80
#define RENESSAS_SR2_CMP        0x40
#define RENESSAS_SR2_LB3        0x20
#define RENESSAS_SR2_LB2        0x10
#define RENESSAS_SR2_LB1        0x08
#define RENESSAS_SR2_P_SUS      0x04
#define RENESSAS_SR2_QE         0x02
#define RENESSAS_SR2_SRP1       0x01

#define RENESSAS_tCE            30000UL  //Maximum chip erase time (30s)

#if 0
static flashdev_res_t renessas_rdsr(void *bus, uint8_t cmd, uint8_t *status)
{
    uint16_t sr_data = cmd;
    flashdev_t *fdev = bus;

    fdev->ops.select(FLASH_SELECTED);
    SPI_Transfer(bus, (uint8_t*)&sr_data, 2);
    fdev->ops.select(FLASH_DESELECTED);

    *status = (uint8_t)sr_data;

    return FLASH_DEV_OK;
}

static void renessas_wrsr_ex(flashdev_t *fdev, uint8_t sr, uint16_t data)
{
    //flashspi_write_enable();

    //flashSelect(FLASH_SELECTED);
    //spiflash_sendbyte (sr);
    //spiflash_sendbyte (data);
    //flashSelect(FLASH_DESELECTED);
}

static flashdev_res_t renessas_wait_ready(void)
{
   return flashspi_wait_ready (RENESSAS_tCE);
}
#endif
static flashdev_res_t at25sf321b_init(flashdev_t *fdev)
{
    uint32_t id;
    flashdev_res_t res;

    if((res = flashDevReadId(fdev, (uint8_t*)&id)) != FLASH_DEV_OK){
        return res;
    }
/*
if((res = resrenessas_rdsr_ex(AT25SF321B_CMD_RDSR2, &status)) == FLASH_DEV_OK){
    if(status & RENESSAS_SR2_QE){
        renessas_wrsr_ex(AT25SF321B_CMD_RDSR2, status & ~RENESSAS_SR2_QE);
    }
}
*/

    return (id == AT25SF321B_RDID) ? FLASH_DEV_OK: FLASH_DEV_ERROR_ID;
}

const flashdev_info_t at25sf321b =
{
    .name = "AT25SF321B",
    .mid = AT25SF321B_RDID,
    .size = 0x00400000, /*4m byte*/
    .pagesize = 256,
    .sectorsize = 0x1000,
    .blocksize = 0x10000,
    .itf = FLASH_IF_SPI,
    .tce = RENESSAS_tCE,
    .ops = {
        .init = at25sf321b_init,
        .erase = flashDevSpiErase,
        .read = flashDevSpiRead,
        .write = flashDevSpiWrite,
        .readId = flashDevSpiReadId,
        .command = NULL
    }
};
