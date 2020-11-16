#include "spi_flash.h"
#include "board.h"


#define LOW     0
#define HIGH    1

uint32_t flashDevice_ID(void){
    uint32_t id;
    
    id = 0xFFFFFF00 | (RDID << 00);    

    SPI_SetCS(LOW);
    SPI_Read((uint8_t*)&id, 4);
    id = 0xFFFFFFFF;
    SPI_Read((uint8_t*)&id, 3);
    SPI_SetCS(HIGH);

    return id & 0x00FFFFFF;
}

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

    SPI_SetCS(LOW);
    SPI_Read((uint8_t*)&id, 4);
    SPI_SetCS(HIGH);

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

    SPI_SetCS(LOW);
    SPI_Read((uint8_t*)&id, 3);
    id = 0xFFFFFFFF;
    SPI_Read((uint8_t*)&id, 3);
    SPI_SetCS(HIGH);

    return id & 0x00FFFFFF;
}

uint8_t flashReadStatus(void){
    uint8_t status[2];
    
    status[0] = RDSR;
    status[1] = 0xFF;

    SPI_SetCS(LOW);
    SPI_Read(status, 2);
    SPI_SetCS(HIGH);

    return status[1];
}

static void flashCommand(uint8_t *cmd, uint8_t size){
    SPI_SetCS(LOW);
    SPI_Write(cmd, size);
    SPI_SetCS(HIGH);       
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

    SPI_SetCS(LOW);
    SPI_Write(hdr, sizeof(hdr));
    SPI_Read(dst, size);
    SPI_SetCS(HIGH);
    return size; 
}
