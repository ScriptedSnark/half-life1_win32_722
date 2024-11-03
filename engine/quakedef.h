// quakedef.h -- primary header for client

#ifdef _WIN32
#pragma warning( disable : 4244 4127 4201 4214 4514 4305 4115 4018)
#endif

#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>





#include "common.h"
#include "dll_state.h"
#include "sys.h"











#include "server.h"







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


//=============================================================================

#define MAX_NUM_ARGVS	50












void Host_Shutdown( void );