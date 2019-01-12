#include "board.h"
#include "cmdi2c.h"


I2C_Controller *bus[I2C_MAX_ITF];

Vcom *_this;
uint8_t data[256];

extern "C" void cb(void *data){
     (dynamic_cast<Vcom*>(_this))->bufferHex((uint8_t*)data,16);
     (dynamic_cast<Vcom*>(_this))->putc(' ');
     (dynamic_cast<Vcom*>(_this))->bufferAscii((uint8_t*)data,16);
     (dynamic_cast<Vcom*>(_this))->putc('\n');
}

void CmdI2c::help(void){
    vcom->printf("Usage: i2c -b <bus> <-r|-w> [option] \n\n");  
    vcom->printf("\t -d, 8-Bit device address\n");
    vcom->printf("\t -b, bus 0-2\n");
    vcom->printf("\t -r, read access\n");
    vcom->printf("\t -w, write access\n");
    vcom->printf("\t -s, Scan devices on bus\n");
}

char CmdI2c::execute(void *ptr){
uint8_t slave, op, busnum;
char *p1;

	p1 = (char*)ptr;

    _this = this->vcom;

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
		}else if( !xstrcmp(p1,"-d")){
			p1 = nextParameter(p1);
            slave = nextHex(&p1);
		}else if( !xstrcmp(p1,"-r")){
			p1 = nextParameter(p1);
            op = I2C_READ;
		}else if( !xstrcmp(p1,"-w")){
			p1 = nextParameter(p1);
            data[0] = nextHex(&p1);
            op = I2C_WRITE;
        }else if( !xstrcmp(p1,"-s")){
			p1 = nextParameter(p1);
            op = I2C_SCAN;
        }else{
			p1 = nextParameter(p1);
		}
	}

    // Validate parameters
	if(busnum > I2C_MAX_ITF || op == 255){
		return CMD_BAD_PARAM;
	}    

    if(bus[busnum] == NULL){
        if(slave == 255 ){
            if(op == I2C_SCAN){
                slave = 0;
            }else{
                vcom->printf("Missing device address\n");
                return CMD_BAD_PARAM;
            }
        }
        bus[busnum] = I2C_Init(busnum, slave);
    }

    if(slave != 255){
        bus[busnum]->device = slave;
	}

    switch(op){
        case I2C_WRITE:
            I2C_Write(busnum, data, 1);
            break;

        case I2C_READ:
            I2C_ReadAsync(busnum, data, 16, cb);
            break;

        case I2C_SCAN:
            for(int i = 0; i < 16; i++){
                vcom->printf("%02X ", i);
            }           

            for(int i = 0; i < 128; i++){
                if( (i&15) == 0) vcom->putc('\n');
                bus[busnum]->device = (i << 1);
                if( (I2C_Read(busnum,&op,1) >> 8) == ERROR_SLA_NACK){
                    vcom->printf("-- ");
                }else{
                    vcom->printf("%02X ", i<<1);
                }
                DelayMs(1);
            }
            vcom->putc('\n');
            break;

    } 

    return CMD_OK;
}
