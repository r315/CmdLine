
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

char cmdExecute(char *line, CmdLine *comdList, uint8_t listSize){
CmdLine *pcmd;
char *param, res;
uint8_t i;
  
    param = strtok_s(line, ' ', CMD_MAX_LINE, &line);

    if(param == NULL)
			param = (char*)"";

    pcmd = NULL;

    for(i = 0; i < listSize; i++, comdList++){
        if( xstrcmp((char*)comdList->name, param) == 0){
            pcmd = comdList;
            break;
        }
    }

    if(pcmd == NULL){
        res = CMD_NOT_FOUND;
    }
    else{
        res = pcmd->callCmd((void*)line);
    }
    return res;
}