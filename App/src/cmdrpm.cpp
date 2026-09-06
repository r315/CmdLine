#include "cmdrpm.h"
#include "gpio.h"
#include "stimer.h"
#include "app.h"


static uint32_t count;
static stimer_t rpm_tmr;
static uint32_t pin_nr;

static void pulse_handler(void)
{
    count++;
}

static uint32_t rpm_handler(stimer_t *tmr)
{
    console.printf("\e[1K\rRPM: %lu", count * 60);
    count = 0;
    return tmr->interval;
}

void CmdRpm::help(void)
{
    console->println("Usage: rpm <start|stop>");
    console->println("\tstart <pin number(hex)>");
}

char CmdRpm::execute(int argc, char **argv)
{
    if(argc == 1){
        help();
        return CMD_OK;
    }

    if(!xstrcmp("start", argv[1])){
        if(!ha2u(argv[2], &pin_nr)) return CMD_BAD_PARAM;
        rpm_tmr.interval = 1000;
        rpm_tmr.callback = rpm_handler;
        STIMER_Config(&rpm_tmr);

        GPIO_AttachInt(pin_nr, GPIO_INT_EDGE_FALLING, pulse_handler);

        STIMER_Start(&rpm_tmr);
        return CMD_OK;
    }

    if(!xstrcmp("stop", argv[1])){
        STIMER_Stop(&rpm_tmr);
        GPIO_RemoveInt(pin_nr);
        return CMD_OK;
    }

    return CMD_BAD_PARAM;
}



