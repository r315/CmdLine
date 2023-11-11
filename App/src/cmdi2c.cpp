#include "board.h"
#include "cmdi2c.h"
#include "i2c.h"


void CmdI2c::help(void){
    console->print("Usage: i2c <read|write|init|scan> [option] \n\n");  
    console->print("\tinit <bus>, \n");
    console->print("\tread <device> <count>, read data\n");
    console->print("\twrite <device> <data+0 .. data+n>, write data\n");
    console->print("\tscan, Find devices on bus\n");
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
    
    if(m_i2c.peripheral == NULL){
        console->println("I2C not initialized");
		return CMD_BAD_PARAM;
    }     
    
    if( !xstrcmp("read", argv[1])){
        if(ha2i(argv[2], (uint32_t*)&val)){
            m_i2c.addr = val;
            if(ia2i(argv[3], &val)){ // count
                count = val;
                if(I2C_Read(&m_i2c, i2c_buf, count) == 0){
                    console->print("Failed to read");
                }else{
                    for(int i = 0; i < count; i ++){
                        if( (i & 15) == 0) 
                            console->printf("\n%02X: ", i & 0xF0);                
                        console->printf("%02X ", i2c_buf[i]);
                    }        
                }
                console->printchar('\n');
            }else{
                console->println("Invalid read count");
            }
            return CMD_OK;
        }       
    }
    
    if( !xstrcmp("write", argv[1])){
        if(ha2i(argv[2], (uint32_t*)&val)){
            m_i2c.addr = val;
            count = 0;
            while(ha2i(argv[3 + count], (uint32_t*)&val)){
                i2c_buf[count++] = (uint8_t)val;
            }   
                
            if(I2C_Write(&m_i2c, i2c_buf, count) == 0){
                console->println("Failed to write");
            }
            return CMD_OK;
        }
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
            
            if(I2C_Read(&m_i2c, &count, 1) == 0){
                console->print("-- ");
            }else{
                console->printf("%02X ", i << 1);
            }
            
            DelayMs(1);
        }
        console->printchar('\n');

        return CMD_OK;
    }

	return CMD_NOT_FOUND;
}
