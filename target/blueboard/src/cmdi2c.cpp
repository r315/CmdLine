#include "board.h"
#include "cmdi2c.h"
#include "debug.h"

enum {
    I2C_OP_INIT = 0,
    I2C_OP_WRITE,
    I2C_OP_READ,
    I2C_OP_SCAN
};

static i2cbus_t i2c = {
    NULL,
    255,
    255
};

void CmdI2c::help(void){
    console->print("Usage: i2c <read|write|init|scan> <bus> [option] \n\n");  
    console->print("\tinit <bus>, \n");
    console->print("\tread <bus> <device> <count>, read data\n");
    console->print("\twrite <bus> <device> <data0 .. datan>, write data\n");
    console->print("\tscan <bus>, Scan devices on bus\n");
}

char CmdI2c::execute(int argc, char **argv){
    uint8_t op;
    int32_t val;
    uint8_t i2c_buf[256];

    if(argc < 2){
        help();
        return CMD_OK;
    }

    if( !xstrcmp("init", argv[1])){
	    op = I2C_OP_INIT;
    }else if( !xstrcmp("read", argv[1])){
        op = I2C_OP_READ;
    }else if( !xstrcmp("write", argv[1])){
        op = I2C_OP_WRITE;
    }else if( !xstrcmp("scan", argv[1])){
        op = I2C_OP_SCAN;            
    }else{
		return CMD_NOT_FOUND;
    }

    if(yatoi(argv[2], &val) == 0){
        return CMD_BAD_PARAM;
    }

    i2c.bus_num = val;

	if(i2c.bus_num > I2C_MAX_ITF){
        console->print("Invalid bus %d\n", i2c.bus_num);
		return CMD_BAD_PARAM;
	}

    if(i2c.ctrl == NULL){
        I2C_Init(&i2c);
    }

    if(hatoi(argv[3], (uint32_t*)&val)){
        i2c.addr = val;    
    }   

    switch(op){
        case I2C_OP_INIT:
            I2C_Init(&i2c);
            break;

        case I2C_OP_WRITE:
            op = 0;
            while(hatoi(argv[op + 4], (uint32_t*)&val)){
                i2c_buf[op++] = (uint8_t)val;
            }   
                
            if(I2C_Write(&i2c, i2c_buf, op) == 0){
                console->putString("Failed to write");            
            }
            break;

        case I2C_OP_READ:
            if(yatoi(argv[4], &val)){ // count
                if(I2C_Read(&i2c, i2c_buf, val)){
                    for(int i = 0; i < val; i ++){
                        if( (i & 15) == 0) console->print("\n%02X: ", i & 0xF0);                
                        console->print("%02X ", i2c_buf[i]);
                    }        
                }
            console->putChar('\n');
            }else{
                console->putString("Invalid read count");
            }

            break;

        case I2C_OP_SCAN:
            console->print("\n   ");
        
            for(int i = 0; i < 16; i++){
                console->print("%02X ", i);
            }           

            for(int i = 0; i < 128; i++){
                if( (i & 15) == 0) 
                    console->print("\n%02X ", i & 0xF0);
                
                i2c.addr = (i << 1);
                
                if(I2C_Read(&i2c, &op, 1) == 0){
                    console->print("-- ");
                }else{
                    console->print("%02X ", i << 1);
                }
                
                DelayMs(1);
            }
            console->putChar('\n');
            break;

    } 

    return CMD_OK;
}
