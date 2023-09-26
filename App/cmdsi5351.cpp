#include "board.h"
#include "cmdSi5351.h"
#include "si5351.h"


void CmdSi5351::help(void)
{
    console->println("Usage: si5351 <init|freq|enable|disable|drive> [option] \n");
    console->println("\tstart <clk> <freq>, Disables Clock output");
    console->println("\tinit,               Initialize si5351, clock's disabled");
    console->println("\tfreq <clk> <freq>,  Sets desired clock frequency");
    console->println("\t\tclk: 0-7");
    console->println("\t\tfreq: 800000-15000000000 (8kHz-150MHz)");
    console->println("\tenable <clk>,       Enables Clock output");
    console->println("\tdisable <clk>,      Disables Clock output");
    console->println("\tdrive <clk> <0-3>,  Clock output drive 2, 4, 6, 8mA ");
}


char CmdSi5351::execute(int argc, char **argv)
{
    if( !xstrcmp("help", argv[1]) || argc < 2){
        help();
        return CMD_OK;
    }

    if ( !xstrcmp("start", argv[1])){
        int32_t val1, val2;
        if(si5351.update_status() == 0){
            if(si5351.init(&m_i2c, 0, 0) == 0){
                console->println("Failed to initialize Si5351");
                return CMD_OK;
            }
        }
        
        if(ia2i(argv[2], &val1)){
            if(ia2i(argv[3], &val2)){
                si5351.set_freq((si5351_clock)val1, val2);
                si5351.set_clock_pwr((si5351_clock)val1, 1);            
                si5351.set_output_enable((si5351_clock)val1, 1);
                return CMD_OK;
            }
        }
    }

    if( !xstrcmp("init", argv[1])){
        if(si5351.init(&m_i2c, 0, 0)){
            return CMD_OK;
        }
    }

    if ( !xstrcmp("freq", argv[1])){
        int32_t val1, val2;
        if(ia2i(argv[2], &val1)){
            if(ia2i(argv[3], &val2)){
                si5351.set_freq((si5351_clock)val1, val2);
                return CMD_OK;
            }
        }
    }

    if( !xstrcmp("reset", argv[1])){
        si5351.reset();
        return CMD_OK;
    }

    if( !xstrcmp("regs", argv[1])){
        uint8_t count = 192;
        uint8_t i2c_buf[count];
        i2c_buf[0] = 0;       
        
        I2C_Write(&m_i2c, i2c_buf, 1);

        if(I2C_Read(&m_i2c, i2c_buf, count) == 0){
            console->print("Failed to read");
        }else{
            for(int i = 0; i < count; i ++){
                if( (i & 15) == 0) 
                    console->printf("\n%02X: ", i & 0xF0);                
                console->printf("%02X ", i2c_buf[i]);
            }        
        }

        console->print("\n");
        
        return CMD_OK_LF;
    }

    if( !xstrcmp("write", argv[1])){
        int32_t val;
        uint8_t addr;
        if(ha2i(argv[2], (uint32_t*)&val)){
            addr = (uint8_t)val;
            if(ha2i(argv[3], (uint32_t*)&val)){
                si5351.write_reg(addr, val);
                return CMD_OK;
            }
        }
    }

    if( !xstrcmp("enable", argv[1])){
        uint32_t val;
        if(ha2i(argv[2], &val)){
            si5351.set_clock_pwr((si5351_clock)val, 1);            
            si5351.set_output_enable((si5351_clock)val, 1);
            return CMD_OK;
        }
    }

    if( !xstrcmp("disable", argv[1])){
        uint32_t val;
        if(ha2i(argv[2], &val)){
            si5351.set_clock_pwr((si5351_clock)val, 0);
            si5351.set_output_enable((si5351_clock)val, 0);
            return CMD_OK;
        }
    }

    if ( !xstrcmp("drive", argv[1])){
        uint32_t val1, val2;
        if(ha2i(argv[2], &val1)){
            if(ha2i(argv[3], &val2)){
                val2 &= 3;
                si5351.set_output_drive_strength((si5351_clock)val1, (si5351_drive)val2);
                return CMD_OK;
            }
        }
    }

    if ( !xstrcmp("clk-sel", argv[1])){
        int32_t val1, val2;
        if(ia2i(argv[2], &val1)){
            if(ia2i(argv[3], &val2)){
                si5351.set_clock_source((si5351_clock)val1, (si5351_clock_source)val2);
                return CMD_OK;
            }
        }
    }

    if ( !xstrcmp("clk-r-div", argv[1])){
        int32_t val1, val2;
        if(ia2i(argv[2], &val1)){
            if(ia2i(argv[3], &val2)){
                si5351.set_clock_r_div((si5351_clock)val1, (si5351_clock_r_div)val2);
                return CMD_OK;
            }
        }
    }


    return CMD_BAD_PARAM;
}



