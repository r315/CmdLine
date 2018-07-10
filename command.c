
#include "command.h"
#include "strfunctions.h"

char *cmdNextParameter(char *line){
    line = strchr(line, ' ');
    if(*line != '\0')
        line++;
    return line;
}

uint32_t cmdNextHex(char **line){
uint32_t hex;    
    hex = hatoi(*line);
    *line = cmdNextParameter(*line);
    return hex;
}

char cmdNextChar(char **line){
char c;    
    c = *line[0];
    *line = cmdNextParameter(*line);
    return c;
}

char cmdProcess(char *line, CmdLine *commands, uint8_t ncommands){
CmdLine *pcmd;
char *tmp, res;
uint8_t i;
  
    tmp = stringSplit(line,' ', CMD_MAX_LINE);

    if(tmp == NULL)
			tmp = "";

    pcmd = NULL;

    for(i = 0; i < ncommands; i++, commands++){
        if( xstrcmp((char*)commands->name, tmp) == 0){
            pcmd = commands;
            break;
        }
    }

    if(pcmd == NULL){
        res = CMD_NOT_FOUND;
    }
    else{
        res = pcmd->callCmd((void*)(line + strlen(tmp) + 1)); // pass arguments       
    }
    return res;
}