
#include "cmdbuz.h"
#include "buzzer.h"

static const char *demo[2] = {
        "mkombat:d=4,o=5,b=70:16a#,16a#,16c#6,16a#,16d#6,16a#,16f6,16d#6,16c#6,16c#6,16f6,16c#6,16g#6,16c#6,16f6,16c#6,16g#,16g#,16c6,16g#,16c#6,16g#,16d#6,16c#6,16f#,16f#,16a#,16f#,16c#6,16f#,16c#6,16c6",
        "Axel-F:d=4,o=5,b=125:g,8a#.,16g,16p,16g,8c6,8g,8f,g,8d.6,16g,16p,16g,8d#6,8d6,8a#,8g,8d6,8g6,16g,16f,16p,16f,8d,8a#,2g,p,16f6,8d6,8c6,8a#,g,8a#.,16g,16p,16g,8c6,8g,8f,g,8d.6,16g,16p,16g,8d#6,8d6,8a#,8g,8d6,8g6,16g,16f,16p,16f,8d,8a#,2g"
};

void CmdBuz::help(void) {
		console->putString("usage: buz <freq | vol | rtttl | demo > <duration | level | string | 1,0>");
}

char CmdBuz::execute(void *ptr) {
    int32_t freq, duration, level;
    char *argv[4];
    int argc;

    argc = strToArray((char*)ptr, argv);

    if(argc < 2){
        help();
        return CMD_BAD_PARAM;
    }

    if(strcmp((const char*)argv[0], "rtttl") == 0){
        buzPlayRtttl((const char*)argv[1]);
        buzWaitEnd();
        return CMD_OK;
    }

    if(strcmp((const char*)argv[0], "vol") == 0){
        if(!yatoi(argv[1], &level)){
            console->print("vol: %d\n", buzSetLevel(200));
        }else{
            console->print("vol: %d\n", buzSetLevel(level));
        }
        return CMD_OK;
    }

    if(strcmp((const char*)argv[0], "demo") == 0){
        if(!yatoi(argv[1], &level)){

            return CMD_OK;
        }

        buzPlayRtttl(demo[level]);
        buzWaitEnd();
        return CMD_OK;
    }

    if(!yatoi(argv[0], &freq) || !yatoi(argv[1], &duration)){
        help();
        return CMD_BAD_PARAM;
    }

    buzPlayTone(freq, duration);

    return CMD_OK;
}
