
/* Includes */
#include <errno.h>
#include <stdio.h>


extern int errno;
register char *stack_ptr asm("sp");

caddr_t _sbrk(int incr)
{
	extern char _end asm("_end");
	static char *heap_end;
	char *prev_heap_end;

	if (heap_end == 0)
		heap_end = &_end;

	prev_heap_end = heap_end;
	if (heap_end + incr > stack_ptr)
	{
		errno = ENOMEM;
		return (caddr_t) -1;
	}

	heap_end += incr;

	return (caddr_t) prev_heap_end;
}

