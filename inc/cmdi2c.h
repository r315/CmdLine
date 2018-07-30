#ifndef _cmdi2c_h_
#define _cmdi2c_h_


#ifdef __cplusplus
extern "C" {
#endif

#include <common.h>
#include "command.h"

class CmdI2c : public Command{
    I2C_Controller bus[I2C_MAX_IF];
public:
    CmdI2c (Vcom *vc) : Command("i2c", vc) { 
        for (uint8_t i = 0; i < I2C_MAX_IF; i++){
            bus[i].status = 255;
        } 
    }
    char execute(void *ptr);
    void help(void);
};


#define I2C_WRITE 0
#define I2C_READ  1

#ifdef __cplusplus
}
#endif

#endif