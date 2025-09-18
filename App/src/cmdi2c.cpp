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
    console->print("\tread <device> <count> [ascii], read data and print in ascii\n");
    console->print("\twrite <device> <data+0 .. data+n>, write data\n");
    console->print("\tscan, Find devices on bus\n");
    console->print("\trr <addr> [count], read register at address\n");
    console->print("\twr <addr> <value>, write to register at address\n");
    console->print("\tslave <addr>, select slave address\n");
}

char CmdI2c::execute(int argc, char **argv){
    int32_t val;
    uint8_t i2c_buf[256], count;

    if(argc < 2){
        help();
        return CMD_OK;
    }

    if( !xstrcmp("init", argv[1])){
        if(ia2i(argv[2], &val) == 0){
            return CMD_BAD_PARAM;
        }

        if(val > I2C_MAX_ITF){
            console->printf("Invalid bus %d\n", val);
		    return CMD_BAD_PARAM;
	    }

        m_i2c.bus_num = (i2cbusnum_t)val;
        m_i2c.speed = 100000;

        I2C_Init(&m_i2c);

        return CMD_OK;
    }

    if(m_i2c.handle == NULL){
        console->println("I2C not initialized");
		return CMD_BAD_PARAM;
    }

    if(!xstrcmp("slave", argv[1])){
        if(ha2u(argv[2], (uint32_t*)&val)){
            m_i2c.addr = val << 1;
            return CMD_OK;
        }
    }

    if( !xstrcmp("read", argv[1])){
        if(ia2i(argv[2], &val)){// count
            count = val;
            if(I2C_Read(&m_i2c, m_i2c.addr, i2c_buf, count) == 0){
                console->print("Failed to read");
            }else{
                uint8_t asc = !xstrcmp("ascii", argv[3]);
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
        }else{
            console->print("Invalid read count");
        }
    }

    if( !xstrcmp("write", argv[1])){
        count = 0;
        while(ha2u(argv[2 + count], (uint32_t*)&val)){
            i2c_buf[count++] = (uint8_t)val;
        }

        if(I2C_Write(&m_i2c, m_i2c.addr, i2c_buf, count) == 0){
            console->println("Failed to write");
        }
        return CMD_OK;
    }

    if( !strcmp("rr", argv[1])){
        if(ha2u(argv[2], (uint32_t*)&val)){
            uint8_t reg = val;
            count = ia2i(argv[3], &val) ? val : 1;
            I2C_Write(&m_i2c, m_i2c.addr, (uint8_t*)&reg, 1);
            if(I2C_Read(&m_i2c, m_i2c.addr, i2c_buf, count) > 0){
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
    }

    if(!xstrcmp("wr", argv[1])){
        uint32_t val;
        if(ha2u(argv[2], &val)){
            i2c_buf[0] = val;
            if(ha2u(argv[3], &val)){
                i2c_buf[1] = val;
                I2C_Write(&m_i2c, m_i2c.addr, i2c_buf, 2);
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

            m_i2c.addr = (i << 1);

            if(I2C_Read(&m_i2c, m_i2c.addr, &count, 1) == 0){
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
        I2C_Reset(&m_i2c);
        return CMD_OK;
    }

	return CMD_NOT_FOUND;
}
