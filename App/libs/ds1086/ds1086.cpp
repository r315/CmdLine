#include "ds1086.h"
#include "board.h"

DS1086::DS1086()
{
    m_i2c = NULL;
}

bool DS1086::init(i2cbus_t *i2c)
{
    uint16_t reg_val;
    m_i2c = i2c;
    m_i2c->addr = DS1086_DEVICE_ADDR << 1;

    if(m_i2c->peripheral == NULL){
        I2C_Init(m_i2c);
        DelayMs(100);
    }

    if(read_reg(DS1086_RANGE, &reg_val) == true){
        m_offset = (uint8_t)reg_val & 0x1f;
        return true;
    }

    return false;
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