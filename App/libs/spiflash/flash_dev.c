#include "flash_dev.h"
#include "flash_dev_spi.h"

/**
 * @brief
 * @param fdev
 * @return
 */
flashdev_res_t flashDevInit(flashdev_t *fdev)
{
    uint32_t id[2];
    flashdev_res_t res;
    const flashdev_info_t *info;

    if(!fdev){
        return FLASH_DEV_ERROR_PARM;
    }

    info = fdev->info;

    if(info->ops.init){
        info->ops.init(fdev);
    }

    res = flashDevReadId(fdev, (uint8_t*)id);

    if(res == FLASH_DEV_OK){
        res = (id[0] == info->mid) ? FLASH_DEV_OK : FLASH_DEV_ERROR_ID;
    }

    return res;
}
/**
 * @brief Read all manufacturer data
 * @param fdev
 * @param id    buffer with minimum of 4 bytes
 * @return
 */
flashdev_res_t flashDevReadId(flashdev_t *fdev, uint8_t *id)
{
    const flashdev_info_t *info = fdev->info;

    if(info->ops.readId){
        return info->ops.readId(fdev, id);
    }

    return FLASH_DEV_NOT_SUPPORTED;
}

/**
 * @brief
 * @param fdev
 * @param buffer
 * @param addr
 * @param len
 * @return
 */
flashdev_res_t flashDevRead(flashdev_t *fdev, uint8_t *buffer, uint32_t addr, uint32_t len)
{
    const flashdev_info_t *info = fdev->info;

    if(info->ops.read){
        return info->ops.read(fdev, buffer, addr, len);
    }

    return FLASH_DEV_NOT_SUPPORTED;
}

/**
 * @brief
 * @param fdev
 * @param pbuffer
 * @param addr
 * @param len
 * @return
 */
flashdev_res_t flashDevWrite(flashdev_t *fdev, const uint8_t *buffer, uint32_t addr, uint32_t len)
{
    const flashdev_info_t *info = fdev->info;

    if(info->ops.write){
        return info->ops.write(fdev, buffer, addr, len);
    }

    return FLASH_DEV_NOT_SUPPORTED;
}

/**
 * @brief
 * @param fdev
 * @param addr
 * @param
 * @return
 */
flashdev_res_t flashDevErase(flashdev_t *fdev, uint32_t addr, enum flashdev_blk_sz size)
{
    const flashdev_info_t *info = fdev->info;

    if(info->ops.erase){
        return info->ops.erase(fdev, addr, size);
    }

    return FLASH_DEV_NOT_SUPPORTED;
}