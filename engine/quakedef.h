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

#if defined(_M_IX86)
#define __i386__	1
#endif

#if defined( _WIN32 )

// Used for dll exporting and importing
#define DLL_EXPORT				C_EXTERN __declspec( dllexport )
#define DLL_IMPORT				C_EXTERN __declspec( dllimport )

#endif

#if defined __i386__ // && !defined __sun__
#define id386	1
#else
#define id386	0
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

// SYSTEM INFO
#define	MAX_QPATH		64			// max length of a quake game pathname
#define	MAX_OSPATH		128			// max length of a filesystem pathname

#define ON_EPSILON		0.1			// point on plane side epsilon

#define MAX_MSGLEN		7500		// max length of a reliable message
#define MAX_DATAGRAM	4000		// max length of unreliable message
#define MAX_MULTICAST	1024		// max length of a message sent to all clients


#define MAX_LIGHTSTYLE_INDEX_BITS	6
#define MAX_LIGHTSTYLES				(1<<MAX_LIGHTSTYLE_INDEX_BITS)



// Resource counts
#define MAX_MODEL_INDEX_BITS		9	// sent as a short
#define MAX_MODELS					(1<<MAX_MODEL_INDEX_BITS)
#define MAX_SOUND_INDEX_BITS		9
#define MAX_SOUNDS					(1<<MAX_SOUND_INDEX_BITS)





#define MAX_BASE_DECALS_INDEX_BITS	9
#define MAX_BASE_DECALS				(1<<MAX_BASE_DECALS_INDEX_BITS)


#define	MAX_SFX				512

#define MAX_RESOURCES		1280 // TODO: (MAX_MODELS + MAX_SOUNDS + MAX_EVENTS)


#define MAX_USERMSGS		128

#define MAX_BEAMENTS_INDEX_BITS	6
#define MAX_BEAMENTS			(1<<MAX_BEAMENTS_INDEX_BITS)

// Client dispatch function for usermessages
typedef int (*pfnUserMsgHook)( const char* pszName, int iSize, void* pbuf );
pfnUserMsgHook HookServerMsg( const char* pszName, pfnUserMsgHook pfn );

int DispatchDirectUserMsg( const char* pszName, int iSize, void* pBuf );

typedef struct _UserMsg UserMsg;

typedef struct _UserMsg
{
	int				iMsg;
	// byte size of message, or -1 for variable sized
	int				iSize;
	char			szName[12];
	UserMsg*		next;
	// Client only dispatch function for message
	pfnUserMsgHook	pfn;
} UserMsg;

// Max user message data size
#define MAX_USER_MSG_DATA	64


#define	MAX_STYLESTRING	64

//
// stats are integers communicated to the client by the server
//
#define	MAX_CL_STATS		32
#define	STAT_HEALTH			0
//define	STAT_FRAGS			1
#define	STAT_WEAPON			2



#include "platform.h"
#include "bothdefs.h"
#include "mathlib.h"
#include "const.h"
#include "progs.h"
#include "common.h"
#include "modelgen.h"
#include "bspfile.h"
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
#include "world.h"
#include "keys.h"
#include "console.h"
#include "save.h"
#include "view.h"
#include "crc.h"
#include "beamdef.h"

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



extern qboolean noclip_anglehack;


//
// host
//
extern	quakeparms_t host_parms;

extern	cvar_t		developer;

extern	qboolean	host_initialized;		// true if into command execution
extern	double		host_frametime;
extern	unsigned short* host_basepal;
extern	unsigned char*	host_colormap;
extern	int			host_framecount;	// incremented every frame, never reset
extern	double		realtime;			// not bounded in any way, changed at
										// start of every frame, never reset

// Master server
extern	qboolean	gfNoMasterServer;
extern	double		gfLastHearbeat;
extern	qboolean	gfHeartbeatWaiting;
extern	float		gfHeartbeatWaitingTime;
extern	int			gHeartbeatSequence;
extern	int			gHeartbeatChallenge;
extern	char		gszMasterAddress[128];
extern	netadr_t	gMasterAddress;

void Master_Heartbeat( void );
void Master_Shutdown( void );
void Master_RequestMOTD_f( void );

extern	char		gszDefaultRoom[64];

extern	cvar_t		host_speeds;
// start of every frame, never reset


extern cvar_t		rcon_password;
extern cvar_t		rcon_address;
extern cvar_t		rcon_port;

void Host_ClearMemory( qboolean bQuiet );

void Host_InitCommands( void );
int Host_Init( quakeparms_t* parms );
DLL_EXPORT void Host_Shutdown( void );
void Host_Error( char* error, ... );
void Host_EndGame( char* message, ... );
void Host_ClientCommands( char* fmt, ... );
void Host_Quit_f( void );
void Host_ShutdownServer( qboolean crash );
void Host_DeallocateDynamicData( void );
void Host_ReallocateDynamicData( void );

void Master_Init( void );


extern qboolean		isDedicated;

extern int			minimum_memory;

//
// chase
//
extern	cvar_t	chase_active;

void Chase_Init( void );
void Chase_Reset( void );
void Chase_Update( void );

#endif // QUAKEDEF_H