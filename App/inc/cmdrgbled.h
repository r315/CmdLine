#ifndef _cmdrgbled_h_
#define _cmdrgbled_h_


#ifdef __cplusplus
extern "C" {
#endif
 
#include "console.h"

class CmdRgbled : public ConsoleCommand{
    Console *console;
public:
    void init(void *params) { console = static_cast<Console*>(params); }
    
    void help(void);
    char execute(int argc, char **argv);

    CmdRgbled () : ConsoleCommand("rgbled") { }

private:
    uint32_t Wheel(uint8_t WheelPos);
    void visRainbow(uint8_t *frameBuffer, uint32_t frameBufferSize, uint32_t effectLength);
    void visDots(uint8_t *frameBuffer, uint32_t frameBufferSize, uint32_t random, uint32_t fadeOutFactor);
    void visHandle2(void);
    void visInit(void);
    void visHandle(void);
};


#ifdef __cplusplus
}
#endif

#endif