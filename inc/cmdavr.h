#ifndef _cmdavr_h_
#define _cmdavr_h_



char avrCmd(void *);
char avrHelp(void *);
char avrWrite(void *);
char avrRead(void *);
char avrEnable(void *);
char avrTest(void *ptr);
char avrFuses(void *ptr);
char avrErase(void *ptr);
#endif
