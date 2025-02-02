#ifndef _ds1086_h_
#define _ds1086_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "i2c.h"

/**
 * @brief Basic driver to DS1086L
 * https://www.analog.com/media/en/technical-documentation/data-sheets/DS1086L.pdf
 */

#define DS1086_DEVICE_ADDR  0x58
#define DS1086_PRES         0x02
#define DS1086_DAC          0x08
#define DS1086_OFFSET       0x0E
#define DS1086_ADDR         0x0D
#define DS1086_RANGE        0x37
#define DS1086_WRITE_EE     0x3F

#define DS1086L_FREQUENCY_MIN   130000UL
#define DS1086L_FREQUENCY_MAX   66600000UL
#define DS1086L_OSCILLATOR_MIN  33300000UL
#define DS1086L_OSCILLATOR_MAX  66600000UL
#define DS1086L_PRESCALLER_MASK 0x03C0
#define DS1086L_JITTER_MASK     0xF800
#define DS1086L_OUT_CTRL_MASK   0x0400
#define DS1086_RANGE_MASK       0x1F
#define DS1086_OFFSET_MASK      0x1F
#define DS1086_DAC_MASK         0xFFC0
#define DS1086L_DAC_STEP        5000
#define DS1086_ADDR_WC          (1 << 3)

class DS1086
{
public:
    DS1086();
    int8_t init(i2cbus_t*);
    uint8_t frequency_set(uint32_t freq);
    uint32_t min_freq_get(void){return DS1086L_FREQUENCY_MIN;}
    uint32_t max_freq_get(void){return DS1086L_FREQUENCY_MAX;}

    bool prescaller_read(uint16_t *value);
    bool prescaller_write(uint16_t mask, uint16_t value);
    bool dac_read(uint16_t *value);
    bool dac_write(uint16_t value);
    bool offset_read(uint8_t *of);
    bool offset_write(uint8_t of);
    bool range_read(uint8_t *os);
    bool addr_read(uint8_t *ad);
    bool addr_write(uint8_t ad);

    bool read_reg(uint8_t, uint16_t*);
	bool write_reg(uint8_t, uint16_t);
private:
    i2cbus_t *m_i2c;
    uint8_t m_os;
    int8_t findOffset(uint32_t master_oscillator, uint32_t *min);
};

#ifdef __cplusplus
}
#endif

#endif