#include "flash_dev.h"
#include "flash_dev_spi.h"

#include "board.h"

#define LOW     0
#define HIGH    1

#define FLASH_OP_ASSIGN(dev, op) if(##dev->info->ops.##op){ ##dev->ops.##op=##dev->info->ops.##op;}

/**
 * @brief Generic spi MFD read
 * @param fdev
 * @param id
 * @return
 */
static flashdev_res_t flashDevSpiReadId(flashdev_t *fdev, uint8_t *id)
{
    spibus_t *spi = fdev->bus;

    uint32_t trf = 0xFFFFFF00 | (FLASH_DEV_SPI_MFD << 00);

    BOARD_SPI_CS_LOW;
    SPI_Transfer(spi, (uint8_t*)&trf, 4);
    trf = 0xFFFFFFFF;
    SPI_Transfer(spi, (uint8_t*)&trf, 3);
    BOARD_SPI_CS_HIGH;

    memcpy(id, trf, 4);

    return FLASH_DEV_OK;
}


/**
 * @brief
 * @param fdev
 * @return
 */
flashdev_res_t flashDevInit(flashdev_t *fdev)
{
    uint8_t id[8];
    flashdev_res_t res;

    if(!fdev){
        return FLASH_DEV_ERROR_PARM;
    }

    if(fdev->info->ops.init){
        fdev->ops.init = fdev->info->ops.init;
        fdev->ops.init(fdev->bus);
    }

    FLASH_OP_ASSIGN(fdev, read);
    FLASH_OP_ASSIGN(fdev, write);
    FLASH_OP_ASSIGN(fdev, erase);
    FLASH_OP_ASSIGN(fdev, erasePage);
    FLASH_OP_ASSIGN(fdev, readId);

    res = flashdevReadId(fdev, &id);

    if(res == FLASH_DEV_OK){
        res = (id == fdev->info->mid) ? FLASH_DEV_OK : FLASH_DEV_ERROR_ID;
    }

    return res;
}
/**
 * @brief Read all manufacturer data
 * @param fdev
 * @param id    buffer with minimum of 8 bytes
 * @return
 */
flashdev_res_t flashdevReadId(flashdev_t *fdev, uint8_t *id)
{
    flashdev_res_t res;

    if(fdev->ops.readId){
        return fdev->ops.readId(fdev->bus, id);
    }

    switch(fdev->info->itf){
        case FLASH_IF_SPI:
            return flashDevSpiReadId(fdev, id);
        default:
            return FLASH_DEV_ERROR_PARM;
    }
}

#if 0
/**
 * @brief Read flashJEDEC_ID Flash id
 *
 * \return : id[0] = Manufacturer ID
 *           id[1] = Device ID
 *           id[2] = Second Manufacturer ID
 * */
uint32_t flashJEDEC_ID(void){
    uint32_t id;

    id = 0xFFFFFF00 | (JEDEC_ID << 00);

    BOARD_SPI_CS_LOW;
    BOARD_SPI_Read((uint8_t*)&id, 4);
    BOARD_SPI_CS_HIGH;

    return id;// & 0x00FFFFFF;
}

/**
 * @brief Read flashMFR_ID Flash id
 *
 * \return : id[0] = Manufacturer ID
 *           id[1] = Device ID
 *           id[2] = Second Manufacturer ID
 * */
uint32_t flashMFD_ID(void){
    uint32_t id;

    id = 0xFFFFFF00 | (MFD_ID << 00);

    BOARD_SPI_CS_LOW;
    BOARD_SPI_Read((uint8_t*)&id, 3);
    id = 0xFFFFFFFF;
    BOARD_SPI_Read((uint8_t*)&id, 3);
    BOARD_SPI_CS_HIGH;

    return id & 0x00FFFFFF;
}

uint8_t flashReadStatus(void){
    uint8_t status[2];

    status[0] = RDSR;
    status[1] = 0xFF;

    BOARD_SPI_CS_LOW;
    BOARD_SPI_Read(status, 2);
    BOARD_SPI_CS_HIGH;

    return status[1];
}

static void flashCommand(uint8_t *cmd, uint8_t size){
    BOARD_SPI_CS_LOW;
    BOARD_SPI_Write(cmd, size);
    BOARD_SPI_CS_HIGH;
}
/*
static void flashWriteEnable(uint8_t en){
    if(en){
        flashCommand(WREN);
    }else{
        flashCommand(WRDI);
    }
}
*/
void flashWriteStatus(uint8_t st){
    uint8_t data[2];

    data[0] = WREN;
    if(!(flashReadStatus() & SR_WEN)){
        flashCommand(data, 1);
    }
    data[0] = WRSR;
    data[1] = st;
    flashCommand(data, 2);
}

uint32_t flashRead(uint32_t addr, uint8_t *dst, uint32_t size){
    uint8_t hdr[5];
    hdr[0] = FAST_READ;
    hdr[1] = (uint8_t)(addr >> 16);
    hdr[2] = (uint8_t)(addr >> 8);
    hdr[3] = (uint8_t)(addr >> 0);
    hdr[4] = 0xFF;

    BOARD_SPI_CS_LOW;
    BOARD_SPI_Write(hdr, sizeof(hdr));
    BOARD_SPI_Read(dst, size);
    BOARD_SPI_CS_HIGH;
    return size;
}
#endif