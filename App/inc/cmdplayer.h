#ifndef _cmdplayer_h_
#define _cmdplayer_h_


#ifdef __cplusplus
extern "C" {
#endif
 
#include "console.h"

class CmdPlayer : public ConsoleCommand{
public:
    CmdPlayer () : ConsoleCommand("player") { }
    void init(void *params) { console = static_cast<Console*>(params); }
    char execute(int argc, char **argv);
    void help(void);

    void playFile(const char *file);
    void playBuffer(void *buf, uint32_t len);
    void playTone(uint32_t freq);
    void stop(void);

private:
    void rawFile();
    void mp3File();
    Console *console;
};


#ifdef __cplusplus
}
#endif

#endif