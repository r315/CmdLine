
#include <stdlib.h>
#include "strfunctions.h"

size_t strlen(const char *str){
size_t count = 0;
	while(*str++){
		count++;
	}
	return count;	
}

char * strchr ( const char *str, int c){
	while(*str){
		if(*str == c)
			break;
		str += 1;
	}
	return (char*)str;
}

char *stringSplit(char *str, const char token, uint8_t len){
uint8_t i;

	if(str == NULL){
		return NULL;
	}

    i = 0;

	while(*str && i < len){
		if(*str == token){
			*str = '\0';  // end string
			break;
		}
		str += 1;
        i++;
	}

    if ( i == len ){
        return NULL;
	}

	return (char*)(str - i);
}

char xstrcmp(char *str1, char *str2){
	do{
		if(*str1 != *str2)
			return (*str1 - *str2);		
	}while(*str1++ && *str2++);
	return 0;
}

// convert decimal str to integer
int yatoi(char *str){
int val=0;	
char c,radix = 10;
	while(*str){
		c = *str;
		if(c < '0' || c > '9')
			return -1;
		c -= '0';
		val = val*radix + c;
		str +=1;
	}
	return val;
}

#if 0
// convert str to integer seams not working
int hatoi(char *str){
int val = -1;	
char c;
	while(*str){		
		c = (*str) - ('a'+'A');
		if(c > '/' && c < ':')
			c -= '0';
		else if((c >'@' && c < 'G'))
			c -= 'A';
		else
			return -1;			
		val |= c;
		val <<= 4;
		str +=1;
	}
	return val;
}
#endif

// convert hex str to integer
int hatoi(char *str){
int val = 0;	
char c;
	do{
        val <<= 4;
        c = (*str);
        if(c > '`' && c < 'g'){
            c -= 'W'; 
        }else if((c >'@' && c < 'G')){
            c -= '7';
        }else if(c > '/' && c < ':'){
			c -= '0';
        }		
		else
			return -1;			
		val |= c;		
		str +=1;
	}while(*str);

	return val;
}


// convert integer to string
void hitoa (void *putc(char), long val, int radix, int len)
{
	uint8_t c, r, sgn = 0, pad = ' ';
	uint8_t s[20], i = 0;
	uint32_t v;


	if (radix < 0) {
		radix = -radix;
		if (val < 0) {
			val = -val;
			sgn = '-';
		}
	}
	v = val;
	r = radix;
	if (len < 0) {
		len = -len;
		pad = '0';
	}
	if (len > 20) return;
	do {
		c = (uint8_t)(v % r);
		if (c >= 10) c += 7;
		c += '0';
		s[i++] = c;
		v /= r;
	} while (v);
	if (sgn) s[i++] = sgn;
	while (i < len)
		s[i++] = pad;
	do
		putc(s[--i]);
	while (i);
}

/* Original code by ELM_ChaN. Modified by Martin Thomas */
int xatoi (char **str, long *res)
{
	uint32_t val;
	uint8_t c, radix, s = 0;


	while ((c = **str) == ' ') (*str)++;
	if (c == '-') {
		s = 1;
		c = *(++(*str));
	}
	if (c == '0') {
		c = *(++(*str));
		if (c <= ' ') {
			*res = 0; return 1;
		}
		if (c == 'x') {
			radix = 16;
			c = *(++(*str));
		} else {
			if (c == 'b') {
				radix = 2;
				c = *(++(*str));
			} else {
				if ((c >= '0')&&(c <= '9'))
					radix = 8;
				else
					return 0;
			}
		}
	} else {
		if ((c < '1')||(c > '9'))
			return 0;
		radix = 10;
	}
	val = 0;
	while (c > ' ') {
		if (c >= 'a') c -= 0x20;
		c -= '0';
		if (c >= 17) {
			c -= 7;
			if (c <= 9) return 0;
		}
		if (c >= radix) return 0;
		val = val * radix + c;
		c = *(++(*str));
	}
	if (s) val = -val;
	*res = val;
	return 1;
}

void * memcpy ( void * destination, const void * source, size_t num ){
	for(size_t i = 0; i < num; i++){
		*((uint8_t*)destination + i) = *((uint8_t*)source);
		source = (uint8_t*)source + 1; 
	}
	return destination;
}

void * memset ( void * ptr, int value, size_t num ){
	for(size_t i = 0; i < num; i++){
		*((uint8_t*)ptr + i) = (uint8_t)value;
	}
	return ptr;
}
//-----------------------------------------------------------
//
//-----------------------------------------------------------
void vitoa (void *putc(char), long val, signed char radix, signed char len){
	unsigned char c, r, sgn = 0, pad = ' ';
	unsigned char s[16], i = 0;
	unsigned int v;

	if (radix < 0) {
		radix = -radix;
		if (val < 0) {
			val = -val;
			sgn = '-';
		}
	}
	
	v = val;
	r = radix;
	
	if (len < 0) {
		len = -len;
		pad = '0';
	}
	
	if (len > 16) len = 16;
	
	do {
		c = (unsigned char)(v % r);
		if (c >= 10) c += 7;
		c += '0';
		s[i++] = c;
		v /= r;
	} while (v);
	
	if(sgn) 
		s[i++] = sgn;
	
	while (i < len)
		s[i++] = pad;
		
	do{
		putc(s[--i]);
	}while (i);
}
//-----------------------------------------------------------
//https://en.wikipedia.org/wiki/Single-precision_floating-point_format
//https://wirejungle.wordpress.com/2011/08/06/displaying-floating-point-numbers
//-----------------------------------------------------------
void vftoa(void *putc(char), double f, char places){
long int_part, frac_part;
char prec;
  
    int_part = (long)(f);  
    
	if(places > FLOAT_MAX_PRECISION)
		places = FLOAT_MAX_PRECISION;
		
	frac_part = 0;
	prec = 0;
	while ((prec++) < places){
			f *= 10;
			frac_part = (frac_part * 10) + (long)f - ((long)f / 10) * 10;  //((long)f%10);			
	}

    vitoa(putc, int_part, -10, 0);
    putc('.');   
    vitoa(putc, abs(frac_part), 10, -places);  
}
