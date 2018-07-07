
#include <stdint.h>
#include <stdio.h>

#include "vcom.h"
#include "strfunctions.h"

// ---------------------------------------------------
// Fifo handling
// ---------------------------------------------------

void Vcom::fifo_init(void)
{
	txfifo.head = 0;
	txfifo.tail = 0;

    rxfifo.head = 0;
	rxfifo.tail = 0;
}


uint8_t Vcom::fifo_put(fifo_t *fifo, uint8_t c)
{
	int next;
	
	// check if FIFO has room
	next = (fifo->head + 1) % VCOM_FIFO_SIZE;
	if (next == fifo->tail) {
		// full
		return 0;
	}
	
	fifo->buf[fifo->head] = c;
	fifo->head = next;
	
	return 1;
}


uint8_t Vcom::fifo_get(fifo_t *fifo, uint8_t *pc)
{
	int next;
	
	// check if FIFO has data
	if (fifo->head == fifo->tail) {
		return 0;
	}
	
	next = (fifo->tail + 1) % VCOM_FIFO_SIZE;
	
	*pc = fifo->buf[fifo->tail];
	fifo->tail = next;

	return 1;
}

int Vcom::fifo_avail(fifo_t *fifo)
{
    return (VCOM_FIFO_SIZE + fifo->head - fifo->tail) % VCOM_FIFO_SIZE;
}


int Vcom::fifo_free(fifo_t *fifo)
{
	return (VCOM_FIFO_SIZE - 1 - fifo_avail(fifo));
}

void Vcom::fifo_flush(fifo_t *fifo)
{
	fifo->head = 0;
	fifo->tail = 0;
}

//-------------------------------------------
// Standard input output functions
//-------------------------------------------
/**
	Initialises the VCOM port.
	Call this function before using VCOM_puttchar or VCOM_getchar
 */
void Vcom::init(void)
{
	fifo_init();
}

void Vcom::flush(void)
{
	fifo_flush(&txfifo);
	fifo_flush(&rxfifo);
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

char Vcom::gets_echo(char *str)
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
				str--;
				s--;
			}
		}else{
			putc(c);
			*str++ = c;
			s++;
		}
		c = getc();
	}
	*str = '\0';
	return s;
}

void Vcom::printf (const char* str, ...)
{
	va_list arp;
	int d, r, w, s, l, f;


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
		/* if(d == 'f'){
			if(!f)
				w = 6;						// dafault 6 decimal places
			vftoa(this->putc, va_arg(arp, double), w);			
			continue;
		}	
		if (!r) break;
		if (s) w = -w;
		if (l) {
			//hitoa(putc, (long)va_arg(arp, long), r, w);
		} else {
			if (r > 0)
				hitoa(putc, (unsigned long)va_arg(arp, int), r, w);
			else
				hitoa(putc, (long)va_arg(arp, int), r, w);
		}	*/	
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
