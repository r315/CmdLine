
#include "cmdsd.h"

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
    console->putString("\nUsage: ");
    console->putString(" d <sector>, dump sector"); 
    console->putString(" x <sector>, Erase sector");
    console->putString(" i         , Initialise SD");

}

char CmdSd::execute(int argc, char **argv){
    uint32_t sector;

	// check parameters
    if( argc < 2){
        help();
        return CMD_OK;
    }

    if(xstrcmp("init", (const char*)argv[1]) == 0){
        DSTATUS sta = disk_initialize();
        d_status(sta);
        if(sta == STA_OK){
            f_error(pf_mount(&sdcard));
        }
        return CMD_OK;
    }else if(xstrcmp("dump", (const char*)argv[1]) == 0){
        if(hatoi(argv[2], &sector)){
            dumpSector(sector);
            return CMD_OK;
        }
    }else if(xstrcmp("erase", (const char*)argv[1]) == 0){
        if(hatoi(argv[2], &sector)){
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
    }

    return CMD_BAD_PARAM;
}