
#include <spi.h>
#include "cmdsd.h"

#define SECTOR_SIZE 512

Spi_Type mmc_spi = {0};
FATFS sdcard;
uint8_t sector_data[SECTOR_SIZE];
/*  
  b15-b8     b7 b3   b2-b1    b0
|   size  | -      | OP    | start | */
#define SD_OP_DUMP_SECTOR   1
#define SD_OP_INIT_DISK     2
#define SD_OP_ERASE_SECTOR  3
#define SD_OP_START  (1 << 0)
#define SD_OP_SET_FLAG(_o, _f) _o |= _f
#define SD_OP_SET_OPER(o, n) o = ((o & ~(3 << SD_OP_START)) | (n << SD_OP_START))
#define SD_OPER(x) ((x>>SD_OP_START) & 3)

void CmdSd::f_error(FRESULT res)
{
	switch(res)
	{
        default:
            console->print("error: %x\n", res);

		case FR_OK: break;
			
		case FR_DISK_ERR: 
			console->print("disk error\n");break;
		case FR_NOT_READY:
			console->print("disk not ready\n");break;		
		case FR_NO_FILE:  
			console->print("invalid file\n");break;		
	}	
}

void CmdSd::d_error(DRESULT res)
{
	switch(res)
	{
        default:
            console->print("error: %x\n", res);

		case RES_OK: break;
			
		case RES_ERROR: 
			console->print("disk error\n");break;
		case RES_NOTRDY:
			console->print("disk not ready\n");break;		
		case RES_PARERR:  
			console->print("invalid parameter\n");break;		
	}	
}


void mmcSpiInit(void){
    mmc_spi.bus = 0;
    mmc_spi.freq = 500000;
    mmc_spi.cfg  = SPI_MODE0 | SPI_8BIT;
    SPI_Init(&mmc_spi);
	//LPC_PINCON->PINSEL0 |= SSP1_SSEL;	
}

extern "C" uint8_t SPI(uint8_t outb) 
{
    return SPI_Send(&mmc_spi, outb);
}

//--------------------------------------
//
//--------------------------------------
void CmdSd::dumpSector(uint32_t sector){
    disk_readp(sector_data, sector, 0, SECTOR_SIZE);

    for(uint32_t i = 0; i < SECTOR_SIZE; i++){
        if((i & 0x0F) == 0){
            console->putc('\n');
            console->print("%08X: ", sector + i);
        }
        console->print("%02X ", sector_data[i] );
    }
    console->putc('\n');
}

void CmdSd::help(void){
    console->puts("\nUsage: ");
    console->puts(" d <sector>  \n"); 

}

char CmdSd::execute(void *ptr){
char c, *p1 = (char*)ptr;
uint32_t operation, sector;

	 // check parameters
    if( p1 == NULL || *p1 == '\0'){
        help();
        return CMD_OK;
    }

    while( !(operation & SD_OP_START) ){
        c = nextChar(&p1);
         switch(c){
            case 'd':                
                if(!nextHex(&p1, &sector)){
                  console->print("Invalid sector\n");
                    return CMD_BAD_PARAM;
                }
                SD_OP_SET_OPER(operation, SD_OP_DUMP_SECTOR);
                break;

            case 'i':
                SD_OP_SET_OPER(operation, SD_OP_INIT_DISK);
                break;

            case 'x':
                if(!nextHex(&p1, &sector)){
                  console->print("Invalid sector\n");
                    return CMD_BAD_PARAM;
                }
                SD_OP_SET_OPER(operation, SD_OP_ERASE_SECTOR);
                break;

            case '\n':
            case '\r':    
            case '\0':
                SD_OP_SET_FLAG(operation, SD_OP_START);
        }
    }

    if(mmc_spi.freq == 0){
		mmcSpiInit();
        f_error(pf_mount(&sdcard));	
	}    

	switch(SD_OPER(operation)){
        case SD_OP_DUMP_SECTOR:
             dumpSector(sector);
            break;

        case SD_OP_INIT_DISK:
            console->print("sdcard init: %x\n", disk_initialize());
            break;

        case SD_OP_ERASE_SECTOR:
            for(int i = 0; i < SECTOR_SIZE; i++){
                sector_data[i] = 0xFF;
            }
            d_error(disk_writep(NULL, sector));
            d_error(disk_writep(sector_data, SECTOR_SIZE));
            d_error(disk_writep(NULL, 0));
            break;

        default:
            console->print("Not implemented\n");
            break;
    }

    return CMD_OK;
}