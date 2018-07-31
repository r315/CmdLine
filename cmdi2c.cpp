
#include "cmdi2c.h"


I2C_Controller *bus[I2C_MAX_ITF];

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

	if(busnum > I2C_MAX_ITF || op == 255){
		return CMD_BAD_PARAM;
	}    

    if(bus[busnum] == NULL){
        if(slave == 255){
            return CMD_BAD_PARAM;
        }
        bus[busnum] = I2C_Init(busnum, slave);
    }

    if(slave != 255){
        bus[busnum]->device = slave;
	}

    if(op == I2C_WRITE){
        I2C_Write(busnum, (uint8_t*)"\x00\x00\x30serial number 123456", 20);
    }else{
        I2C_Write(busnum, (uint8_t*)"\x00\x00", 2);
        I2C_Read(busnum, data, 16);
        vcom->bufferHex(data,16);
    }

    //I2C_Read(I2C_IF0, data, 16);
    //vcom->bufferHex(data,16);

    return CMD_OK;
}