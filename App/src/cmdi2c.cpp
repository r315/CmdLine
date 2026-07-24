#include "board.h"
#include "cmdi2c.h"
#include "i2c.h"


void CmdI2c::printAsc(uint8_t *buf, int count)
{
	while(count--){
		if(*buf > 0x1f && *buf < 0x80) {
			console->printf("%c", *((char*)buf));
		} else {
			console->print(".");
		}
		buf++;
	}
}

void CmdI2c::help(void){
    console->print("Usage: i2c <read|write|init|scan> [option] \n\n");
    console->print("\tinit <bus>, \n");
    console->print("\tspeed <100-4000>, speed in kHz\n");
    console->print("\tread <device> <count> [ascii], read data and print in ascii\n");
    console->print("\twrite <device> <data+0 .. data+n>, write data\n");
    console->print("\tscan, Find devices on bus\n");
    console->print("\trr <addr> [count], read register at address\n");
    console->print("\twr <addr> <value>, write to register at address\n");
    console->print("\tslave <addr>, select slave address\n");
}

char CmdI2c::execute(int argc, char **argv){
    int32_t val, count;
    uint8_t i2c_buf[256];
    uint32_t device;

    if(argc < 2){
        help();
        return CMD_OK;
    }

    if(!xstrcmp("init", argv[1])){
        if(ia2i(argv[2], &val) == 0){
            return CMD_BAD_PARAM;
        }

        if(val > I2C_MAX_ITF){
            console->printf("Invalid bus %d\n", val);
		    return CMD_BAD_PARAM;
	    }

        board_i2cbus.addr = 1;
        board_i2cbus.speed = 100;
        board_i2cbus.bus_num = (uint8_t)val;
        board_i2cbus.cfg |= I2C_CFG_PINS;
        I2C_Init(&board_i2cbus);

        return CMD_OK;
    }

    if(board_i2cbus.handle == NULL){
        console->println("I2C not initialized");
		return CMD_BAD_PARAM;
    }

    if(!xstrcmp("speed", argv[1])){
        if(ia2i(argv[2], (int32_t*)&board_i2cbus.speed)){
            return CMD_OK;
        }
    }

    if(!xstrcmp("slave", argv[1])){
        if(ha2u(argv[2], (uint32_t*)&val)){
            board_i2cbus.addr = val;
            return CMD_OK;
        }
    }

    if( !xstrcmp("read", argv[1])){
        if(!ha2u(argv[2], &device)){ return CMD_BAD_PARAM; }
        if(!ia2i(argv[3], &count)){ return CMD_BAD_PARAM; }
        if(I2C_Read(&board_i2cbus, device, i2c_buf, count) == 0){
            console->print("Failed to read\n");
            return CMD_OK;
        }else{
            uint8_t asc = !xstrcmp("ascii", argv[4]);
            int i,k;
            for(i = 0, k = 0; i < count; i ++){
                if( (i & 15) == 0) {
                    if(asc) {
                        printAsc(&i2c_buf[k], i - k);
                        k = i;
                    }
                    if(i == (count - 1)){
                        console->printchar('\n');
                    }else{
                        console->printf("\n%02X: ", i & 0xF0);
                    }
                }
                console->printf("%02X ", i2c_buf[i]);
            }

            if(asc) {
                printAsc(&i2c_buf[k], i - k);
            }

            return CMD_OK_LF;
        }
    }

    if( !xstrcmp("write", argv[1])){
        if(!ha2u(argv[2], &device)){return CMD_BAD_PARAM;}
        count = 0;
        while(ha2u(argv[3 + count], (uint32_t*)&val)){
            i2c_buf[count++] = (uint8_t)val;
        }

        if(I2C_Write(&board_i2cbus, device, i2c_buf, count) == 0){
            console->println("Failed to write");
        }

        return CMD_OK;
    }

    if( !strcmp("rr", argv[1])){
        if(ha2u(argv[2], (uint32_t*)&val)){
            uint8_t reg = val;
            count = ia2i(argv[3], &val) ? val : 1;
            if(!I2C_Write(&board_i2cbus, board_i2cbus.addr, (uint8_t*)&reg, 1)){
                console->println("Fail i2c write");
                return CMD_OK;
            }

            if(!I2C_Read(&board_i2cbus, board_i2cbus.addr, i2c_buf, count)){
                console->println("Fail i2c read");
                return CMD_OK;
            }

            for(uint8_t i = 0; i < count; i ++){
                if( (i & 15) == 0) {
                    if(i == (count - 1)){
                        console->printchar('\n');
                    }else{
                        console->printf("\n%02X: ", i & 0xF0);
                    }
                }
                console->printf("%02X ", i2c_buf[i]);
            }

            return CMD_OK_LF;
        }
    }

    if(!xstrcmp("wr", argv[1])){
        uint32_t val;
        if(ha2u(argv[2], &val)){
            i2c_buf[0] = val;
            if(ha2u(argv[3], &val)){
                i2c_buf[1] = val;
                if(!I2C_Write(&board_i2cbus, board_i2cbus.addr, i2c_buf, 2)){
                    console->println("Fail i2c write");
                }
            }
        }
        return CMD_OK;
    }

    if( !xstrcmp("scan", argv[1])){
        console->print("\n   ");

        for(int i = 0; i < 16; i++){
            console->printf("%02X ", i);
        }

        for(int i = 0; i < 128; i++){
            if( (i & 15) == 0)
                console->printf("\n%02X ", i & 0xF0);

            if(I2C_Read(&board_i2cbus, i, (uint8_t*)&device, 1) == 0){
                console->print("-- ");
            }else{
                console->printf("%02X ", i);
            }

            DelayMs(1);
        }
        console->printchar('\n');

        return CMD_OK;
    }

    if(!xstrcmp("reset", argv[1])){
        I2C_Reset(&board_i2cbus);
        return CMD_OK;
    }

	return CMD_NOT_FOUND;
}
