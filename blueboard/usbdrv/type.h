/*****************************************************************************
 *   type.h:  Type definition Header file for NXP LPC17xx Family 
 *   Microprocessors
 *
 *   Copyright(C) 2008, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2008.08.21  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#ifndef __TYPE_H__
#define __TYPE_H__

#include <stdint.h>

#ifndef NULL
#define NULL    ((void *)0)
#endif

#ifndef FALSE
#define FALSE   (0)
#endif

#ifndef TRUE
#define TRUE    (1)
#endif

typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef unsigned int   BOOL;

typedef unsigned char		U8;		/**< unsigned 8-bit */
typedef unsigned short int	U16;	/**< unsigned 16-bit */
typedef unsigned int		U32;	/**< unsigned 32-bit */

typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus;
typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;

/* Pointer to Function returning Void (any number of parameters) */
typedef void (*PFV)();

#define MIN(x,y)	((x)<(y)?(x):(y))	/**< MIN */
#define MAX(x,y)	((x)>(y)?(x):(y))	/**< MAX */

#endif  /* __TYPE_H__ */
