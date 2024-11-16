// quakedef.h -- primary header for client

#ifdef _WIN32
#pragma warning( disable : 4244 4127 4201 4214 4514 4305 4115 4018)
#endif

#define	GAMENAME	"valve"

#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <time.h>

#ifdef __cplusplus
#define C_EXTERN extern "C"
#else
#define C_EXTERN extern
#endif

#ifdef __cplusplus
#define DECLTYPE(func) (decltype(func))
#else
#define DECLTYPE(func) (void*)
#endif

#if defined( _WIN32 )

// Used for dll exporting and importing
#define DLL_EXPORT				C_EXTERN __declspec( dllexport )
#define DLL_IMPORT				C_EXTERN __declspec( dllimport )

#endif




#define	MAX_QPATH		64			// max length of a quake game pathname
#define	MAX_OSPATH		128			// max length of a filesystem pathname


#define	MAX_SFX				512




#include "platform.h"
#include "mathlib.h"
#include "const.h"
#include "progs.h"
#include "common.h"
#include "dll_state.h"
#include "eiface.h"
#include "pr_dlls.h"
#include "color.h"
#include "vid.h"
#include "sys.h"
#include "zone.h"








#include "wad.h"
#include "draw.h"
#include "cvar.h"
#include "screen.h"
#include "net.h"
#include "protocol.h"
#include "cmd.h"


#include "client.h"
#include "progs.h"
#include "server.h"






#include "keys.h"
#include "console.h"

#include "crc.h"





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




//
// host
//
extern	quakeparms_t host_parms;

extern	cvar_t		developer;

extern	qboolean	host_initialized;		// true if into command execution




int Host_Init( quakeparms_t* parms );
void Host_Shutdown( void );