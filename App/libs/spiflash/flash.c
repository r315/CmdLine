/**
 *
 * \file
 *
 * Flash Functions.
 *
 * AT25Q641 External Flashloader for STM32 with QSPI.
 *
 * Author: Jesper Hansen, 2019
 *
 */

#include "flash.h"

#define QUADSPI_CR_FTHRES_1         (0 << 8)
#define QUADSPI_CR_FTHRES_2         (1 << 8)
#define QUADSPI_CR_FTHRES_3         (2 << 8)
#define QUADSPI_CR_FTHRES_4         (3 << 8)
#define QUADSPI_CCR_FMODE_IND_WRITE (0 << 26)   /* Write in indirect mode */
#define QUADSPI_CCR_FMODE_IND_READ  (1 << 26)   /* Read in indirect mode */
#define QUADSPI_CCR_FMODE_AUTO_POLL (2 << 26)   /* Automatic pollin mode */
#define QUADSPI_CCR_FMODE_MAPPED    (3 << 26)   /* Memory mapped mode */
#define QSPI_DUMMY_CYCLES(n)        (n << 18)

/**
 * @brief
 * @param khz
 */
static void set_fclk(uint32_t khz)
{
    uint32_t prescaller;
    uint32_t clk = HAL_RCC_GetHCLKFreq() / 1000UL;

    prescaller = (clk / khz) - 1;

    if(prescaller > 255)
    {
        prescaller = 255;
    }

    /* Configure QSPI Clock Prescaler and Sample Shift */
    QUADSPI->CR &= ~(QUADSPI_CR_PRESCALER | QUADSPI_CR_SSHIFT);
    QUADSPI->CR |= ((prescaller << 24) | QSPI_SAMPLE_SHIFTING_HALFCYCLE);
}

/**
 * @brief Wait for flag of QUADSPI peripheral
 * @param flag      QUADSPI SR flag to wait on
 * @param state     Wanted flag state
 * @param ms        Timeout in milliseconds
 * @return          QSPI_OK or QSPI_ERROR on timeout
 */
static int wait_flag(uint32_t flag, int state, uint32_t ms)
{
	uint32_t start_tick = HAL_GetTick();

	// Wait until flag is in expected state
	while (!!(QUADSPI->SR & flag) != state)
	{
        if(HAL_GetTick() - start_tick > ms)
        {
            return QSPI_TIMEOUT;
        }
	}

	return QSPI_OK;
}


static int send_single_command(uint8_t cmd)
{
	// wait for not busy
	if (wait_flag(QSPI_FLAG_BUSY, RESET, 1000) != QSPI_OK)
	{
		return QSPI_BUSY;
	}

    QUADSPI->CCR = QSPI_INSTRUCTION_1_LINE | cmd;

	// When there is no data phase, the transfer start as soon as the configuration is done
	// so wait until TC flag is set to go back in idle state
	if (wait_flag(QSPI_FLAG_TC, SET, 1000) != QSPI_OK)
	{
        return QSPI_ERROR;
	}

    // clear flag
    QUADSPI->FCR = QSPI_FLAG_TC;

    return QSPI_OK;
}

/**
 * @brief Read flash status register
 *
 * @param sr_cm  read status register command
 * @param buffer status register output value
 *
 * @return QSPI_OK, QSPI_ERROR on timeout
 */
static int read_status_register(uint8_t sr_cmd, uint8_t *buffer)
{
    uint32_t addr_reg = QUADSPI->AR;

    QUADSPI->DLR = 0;
    QUADSPI->CCR = QSPI_INSTRUCTION_1_LINE | QSPI_DATA_1_LINE | QUADSPI_CCR_FMODE_IND_READ | sr_cmd;

	// Start the transfer by re-writing the address in AR register
	QUADSPI->AR = addr_reg;

    if (wait_flag(QSPI_FLAG_TC, SET, 1000) != QSPI_OK)
    {
        return QSPI_TIMEOUT;
    }

    *buffer = *(__IO uint8_t *)&QUADSPI->DR;

    QUADSPI->FCR = QSPI_FLAG_TC;	// clear flag

	return QSPI_OK;
}

/**
 * @brief Write to status register
 * This flash only support single status register write,
 * to write to status 2 two bytes must be sent on
 * write status register command
 *
 * @param sr
 * @return
 */
static int write_status_register(uint16_t sr)
{
    int res;
#ifdef FLASH_DEV_VOLATILE_SR
    if((res = send_single_command(FLASH_DEV_VOLATILE_SR)) != QSPI_OK)
#else
    if((res = send_single_command(WRITE_ENABLE_CMD)) != QSPI_OK)
#endif
    {
        return res;
    }

    QUADSPI->DLR = (2 - 1);
    QUADSPI->CCR = QSPI_INSTRUCTION_1_LINE | QSPI_DATA_1_LINE | QUADSPI_CCR_FMODE_IND_WRITE | WRITE_STATUS_REG_CMD;

	// Start the transfer
	QUADSPI->DR = sr;

    if (wait_flag(QSPI_FLAG_TC, SET, 1000) != QSPI_OK){
        return QSPI_ERROR;
    }

    QUADSPI->FCR = QSPI_FLAG_TC;	// clear flag

#ifdef FLASH_DEV_VOLATILE_SR
    return QSPI_OK;
#else
	return send_single_command(WRITE_DISABLE_CMD);
#endif
}

/**
  * @brief  This function send a Write Enable and wait it is effective.
  *
  * @retval QSPI_OK or QSPI_ERROR on timeout
  */
 static int write_enable(void)
 {
    return send_single_command(WRITE_ENABLE_CMD);
 }

 /**
  * @brief Clears WEL bin in status register
  * @param
  * @return QSPI Status
  */
 static int write_disable(void)
 {
    return send_single_command(WRITE_DISABLE_CMD);
 }

/**
 * @brief Wait for flash status register bit
 *
 * @param ms Timeout in milliseconds
 *
 * @return QSPI_OK or QSPI_ERROR on timeout
 */
static int wait_sr_bit(uint8_t sr_cmd, uint8_t bit, uint8_t expected, uint32_t ms)
{
	uint32_t start_tick = HAL_GetTick();
	uint8_t sr;
    int res;

	while (1)
	{
		if ((res = read_status_register(sr_cmd, &sr)) != QSPI_OK)
        {
			return res;
        }

		if (!!(sr & bit) == expected)
        {
			return QSPI_OK;
        }

        if(HAL_GetTick() - start_tick > ms)
        {
			return QSPI_TIMEOUT;
        }
	}
}

/**
  * @brief  This function reset the QSPI memory.
  *
  * @retval QSPI_OK, QSPI_BUSY, QSPI_ERROR
  */
static uint8_t reset_memory(void)
{
    int res;

	if ((res = send_single_command(RESET_ENABLE_CMD)) != QSPI_OK)
	{
		return res;
	}

	if ((res = send_single_command(RESET_MEMORY_CMD)) != QSPI_OK)
	{
		return res;
	}

	return wait_sr_bit(READ_STATUS_REG_CMD, FLASH_DEV_SR_BUSY, RESET, 1000);
}


/********************************************************************************************/
/********************************************************************************************/
/********************************************************************************************/

static void flash_deinit(void)
{
  /* De-Configure QSPI pins */
	HAL_GPIO_DeInit(QSPI_CLK_GPIO_PORT, QSPI_CLK_PIN);

	HAL_GPIO_DeInit(QSPI_BK1_CS_GPIO_PORT, QSPI_BK1_CS_PIN);
	HAL_GPIO_DeInit(QSPI_BK1_D0_GPIO_PORT, QSPI_BK1_D0_PIN);
	HAL_GPIO_DeInit(QSPI_BK1_D1_GPIO_PORT, QSPI_BK1_D1_PIN);
	HAL_GPIO_DeInit(QSPI_BK1_D2_GPIO_PORT, QSPI_BK1_D2_PIN);
	HAL_GPIO_DeInit(QSPI_BK1_D3_GPIO_PORT, QSPI_BK1_D3_PIN);
#if ENABLE_BK2
	HAL_GPIO_DeInit(QSPI_BK2_CS_GPIO_PORT, QSPI_BK2_CS_PIN);
	HAL_GPIO_DeInit(QSPI_BK2_D0_GPIO_PORT, QSPI_BK2_D0_PIN);
	HAL_GPIO_DeInit(QSPI_BK2_D1_GPIO_PORT, QSPI_BK2_D1_PIN);
	HAL_GPIO_DeInit(QSPI_BK2_D2_GPIO_PORT, QSPI_BK2_D2_PIN);
	HAL_GPIO_DeInit(QSPI_BK2_D3_GPIO_PORT, QSPI_BK2_D3_PIN);
#endif
	/* Reset the QuadSPI memory interface */
	QSPI_FORCE_RESET();
	QSPI_RELEASE_RESET();

	/* Disable the QuadSPI memory interface clock */
	QSPI_CLK_DISABLE();
}



int flash_init(void)
{
	GPIO_InitTypeDef gpio_init_structure;

	flash_deinit();

	/* Enable the QuadSPI memory interface clock */
	QSPI_CLK_ENABLE();

	/* Enable GPIO clocks */
	QSPI_CS_GPIO_CLK_ENABLE();
	QSPI_CLK_GPIO_CLK_ENABLE();
	QSPI_D0_GPIO_CLK_ENABLE();
	QSPI_D1_GPIO_CLK_ENABLE();
	QSPI_D2_GPIO_CLK_ENABLE();
	QSPI_D3_GPIO_CLK_ENABLE();

	/* QSPI CLK GPIO pin configuration  */
	gpio_init_structure.Pin = QSPI_CLK_PIN;
	gpio_init_structure.Mode = GPIO_MODE_AF_PP;
	gpio_init_structure.Pull = GPIO_NOPULL;
	gpio_init_structure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	gpio_init_structure.Alternate = QSPI_CLK_GPIO_AF;
	HAL_GPIO_Init(QSPI_CLK_GPIO_PORT, &gpio_init_structure);

	// Bank 1
#if ENABLE_BK1
	/* QSPI CS GPIO pin configuration  */
	gpio_init_structure.Pin = QSPI_BK1_CS_PIN;
	gpio_init_structure.Alternate = QSPI_BK1_CS_GPIO_AF;
	HAL_GPIO_Init(QSPI_BK1_CS_GPIO_PORT, &gpio_init_structure);

	/* QSPI D0 GPIO pin configuration  */
	gpio_init_structure.Pin = QSPI_BK1_D0_PIN;
	gpio_init_structure.Alternate = QSPI_BK1_D0_GPIO_AF;
	HAL_GPIO_Init(QSPI_BK1_D0_GPIO_PORT, &gpio_init_structure);

	/* QSPI D1 GPIO pin configuration  */
	gpio_init_structure.Pin = QSPI_BK1_D1_PIN;
	gpio_init_structure.Alternate = QSPI_BK1_D1_GPIO_AF;
	HAL_GPIO_Init(QSPI_BK1_D1_GPIO_PORT, &gpio_init_structure);

	/* QSPI D2 GPIO pin configuration  */
	gpio_init_structure.Pin = QSPI_BK1_D2_PIN;
	gpio_init_structure.Alternate = QSPI_BK1_D2_GPIO_AF;
	HAL_GPIO_Init(QSPI_BK1_D2_GPIO_PORT, &gpio_init_structure);

	/* QSPI D3 GPIO pin configuration  */
	gpio_init_structure.Pin = QSPI_BK1_D3_PIN;
	gpio_init_structure.Alternate = QSPI_BK1_D3_GPIO_AF;
	HAL_GPIO_Init(QSPI_BK1_D3_GPIO_PORT, &gpio_init_structure);
#endif

	// Bank 2

#if ENABLE_BK2
	/* QSPI CS GPIO pin configuration  */
	gpio_init_structure.Pin = QSPI_BK2_CS_PIN;
	gpio_init_structure.Alternate = QSPI_BK2_CS_GPIO_AF;
	HAL_GPIO_Init(QSPI_BK2_CS_GPIO_PORT, &gpio_init_structure);

	/* QSPI D0 GPIO pin configuration  */
	gpio_init_structure.Pin = QSPI_BK2_D0_PIN;
	gpio_init_structure.Alternate = QSPI_BK2_D0_GPIO_AF;
	HAL_GPIO_Init(QSPI_BK2_D0_GPIO_PORT, &gpio_init_structure);

	/* QSPI D1 GPIO pin configuration  */
	gpio_init_structure.Pin = QSPI_BK2_D1_PIN;
	gpio_init_structure.Alternate = QSPI_BK2_D1_GPIO_AF;
	HAL_GPIO_Init(QSPI_BK2_D1_GPIO_PORT, &gpio_init_structure);

	/* QSPI D2 GPIO pin configuration  */
	gpio_init_structure.Pin = QSPI_BK2_D2_PIN;
	gpio_init_structure.Alternate = QSPI_BK2_D2_GPIO_AF;
	HAL_GPIO_Init(QSPI_BK2_D2_GPIO_PORT, &gpio_init_structure);

	/* QSPI D3 GPIO pin configuration  */
	gpio_init_structure.Pin = QSPI_BK2_D3_PIN;
	gpio_init_structure.Alternate = QSPI_BK2_D3_GPIO_AF;
	HAL_GPIO_Init(QSPI_BK2_D3_GPIO_PORT, &gpio_init_structure);
#endif

	/**
     * Configure QSPI FIFO Threshold to
     * set FTF flag when 4 or more bytes are available
     */
	QUADSPI->CR &= QUADSPI_CR_FTHRES;
	QUADSPI->CR |= QUADSPI_CR_FTHRES_4;

	set_fclk(1000UL);

    /* Disable dual flash */
     QUADSPI->CR &= ~(QUADSPI_CR_FSEL | QUADSPI_CR_DFM);

	/* Configure QSPI Flash Size, CS High Time and Clock Mode */
	QUADSPI->DCR &=
			~(QUADSPI_DCR_FSIZE | QUADSPI_DCR_CSHT | QUADSPI_DCR_CKMODE);
	QUADSPI->DCR |= (((POSITION_VAL(FLASH_DEV_FLASH_SIZE) - 1) << 16) |
	                QSPI_CS_HIGH_TIME_6_CYCLE |
	                QSPI_CLOCK_MODE_0);

	/* Enable the QSPI peripheral */
	QUADSPI->CR |= QUADSPI_CR_EN;

	reset_memory();

	return QSPI_OK;
}


int flash_read( uint32_t ReadAddr, uint8_t *pData, uint32_t Size)
{
    int res;
    __IO uint8_t *fifo = (__IO uint8_t *)&QUADSPI->DR;

	if (wait_flag(QSPI_FLAG_BUSY, RESET, 1000) != QSPI_OK)
    {
		return QSPI_BUSY;
    }

    /* Configure QSPI: DLR register with the number of data to read or write */
    QUADSPI->DLR = (Size - 1);

    /* Configure QSPI: ABR register with alternate bytes value */
    QUADSPI->ABR = 0;

    /*---- Command with instruction, address and alternate bytes ----*/
    /* Configure QSPI: CCR register with all communications parameters */
    #if 0
    // Not sure why but in quad mode read fails.
    QUADSPI->CCR =  QSPI_DATA_4_LINES |
                    QSPI_ADDRESS_4_LINES | QSPI_ADDRESS_24_BITS |
    				QSPI_ALTERNATE_BYTES_4_LINES | QSPI_ALTERNATE_BYTES_8_BITS | QSPI_DUMMY_CYCLES(4) |
					QSPI_INSTRUCTION_1_LINE | QUADSPI_CCR_FMODE_IND_READ | QUAD_INOUT_FAST_READ_CMD;
    #else
    QUADSPI->CCR =  QSPI_DATA_1_LINE |
                    QSPI_ADDRESS_1_LINE | QSPI_ADDRESS_24_BITS |
    				QSPI_ALTERNATE_BYTES_1_LINE | QSPI_ALTERNATE_BYTES_8_BITS | QSPI_DUMMY_CYCLES(0) |
					QSPI_INSTRUCTION_1_LINE | QUADSPI_CCR_FMODE_IND_READ | FAST_READ_CMD;
    #endif
    // Start the transfer by the address into AR register
    QUADSPI->AR = ReadAddr;

    while (Size)
    {
        if ((res = wait_flag((QSPI_FLAG_TC | QSPI_FLAG_FT), SET, 1000)) != QSPI_OK)
        {
            return res;
        }

        *pData++ = *fifo;
        Size--;
    }

    if ((res = wait_flag((QSPI_FLAG_TC), SET, 1000)) != QSPI_OK)
    {
        QUADSPI->FCR = QSPI_FLAG_TC;	// clear flag
    }

	return res;
}

/**
 * @brief
 * @param WriteAddr
 * @param pData
 * @param Size
 * @return
 */
int flash_write( uint32_t WriteAddr, uint8_t *pData, uint32_t Size)
{
	uint32_t end_addr, current_size, current_addr;
    int res;

    if (wait_flag(QSPI_FLAG_BUSY, RESET, 1000) != QSPI_OK)
    {
		return QSPI_BUSY;
    }

    __IO uint32_t *data_reg = &QUADSPI->DR;

	/* Calculation of the size between the write address and the end of the page */
	current_size = FLASH_DEV_PAGE_SIZE - (WriteAddr % FLASH_DEV_PAGE_SIZE);

	/* Check if the size of the data is less than the remaining place in the page */
	if (current_size > Size)
	{
		current_size = Size;
	}

	/* Initialize the adress variables */
	current_addr = WriteAddr;
	end_addr = WriteAddr + Size;


	/* Perform the write page by page */
	do
	{
        /* Enable write operations */
        if ((res = write_enable()) != QSPI_OK)
        {
            return res;
        }

	    /* Configure QSPI: DLR register with the number of data to read or write */
	    QUADSPI->DLR =  (current_size - 1);

	    /* Configure QSPI: ABR register with alternate bytes value */
	    QUADSPI->ABR = 0;

	    /*---- Command with instruction, address and alternate bytes ----*/
	    /* Configure QSPI: CCR register with all communications parameters */
	    QUADSPI->CCR = (QSPI_DATA_1_LINE |
                        QSPI_ADDRESS_1_LINE | QSPI_ADDRESS_24_BITS |
                        QSPI_ALTERNATE_BYTES_NONE | QSPI_DUMMY_CYCLES(0) |
						QSPI_INSTRUCTION_1_LINE | PAGE_PROG_CMD);

	    /* Configure QSPI: AR register with address value */
	    QUADSPI->AR = current_addr;

		current_addr += current_size;

		while (current_size > 0)
		{
            while ((QUADSPI->SR & QSPI_FLAG_FT) == 0);  // wait

            *(__IO uint8_t *)data_reg = *pData++;       // send byte
            current_size--;                             // decrement size
		}

		if ((res = wait_flag(QSPI_FLAG_TC, SET, 1000)) != QSPI_OK)
        {
            break;
        }

        QUADSPI->FCR = QSPI_FLAG_TC;	// clear flag

        res = wait_sr_bit(READ_STATUS_REG_CMD, FLASH_DEV_SR_BUSY, RESET, 1000);

		if (res != QSPI_OK)
		{
			break;
		}

		current_size = ((current_addr + FLASH_DEV_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : FLASH_DEV_PAGE_SIZE;
	} while (current_addr < end_addr);


	return res;
}


/**
 * @brief Erases flash blocks
 *
 * @param address
 * @param blocktype
 * @return
 */
int flash_erase(uint32_t address, blocksize_e blocktype)
{
	uint8_t cmd;
	uint32_t erase_time;
    int res;

    if (wait_flag(QSPI_FLAG_BUSY, RESET, 1000) != QSPI_OK)
    {
		return QSPI_BUSY;
    }

	switch (blocktype)
	{
	default:
        return QSPI_NOT_SUPPORTED;

	case BLOCKSIZE_4K:
		cmd	= SECTOR_ERASE_CMD;
		erase_time	= FLASH_DEV_SECTOR_ERASE_MAX_TIME;
		break;

	case BLOCKSIZE_32K:
		cmd   		= BLOCK_ERASE_32_CMD;
		erase_time	= FLASH_DEV_BLOCK_ERASE_MAX_TIME;
		break;

	case BLOCKSIZE_64K:
		cmd   		= BLOCK_ERASE_64_CMD;
		erase_time	= FLASH_DEV_BLOCK_ERASE_MAX_TIME;
		break;

	case BLOCKSIZE_ALL:
		return flash_chiperase();
	}

    // Enable write operations
	if ((res = write_enable()) != QSPI_OK)
	{
		return res;
	}

    /*---- Command with instruction and address ----*/
    /* Configure QSPI: CCR register with all communications parameters */
    QUADSPI->CCR = (QSPI_DATA_NONE |
                    QSPI_ADDRESS_1_LINE | QSPI_ADDRESS_24_BITS |
                    QSPI_ALTERNATE_BYTES_NONE | QSPI_DUMMY_CYCLES(0) |
                    QSPI_INSTRUCTION_1_LINE | QUADSPI_CCR_FMODE_IND_WRITE | cmd);

    /* Configure QSPI: AR register with address value */
    QUADSPI->AR = address;

    return wait_sr_bit(READ_STATUS_REG_CMD, FLASH_DEV_SR_BUSY, RESET, erase_time);
}

/**
 * @brief
 * @param
 * @return
 */
int flash_chiperase(void)
{
    int res;

    if (wait_flag(QSPI_FLAG_BUSY, RESET, 1000) != QSPI_OK)
    {
		return QSPI_BUSY;
    }

	if ((res = write_enable()) != QSPI_OK)
    {
		return res;
    }

	if ((res = send_single_command(BULK_ERASE_CMD)) != QSPI_OK)
    {
		return res;
    }

    return wait_sr_bit(READ_STATUS_REG_CMD, FLASH_DEV_SR_BUSY, RESET, FLASH_DEV_BULK_ERASE_MAX_TIME);
}

/**
 * @brief Read JEDEC Manufacturer and device ID
 *
 * @param id    JEDEC id output
 * @return      QSPI_OK, QSPI_ERROR
 */
int flash_read_id(uint16_t *id)
{
    int res;

    if (wait_flag(QSPI_FLAG_BUSY, RESET, 1000) != QSPI_OK)
    {
		return QSPI_BUSY;
    }

    /* REMS response is 2 bytes */
    QUADSPI->DLR = (2 - 1);

    /*---- Command with instruction, address and alternate bytes ----*/
    QUADSPI->CCR = QUADSPI_CCR_FMODE_IND_READ | QSPI_DATA_1_LINE |
                   QSPI_ADDRESS_1_LINE | QSPI_ADDRESS_24_BITS |
                   QSPI_INSTRUCTION_1_LINE | READ_MANUF_ID;

    /* Start the transfer, Manufacturer data is read from address 0 */
	QUADSPI->AR = 0;

    if ((res = wait_flag(QSPI_FLAG_TC, SET, 1000)) != QSPI_OK)
	{
        return res;
	}

    QUADSPI->FCR = QSPI_FLAG_TC;	// clear flag

    *id = *(__IO uint16_t *) &QUADSPI->DR;

	return QSPI_OK;
}

/**
 * @brief Enable disable quad mode
 * @param en
 *
 * @return  QSPI_OK, QSPI_ERROR
 */
int flash_quad_en(uint8_t en)
{
    uint16_t sr;
    int res;

    if (wait_flag(QSPI_FLAG_BUSY, RESET, 1000) != QSPI_OK)
    {
		return QSPI_BUSY;
    }
    // Read SR 2
    if((res = read_status_register(READ_STATUS_REG2_CMD, (uint8_t*)&sr)) != QSPI_OK)
    {
        return res;
    }
    // Check if already in requested state
    if(!!(sr & FLASH_DEV_SR2_QE) == en)
    {
        return QSPI_OK;
    }
    // Set QE bit
    sr = en ? sr | FLASH_DEV_SR2_QE : sr & ~(FLASH_DEV_SR2_QE);
    // shift SR 2 to high byte
    sr <<= 8;
    // Append SR 1
    if((res = read_status_register(READ_STATUS_REG_CMD, (uint8_t*)&sr)) != QSPI_OK)
    {
        return res;
    }
    // Write SR1 and SR2
    if((res = write_status_register(sr)) != QSPI_OK)
    {
        return res;
    }

    return wait_sr_bit(READ_STATUS_REG_CMD, FLASH_DEV_SR_BUSY, RESET, 1000);
}

/**
 * @brief Configures memory mapped read
 * @param en
 * @return
 */
int flash_mapped_mode(uint8_t en)
{
    if (wait_flag(QSPI_FLAG_BUSY, RESET, 1000) != QSPI_OK)
    {
		return QSPI_BUSY;
    }

    QUADSPI->CR  &= ~( QUADSPI_CR_EN );
    if(en){
        QUADSPI->CCR = QUADSPI_CCR_FMODE_MAPPED |
                    QSPI_DATA_4_LINES |
                    QSPI_ADDRESS_4_LINES | QSPI_ADDRESS_24_BITS |
                    QSPI_ALTERNATE_BYTES_4_LINES | QSPI_ALTERNATE_BYTES_8_BITS | QSPI_DUMMY_CYCLES(4) |
                    QSPI_INSTRUCTION_1_LINE | QUAD_IO_FAST_READ_CMD;
    }else{
        QUADSPI->CCR = QUADSPI_CCR_FMODE_IND_READ |
                    QSPI_DATA_1_LINE |
                    QSPI_ADDRESS_1_LINE | QSPI_ADDRESS_24_BITS |
                    QSPI_ALTERNATE_BYTES_1_LINE | QSPI_ALTERNATE_BYTES_8_BITS | QSPI_DUMMY_CYCLES(0) |
                    QSPI_INSTRUCTION_1_LINE | FAST_READ_CMD;
    }
    QUADSPI->CR  |=  ( QUADSPI_CR_EN );

    return QSPI_OK;
}