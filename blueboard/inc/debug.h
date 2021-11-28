#include <stdio.h>
#include "board.h"


#ifdef DEBUG
	#define DEBUG_OUT(x ...) // TODO: implement
   #define ASSERT(x) if(!(x)){DEBUG_OUT("\nAssertion '%s' failed in %s:%s#%d!\n",#x,__FILE__,__FUNCTION__,__LINE__);while(1);}
#else
   #define DEBUG_OUT(x ...)
   #define ASSERT(x)
#endif

