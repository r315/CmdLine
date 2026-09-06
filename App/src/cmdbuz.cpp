
#include "cmdbuz.h"
#include "rtttl.h"
#include "tone.h"

static const char *demo[2] = {
        "mkombat:d=4,o=5,b=70:16a#,16a#,16c#6,16a#,16d#6,16a#,16f6,16d#6,16c#6,16c#6,16f6,16c#6,16g#6,16c#6,16f6,16c#6,16g#,16g#,16c6,16g#,16c#6,16g#,16d#6,16c#6,16f#,16f#,16a#,16f#,16c#6,16f#,16c#6,16c6",
        "Axel-F:d=4,o=5,b=125:g,8a#.,16g,16p,16g,8c6,8g,8f,g,8d.6,16g,16p,16g,8d#6,8d6,8a#,8g,8d6,8g6,16g,16f,16p,16f,8d,8a#,2g,p,16f6,8d6,8c6,8a#,g,8a#.,16g,16p,16g,8c6,8g,8f,g,8d.6,16g,16p,16g,8d#6,8d6,8a#,8g,8d6,8g6,16g,16f,16p,16f,8d,8a#,2g"
};

static const tone_t chirp [] = {
    {50, 100},
    {100, 100},
    {150, 100},
    {200, 100},
    {250, 100},
    {300, 100},
    {350, 100},
    {400, 100},
    {450, 100},
    {500, 100},
    {550, 100},
    {600, 100},
    {650, 100},
    {700, 100},
    {750, 100},
    {800, 100},
    {850, 100},
    {900, 100},
    {950, 100},
    {1000, 100},
    {0,0}
};

void CmdBuz::help(void)
{
    console->print("usage: buz <freq> <duration>\n");
    console->print("\tvol [level]\n");
    console->print("\trtttl <string>\n");
    console->print("\tdemo <nr>\n");
    console->print("\tchirp\n");
    console->print("\tstop\n");
}

char CmdBuz::execute(int argc, char **argv)
{
    int32_t freq, duration, level;

    if(argc < 2){
        help();
        return CMD_BAD_PARAM;
    }

    if(strcmp((const char*)argv[1], "rtttl") == 0){
        RTTTL_Play((const char*)argv[2]);
        return CMD_OK;
    }

    if(strcmp((const char*)argv[1], "vol") == 0){
        if(!ia2i(argv[2], &level)){
            console->printf("vol: %d\n", TONE_Volume(200));
        }else{
            console->printf("vol: %d\n", TONE_Volume(level));
        }
        return CMD_OK;
    }

    if(strcmp((const char*)argv[1], "demo") == 0){
        if(ia2i(argv[2], &level)){
            if(RTTTL_Play(demo[level & 1]) == RTTTL_ERR_INIT){
                console->printf("calling TONE_Init()\n");
                TONE_Init();
            }
            return CMD_OK;
        }
    }

    if(strcmp((const char*)argv[1], "init") == 0){
        TONE_Init();
        return CMD_OK;
    }

    if(strcmp((const char*)argv[1], "chirp") == 0){
        TONE_Play(chirp);
        return CMD_OK;
    }

    if(strcmp((const char*)argv[1], "stop") == 0){
        TONE_Stop();
        return CMD_OK;
    }

    if(!ia2i(argv[1], &freq) || !ia2i(argv[2], &duration)){
        help();
    }else{
        TONE_Start(freq, duration);
        return CMD_OK;
    }

    return CMD_BAD_PARAM;
}
