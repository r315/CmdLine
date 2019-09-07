#ifndef _cmdlsd_h_
#define _cmdlsd_h_


#ifdef __cplusplus
extern "C" {
#endif
 
#include "console.h"
#include <pff.h>
#include <diskio.h>


class CmdSd : public ConsoleCommand{
    Console *console;
public:
    CmdSd () : ConsoleCommand("sd") { }
    void init(void *params) { console = static_cast<Console*>(params); }
    char execute(void *ptr);
    void help(void);
    void dumpSector(uint32_t);
    void f_error(FRESULT res);
    void d_error(DRESULT res);
};


#ifdef __cplusplus
}
#endif

#endif