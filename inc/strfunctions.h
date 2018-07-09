#ifndef _strfunctions_h_
#define _strfunctions_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define FLOAT_MAX_PRECISION 6

char *stringSplit(char *str, const char token, uint8_t len);
char xstrcmp(char *str1, char *str2);
int yatoi(char *str);
int hatoi(char *str);
int xatoi (char **str, long *res);
void vftoa(void *putc(char), double f, char places);
char * strchr ( const char *str, int c);
size_t strlen(const char *str);
void hitoa (void *putc(char), long val, int radix, int len);

#ifdef __cplusplus
}
#endif
#endif
