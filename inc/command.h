#ifndef _command_h_
#define _command_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "type.h"

typedef void (*CmdCB) (void *ptr);
typedef char (CallCmd)(void *);

typedef struct _CmdLine{
	const char *name;
	CallCmd *callCmd;
	uint8_t type;
}CmdLine;

typedef struct _CmdParam{
	const char *name;
	void *param;
}CmdParam;

enum{
	CMD_OK,
    CMD_NOT_FOUND,
    CMD_BAD_PARAM
};

#define CMD_MAX_LINE 16

char cmdProcess(char *, CmdLine *, uint8_t);
char cmdNextChar(char **line);
uint32_t cmdNextHex(char **line);

#ifdef __cplusplus
}
#endif

#endif