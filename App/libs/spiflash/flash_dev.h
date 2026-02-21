#ifndef _FLASHDEV_H_
#define _FLASHDEV_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

#define FLASH_DESELECTED  0
#define FLASH_SELECTED    1

typedef enum flashdev_res{
    FLASH_DEV_OK,
    FLASH_DEV_ERROR,
    FLASH_DEV_BUSY,
    FLASH_DEV_TIMEOUT,
    FLASH_DEV_NOT_SUPPORTED,
    FLASH_DEV_ERROR_PARM,
    FLASH_DEV_ERROR_ID,
    FLASH_DEV_SUSPENDED,
    FLASH_DEV_BP,         // Block is protected
}flashdev_res_t;

enum flash_if{
    FLASH_IF_PARALLEL,
    FLASH_IF_SPI,
    FLASH_IF_QSPI,
};

enum flashdev_blk_sz{
    FLASH_DEV_BLK_4K,
	FLASH_DEV_BLK_32K,
	FLASH_DEV_BLK_64K,
	FLASH_DEV_BLK_ALL
};

typedef struct flashdev {
    void *bus;              // This implementation requires that bus member is the first one
    const void *info;
    void(*select)(uint8_t active);
}flashdev_t;

typedef struct flashmem_ops{
    flashdev_res_t (*init)(flashdev_t *fdev);
    flashdev_res_t (*read)(flashdev_t *fdev, uint8_t *buffer, uint32_t addr, uint32_t len);
    flashdev_res_t (*write)(flashdev_t *fdev, const uint8_t *buffer, uint32_t addr, uint32_t len);
    flashdev_res_t (*erase)(flashdev_t *bus, uint32_t addr, uint32_t len);
    flashdev_res_t (*readId)(flashdev_t *fdev, uint8_t *id);
    flashdev_res_t (*command)(flashdev_t *fdev, uint8_t opcode, uint8_t *buffer, uint32_t addr, uint32_t len);
}flashdev_ops_t;

typedef struct flashdev_info{
    const char *name;           // Mem device name
    const uint32_t size;        // Size in bytes
    const uint32_t blocksize;   // Biggest erasable unit by single command
    const uint32_t sectorsize;  // Second biggest unit erasable unit by single command
    const uint32_t pagesize;    // Minimal programmable unit by single command
    const uint32_t mid;         // Manufacturer id
    const uint32_t tce;
    const enum flash_if itf;
    const flashdev_ops_t ops;
}flashdev_info_t;

flashdev_res_t flashDevInit(flashdev_t *fdev);
flashdev_res_t flashDevRead(flashdev_t *fdev, uint8_t *pbuffer, uint32_t addr, uint32_t len);
flashdev_res_t flashDevWrite(flashdev_t *fdev, const uint8_t *pbuffer, uint32_t addr, uint32_t len);
flashdev_res_t flashDevReadId(flashdev_t *fdev, uint8_t *id);
flashdev_res_t flashDevErase(flashdev_t *fdev, uint32_t addr, enum flashdev_blk_sz size);

#ifdef __cplusplus
}
#endif

#endif