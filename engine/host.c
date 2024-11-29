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
	Sys_Error("Host_Init: Not implemented yet");

	// TODO: Implement
	return 0;
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