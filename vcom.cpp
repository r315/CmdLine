
#include <stdint.h>
#include <stdio.h>

#include "vcom.h"
#include "usbserial.h"
#include "strfunctions.h"

//-------------------------------------------
// Standard input output functions
//-------------------------------------------
/**
	Initialises the VCOM port.
	Call this function before using VCOM_puttchar or VCOM_getchar
 */
void Vcom::init(void)
{
	rxfifo.size = VCOM_FIFO_SIZE;
	txfifo.size = VCOM_FIFO_SIZE;

	fifo_init(&txfifo);
	fifo_init(&rxfifo);
	USBSERIAL_Init(&txfifo, &rxfifo);
}

/**
	Writes one character to VCOM port
	
	@param [in] c character to write
	@returns character written, or EOF if character could not be written
 */
int Vcom::putchar(int c)
{
	return fifo_put(&txfifo, c) ? c : EOF;
}

/**
	Reads one character from VCOM port
	
	@returns character read, or EOF if character could not be read
 */
int Vcom::getchar(void)
{
	uint8_t c;	
	return fifo_get(&rxfifo, &c) ? c : EOF;
}

void Vcom::putc(char c)
{
	while(putchar(c) == EOF);
}

void Vcom::puts(const char* str)
{
	while(*str != '\0')
	{
		putchar(*str++);
	}
}

char Vcom::getc()
{
	int c;
	c = EOF;
	while(c == EOF)
	{
		c = getchar();
	}
	return (char)c;
}

void Vcom::putHex(uint8_t hex)
{
	uint8_t temp;
	puts("0x");
	
	temp = ((hex >> 4) & 0x0F) + 0x30;// add 0x30 to get ASCII value
	if(temp > 0x39)
		temp += 7; // alphabet, not numeral
	putc((char)temp); 
	
	temp = ((hex) & 0x0F) + 0x30;
	if(temp > 0x39)
		temp += 7;
		
	putc((char)temp);
}

void Vcom::gets(char* str)
{
	char c;
	c = getc();
	while((c != '\n') && (c != '\r'))
	{
		*str++ = c;
		c = getc();
	}
	*str = '\0';
}

char Vcom::getc_echo()
{
	char c;
	c = getc();
	putc(c);
	return c;
}

char Vcom::getLine(char *line, uint8_t max)
{
char s = 0;
	char c;
	c = getc();
	while((c != '\n') && (c != '\r'))
	{
		if(c == '\b'){
			if(s != 0){
				putc(c);
				putc(' ');
				putc(c);
				line--;
				s--;
			}
		}else{
			if(s < max){
				putc(c);
			   *line++ = c;
			   s++;
			}			
		}
		c = getc();
	}
	*line = '\0';
	return s;
}

void Vcom::printf (const char* str, ...)
{
	va_list arp;
	int d, r, w, s, l,f;

	va_start(arp, str);

	while ((d = *str++) != 0) {
		if (d != '%') {
			putc(d); continue;
		}
		d = *str++; w = r = s = l = 0;
		if(d == '.'){           
			d = *str++; f = 1; 
		}
		if (d == '0') {
			d = *str++; s = 1;
		}
		while ((d >= '0')&&(d <= '9')) {
			w += w * 10 + (d - '0');
			d = *str++;
		}		
		if (d == 'l') {
			l = 1;
			d = *str++;
		}
		if (!d) break;
		if (d == 's') {
			puts(va_arg(arp, char*));
			continue;
		}
		if (d == 'c') {
			putc((char)va_arg(arp, int));
			continue;
		}		
		if (d == 'u') r = 10;
		if (d == 'd') r = -10;
		if (d == 'X' || d == 'x') r = 16; // 'x' added by mthomas in increase compatibility
		if (d == 'b') r = 2;
		 if(d == 'f'){
			if(!f)
				w = 6;						// dafault 6 decimal places
			puts(pftoa(va_arg(arp, double), w));			
			continue;
		}	
		if (!r) break;
		if (s) w = -w;
		if (l) {
			puts(pitoa((long)va_arg(arp, long), r, w));
		} else {
			if (r > 0)
				puts(pitoa((unsigned long)va_arg(arp, int), r, w));
			else
				puts(pitoa((long)va_arg(arp, int), r, w));
		}
	}

	va_end(arp);
}


void Vcom::bufferHex(uint8_t *buf, uint8_t len){
	putc('\n');
	while(len--){
		uint8_t data = *buf++;
		if(data < 16)
			putc('0');
		printf("%X ", data);
	}
	putc('\n');
}
