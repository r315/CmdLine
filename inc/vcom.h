//#define POLLED_USBSERIAL TRUE


/*
	LPCUSB, an USB device driver for LPC microcontrollers	
	Copyright (C) 2006 Bertrik Sikken (bertrik@sikken.nl)

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright
	   notice, this list of conditions and the following disclaimer.
	2. Redistributions in binary form must reproduce the above copyright
	   notice, this list of conditions and the following disclaimer in the
	   documentation and/or other materials provided with the distribution.
	3. The name of the author may not be used to endorse or promote products
	   derived from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
	IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
	OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, 
	INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
	NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
	THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
	Minimal implementation of a USB serial port, using the CDC class.
	This example application simply echoes everything it receives right back
	to the host.

	Windows:
	Extract the usbser.sys file from .cab file in C:\WINDOWS\Driver Cache\i386
	and store it somewhere (C:\temp is a good place) along with the usbser.inf
	file. Then plug in the LPC176x and direct windows to the usbser driver.
	Windows then creates an extra COMx port that you can open in a terminal
	program, like hyperterminal.

	Linux:
	The device should be recognised automatically by the cdc_acm driver,
	which creates a /dev/ttyACMx device file that acts just like a regular
	serial port.

*/

/* Modified by Sagar G V, Feb 2011
Used the USB CDC example to create a library. Added the following functions

void VCOM_puts(const char* str); //writes a null terminated string. 
void VCOM_putc(char c); // writes a character.
void VCOM_putHex(uint8_t hex); // writes 0x.. hex value on the terminal.
char VCOM_getc(); // returns character entered in the terminal. blocking function
void VCOM_gets(char* str); // returns a string. '\r' or '\n' will terminate character collection.
char VCOM_getc_echo(); // returns character entered and echoes the same back.
void VCOM_gets_echo(char *str); // gets string terminated in '\r' or '\n' and echoes back the same.

*/

#ifndef __vcom_h__
#define __vcom_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdint.h>
//#include "usbapi.h"
//#include "usbdebug.h"

#define VCOM_FIFO_SIZE	512

typedef struct {
	int	    head;
	int     tail;
	uint8_t	buf[VCOM_FIFO_SIZE];
} fifo_t;


class Vcom{

public:
    fifo_t txfifo;
    fifo_t rxfifo;

	void flush(void);
	void fifo_init(void);	
	void fifo_flush(fifo_t *fifo);

   /**
	Reads one character from VCOM port
	
	@returns character read, or EOF if character could not be read
 */

	void init(void);
	int getchar(void);
	void puts(const char* str); //writes a null terminated string. 
	void putc(char c); // writes a character.
	void putHex(uint8_t hex); // writes 0x.. hex value on the terminal.
	char getc(); // returns character entered in the terminal. blocking function
	void gets(char* str); // returns a string. '\r' or '\n' will terminate character collection.
	char getc_echo(); // returns character entered and echoes the same back.
	char gets_echo(char *str); // gets string terminated in '\r' or '\n' and echoes back the same, return sizeof string.
	void printf(const char* str, ...); // Original code by Elm_CHaN. Modified by Martin Thomas
	void bufferHex(uint8_t *buf, uint8_t len);
	uint8_t fifo_put(fifo_t *fifo, uint8_t c);
	uint8_t fifo_get(fifo_t *fifo, uint8_t *pc);
	int  fifo_avail(fifo_t *fifo);
	int	 fifo_free(fifo_t *fifo);

	/*
		Writes one character to VCOM port
		
		@param [in] c character to write
		@returns character written, or EOF if character could not be written
	*/
	int putchar(int c);
};


int usbSerialInit(); // run once in main b4 main loop starts.


#ifdef __cplusplus
}
#endif

#endif