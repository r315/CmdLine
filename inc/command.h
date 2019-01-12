#ifndef _command_h_
#define _command_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <string.h>

#include "type.h"
#include "vcom.h"
#include "strfunctions.h"


#define COMMAND_MAX_CMD  50
#define COMMAND_MAX_LEN 64
#define ON   1
#define OFF  0

enum{
	CMD_OK,
    CMD_NOT_FOUND,
    CMD_BAD_PARAM
};

class Command{

private:
Command *cmdList[COMMAND_MAX_CMD];

protected:
	const char *name;
	Vcom *vcom;

public:
	const char *getName(void) { return this->name; }

	void toString(void) { vcom->puts(this->name); }

	char isNameEqual(char *cmd){ return xstrcmp(cmd, (char*)this->name) == 0; }

	virtual char execute(void *ptr);
	virtual void help(void);

	void add(Command *cmd);
	char parse(char *line);

    Command(const char *nm, Vcom *vc){
        this->vcom = vc;
        this->name = nm;
    }

    Command(Vcom *vc);   
};


#ifdef __cplusplus
}
#endif

#endif
