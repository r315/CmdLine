#include "cmdds1086.h"
#include "board.h"

#define OSCILLATOR_MIN  33300000UL
#define OSCILLATOR_MAX  66600000UL

typedef struct {
    uint32_t min;
    uint32_t max;
}range_t;

const range_t os_table [] = {
    {30740000, 35860000},   // OS-6
    {33300000, 38420000},   // OS-5
    {35860000, 40980000},   // OS-4
    {38420000, 43540000},   // OS-3
    {40980000, 46100000},   // OS-2
    {43540000, 48660000},   // OS-1
    {46100000, 51220000},   // OS+0
    {48660000, 53780000},   // OS+1
    {51220000, 56340000},   // OS+2
    {53780000, 58900000},   // OS+3
    {56340000, 61460000},   // OS+4
    {58900000, 64020000},   // OS+5
    {61460000, 66580000},   // OS+6
};

/**
 * @brief Find suitable table offset for a given
 * master frequency.
 * 
 * TODO: select offset for which the master oscillator is closer to
 *       mid value
 * 
 * @param [in] master_oscillator - Desired master oscillator frequency
 * @param [out] min              - Minimum frequency of selected range
 * @return int8_t                - Offset value [-6, 6]
 */
int8_t findOffset(uint32_t master_oscillator, uint32_t *min)
{
    int8_t offset = -6;
    uint8_t i;

    for(i = 0; i < sizeof(os_table)/sizeof(range_t); i++) {
        if(master_oscillator >= os_table[i].min && master_oscillator <= os_table[i].max) {
            break;
        } 
    }
    
    if(i == sizeof(os_table)/sizeof(range_t)){
        i = 6; // OS + 0
    }

    *min = os_table[i].min;
    
    return offset + i;
}

void CmdDS1086::help(void)
{
    console->println("Usage: ds1086 <init|regs|rr|wr|freq> [option] \n");
    console->println("\t freq <130000 - 66600000> \n");
}

char CmdDS1086::execute(int argc, char **argv)
{
    int32_t val;

    if(!xstrcmp("help", argv[1])){
        help();
        return CMD_OK;
    }

    if( !xstrcmp("init", argv[1])){
        if(ds1086.init(&m_i2c) == 0){
            console->println("Failed to initialize DS1086");
        }

        console->printf("OFFSET Val: %x (%d)\n", ds1086.getOffset(), ds1086.getOffset());
        return CMD_OK;
    }

    if( !xstrcmp("regs", argv[1])){
        if(ds1086.read_reg(DS1086_PRES, (uint16_t*)&val) == false) return CMD_NOT_FOUND;
        console->printf("[02] PRES:\t%x\n", (uint16_t)val);
        
        if(ds1086.read_reg(DS1086_DAC, (uint16_t*)&val) == false) return CMD_NOT_FOUND;
        console->printf("[08] DAC:\t%x\n", (uint16_t)val);
        

        if(ds1086.read_reg(DS1086_OFFSET, (uint16_t*)&val) == false) return CMD_NOT_FOUND;
        console->printf("[0E] OFFSET:\t%x\n", (uint8_t)val & 0x1f);

        if(ds1086.read_reg(DS1086_ADDR, (uint16_t*)&val) == false) return CMD_NOT_FOUND;
        console->printf("[0D] ADDR:\t%x\n", (uint8_t)val & 0x0f);

        if(ds1086.read_reg(DS1086_RANGE, (uint16_t*)&val) == false) return CMD_NOT_FOUND;
        console->printf("[37] RANGE:\t%x\n", (uint8_t)val & 0x1f);
                
        return CMD_OK;
    }

    if( !xstrcmp("rr", argv[1])){                
        if(ha2i(argv[2], (uint32_t*)&val)){
            if(ds1086.read_reg((uint8_t)val, (uint16_t*)&val) == false){
                return CMD_NOT_FOUND;
            }
            console->printf(" %x\n", (uint16_t)val);
            return CMD_OK;
        }
    }

    if( !xstrcmp("wr", argv[1])){
        uint8_t addr;
        if(ha2i(argv[2], (uint32_t*)&val)){
            addr = (uint8_t)val;
            if(ha2i(argv[3], (uint32_t*)&val)){
                ds1086.write_reg(addr, val);
                return CMD_OK;
            }
        }
    }

    if( !xstrcmp("freq", argv[1])){
        if(ia2i(argv[2], &val)){
            if(val > 130000 && val < 66600000){
                uint32_t master_oscillator, dac;
                uint8_t exp, offset;
                
                // Find exponent
                for(exp = 0; exp < 9; exp++){
                    master_oscillator = val << exp;
                    if(master_oscillator > OSCILLATOR_MIN && master_oscillator < OSCILLATOR_MAX){
                        //console->printf("exp[%d] master freq %d \n", exp, master_oscillator);
                        break;
                    }
                }

                // get offset reg value
                offset = ds1086.getOffset() + findOffset(master_oscillator, (uint32_t*)&val);              

                // calculate value for DAC
                dac = (master_oscillator - val) / 5000;            

                console->printf("Setting regs to:\n"
                                "DAC = %x \n"
                                "OFFSET = %x \n"
                                "PRESCALLER = %d \n",
                                dac, offset, (1 << exp));

                ds1086.read_reg(DS1086_PRES, (uint16_t*)&master_oscillator);
                ds1086.write_reg(DS1086_PRES, (uint16_t)((master_oscillator & 0xFC00) | (exp << 6)));
                ds1086.write_reg(DS1086_DAC, (uint16_t)(dac << 6));
                ds1086.write_reg(DS1086_OFFSET, offset);

                return CMD_OK;
            }

        }
    }

    return CMD_BAD_PARAM;
}