/**
 LD script for flash and ram 19/02
**/ 

ENTRY(Reset_Handler)

MEMORY {
	flash (rx) : ORIGIN = 0x00000000, LENGTH = 512k
	ram0 (rwx) : ORIGIN = 0x10000000, LENGTH = 32k
	ram1 (rwx) : ORIGIN = 0x2007C000, LENGTH = 16k
    ram2 (rwx) : ORIGIN = 0x20080000, LENGTH = 16k
}

/* STACK_SIZE = DEFINED(__stack_size__) ? __stack_size__ : 16k; */

_ram0  = ORIGIN(ram0);
_ram1  = ORIGIN(ram1);
_stack = ORIGIN(ram2) + LENGTH(ram2);

SECTIONS {
	.text : 
	{
		KEEP(*(.startup .startup.*))
		*(.text)
		*(.text*)		
		. = ALIGN(4);
		_etext = .;
		_stext = LOADADDR(.text);
		_estartup = _stext;
	} > flash

	.rodata :
  	{
		. = ALIGN(4);
		*(.rodata)         /* .rodata sections (constants, strings, etc.) */
		*(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
		. = ALIGN(4);
	} > flash
	
	.data : 
	{
		. = ALIGN(4);
		_sidata = LOADADDR (.data); /* get the address where data content is stored on flash*/
		_sdata = .;			        /* get the actual data address on ram */
		*(.data)
		*(.data.*)				
		. = ALIGN(4);
		_edata = .;
	} > ram0 AT > flash  /* instructs that the content of data section is stored no flash */
    
	.bss : 
	{
		. = ALIGN(4);		
		_sbss = .;
		__bss_start__ = .;
		*(.bss)
		*(.bss.*)		
		. = ALIGN(4);
		_ebss = .;
		__bss_end__ = .;
		. = ALIGN(8);
		_end = .;        /* used for heap management */
	} > ram0

	/*section for libc exceptions and stack trace */
	.ARM.exidx ALIGN(4) : {		 /* understand this...., required for cpp*/
		__exidx_start = LOADADDR(.ARM.exidx);
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)		
		__exidx_end = .;
	} > ram0

    .data1 : 			/* functions that will run on ram */
	{
		. = ALIGN(4);
		_siramcode = LOADADDR(.data1);
		_sramcode = .;
        *(.data1 .data1.*);
		. = ALIGN(4);
		_eramcode = .;
	} > ram1 AT > flash

    .data2 : 
    {
		. = ALIGN(4);
        _sdata2 = .;
        *(.data2 .data2.*);
		_edata2 = .;	
	} > ram2
	
/*		
	.stack (NOLOAD) : 
    {
		. = ALIGN(8);
        _sstack = .;
		. = . + STACK_SIZE;
		. = ALIGN(8);	
		_sstack = .;	
	} > ram2
*/
}
