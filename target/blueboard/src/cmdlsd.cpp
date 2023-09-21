#include "board.h"
#include "cmdsd.h"
#include "memcard.h"

#define SECTOR_SIZE 512

FATFS sdcard;
uint8_t sector_data[SECTOR_SIZE];
/*  
  b15-b8     b7 b3   b2-b1    b0
|   size  | -      | OP    | start | */
#define SD_OP_DUMP_SECTOR       1
#define SD_OP_INIT_DISK         2
#define SD_OP_ERASE_SECTOR      3
#define SD_OP_START             (1 << 0)
#define SD_OP_SET_FLAG(_o, _f)  _o |= _f
#define SD_OP_SET_OPER(o, n)    o = ((o & ~(3 << SD_OP_START)) | (n << SD_OP_START))
#define SD_OPER(x)              ((x>>SD_OP_START) & 3)

static uint32_t getBits(uint8_t *arr, uint32_t *pcur, uint32_t nbits){
    uint32_t res = 0;
    uint32_t cur = *pcur;
    uint8_t *pbyte = arr + (cur / 8);
    uint8_t mask = (0x80 >> (cur % 8));

    for(uint32_t bit = 0; bit < nbits; bit++, cur++, mask >>= 1){
        res <<= 1;

        if(mask == 0){
            pbyte = arr + (cur / 8);
            mask = (1 << 7);
        }

        if(*pbyte & mask){
            res |= 1;
        }
    }

    *pcur = cur;

    return res;
}

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
        case FR_NOT_OPENED:  
			console->print("file not opened\n");break;
        case FR_NOT_ENABLED:  
			console->print("file system not enabled\n");break;
        case FR_NO_FILESYSTEM:  
			console->print("no file system\n");break;
	}	
}

void CmdSd::d_error(DRESULT res)
{
	switch(res)
	{
        default:
            console->print("disk result error: %x\n", res);

		case RES_OK:
            console->print("disk result ok\n");
            break;
			
		case RES_ERROR: 
			console->print("disk result error\n");break;
		case RES_NOTRDY:
			console->print("disk result not ready\n");break;		
		case RES_PARERR:  
			console->print("disk result invalid parameter\n");break;		
	}	
}

void CmdSd::d_status(DSTATUS sta)
{
	switch(sta)
	{
        default: console->print("disk status error: %x\n", sta); break;
		case STA_OK: console->print("disk status ok\n"); break;			
		case STA_NOINIT: console->print("disk status not initialised\n");break;
		case STA_NODISK: console->print("disk status not present\n");break;		
	}	
}

FRESULT CmdSd::listDir (const char* path, bool recursive)
{
    FRESULT res;
    FILINFO fno;
    DIR dir;
    uint32_t i;
    char localpath[32];

    xstrcpy(localpath, path, sizeof(localpath));

    res = pf_opendir(&dir, localpath);
    if (res == FR_OK) {
        for (;;) {
            res = pf_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;   /* Break on error or end of dir */
            if (fno.fattrib & AM_DIR && recursive) {                     /* It is a directory */
                i = strlen(localpath);
                xsprintf(&localpath[i], "/%s", fno.fname);
                res = listDir(localpath, true);                   /* Enter the directory */
                if (res != FR_OK) break;
                localpath[i] = 0;
            } else {                                        /* It is a file. */
                console->print("%s/%s\n", localpath, fno.fname);
            }
        }
    }

    return res;
}
//--------------------------------------
//
//--------------------------------------
void CmdSd::dumpSector(uint32_t sector){
    disk_readp(sector_data, sector, 0, SECTOR_SIZE);

    console->print("\n Sector : %d", sector);

    for(uint32_t i = 0; i < SECTOR_SIZE; i++){
        if((i & 0x0F) == 0){
            if(i > 0){
                for(uint32_t j = i - 16; j < i; j++){
                    char c = sector_data[j];
                    if(c < ' ' || c > '~'){
                        c = ' ';
                    }
                    console->putChar(c);
                }
            }           
            console->print("\n%08X: ", i);
        }
        console->print("%02X ", sector_data[i]);
    }
    console->putChar('\n');
}

void CmdSd::help(void){
    console->putString("\nUsage: sd <option>");
    console->putString("Options:"); 
    console->putString(" dump <lba>, dump sector"); 
    console->putString(" list      , List files");
    console->putString(" init      , Initialise SD");
    console->putString(" info      , SD Info");

}

char CmdSd::execute(int argc, char **argv){
    uint32_t sector;
    uint8_t data[20];

	// check parameters
    if( argc < 2){
        help();
        return CMD_OK;
    }

    if(xstrcmp("init", (const char*)argv[1]) == 0){
        FRESULT res = pf_mount(&sdcard);
        if(res == FR_OK){
            console->print("memory card init ok\n");
        }else{
            f_error(res);
        }
        return CMD_OK;
    }else if(xstrcmp("dump", (const char*)argv[1]) == 0){
        if(ha2i(argv[2], &sector)){
            dumpSector(sector);
            return CMD_OK;
        }
    }else if(xstrcmp("erase", (const char*)argv[1]) == 0){
        if(ha2i(argv[2], &sector)){
            for(int i = 0; i < SECTOR_SIZE; i++){
                sector_data[i] = 0xFF;
            }
            d_error(disk_writep(NULL, sector));
            d_error(disk_writep(sector_data, SECTOR_SIZE));
            d_error(disk_writep(NULL, 0));
            return CMD_OK;
        }
    }else if(xstrcmp("list", (const char*)argv[1]) == 0){
        f_error(listDir(argv[2], false));
        return CMD_OK;
    }else if(xstrcmp("info", argv[1]) == 0){
        SDGetCID(data);
        console->putString("--------------------------------------------");
        console->putString("              Card ID (CID)");
        console->putString("--------------------------------------------");
        console->print("Manufacturer ID: %02x\n", data[0]);
        console->print("Application ID: %c%c, (%04x)\n", data[1], data[2], data[1] << 8 | data[2]);
        console->print("Product Name: ");
        for(uint8_t i = 0; i < 5; i++) console->print("%c", (char)data[3 + i]);
        console->print("\nProduct Revision: %02x\n", data[8]);
        console->print("Serial Number: ");
        for(uint8_t i = 0; i < 4; i++) console->print("%02x", data[9 + i]);
        uint16_t mdt = data[13] << 8 | data[14];
        console->print("\nManufacture date: %d/%d\n", 2000 + (mdt >> 4), mdt & 15);

        SDGetCSD(data);
        console->putString("--------------------------------------------");
        console->putString("          Card Specific Data (CSD)");
        console->putString("--------------------------------------------");

        sector = 0;
        
        for(uint8_t i = 0; i < 16; i++){
            console->print("%02x ", data[i]);
        }
        
        console->print("\nCSD Structure: %u\n", getBits(data, &sector, 2));
        getBits(data, &sector, 6); // Reserved
        console->print("TAAC: %u\n", getBits(data, &sector, 8));
        console->print("NSAC: %u\n", getBits(data, &sector, 8));
        console->print("TRANS_SPEED: %xh\n", getBits(data, &sector, 8));
        console->print("CCC: %xh\n",getBits(data, &sector, 12));
        uint32_t block_len = getBits(data, &sector, 4);
        console->print("READ_BLOCK_LEN: %u (%ubyte)\n", block_len, (1UL << block_len));
        console->print("READ_BL_PARTIAL: %xb\n", getBits(data, &sector, 1));
        console->print("WRITE_BLK_MISALIGN: %xb\n", getBits(data, &sector, 1));
        console->print("READ_BLK_MISALIGN: %xb\n", getBits(data, &sector, 1));
        console->print("DSR_IMP: %xb\n", getBits(data, &sector, 1));
        getBits(data, &sector, 2); // Reserved
        uint32_t c_size = getBits(data, &sector, 12);
        console->print("C_SIZE: %u\n", c_size);
        console->print("VDD_R_CURR_MIN: %u\n", getBits(data, &sector, 3));
        console->print("VDD_R_CURR_MAX: %u\n", getBits(data, &sector, 3));
        console->print("VDD_W_CURR_MIN: %u\n", getBits(data, &sector, 3));
        console->print("VDD_W_CURR_MAX: %u\n", getBits(data, &sector, 3));
        uint32_t mult = getBits(data, &sector, 3);
        console->print("C_SIZE_MULT: %u (%u)\n", mult, (1UL << (mult + 2)) );
        console->print("ERASE_BLK_EN: %xb\n", getBits(data, &sector, 1));
        console->print("SECTOR_SIZE: %u\n", getBits(data, &sector, 7));
        console->print("WP_GRP_SIZE: %u\n", getBits(data, &sector, 7));
        console->print("WP_GRP_ENABLE: %xb\n", getBits(data, &sector, 1));
        getBits(data, &sector, 2); // Reserved
        console->print("R2W_FACTOR: %x\n", getBits(data, &sector, 3));
        uint32_t w_bl_len = getBits(data, &sector, 4);
        console->print("WRITE_BL_LEN: %x (%d)\n", w_bl_len, (1UL << w_bl_len));
        console->print("WRITE_BL_PARTIAL: %xb\n", getBits(data, &sector, 1));
        getBits(data, &sector, 5); // Reserved
        console->print("FILE_FORMAT_GRP: %xb\n", getBits(data, &sector, 1));
        console->print("COPY: %xb\n", getBits(data, &sector, 1));
        console->print("PERM_WRITE_PROTECT: %xb\n", getBits(data, &sector, 1));
        console->print("TEM_WRITE_PROTECT: %xb\n", getBits(data, &sector, 1));
        console->print("FILE_FORMAR: %x\n", getBits(data, &sector, 2));
        console->putString("--------------------------------------------");
       
        console->print("Card size %d bytes\n", (c_size + 1) * (1UL << (mult + 2)) * (1UL << block_len));

        
        

/*
        SDGetSCR(data);
        console->putString("--------------------------------------------");
        console->putString("     Card Configuration Register (SCR)");
        console->putString("--------------------------------------------");
        */
        return CMD_OK;
    }

    return CMD_BAD_PARAM;
}