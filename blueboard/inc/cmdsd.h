#ifndef _cmdlsd_h_
#define _cmdlsd_h_


#ifdef __cplusplus
extern "C" {
#endif
 
#include "console.h"
#include "pff.h"
#include "diskio.h"
#include "spi.h"


class CmdSd : public ConsoleCommand{
    Console *console;
    spibus_t *m_spi;
public:
    CmdSd () : ConsoleCommand("sd") { }
    void init(void *params) { console = static_cast<Console*>(params); }
    char execute(int argc, char **argv);
    void help(void);
    void dumpSector(uint32_t);
    void f_error(FRESULT res);
    void d_error(DRESULT res);
    void d_status(DSTATUS sta);
    FRESULT listDir (const char* pat, bool recursive);
};


#ifdef __cplusplus
}
#endif

#endif