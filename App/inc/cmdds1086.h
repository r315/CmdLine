#ifndef _cmdds1086_h_
#define _cmdds1086_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "console.h"
#include "i2c.h"
#include "ds1086.h"
#include "stimer.h"

typedef struct {
    int32_t start;
    int32_t end;
    int32_t step;
    int32_t freq;
    DS1086 *ds1086;
}sweep_t;

class CmdDS1086 : public ConsoleCommand{
    Console *console;
    i2cbus_t m_i2c;
    DS1086 ds1086;
    stimer_t m_timer;
    sweep_t m_sweep;
public:
    void init(void *params)
    {
        console = static_cast<Console*>(params);
        m_sweep.start = 1000000;
        m_sweep.end   = 66000000;
        m_sweep.step  = 1000000;
        m_timer.interval = 100;
        m_timer.data = &m_sweep;
    }

    char execute(int argc, char **argv);
    void help(void);

    CmdDS1086 () : ConsoleCommand("ds1086") {  m_i2c = {NULL, 0, 0, 0, 0}; }
};


#ifdef __cplusplus
}
#endif

#endif