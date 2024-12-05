// quakedef.h -- primary header for client
#if !defined( QUAKEDEF_H )
#define QUAKEDEF_H
#ifdef _WIN32
#pragma once
#endif

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



#define	MINIMUM_MEMORY			0x550000
#define	MINIMUM_MEMORY_LEVELPAK	(MINIMUM_MEMORY + 0x100000)

#define MAX_NUM_ARGVS	50

// up / down
#define	PITCH	0

// left / right
#define	YAW		1

// fall over
#define	ROLL	2


#define	MAX_QPATH		64			// max length of a quake game pathname
#define	MAX_OSPATH		128			// max length of a filesystem pathname


#define	MAX_SFX				512

// Client dispatch function for usermessages
typedef int (*pfnUserMsgHook)( const char* pszName, int iSize, void* pbuf );





#include "platform.h"
#include "bothdefs.h"
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
#include "sbar.h"
#include "sound.h"
#include "render.h"
#include "client.h"
#include "progs.h"
#include "server.h"
#include "host_cmd.h"

#ifdef GLQUAKE
#include "gl_model.h"
#else
//#include "model.h"
//#include "d_iface.h"
#endif

#include "input.h"

#include "keys.h"
#include "console.h"
#include "view.h"
#include "crc.h"



#ifdef GLQUAKE
#include "qgl.h"
#include "glquake.h"
#endif



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




//
// host
//
extern	quakeparms_t host_parms;

extern	cvar_t		developer;

extern	qboolean	host_initialized;		// true if into command execution
extern	double		host_frametime;



extern	int			host_framecount;	// incremented every frame, never reset
extern	double		realtime;			// not bounded in any way, changed at
// start of every frame, never reset





int Host_Init( quakeparms_t* parms );
DLL_EXPORT void Host_Shutdown( void );


void Master_Init( void );


extern qboolean		isDedicated;

extern int			minimum_memory;

//
// chase
//
extern	cvar_t	chase_active;

void Chase_Init( void );
void Chase_Reset( void );

#endif // QUAKEDEF_H