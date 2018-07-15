
#include "command.h"
#include "strfunctions.h"

void Command::add(Command *cmd){
    for (uint8_t i = 0; i < COMMAND_MAX_CMD; i++){
        if (cmdList[i] == NULL){
            cmdList[i] = cmd;
            return;
        }
    }
}

char Command::parse(char *line){
char res = CMD_NOT_FOUND, *cmdname, *param;  
Command **cmd = cmdList;

    cmdname = strtok_s(line, ' ', CMD_MAX_LINE, &param);

    while (*cmd != NULL){
        if((*cmd)->checkCommand(cmdname) != 0){
            res = (*cmd)->execute((void*)param);
            if (res == CMD_NOT_FOUND){
                vcom->printf("Command not found\n");
            }else if(res == CMD_BAD_PARAM){ 
                vcom->printf("Bad parameter \n");
            break;			
            }
        }
        cmd += 1;
    }
    return res;
}

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