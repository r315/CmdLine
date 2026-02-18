/**
 *
 * \file
 *
 * GD25LQ16 constants and commands.
 *
 * GD25LQ16 External Flash loader for STM32 with QSPI.
 *
 * Author: Hugo Reis, 2026
 *
 */

#ifndef __SPI_FLASH_DEV_H
#define __SPI_FLASH_DEV_H

#define FLASH_DEV_INFO                      "GD26LQ16_STM32L412KB"
#define FLASH_DEV_FLASH_SIZE        		0x200000 	// 16 MBits => 2MBytes
#define FLASH_DEV_BLOCK_SIZE                0x10000   	// 32 block of 64KBytes
#define FLASH_DEV_SECTOR_SIZE               0x1000    	// 512 sectors of 4kBytes
#define FLASH_DEV_PAGE_SIZE                 0x100     	// 8192 pages of 256 bytes

#define FLASH_DEV_BLOCK_COUNT               32
#define FLASH_DEV_ADDRESS_MASK              0x00FFFFFF

/* Timings */
#define FLASH_DEV_BULK_ERASE_MAX_TIME      	20000
#define FLASH_DEV_BLOCK_ERASE_MAX_TIME    	1200
#define FLASH_DEV_SECTOR_ERASE_MAX_TIME    	500

#define FLASH_DEV_DUMMY_CYCLES_READ_FAST    8
#define FLASH_DEV_DUMMY_CYCLES_READ_QUAD    4

/* Reset Operations */
#define RESET_ENABLE_CMD                    0x66
#define RESET_MEMORY_CMD 		            0x99

/* Identification Operations */
#define READ_MANUF_ID                       0x90
#define READ_MANUF_ID_DUAL                  0x92
#define READ_MANUF_ID_QUAD                  0x94
#define READ_JEDEC_ID                       0x9F

/* Read Operations */
#define READ_CMD                            0x03
#define FAST_READ_CMD                       0x0B
#define DUAL_OUT_FAST_READ_CMD              0x3B
#define DUAL_IO_FAST_READ_CMD               0xBB
#define QUAD_OUT_FAST_READ_CMD              0x6B
#define QUAD_IO_FAST_READ_CMD               0xEB
#define QUAD_IO_WORD_FAST_READ_CMD          0xE7

/* Write Operations */
#define WRITE_ENABLE_CMD                    0x06
#define WRITE_DISABLE_CMD                   0x04

/* Register Operations */
#define READ_STATUS_REG_CMD                 0x05
#define READ_STATUS_REG2_CMD                0x35
#define WRITE_STATUS_REG_CMD                0x01
#define WRITE_STATUS_VOLATILE               0x50

/* Program Operations */
#define PAGE_PROG_CMD						0x02
#define QUAD_PAGE_PROG_CMD					0x32

/* Erase Operations */
#define SECTOR_ERASE_CMD              	 	0x20		// sector erase 4kB
#define BLOCK_ERASE_32_CMD                  0x52		// block erase 32kB
#define BLOCK_ERASE_64_CMD                  0xD8		// block erase 64kB
#define BULK_ERASE_CMD                      0xC7		// full chip erase

/* Other comands */
#define PROG_ERASE_SUSPEND_CMD             	0x75
#define PROG_ERASE_RESUME_CMD          		0x7A
#define ENABLE_QPI_CMD                      0x38
#define SET_BURST_WRAP_CMD                  0x77
#define DEEP_PWR_DOWN_CMD                   0xB9
#define RELEASE_PWR_DOWN_CMD                0xAB        // Release power down and read id

/* Security */
#define ERASE_REGISTERS_CMD                 0x44
#define PROGRAM_REGISTERS_CMD               0x42
#define READ_REGISTERS_CMD                  0x48

/**
  * @brief  Registers
  */
/* Status Register */
#define FLASH_DEV_SR_BUSY    				((uint8_t)0x01)    /*!< Write in progress */
#define FLASH_DEV_SR_WEL                    ((uint8_t)0x02)    /*!< Write enable latch */
#define FLASH_DEV_SR_BL0                    ((uint8_t)0x04)    /*!< Block Protect 0 */
#define FLASH_DEV_SR_BL1                    ((uint8_t)0x08)    /*!< Block Protect 1 */
#define FLASH_DEV_SR_BL2                    ((uint8_t)0x10)    /*!< Block Protect 2 */
#define FLASH_DEV_SR_BL3		            ((uint8_t)0x20)    /*!< Block Protect 3 */
#define FLASH_DEV_SR_BL4		            ((uint8_t)0x40)    /*!< Block Protect 4 */
#define FLASH_DEV_SR_SRP0					((uint8_t)0x80)    /*!< Status register write enable/disable */

/* Status Register 2 */
#define FLASH_DEV_SR2_SRP1                	((uint8_t)0x01)    /*!< Status register 2 write enable/disable */
#define FLASH_DEV_SR2_QE            		((uint8_t)0x02)    /*!< Quad Enable */
#define FLASH_DEV_SR2_SUS2               	((uint8_t)0x04)    /*!< Suspend Status 2 */
#define FLASH_DEV_SR2_LB1               	((uint8_t)0x08)    /*!< Write protect 1 */
#define FLASH_DEV_SR2_LB2               	((uint8_t)0x10)    /*!< Write protect 2 */
#define FLASH_DEV_SR2_LB3               	((uint8_t)0x20)    /*!< Write protect 3 */
#define FLASH_DEV_SR2_CMP	    	       	((uint8_t)0x40)    /*!< Complement Protect */
#define FLASH_DEV_SR2_SUS1               	((uint8_t)0x80)    /*!< Suspend Status 1 */

#endif /* __SPI_FLASH_DEV_H */
