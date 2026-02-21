#include <string.h>
#include "flash_dev.h"
#include "flash_dev_spi.h"
#include "board.h"

static void flashDevSpiCmd(flashdev_t *fdev, uint8_t *cmd, uint8_t len)
{
    fdev->select(FLASH_SELECTED);
    SPI_Xchg(fdev->bus, cmd, len);
    fdev->select(FLASH_DESELECTED);
}

static void flashDevSpiSingleCmd(flashdev_t *fdev, uint8_t cmd)
{
    flashDevSpiCmd(fdev, &cmd, 1);
}
/*
static uint8_t flashDevSpiReadStatus(flashdev_t *fdev, uint8_t sr)
{
    uint8_t cmd[2];
    cmd[0] = sr;

    flashDevSpiCmd(fdev, cmd, sizeof(cmd));

    return cmd[1];
}
*/
static flashdev_res_t flashDevSpiWaitStatus(flashdev_t *fdev, uint8_t sr, uint8_t bit, uint8_t expected, uint32_t timeout)
{
    uint32_t start_tick = GetTick();
    flashdev_res_t res = FLASH_DEV_OK;

    // Assert CS and send command
    fdev->select(FLASH_SELECTED);
    SPI_Transfer(fdev->bus, (uint8_t*)&sr, 1);

	while (1)
	{
        // polling status
        sr = 0;
        SPI_Transfer(fdev->bus, (uint8_t*)&sr, 1);

		if (!!(sr & bit) == expected){
			break;
        }

        if(GetTick() - start_tick > timeout){
			res = FLASH_DEV_TIMEOUT;
            break;
        }
	}
    // Deassert CS
    fdev->select(FLASH_DESELECTED);

    return res;
}

/**
 * @brief Generic read device identification
 * @param fdev
 * @param id id[0] = Manufacturer ID
 *           id[1] = Device ID
 *           id[2] = Second Manufacturer ID
 * @return operation status
 */
flashdev_res_t flashDevSpiReadId(flashdev_t *fdev, uint8_t *id)
{
    uint32_t trf = 0xFFFFFF00 | FLASH_DEV_SPI_RDID;

    flashDevSpiCmd(fdev, (uint8_t*)&trf, 4);

    id[0] = *((uint8_t*)&trf + 3);
    id[1] = *((uint8_t*)&trf + 2);
    id[2] = *((uint8_t*)&trf + 1);

    return FLASH_DEV_OK;
}

/**
 * @brief
 * @param addr
 * @param dst
 * @param size
 * @return
 */
flashdev_res_t flashDevSpiRead(flashdev_t *fdev, uint8_t *buffer, uint32_t addr, uint32_t len)
{
    uint8_t cmd[] = {
        FLASH_DEV_SPI_READ_FAST,
        (uint8_t)(addr >> 16),
        (uint8_t)(addr >> 8),
        (uint8_t)(addr >> 0),
        0xff
    };

    fdev->select(FLASH_SELECTED);
    SPI_Transfer(fdev->bus, cmd, sizeof(cmd));  // Command has to be sent separately
    SPI_Xchg(fdev->bus, buffer, len);
    fdev->select(FLASH_DESELECTED);

    return FLASH_DEV_OK;
}

/**
 * @brief
 * @param fdev
 * @param buffer    Data to be written, this buffer must be writable
 * @param addr
 * @param len
 * @return
 */
flashdev_res_t flashDevSpiWrite(flashdev_t *fdev, const uint8_t *buffer, uint32_t addr, uint32_t len)
{
    uint32_t write_size, end_addr;
    uint8_t cmd[4];
    flashdev_res_t res;
    const flashdev_info_t *info = fdev->info;

    /* Calculation of the size between the write address and the end of the page */
	write_size = info->pagesize - (addr % info->pagesize);
    /* Check if the size of the data is less than the remaining place in the page */
	if (write_size > len){
		write_size = len;
	}

    end_addr = addr + len;

	do {
        /* Perform the write page by page */
        flashDevSpiSingleCmd(fdev, FLASH_DEV_SPI_WREN);
        /* Page program */
        cmd[0] = FLASH_DEV_SPI_PAGE_PROG,
        cmd[1] = (uint8_t)(addr >> 16),
        cmd[2] = (uint8_t)(addr >> 8),
        cmd[3] = (uint8_t)(addr >> 0),

        fdev->select(FLASH_SELECTED);
        SPI_Transfer(fdev->bus, cmd, sizeof(cmd));  /* Send command */
        SPI_Transfer(fdev->bus, buffer, len);       /* Send data */
        fdev->select(FLASH_DESELECTED);

        res = flashDevSpiWaitStatus(fdev, FLASH_DEV_SPI_RDSR, FLASH_DEV_SPI_SR_BSY, RESET, info->tce);
		if (res != FLASH_DEV_OK){
            break;
        }

        addr += write_size;

		write_size = ((addr + info->pagesize) > end_addr) ? (end_addr - addr) : info->pagesize;
	} while (addr < end_addr);

    return res;
}

/**
 * @brief
 * @param fdev
 * @param addr
 * @param size
 * @return
 */
flashdev_res_t flashDevSpiErase(flashdev_t *fdev, uint32_t addr, uint32_t size)
{
    const flashdev_info_t *info = fdev->info;

    uint8_t cmd[] = {
        0x00,
        (uint8_t)(addr >> 16),
        (uint8_t)(addr >> 8),
        (uint8_t)(addr >> 0),
    };

	switch (size)
	{
	    default:
            return FLASH_DEV_NOT_SUPPORTED;
        case FLASH_DEV_BLK_4K:
            cmd[0]	    = FLASH_DEV_SPI_BE_4K;
            break;
        case FLASH_DEV_BLK_32K:
            cmd[0]	    = FLASH_DEV_SPI_BE_32K;
            break;
        case FLASH_DEV_BLK_64K:
            cmd[0]	    = FLASH_DEV_SPI_BE_64K;
            break;
        case FLASH_DEV_BLK_ALL:
            cmd[0]	    = FLASH_DEV_SPI_CE;
            break;
	}

    // Enable write operations
	flashDevSpiSingleCmd(fdev, FLASH_DEV_SPI_WREN);
    // Send Erase command
    flashDevSpiCmd(fdev, cmd, (cmd[0] == FLASH_DEV_SPI_CE) ? 1 : sizeof(cmd));

    return flashDevSpiWaitStatus(fdev, FLASH_DEV_SPI_RDSR, FLASH_DEV_SPI_SR_BSY, RESET, info->tce);
}
