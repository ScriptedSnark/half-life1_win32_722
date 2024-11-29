// host.c -- coordinates spawning and killing of local servers

#include "quakedef.h"

/*

A server can allways be started, even if the system started out as a client
to a remote system.

A client can NOT be started if the system started as a dedicated server.

Memory is cleared / released when a server or client begins, not when they end.

*/

quakeparms_t host_parms;

qboolean	host_initialized;		// true if into command execution

double		host_frametime;
double		realtime;			// without any filtering or bounding
double		oldrealtime;		// last frame run



int			minimum_memory;



jmp_buf 	host_abortserver;


// TODO: Implement


cvar_t	developer = { "developer", "0" };


// TODO: Implement


/*
=================
SV_BroadcastPrintf

Sends text to all active clients
=================
*/
void SV_BroadcastPrintf( char* fmt, ... )
{
	// TODO: Implement
}


// TODO: Implement


/*
====================
Host_Init
====================
*/
int Host_Init( quakeparms_t* parms )
{
	if (standard_quake)
		minimum_memory = MINIMUM_MEMORY;
	else
		minimum_memory = MINIMUM_MEMORY_LEVELPAK;

	if (COM_CheckParm("-minmemory"))
		parms->memsize = minimum_memory;

	host_parms = *parms;

	if (parms->memsize < minimum_memory)
		Sys_Error("Only %4.1f megs of memory available, can't execute game", parms->memsize / (float)0x100000);

	com_argc = parms->argc;
	com_argv = parms->argv;

	Master_Init();

	realtime = 0.0;

	Memory_Init(parms->membase, parms->memsize);
	Cbuf_Init();
	Cmd_Init();
	Cvar_CmdInit();
//	V_Init(); TODO: Implement
//	Chase_Init(); TODO: Implement
	COM_Init(parms->basedir);
//	Host_InitLocal(); TODO: Implement
//	W_LoadWadFile("gfx.wad"); TODO: Implement
	Key_Init();
	Con_Init();

	// TODO: Implement



//	Sys_Error("Host_Init: Not implemented yet");

	// TODO: Implement
	return 1;
}


/*
===============
Host_Shutdown

FIXME: this is a callback from Sys_Quit and Sys_Error.  It would be better
to run quit through here before the final handoff to the sys code.
===============
*/
void Host_Shutdown( void )
{
	// TODO: Implement
}


// TODO: Implement


void Master_Init( void )
{
	// TODO: Implement
}


/*
==================
Host_GetHostInfo
==================
*/
DLL_EXPORT void Host_GetHostInfo( float* fps, int* nActive, int* nBots, int* nMaxPlayers, char* pszMap )
{
	// TODO: Implement
}



/*
==============================
Host_Frame

==============================
*/
DLL_EXPORT int Host_Frame( float time, int iState, int* stateInfo )
{
	// TODO: Implement
	Sys_Error("_Host_Frame: Not implemented yet");

	return 0;
}