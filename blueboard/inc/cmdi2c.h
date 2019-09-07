#ifndef _cmdi2c_h_
#define _cmdi2c_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "command.h"

class CmdI2c : public Command{

public:
    CmdI2c (Vcom *vc) : Command("i2c", vc) { 
       
    }
    char execute(void *ptr);
    void help(void);
};


#define I2C_WRITE   0
#define I2C_READ    1
#define I2C_SCAN    2

#ifdef __cplusplus
}
#endif

#endif
