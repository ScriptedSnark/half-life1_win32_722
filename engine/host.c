// host.c -- coordinates spawning and killing of local servers

#include "quakedef.h"
#include "winquake.h"
#include "profile.h"

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
jmp_buf		host_enddemo;



cvar_t	maxfps = { "maxfps", "72.0" };

cvar_t	host_framerate = { "host_framerate", "0" };
cvar_t	host_speeds = { "host_speeds", "0" };			// set for running times

cvar_t	serverprofile = { "serverprofile", "0" };




// TODO: Implement


cvar_t	developer = { "developer", "0" };


// TODO: Implement


/*
================
Host_InitLocal
================
*/
void Host_InitLocal( void )
{
	// TODO: Implement

	Cvar_RegisterVariable(&maxfps);
	Cvar_RegisterVariable(&host_framerate);
	Cvar_RegisterVariable(&host_speeds);

	// TODO: Implement

	Cvar_RegisterVariable(&serverprofile);

	// TODO: Implement

	Cvar_RegisterVariable(&developer);

	// TODO: Implement
}



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



//============================================================================


/*
===============
Host_FilterTime

Computes simulation time (FPS value)
===============
*/
qboolean Host_FilterTime( float time )
{
	realtime += time;

	if (!isDedicated)
	{
		if (maxfps.value >= 0.1)
		{
			if (maxfps.value > 72.0)
				Cvar_SetValue("maxfps", 72.0);
		}
		else
		{
			Cvar_SetValue("maxfps", 0.1);
		}

		if (!cls.timedemo && realtime - oldrealtime < 1.0 / maxfps.value)
			return FALSE;		// framerate is too high
	}

	host_frametime = realtime - oldrealtime;
	oldrealtime = realtime;

	if (host_framerate.value > 0.0)
		host_frametime = host_framerate.value;
	else
	{	// don't allow really long or short frames
		if (host_frametime > 0.1)
			host_frametime = 0.1;
		if (host_frametime < 0.001)
			host_frametime = 0.001;
	}	
		
	return TRUE;
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
	Host_InitLocal();
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
==================
Host_Frame

Runs all active servers
==================
*/
void _Host_Frame( float time )
{
	if (setjmp(host_enddemo))
		return;			// demo finished.

// decide the simulation time
	if (!Host_FilterTime(time))
		return;			// don't run too fast, or packets will flood out

	R_SetStackBase();

	if ((cls.state == ca_connected || cls.state == ca_uninitialized) && cls.signon == SIGNONS)
	{
		cls.state = ca_active;
	}

// get new key events
	Sys_SendKeyEvents();

	if (g_bInactive)
		return;

// allow mice or other external controllers to add commands
	IN_Commands();

// process console commands
	Cbuf_Execute();

	if (cls.state == ca_active)
		ClientDLL_UpdateClientData();

	// TODO: Implement
}


/*
==============================
Host_Frame

==============================
*/
DLL_EXPORT int Host_Frame( float time, int iState, int* stateInfo )
{
	double	time1, time2;
	static double	timetotal = 0;
	static int		timecount = 0;
	int		i, c, m;

	if (setjmp(host_abortserver))
		return giActive;			// something bad happened, or the server disconnected

	if (cls.state == ca_active && g_bForceReloadOnCA_Active)
	{
		ReloadProfile(g_szProfileName);
		g_bForceReloadOnCA_Active = FALSE;
		memset(g_szProfileName, 0, sizeof(g_szProfileName));
	}

	giActive = iState;
	*stateInfo = 0;

	if (!serverprofile.value)
	{
		_Host_Frame(time);

		if (giStateInfo)
		{
			*stateInfo = giStateInfo;
			giStateInfo = 0;
			Cbuf_Execute();
		}

		if (cls.state == ca_disconnected && con_loading)
		{
			giActive = DLL_PAUSED;
		}

		return giActive;
	}

	time1 = Sys_FloatTime();
	_Host_Frame(time);
	time2 = Sys_FloatTime();

	if (giStateInfo)
	{
		*stateInfo = giStateInfo;
		giStateInfo = 0;
		Cbuf_Execute();
	}

	if (cls.state == ca_disconnected && con_loading)
	{
		giActive = DLL_PAUSED;
	}

	timetotal += time2 - time1;
	timecount++;

	if (timecount < 1000)
		return giActive;

	m = timetotal * 1000 / timecount;
	timecount = 0;
	timetotal = 0;
	c = 0;
	for (i = 0; i < svs.maxclients; i++)
	{
		if (svs.clients[i].active)
			c++;
	}
	Con_Printf("serverprofile: %2i clients %2i msec\n", c, m);

	return giActive;
}