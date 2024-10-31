//========= Copyright  1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================
// Primary header for engine
#if !defined( QUAKEDEF_H )
#define QUAKEDEF_H
#ifdef _WIN32
#pragma once
#endif

// OPTIONAL DEFINES
//define	PARANOID			// speed sapping error checking

#include <qlimits.h>

// Max length of com_token
#define COM_TOKEN_MAX_LENGTH	2048

#define	MAX_STYLESTRING		64
#define MAX_CONSISTENCY		512
#define	MAX_SFX				1024

// Client dispatch function for usermessages
typedef int (*pfnUserMsgHook)( const char* pszName, int iSize, void* pbuf );

#define CACHE_USER
typedef struct cache_user_s
{
	void* data;
} cache_user_t;

//
// stats are integers communicated to the client by the server
//
#define	MAX_CL_STATS		32
#define	STAT_HEALTH			0
//define	STAT_FRAGS			1
#define	STAT_WEAPON			2
#define	STAT_AMMO			3
#define	STAT_ARMOR			4
//define	STAT_WEAPONFRAME	5
#define	STAT_SHELLS			6
#define	STAT_NAILS			7
#define	STAT_ROCKETS		8
#define	STAT_CELLS			9
#define	STAT_ACTIVEWEAPON	10
#define	STAT_TOTALSECRETS	11
#define	STAT_TOTALMONSTERS	12
#define	STAT_SECRETS		13		// bumped on client side by svc_foundsecret
#define	STAT_MONSTERS		14		// bumped by svc_killedmonster
#define	STAT_ITEMS			15
//define	STAT_VIEWHEIGHT		16

// OPTIONAL DEFINES
//#define	PARANOID		// speed sapping error checking
#include "tier0/commonmacros.h"
#include "dbg/dbg.h"

#ifdef USECRTMEMDEBUG
#include "crtmemdebug.h"
#endif

// C Standard Library Includes
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

// Include SDL2 stuff
#include <SDL2/SDL.h>


//===========================================
// FIXME, remove more of these?
#include "port.h"
#include "mathlib.h"
#include "const.h"
#include "zone.h"
#include "common.h"
#include "mem.h"
#include "eiface.h"
#include "sys.h"
#include "cmd.h"
#include "entity_state.h"
#include "cl_entity.h"

#include <tier0/wchartypes.h>
#include <unicode_strtools.h>

#ifdef GLQUAKE
#include "gl_model.h"
#else
#include "model.h"
#include "d_iface.h"
#endif

#include "protocol.h"
#include "view.h"
#include "cdll_int.h"
#include "cvar.h"
#include "info.h"
#include "console.h"
#include "save.h"
#include "sound.h"
#include "sv_log.h"
#include "render.h"
#include "client.h"
#include "host_cmd.h"
#include "world.h"

#ifdef GLQUAKE
#include "qgl.h"
#include "glquake.h"
#endif

#include "host.h"
#include "filesystem_internal.h"

//=============================================================================



extern qboolean noclip_anglehack;


//
// host
// FIXME, move all this crap somewhere elase
//

#ifdef __cplusplus
extern "C" {
#endif

extern	cvar_t		host_framerate;
extern	cvar_t		host_limitlocal;
extern	cvar_t		pausable;

extern	cvar_t		sys_ticrate;
extern	cvar_t		sys_timescale;
extern	cvar_t		developer;

extern	qboolean	host_initialized;		// true if into command execution
extern	double		host_frametime;
extern	unsigned short* host_basepal;
extern	unsigned char*	host_colormap;
extern	int			host_framecount;	// incremented every frame, never reset
extern	double		realtime;			// not bounded in any way, changed at
										// start of every frame, never reset
extern	qboolean	gfNoMasterServer;

extern	qboolean	g_bIsDedicatedServer;

extern int			lowshift;
extern double		pfreq;

#ifdef __cplusplus
}
#endif

void Host_InitCommands( void );
void Host_Quit_f( void );

#ifdef __cplusplus
extern "C" {
#endif

int Host_Init( quakeparms_t* parms );
void Host_Shutdown( void );
void Host_Error( char* error, ... );
void Host_EndGame( char* message, ... );
int Host_Frame( float time, int iState, int* stateInfo );
void Host_ClientCommands( char* fmt, ... );
void Host_ShutdownServer( qboolean crash );

#ifdef __cplusplus
}
#endif

// Max user message data size
#define MAX_USER_MSG_DATA	192

// build info
// day counter from 10/24/96
extern int build_number( void );

extern int			minimum_memory;

//
// chase
//
extern	cvar_t	chase_active;

void Chase_Init( void );
void Chase_Reset( void );
void Chase_Update( void );

#undef LittleLong
#undef LittleShort
#undef LittleFloat
#undef LittleDword
#undef BigLong
#undef BigShort
#undef BigFloat
#undef BigDword

#endif // QUAKEDEF_H