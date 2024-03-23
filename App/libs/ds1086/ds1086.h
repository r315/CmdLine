#ifndef _ds1086_h_
#define _ds1086_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "i2c.h"

#define DS1086_DEVICE_ADDR  0x58
#define DS1086_PRES         0x02
#define DS1086_DAC          0x08
#define DS1086_OFFSET       0x0E
#define DS1086_ADDR         0x0D
#define DS1086_RANGE        0x37
#define DS1086_WRITE_EE     0x3F

class DS1086
{
public:
    DS1086();
    bool init(i2cbus_t*);
    uint8_t getOffset(){ return m_offset; }
    bool read_reg(uint8_t, uint16_t*);
	bool write_reg(uint8_t, uint16_t);
private:
    i2cbus_t *m_i2c;
    uint8_t m_offset;
};

#ifdef __cplusplus
}
#endif

#endif