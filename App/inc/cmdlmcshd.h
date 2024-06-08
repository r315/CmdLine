#ifndef _cmdlmcshd_h_
#define _cmdlmcshd_h_


#ifdef __cplusplus
extern "C" {
#endif

#include <console.h>
#include "serial.h"

class CmdLmcshd : public ConsoleCommand{
	Console *console;
    serialops_t *m_serial;
    uint8_t m_matrix_w;
    uint8_t m_matrix_h;
    uint8_t *m_matrix_buffer;
public:
    CmdLmcshd () : ConsoleCommand("lmcshd") {}
    void init(void *params);

    char execute(int argc, char **argv);
    void help(void);
};


#ifdef __cplusplus
}
#endif

#endif
