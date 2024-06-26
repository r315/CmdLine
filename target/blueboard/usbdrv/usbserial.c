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

#include "type.h"
#include "usbstruct.h"
#include "usbapi.h"
#include "usbhw_lpc.h"
#include "debug.h"

#include "usbserial.h"

#include "fifo.h"

#define BAUD_RATE	    115200

#define INT_IN_EP01		0x81		// Control

#define BULK_IN_EP02	0x82		// Bulk
#define BULK_OUT_EP02	0x02		// Bulk

#define BULK_IN_EP05	0x85		// Bulk
#define BULK_OUT_EP05	0x05		// Bulk

#define BULK_IN_EP08	0x88		// Bulk

#define BULK_OUT_EP11	0x0B		// Bulk

#define MAX_PACKET_SIZE	64

#define LE_WORD(x)		((x)&0xFF),((x)>>8)

// CDC definitions
#define CS_INTERFACE			0x24
#define CS_ENDPOINT				0x25

#define	SET_LINE_CODING			0x20
#define	GET_LINE_CODING			0x21
#define	SET_CONTROL_LINE_STATE	0x22

#define VID                     0xB00B
#define PID                     0x0001

static TLineCoding LineCoding = {115200, 0, 0, 8};
static U8 abBulkBuf[64];
static U8 abClassReqData[8];

/** data storage area for standard requests */
static U8	abStdReqData[8];

static fifo_t *usb_txfifo, *usb_rxfifo;

static const U8 abDescriptors[] = {
#if 1
/***************************************************
 * Device descriptor (Bus pirate V4)
 ************************************************** */
	0x12,                       // bLength
	DESC_DEVICE,                // bDescriptorType
	LE_WORD(0x0101),			// bcdUSB
	0x02,						// bDeviceClass
	0x00,						// bDeviceSubClass
	0x00,						// bDeviceProtocol
	MAX_PACKET_SIZE0,	        // bMaxPacketSize
	LE_WORD(VID),               // idVendor
	LE_WORD(PID),               // idProduct
	LE_WORD(0x0100),			// bcdDevice
	0x01,						// iManufacturer
	0x02,						// iProduct
	0x03,						// iSerialNumber
	0x01,						// bNumConfigurations

/****************************************************
 * Configuration descriptor
 **************************************************** */
	0x09,
	DESC_CONFIGURATION,
	LE_WORD(9 + 58),	        // wTotalLength
	0x02,						// bNumInterfaces
	0x01,						// bConfigurationValue
	0x00,						// iConfiguration
	0xC0,						// bmAttributes
	0x32,						// bMaxPower

/****************************************************
 * Interface descriptor
 *************************************************** */ 
	0x09,                       // bLength
	DESC_INTERFACE,             // bDescriptorType
	0x00,						// bInterfaceNumber
	0x00,						// bAlternateSetting
	0x01,						// bNumEndPoints
	0x02,						// bInterfaceClass
	0x02,						// bInterfaceSubClass
	0x01,						// bInterfaceProtocol
	0x00,						// iInterface

// ------------ CDC Header ------------------
	0x05,
	CS_INTERFACE,
	0,
	LE_WORD(0x0110),            // bcdCDC

// ------------ CDC ACM ------------------	
	0x04,
	CS_INTERFACE,
	0x02,
	0x02,						// bmCapabilities

// -------------- CDC Union ------------------	
	0x05,
	CS_INTERFACE,
	0x06,
	0x00,						// bMasterInterface
	0x01,						// bSlaveInterface0
	
// ------------ CDC Call Management ------------------
	0x05,
	CS_INTERFACE,
	0x01,
	0x01,						// bmCapabilities
	0x01,					    // bDataInterface

// ------------ Endpoint descriptor ------------------
	0x07,
	DESC_ENDPOINT,
	INT_IN_EP01,	            // bEndpointAddress
	0x03,						// bmAttributes = intr
	LE_WORD(8),  				// wMaxPacketSize
	0x0A,						// bInterval

/****************************************************
 * Interface descriptor
 *************************************************** */ 
	0x09,                       // bLength
	DESC_INTERFACE,             // bDescriptorType
	0x01,						// bInterfaceNumber
	0x00,						// bAlternateSetting
	0x02,						// bNumEndPoints
	0x0A,						// bInterfaceClass
	0x00,						// bInterfaceSubClass
	0x00,						// bInterfaceProtocol, linux requires value of 1 for the cdc_acm module
	0x00,						// iInterface

// ------------ Endpoint descriptor Out ------------------
	0x07,
	DESC_ENDPOINT,
	BULK_OUT_EP05,				// bEndpointAddress
	0x02,						// bmAttributes = bulk
	LE_WORD(MAX_PACKET_SIZE),	// wMaxPacketSize
	0x40,						// bInterval

// ------------ Endpoint descriptor IN ------------------
	0x07,
	DESC_ENDPOINT,
	BULK_IN_EP02,			    // bEndpointAddress
	0x02,						// bmAttributes = bulk
	LE_WORD(MAX_PACKET_SIZE),	// wMaxPacketSize
	0x40,						// bInterval

/****************************************************
 * String descriptors
************************************************** */	
	// String Descriptor Zero
	0x04,
	DESC_STRING,
	LE_WORD(0x0409),

	// String Descriptors
    0x08,
    DESC_STRING,
    'M',0,'e',0,'h',0,
    
    0x12,
    DESC_STRING,
    'L',0,'P',0,'C',0,' ',0,'V',0,'C',0,'O',0,'M',0,
	
	0x12,
	DESC_STRING,
	'1',0,'2',0,'3',0,'4',0,'A',0,'B',0,'D',0,'C',0,

	0
};
#elif 1
/***************************************************
 * Device descriptor
 ************************************************** */
	0x12,                       // bLength
	DESC_DEVICE,                // bDescriptorType
	LE_WORD(0x0101),			// bcdUSB
	0x00,						// bDeviceClass
	0x00,						// bDeviceSubClass
	0x00,						// bDeviceProtocol
	MAX_PACKET_SIZE0,			// bMaxPacketSize
	LE_WORD(VID),               // idVendor
	LE_WORD(PID),               // idProduct
	LE_WORD(0x0100),			// bcdDevice
	0x01,						// iManufacturer
	0x02,						// iProduct
	0x03,						// iSerialNumber
	0x01,						// bNumConfigurations

/****************************************************
 * Configuration descriptor
 **************************************************** */
	0x09,
	DESC_CONFIGURATION,
	LE_WORD(9 + 35 + 23 + 23),	// wTotalLength
	0x03,						// bNumInterfaces
	0x01,						// bConfigurationValue
	0x00,						// iConfiguration
	0xC0,						// bmAttributes
	0x2D,						// bMaxPower

/****************************************************
 * Interface descriptor for CDC
 *************************************************** */ 
	0x09,                       // bLength
	DESC_INTERFACE,             // bDescriptorType
	0x00,						// bInterfaceNumber
	0x00,						// bAlternateSetting
	0x01,						// bNumEndPoints
	0x02,						// bInterfaceClass
	0x02,						// bInterfaceSubClass
	0x01,						// bInterfaceProtocol, linux requires value of 1 for the cdc_acm module
	0x05,						// iInterface

// header functional descriptor
	0x05,
	CS_INTERFACE,
	0x00,
	LE_WORD(0x0110),

// call management functional descriptor
	0x05,
	CS_INTERFACE,
	0x01,
	0x01,						// bmCapabilities = device handles call management
	0x01,						// bDataInterface
	
// ACM functional descriptor
	0x04,
	CS_INTERFACE,
	0x02,
	0x02,						// bmCapabilities

// union functional descriptor
	0x05,
	CS_INTERFACE,
	0x06,
	0x00,						// bMasterInterface
	//0x01,						// bSlaveInterface0
    0x02,						// bSlaveInterface1

// EndPoint Descriptor for Interrupt endpoint
	0x07,
	DESC_ENDPOINT,
	INT_IN_EP01,    			// bEndpointAddress
	0x03,						// bmAttributes = intr
	LE_WORD(8),					// wMaxPacketSize
	0x0A,						// bInterval

/*************************************************** 
* Data class interface descriptor
 *************************************************** */ 
	0x09,
	DESC_INTERFACE,
	0x01,						// bInterfaceNumber
	0x00,						// bAlternateSetting
	0x02,						// bNumEndPoints
	0x0A,						// bInterfaceClass = data
	0x00,						// bInterfaceSubClass
	0x00,						// bInterfaceProtocol
	0x00,						// iInterface

// EndPoint Descriptor for Output endpoint
	0x07,
	DESC_ENDPOINT,
	BULK_OUT_EP05,				// bEndpointAddress
	0x02,						// bmAttributes = bulk
	LE_WORD(MAX_PACKET_SIZE),	// wMaxPacketSize
	0x40,						// bInterval
// EndPoint Descriptor for Input endpoint
	0x07,
	DESC_ENDPOINT,
	BULK_IN_EP02,				// bEndpointAddress
	0x02,						// bmAttributes = bulk
	LE_WORD(MAX_PACKET_SIZE),	// wMaxPacketSize
	0x40,						// bInterval

/*************************************************** 
* Data class interface descriptor
 *************************************************** */ 
	0x09,
	DESC_INTERFACE,
	0x02,						// bInterfaceNumber
	0x00,						// bAlternateSetting
	0x02,						// bNumEndPoints
	0x0A,						// bInterfaceClass = data
	0x00,						// bInterfaceSubClass
	0x00,						// bInterfaceProtocol
	0x00,						// iInterface

// EndPoint Descriptor for Output endpoint
	0x07,
	DESC_ENDPOINT,
	BULK_OUT_EP11,				// bEndpointAddress
	0x02,						// bmAttributes = bulk
	LE_WORD(MAX_PACKET_SIZE),	// wMaxPacketSize
	0x40,						// bInterval
// EndPoint Descriptor for Input endpoint
	0x07,
	DESC_ENDPOINT,
	BULK_IN_EP08,				// bEndpointAddress
	0x02,						// bmAttributes = bulk
	LE_WORD(MAX_PACKET_SIZE),	// wMaxPacketSize
	0x40,						// bInterval

/****************************************************
 * String descriptors
************************************************** */	
	// String Descriptor Zero
	0x04,
	DESC_STRING,
	LE_WORD(0x0409),

	// String Descriptors
	0x0E,
	DESC_STRING,
	'L', 0, 'P', 0, 'C', 0, 'U', 0, 'S', 0, 'B', 0,

	0x14,
	DESC_STRING,
	'U', 0, 'S', 0, 'B', 0, 'S', 0, 'e', 0, 'r', 0, 'i', 0, 'a', 0, 'l', 0,
	
	0x12,
	DESC_STRING,
	'1', 0, '2', 0, '3', 0, '4', 0, '5', 0, '6', 0, '7', 0, '8', 0,

	34,
    DESC_STRING,
    'D',0,
	'u',0,
	'a',0,
	'l',0,
	' ',0,
	'S',0,
	'e',0,
	'r',0,
	'i',0,
	'a',0,
	'l',0,
	' ',0,
	'p',0,
	'o',0,
	'r',0,
	't',0,

	16,
    DESC_STRING,
    'S',0,
	'e',0,
	'r',0,
	'i',0,
	'a',0,
	'l',0,
	'1',0,

	16,
    DESC_STRING,
    'S',0,
	'e',0,
	'r',0,
	'i',0,
	'a',0,
	'l',0,
	'2',0,

	12,
    DESC_STRING,
    'S',0,
	'T',0,
	'R',0,
	'0',0,
	'2',0,

	0
};

#else
/***************************************************
 * Device descriptor
 ************************************************** */
	0x12,                       // bLength
	DESC_DEVICE,                // bDescriptorType
	LE_WORD(0x0101),			// bcdUSB
	0x02,						// bDeviceClass
	0x00,						// bDeviceSubClass
	0x00,						// bDeviceProtocol
	MAX_PACKET_SIZE0,			// bMaxPacketSize
	LE_WORD(VID),               // idVendor
	LE_WORD(PID),               // idProduct
	LE_WORD(0x0100),			// bcdDevice
	0x01,						// iManufacturer
	0x02,						// iProduct
	0x03,						// iSerialNumber
	0x01,						// bNumConfigurations

/****************************************************
 * Configuration descriptor one
 **************************************************** */
	0x09,
	DESC_CONFIGURATION,
	LE_WORD(67),				// wTotalLength
	0x02,						// bNumInterfaces
	0x01,						// bConfigurationValue
	0x00,						// iConfiguration
	0xC0,						// bmAttributes
	0x32,						// bMaxPower

/****************************************************
 * Interface descriptor for CDC
 *************************************************** */ 
	0x09,                       // bLength
	DESC_INTERFACE,             // bDescriptorType
	0x00,						// bInterfaceNumber
	0x00,						// bAlternateSetting
	0x01,						// bNumEndPoints
	0x02,						// bInterfaceClass
	0x02,						// bInterfaceSubClass
	0x01,						// bInterfaceProtocol, linux requires value of 1 for the cdc_acm module
	0x05,						// iInterface

// header functional descriptor
	0x05,
	CS_INTERFACE,
	0x00,
	LE_WORD(0x0110),

// call management functional descriptor
	0x05,
	CS_INTERFACE,
	0x01,
	0x01,						// bmCapabilities = device handles call management
	0x01,						// bDataInterface
	
// ACM functional descriptor
	0x04,
	CS_INTERFACE,
	0x02,
	0x02,						// bmCapabilities

// union functional descriptor
	0x05,
	CS_INTERFACE,
	0x06,
	0x00,						// bMasterInterface
	0x01,						// bSlaveInterface0

// EndPoint Descriptor for Interrupt endpoint
	0x07,
	DESC_ENDPOINT,
	INT_IN_EP,					// bEndpointAddress
	0x03,						// bmAttributes = intr
	LE_WORD(8),					// wMaxPacketSize
	0x0A,						// bInterval

// Data class interface descriptor
	0x09,
	DESC_INTERFACE,
	0x01,						// bInterfaceNumber
	0x00,						// bAlternateSetting
	0x02,						// bNumEndPoints
	0x0A,						// bInterfaceClass = data
	0x00,						// bInterfaceSubClass
	0x00,						// bInterfaceProtocol
	0x00,						// iInterface


// EndPoint Descriptor for Output endpoint
	0x07,
	DESC_ENDPOINT,
	BULK_OUT_EP,				// bEndpointAddress
	0x02,						// bmAttributes = bulk
	LE_WORD(MAX_PACKET_SIZE),	// wMaxPacketSize
	0x00,						// bInterval
// EndPoint Descriptor for Input endpoint
	0x07,
	DESC_ENDPOINT,
	BULK_IN_EP,					// bEndpointAddress
	0x02,						// bmAttributes = bulk
	LE_WORD(MAX_PACKET_SIZE),	// wMaxPacketSize
	0x00,						// bInterval

/****************************************************
 * String descriptors
************************************************** */	
	// String Descriptor Zero
	0x04,
	DESC_STRING,
	LE_WORD(0x0409),

	// String Descriptors
	0x0E,
	DESC_STRING,
	'L', 0, 'P', 0, 'C', 0, 'U', 0, 'S', 0, 'B', 0,

	0x14,
	DESC_STRING,
	'U', 0, 'S', 0, 'B', 0, 'S', 0, 'e', 0, 'r', 0, 'i', 0, 'a', 0, 'l', 0,

	0x12,
	DESC_STRING,
	'1', 0, '2', 0, '3', 0, '4', 0, '5', 0, '6', 0, '7', 0, '8', 0,

	34,
    DESC_STRING,
    'D',0,
	'u',0,
	'a',0,
	'l',0,
	' ',0,
	'S',0,
	'e',0,
	'r',0,
	'i',0,
	'a',0,
	'l',0,
	' ',0,
	'p',0,
	'o',0,
	'r',0,
	't',0,

	16,
    DESC_STRING,
    'S',0,
	'e',0,
	'r',0,
	'i',0,
	'a',0,
	'l',0,
	'1',0,

	16,
    DESC_STRING,
    'S',0,
	'e',0,
	'r',0,
	'i',0,
	'a',0,
	'l',0,
	'2',0,

	12,
    DESC_STRING,
    'S',0,
	'T',0,
	'R',0,
	'0',0,
	'2',0,

// terminating zero
	0
};

#endif
/**
	Interrupt handler
	
	Simply calls the USB ISR
 */
void USB_IRQHandler(void)
{
	USBHwISR();
}


/**
	Local function to handle incoming bulk data
		
	@param [in] bEP
	@param [in] bEPStatus
 */
static void USBSERIAL_BulkOut(U8 bEP, U8 bEPStatus)
{
	int i, iLen;
	bEPStatus = bEPStatus;
	if (fifo_free(usb_rxfifo) < MAX_PACKET_SIZE) {
		// may not fit into fifo
		return;
	}

	// get data from USB into intermediate buffer
	iLen = USBHwEPRead(bEP, abBulkBuf, sizeof(abBulkBuf));
	for (i = 0; i < iLen; i++) {
		// put into FIFO
		if (!fifo_put(usb_rxfifo, abBulkBuf[i])) {
			// overflow... :(
			ASSERT(FALSE);
			break;
		}
	}
}


/**
	Local function to handle outgoing bulk data
		
	@param [in] bEP
	@param [in] bEPStatus
 */
static void USBSERIAL_BulkIn(U8 bEP, U8 bEPStatus)
{
	int i, iLen;
	bEPStatus = bEPStatus;
	if (fifo_avail(usb_txfifo) == 0) {
		// no more data, disable further NAK interrupts until next USB frame
		USBHwNakIntEnable(0);
		return;
	}

	// get bytes from transmit FIFO into intermediate buffer
	for (i = 0; i < MAX_PACKET_SIZE; i++) {
		if (!fifo_get(usb_txfifo, &abBulkBuf[i])) {
			break;
		}
	}
	iLen = i;
	
	// send over USB
	if (iLen > 0) {
		USBHwEPWrite(bEP, abBulkBuf, iLen);
	}
}

#if 0
/**
	Local function to handle incoming bulk data
		
	@param [in] bEP
	@param [in] bEPStatus
 */
static void USBSERIAL_BulkOut_2(U8 bEP, U8 bEPStatus){
	//int iLen;
	//iLen = USBHwEPRead(bEP, abBulkBuf, sizeof(abBulkBuf));
    USBSERIAL_BulkOut(bEP, bEPStatus);
}


/**
	Local function to handle outgoing bulk data
		
	@param [in] bEP
	@param [in] bEPStatus
 */
static void USBSERIAL_BulkIn_2(U8 bEP, U8 bEPStatus){
    USBSERIAL_BulkIn(bEP, bEPStatus);
}
#endif

/**
	Local function to handle the USB-CDC class requests
		
	@param [in] pSetup
	@param [out] piLen
	@param [out] ppbData
 */
static BOOL USBSERIAL_HandleClassRequest(TSetupPacket *pSetup, int *piLen, U8 **ppbData)
{
	int i;
	switch (pSetup->bRequest) {

	// set line coding
	case SET_LINE_CODING:

		//memcpy((U8 *)&LineCoding, *ppbData, 7);
		*piLen = 7;
		for(i=0;i<7;i++)
			((U8 *)&LineCoding)[i] = (*ppbData)[i];

		break;

	// get line coding
	case GET_LINE_CODING:

		*ppbData = (U8 *)&LineCoding;
		*piLen = 7;
		break;

	// set control line state
	case SET_CONTROL_LINE_STATE:
		// bit0 = DTR, bit = RTS

		break;

	default:
		return FALSE;
	}
	return TRUE;
}

static void USBSERIAL_FrameHandler(U16 wFrame)
{
	wFrame = wFrame;
	if (fifo_avail(usb_txfifo) > 0) {
		// data available, enable NAK interrupt on bulk in
		USBHwNakIntEnable(INACK_BI);
	}
}

/**
	USB reset handler
	
	@param [in] bDevStatus	Device status
 */
static void USBSERIAL_HandleUsbReset(U8 bDevStatus)
{
	if (bDevStatus & DEV_STATUS_RESET) {
		DEBUG_OUT("\n!");
	}
}

/*************************************************************************

**************************************************************************/
void USBSERIAL_Init(fifo_t *tx, fifo_t *rx)
{
	// initialise stack
	usb_rxfifo = rx;
	usb_txfifo = tx;
	
	// init hardware
	USBHwInit();
	
	// register bus reset handler
	USBHwRegisterDevIntHandler(USBSERIAL_HandleUsbReset);
	
	// register control transfer handler on EP0
	USBHwRegisterEPIntHandler(0x00, USBHandleControlTransfer);
	USBHwRegisterEPIntHandler(0x80, USBHandleControlTransfer);
	
	// setup control endpoints
	USBHwEPConfig(0x00, MAX_PACKET_SIZE0);
	USBHwEPConfig(0x80, MAX_PACKET_SIZE0);
	
	// register standard request handler
	USBRegisterRequestHandler(REQTYPE_TYPE_STANDARD, USBHandleStandardRequest, abStdReqData);

	// initialise endpoints
	// register descriptors
	USBRegisterDescriptors(abDescriptors);

	// register class request handler
	USBRegisterRequestHandler(REQTYPE_TYPE_CLASS, USBSERIAL_HandleClassRequest, abClassReqData);

	// register endpoint handlers
	USBHwRegisterEPIntHandler(INT_IN_EP01, NULL);
	USBHwRegisterEPIntHandler(BULK_IN_EP02, USBSERIAL_BulkIn);
	USBHwRegisterEPIntHandler(BULK_OUT_EP05, USBSERIAL_BulkOut);
	//USBHwRegisterEPIntHandler(BULK_IN_EP08, USBSERIAL_BulkIn_2);
	//USBHwRegisterEPIntHandler(BULK_OUT_EP11, USBSERIAL_BulkOut_2);
	
	// register frame handler
	USBHwRegisterFrameHandler(USBSERIAL_FrameHandler);

	// enable bulk-in interrupts on NAKs
	USBHwNakIntEnable(INACK_BI);
	
	NVIC_EnableIRQ(USB_IRQn); 
		
	// connect to bus
	USBHwConnect(TRUE);
}

