#include "ds1086.h"
#include "board.h"
#include <string.h>


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

static bool reg_read(i2cbus_t *i2c, uint8_t addr, uint8_t *dst, uint8_t len)
{
    if(I2C_Write(i2c, (uint8_t*)&addr, 1) != 1)
        return false;

    if(I2C_Read(i2c, dst, len) != len)
        return false;

    return true;
}

static bool reg_write(i2cbus_t *i2c, uint8_t addr, uint8_t *src, uint8_t len)
{
    len += 1;
    uint8_t buf[len];
    buf[0] = addr;

    memcpy(buf + 1, src, len - 1);

    return I2C_Write(i2c, buf, len) == len;
}

DS1086::DS1086()
{
    m_i2c = NULL;
}

int8_t DS1086::init(i2cbus_t *i2c)
{
    uint8_t addr;

    m_i2c = i2c;
    m_i2c->addr = DS1086_DEVICE_ADDR << 1;

    if(m_i2c->peripheral == NULL){
        I2C_Init(m_i2c);
        DelayMs(100);
    }

    addr_read(&addr);

    if(!(addr & DS1086_ADDR_WC)){
        // Disable EEPROM auto write
        addr_write (addr | DS1086_ADDR_WC);
        DelayMs(100);
    }

    if(range_read(&m_os)){
        return m_os;
    }

    return -1;
}

bool DS1086::addr_read(uint8_t *addr)
{
    return reg_read(m_i2c, DS1086_ADDR, addr, 1);
}

bool DS1086::addr_write(uint8_t addr)
{
    return reg_write(m_i2c, DS1086_ADDR, &addr, 1);
}

bool DS1086::range_read(uint8_t *os)
{
    uint8_t reg_val;

    if(reg_read(m_i2c, DS1086_RANGE, &reg_val, 1)){
        *os = reg_val & DS1086_RANGE_MASK;
        return true;
    }

    return false;
}

bool DS1086::offset_read(uint8_t *os)
{
    uint8_t reg_val;

    if(reg_read(m_i2c, DS1086_OFFSET, &reg_val, 1)){
        *os = reg_val & DS1086_OFFSET_MASK;
        return true;
    }

    return false;
}

bool DS1086::offset_write(uint8_t os)
{
    return reg_write(m_i2c, DS1086_OFFSET, &os, 1);
}

bool DS1086::dac_read(uint16_t *value)
{
    uint8_t buf[2];

    if(reg_read(m_i2c, DS1086_DAC, buf, 2)){
        *value = (buf[0]<<2) | (buf[1] >> 6);
        return true;
    }

    return false;
}

bool DS1086::dac_write(uint16_t value)
{
    uint8_t buf[2];

    value = (value << 6) & DS1086_DAC_MASK;

    buf[0] = (value >> 8);
    buf[1] = value;

    return reg_write(m_i2c, DS1086_DAC, buf, 2);
}

bool DS1086::prescaller_read(uint16_t *value)
{
    uint8_t buf[2];

    if(reg_read(m_i2c, DS1086_PRES, buf, 2)){
        *value = (buf[0]<<8) | buf[1];
        return true;
    }

    return false;
}

bool DS1086::prescaller_write(uint16_t mask, uint16_t value)
{
    uint8_t buf[2];

    if(!reg_read(m_i2c, DS1086_PRES, buf, 2)){
        return false;
    }

    uint16_t reg_val = (buf[0] << 8) | buf[1];

    reg_val = (reg_val & ~mask);

    switch (mask){
        case DS1086L_PRESCALLER_MASK:
            reg_val |= value << 6;
            break;
        case DS1086L_JITTER_MASK:
            reg_val |= value << 11;
            break;
        case DS1086L_OUT_CTRL_MASK:
            reg_val |= value << 10;
            break;
        default:
            return false;
    }

    buf[0] = reg_val >> 8;
    buf[1] = reg_val;

    return reg_write(m_i2c, DS1086_PRES, buf, 2);
}

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
int8_t DS1086::findOffset(uint32_t master_oscillator, uint32_t *min)
{
    uint8_t table_size = sizeof(os_table) / sizeof(range_t);
    uint8_t candidate = table_size/2;
    int32_t delta = INT32_MAX;

    // Go thtough all values on table and select the one
    // with the smalest deviation of center frequency;
    for(int i = 0; i < table_size; i++) {
        if(master_oscillator >= os_table[i].min && master_oscillator <= os_table[i].max) {
            int32_t center = os_table[i].min + ((os_table[i].max - os_table[i].min) / 2);
            int32_t diff = center - master_oscillator;

            diff = diff < 0 ? -diff : diff;

            if(diff < delta){
                delta = diff;
                candidate = i;
            }
        }
    }

    *min = os_table[candidate].min;

    return candidate - 6;
}

uint8_t DS1086::frequency_set(uint32_t freq)
{
    uint32_t master_oscillator, dac;
    uint8_t exp, offset;

    if(freq < DS1086L_FREQUENCY_MIN || freq > DS1086L_FREQUENCY_MAX){
        return false;
    }

    // Find exponent
    for(exp = 0; exp < 9; exp++){
        master_oscillator = freq << exp;
        if(master_oscillator > DS1086L_OSCILLATOR_MIN && master_oscillator < DS1086L_OSCILLATOR_MAX){
            break;
        }
    }

    uint32_t min;

    // get new offset value and minimum frequency of new offset
    offset = m_os + findOffset(master_oscillator, &min);

    // DAC value is the difference between desired
    // main oscillator frequency and range set by offset
    dac = (master_oscillator - min) / DS1086L_DAC_STEP;

    prescaller_write(DS1086L_PRESCALLER_MASK, exp); // Divide main oscillator to get desired frequency
    dac_write(dac);                                 // Fine tune main oscillator frequency
    offset_write(offset);                           // Set main oscillator frequency range

    return true;
}


bool DS1086::read_reg(uint8_t reg, uint16_t *value)
{
    uint8_t buf[2];

    switch(reg){
        case DS1086_PRES:
        case DS1086_DAC:
            if(I2C_Write(m_i2c, &reg, 1) != 1) break;
            if(I2C_Read(m_i2c, buf, 2) != 2) break;
            *value = buf[0] << 8 | buf[1];
            return true;

        case DS1086_OFFSET:
        case DS1086_ADDR:
        case DS1086_RANGE:
            if(I2C_Write(m_i2c, &reg, 1) != 1) break;
            if(I2C_Read(m_i2c, (uint8_t*)value, 1) != 1) break;
            return true;

        default: break;
    }
    return false;
}

bool DS1086::write_reg(uint8_t reg, uint16_t value)
{
    uint8_t buf[3];

    switch(reg){
        case DS1086_PRES:
        case DS1086_DAC:
            buf[0] = reg;
            buf[1] = value >> 8;
            buf[2] = value;
            if(I2C_Write(m_i2c, (uint8_t*)&buf, 3) != 3) break;
            return true;

        case DS1086_OFFSET:
        case DS1086_ADDR:
        case DS1086_RANGE:
            value = (value << 8) | reg;
            if(I2C_Write(m_i2c, (uint8_t*)&value, 2) != 2) break;
            return true;

        default: break;
    }
    return false;
}