
#include "board.h"
#include "buzzer.h"

#include "console.h"
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
#include "cmdsi5351.h"
#include "cmdds1086.h"
#include "cmdrgbled.h"

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
    //new CmdMem(),
#if defined (ENABLE_TFT_DISPLAY)
	new CmdTft(),
#endif
#if defined (ENABLE_BENCHMARK)
    new CmdBenchmark(),
#endif
#if defined (ENABLE_DIGITAL_AUDIO)
    new CmdI2s(),
#endif
#if defined (ENABLE_I2C)
    new CmdI2c(),
#endif
#if defined (ENABLE_SI5351)
    new CmdSi5351(),
#endif
#if defined (ENABLE_DS1086)
    new CmdDS1086(),
#endif
#if defined (ENABLE_RGBLED)
    new CmdRgbled(),
#endif

#if defined (BOARD_BLUEBOARD)
    new CmdSpi(),
    //new CmdAvr(),
    //new CmdSbus(),
    new CmdSd(),
	new CmdGpio(),
	//new CmdMSensor(),
	new CmdPlayer(),
    
#elif defined (BOARD_BLUEPILL)
	new CmdRfinder(),
	new CmdAdc(),
	new CmdServo(),
	new CmdSpiFlash(),
	new CmdFlashRom(),
	//new CmdEncoder(),
#elif defined (BOARD_NUCLEO_L412KB)
	new CmdBuz(),
	new CmdKeyFob(),
    new CmdLmcshd(),
#endif

#if defined(ENABLE_AVR_PROGRAMMING)
    new CmdAvr(),
#endif
};

extern "C" void App(void)
{   
    Console console;
    // stdout_t and serialops_t must be compatible for this to work
    stdout_t *userio = (stdout_t*)SERIAL_GetSerialOps(-1);

#if defined (BOARD_BLUEBOARD)
    console.init(userio, "Blueboard>");    
#elif defined (BOARD_BLUEPILL)
    console.init(userio, "bluepill>");
#elif defined (BOARD_NUCLEO_L412KB)
    console.init(userio, "nucleo>");
#else
    console.init(userio, "415dk>");
#endif

    for(unsigned int i = 0; i < sizeof(app_commands)/sizeof(ConsoleCommand*); i++){
        console.addCommand(app_commands[i]);
    }

	console.cls();

	console.printf("CPU Clock: %dMHz\n", SystemCoreClock / 1000000);

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

extern "C" int16_t map(int16_t x, int16_t in_min, int16_t in_max, int16_t out_min, int16_t out_max){
    return ((x - in_min) * (out_max - out_min) / (in_max - in_min)) + out_min;
}
