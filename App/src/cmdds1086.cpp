#include "cmdds1086.h"
#include "board.h"

static uint32_t timer_callback(stimer_t *timer)
{
    sweep_t *sweep = (sweep_t*)timer->data;

    sweep->freq += sweep->step;

    if(sweep->freq > sweep->end){
        sweep->freq = sweep->start;
    }

    sweep->ds1086->frequency_set(sweep->freq);

    return timer->interval;
}

void CmdDS1086::help(void)
{
    console->println("Usage: ds1086 <init|regs|rr|wr|freq> [option]\n");
    console->println("\tinit <bus>, i2c bus 0-3");
    console->printf("\tfreq <%d to %d>\n", ds1086.min_freq_get(), ds1086.max_freq_get());
    console->println("\tsweep [start|end|step|time|run|stop]");
    console->println("\t\tstart <freq>, start frequency");
    console->println("\t\tend <freq>, end frequency");
    console->println("\t\tstep <freq>, step frequency");
    console->println("\t\ttime <ms>, step duration");
    console->println("\t\trun, start sweep");
    console->println("\t\tstop, stop sweep");
}

char CmdDS1086::execute(int argc, char **argv)
{
    int32_t val;

    if(!xstrcmp("help", argv[1]) || argc < 2){
        help();
        return CMD_OK;
    }

    if( !xstrcmp("init", argv[1])){
        if(ia2i(argv[2], &val)){
            m_i2c.bus_num = (uint8_t)(val & 3);
            m_i2c.speed = 100000;
            val = ds1086.init(&m_i2c);
            if(val < 0){
                console->println("Failed to initialize DS1086");
            }
            console->printf("OFFSET Val: 0x%x (%d)\n", val, val);
            return CMD_OK;
        }
    }

    if( !xstrcmp("freq", argv[1])){
        if(ia2i(argv[2], &val)){
            if(ds1086.frequency_set(val)){
                return CMD_OK;
            }
        }
    }

    if( !xstrcmp("regs", argv[1])){
        if(ds1086.prescaller_read((uint16_t*)&val))
            console->printf("[02] PRES:\t0x%x\n", (uint16_t)val);
        if(ds1086.dac_read((uint16_t*)&val))
            console->printf("[08] DAC:\t0x%x (%d)\n", (uint16_t)val, (uint16_t)val);
        if(ds1086.offset_read((uint8_t*)&val))
            console->printf("[0E] OFFSET:\t0x%x\n", (uint8_t)val);
        if(ds1086.addr_read((uint8_t*)&val))
            console->printf("[0D] ADDR:\t0x%x\n", (uint8_t)val);
        if(ds1086.range_read((uint8_t*)&val))
            console->printf("[37] RANGE:\t0x%x\n", (uint8_t)val);
        return CMD_OK;
    }

    if( !xstrcmp("rr", argv[1])){
        if(ha2u(argv[2], (uint32_t*)&val)){
            if(ds1086.read_reg((uint8_t)val, (uint16_t*)&val) == false){
                return CMD_NOT_FOUND;
            }
            console->printf(" %x\n", (uint16_t)val);
            return CMD_OK;
        }
    }

    if( !xstrcmp("wr", argv[1])){
        uint8_t addr;
        if(ha2u(argv[2], (uint32_t*)&val)){
            addr = (uint8_t)val;
            if(ha2u(argv[3], (uint32_t*)&val)){
                ds1086.write_reg(addr, val);
                return CMD_OK;
            }
        }
    }

    if(!xstrcmp("sweep", argv[1])){

        if(!xstrcmp("run", argv[2])){
            m_sweep.ds1086 = &ds1086;
            m_sweep.freq = m_sweep.start;
            m_timer.callback = timer_callback;
            STIMER_Config(&m_timer);
            STIMER_Start(&m_timer);
        }

        if(!xstrcmp("stop", argv[2])){
            STIMER_Stop(&m_timer);
        }

        if(!xstrcmp("start", argv[2])){
            ia2i(argv[3], &m_sweep.start);
        }

        if(!xstrcmp("end", argv[2])){
            ia2i(argv[3], &m_sweep.end);
        }

        if(!xstrcmp("step", argv[2])){
            ia2i(argv[3], &m_sweep.step);
        }

        if(!xstrcmp("time", argv[2])){
            ia2i(argv[3], (int32_t*)&m_timer.interval);
        }

        if(argc < 3){
            console->printf("start: %dHz\n", m_sweep.start);
            console->printf("end:   %dHz\n", m_sweep.end);
            console->printf("step:  %dHz\n", m_sweep.step);
            console->printf("step time: %dms\n", m_timer.interval);
        }

        return CMD_OK;
    }

    return CMD_BAD_PARAM;
}
