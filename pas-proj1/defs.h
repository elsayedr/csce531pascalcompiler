#ifndef DEFS_H
#define DEFS_H

/****************************************************************/
/*								*/
/*	CSCE531 - "Pascal" Compiler				*/
/*								*/
/*	--defs.h--						*/
/*								*/
/*	This file contains general definitions for csce531 	*/
/*	Pascal compiler.	       				*/
/*								*/
/*								*/
/*								*/
/****************************************************************/

typedef int BOOLEAN;

#define TRUE 1
#define FALSE 0
#define debug 1

#ifndef NULL
#define NULL 0
#endif

typedef void * ST_ID;	/* symbol table identifier abstraction */

/* Remove comments to generate a parser trace: */
/* #define YYDEBUG 1 */

#endif
