#ifndef _FLASHDEV_H_
#define _FLASHDEV_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum flashdev_res{
    FLASH_DEV_OK,
    FLASH_DEV_ERROR,
    FLASH_DEV_ERROR_PARM,
    FLASH_DEV_ERROR_ID,
    FLASH_DEV_ERROR_TIMEOUT,
    FLASH_DEV_ERROR_BP,         // Block is protected
    FLASH_DEV_ERROR_PP          // Page program
}flashdev_res_t;

enum flash_if{
    FLASH_IF_PARALLEL,
    FLASH_IF_SPI,
    FLASH_IF_QSPI,
};

typedef struct flashmem_ops{
    void(*chipSelect)(uint8_t active);
    flashdev_res_t (*init)(void *bus);
    flashdev_res_t (*write)(void *bus, const uint8_t *pbuffer, uint32_t addr, uint16_t len);
    flashdev_res_t (*read)(void *bus, uint8_t *pbuffer, uint32_t addr, uint16_t len);
    flashdev_res_t (*erase)(void *bus);
    flashdev_res_t (*erasePage)(void *bus, uint32_t addr, uint16_t len);
    flashdev_res_t (*readId)(void *bus, uint8_t *id);
}flashdev_ops_t;

typedef struct flashmem_info{
    const char *name;           // Mem device name
    const uint32_t size;        // Size in bytes
    const uint32_t blocksize;   // Biggest erasable unit by single command
    const uint32_t sectorsize;  // Second biggest unit erasable unit by single command
    const uint32_t pagesize;    // Minimal programmable unit by single command
    const uint32_t mid;         // Manufacturer id
    const enum flash_if itf;
    const flashdev_ops_t ops;
}flashmem_info_t;

typedef struct flashdev {
        void *bus;
        const flashmem_info_t *info;
        flashdev_ops_t ops;     // These ops are modifiable, for example board may assign a different chipSelect
}flashdev_t;

flashdev_res_t flashDevInit(flashdev_t *fm);
flashdev_res_t flashDevRead(flashdev_t *fm, uint8_t *pbuffer, uint32_t addr, uint16_t len);
flashdev_res_t flashDevWrite(flashdev_t *fm, const uint8_t *pbuffer, uint32_t addr, uint16_t len);
flashdev_res_t flashDevReadId(flashdev_t *fm, uint8_t *id);

#ifdef __cplusplus
}
#endif

#endif