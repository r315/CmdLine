
#include "board.h"
#include "buzzer.h"

#include <console.h>
#include "cmdecho.h"
#include "cmdhelp.h"
#include "cmdmem.h"
#include "cmdrfinder.h"
#include "cmdadc.h"
#include "cmdservo.h"
#include "cmdspiflash.h"
#include "cmdflashrom.h"
#include "cmdbuz.h"
#include "cmdversion.h"
#include "cmdkeyfob.h"
#include "cmdpwm.h"
#include "cmdtft.h"
#include "cmdencoder.h"
#include "cmdlmcshd.h"
#include "cmdbenchmark.h"
#include "display.h"
#include "cmdspi.h"
#include "cmdavr.h"
#include "cmdreset.h"
#include "cmdsbus.h"
#include "cmdi2s.h"
#include "cmdi2c.h"
#include "cmdawg.h"
#include "cmdgpio.h"
#include "cmdmsensor.h"
#include "cmdplayer.h"

#if defined (BOARD_BLUEBOARD)
#include "cmdsd.h"
#include "stk500.h"
#endif

static ConsoleCommand *app_commands[] = {
    new CmdHelp(),
	new CmdEcho(),
    new CmdReset(),
	//new CmdPwm(),
	new CmdVersion(),
	new CmdTft(),
    new CmdMem(),
#if defined(BOARD_BLUEBOARD)
    new CmdSpi(),
    new CmdAvr(),
    new CmdSbus(),
    new CmdSd(),
	new CmdGpio(),
	new CmdI2c(),
	new CmdI2s(),
	new CmdMSensor(),
	new CmdPlayer(),
    
#elif defined (BOARD_BLUEPILL)
	new CmdRfinder(),
	new CmdAdc(),
	new CmdServo(),
	new CmdSpiFlash(),
	new CmdFlashRom(),
	new CmdEncoder(),
#elif defined (BOARD_NUCLEO_L412KB)
	new CmdBuz(),
	new CmdKeyFob(),
    new CmdLmcshd(),
    new CmdBenchmark(),
#endif
};

extern "C" void App(void)
{   
    Console console;
    stdout_t *userio = SERIAL_GetStdout(-1);

#if defined (BOARD_BLUEBOARD)
    console.init(userio, "Blueboard>");    
#elif defined (BOARD_BLUEPILL)
    console.init(userio, "bluepill>");
#elif defined (BOARD_NUCLEO_L412KB)
    console.init(userio, "nucleo>");
#endif    

    for(unsigned int i = 0; i < sizeof(app_commands)/sizeof(ConsoleCommand*); i++){
        console.addCommand(app_commands[i]);
    }

	console.cls();
	console.print("Freq: %dMHz\n", SystemCoreClock / 1000000);

#if defined (BOARD_NUCLEO_L412KB)
	LED_OFF;
	buzInit();
	buzSetLevel(50);
	buzPlayRtttl("rtttl_14:d=16,o=6,b=180:c,e,g");
#endif

	while(1){
		console.process();
	}
}
