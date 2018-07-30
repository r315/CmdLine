
#include "cmdi2c.h"



uint8_t data[256];

void CmdI2c::help(void){
    vcom->printf("Usage: i2c -b <bus> [option] \n\n");  
    vcom->printf("\t -s, 8-Bit hex slave address\n");
    vcom->printf("\t -b, bus\n");
    vcom->printf("\t -r, read\n");
    vcom->printf("\t -w, write\n");
}

char CmdI2c::execute(void *ptr){
uint8_t slave, op, busnum;
char *p1;

	p1 = (char*)ptr;

	 // check parameters
    if( p1 == NULL || *p1 == '\0'){
        help();
        return CMD_OK;
    }

    busnum = 255;
    op = 255;
    slave = 255;

	// parse options
	while(*p1 != '\0'){
		if( !xstrcmp(p1,"-b")){
			p1 = nextParameter(p1);
		    busnum = nextInt(&p1);
		}else if( !xstrcmp(p1,"-s")){
			p1 = nextParameter(p1);
            slave = nextHex(&p1);
		}else if( !xstrcmp(p1,"-r")){
			p1 = nextParameter(p1);
            op = I2C_READ;
		}else if( !xstrcmp(p1,"-w")){
			p1 = nextParameter(p1);
            op = I2C_WRITE;
        }else{
			p1 = nextParameter(p1);
		}
	}

	if(busnum > I2C_MAX_IF || op == 255){
		return CMD_BAD_PARAM;
	}

    if(slave != 255){
        bus[busnum].device = slave;
	}

    if(bus[busnum].status == 255){
        if(slave == 255){
            return CMD_BAD_PARAM;
        }
        I2C_Init(&bus[busnum], busnum, 10000, slave);
    }

    if(op == I2C_WRITE){
        I2C_Write(&bus[busnum], data, 16);
    }else{
        I2C_Read(&bus[busnum], data, 16);
        vcom->bufferHex(data,16);
    }

    return CMD_OK;
}