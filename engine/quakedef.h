// quakedef.h -- primary header for client

#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>





#include "common.h"
#include "sys.h"






//=============================================================================

// the host system specifies the base of the directory tree, the
// command line parms passed to the program, and the amount of memory
// available for the program to use

typedef struct
{
	char* basedir;
	char* cachedir;		// for development over ISDN lines
	int		argc;
	char** argv;
	void* membase;
	int		memsize;
} quakeparms_t;


//=============================================================================