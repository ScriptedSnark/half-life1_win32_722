// host.c -- coordinates spawning and killing of local servers

#include "quakedef.h"
#include "winquake.h"
#include "cmodel.h"
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

int			host_framecount;
int			host_hunklevel;

int			minimum_memory;



jmp_buf 	host_abortserver;
jmp_buf		host_enddemo;

unsigned short* host_basepal;
unsigned char* host_colormap;



cvar_t	maxfps = { "maxfps", "72.0" };

cvar_t	host_framerate = { "host_framerate", "0" };
cvar_t	host_speeds = { "host_speeds", "0" };			// set for running times

cvar_t	serverprofile = { "serverprofile", "0" };




// TODO: Implement


cvar_t	developer = { "developer", "0" };




void Profile_Init( void );

extern void	(*Launcher_InitCmds)( void );


// TODO: Implement


/*
================
Host_FindMaxClients
================
*/
void Host_FindMaxClients( void )
{
	int		i;

	svs.maxclients = 1;

	// Check for command line override
	i = COM_CheckParm("-maxplayers");
	if (i)
	{
		svs.maxclients = Q_atoi(com_argv[i + 1]);
	}
	else if (isDedicated)
	{
		svs.maxclients = MAX_CLIENTS;
	}

	if (isDedicated)
		cls.state = ca_dedicated;
	else
		cls.state = ca_disconnected;

	if (svs.maxclients < 1)
	{
		svs.maxclients = DEFAULT_SERVER_CLIENTS;
	}
	else if (svs.maxclients > MAX_CLIENTS)
	{
		svs.maxclients = MAX_CLIENTS;
	}

	// TODO: Implement
}


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

	Host_FindMaxClients();

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

	// Initialize command system
	Cbuf_Init();
	Cmd_Init();
	Cvar_CmdInit();

	V_Init();
	Chase_Init();

	COM_Init(parms->basedir);

	Host_InitLocal();

	W_LoadWadFile("gfx.wad");

	Key_Init();

	Con_Init();

	Decal_Init();
	Mod_Init();

	NET_Init();
	// Sequenced message stream layer.
	Netchan_Init();

	SV_Init();

	Con_DPrintf("Exe: "__TIME__" "__DATE__"\n");
	Con_DPrintf("%4.1f megabyte heap\n", parms->memsize / (1024 * 1024.0));

	R_InitTextures();		// needed even for dedicated servers

	if (cls.state != ca_dedicated)
	{
		int i;
		char* disk_basepal;

		disk_basepal = COM_LoadHunkFile("gfx/palette.lmp");
		if (!disk_basepal)
			Sys_Error("Couldn't load gfx/palette.lmp");

		host_basepal = Hunk_AllocName(sizeof(PackedColorVec) * 256, "palette.lmp");

		// Convert from BGR to RGBA
		for (i = 0; i < 256; i++)
		{
			host_basepal[i * 4 + 0] = disk_basepal[i * 3 + 2];
			host_basepal[i * 4 + 1] = disk_basepal[i * 3 + 1];
			host_basepal[i * 4 + 2] = disk_basepal[i * 3 + 0];
			host_basepal[i * 4 + 3] = 0; // alpha
		}

		ClientDLL_Init();

		if (!VID_Init(host_basepal))
			return 0;

		ClientDLL_HudVidInit();

		Draw_Init();

		SCR_Init();

		R_Init();

		S_Init();

		CL_Init();

		IN_Init();

		Launcher_InitCmds();
	}

	// Execute valve.rc
	Cbuf_InsertText("exec valve.rc\n");

#if defined ( GLQUAKE )
	GL_Config();
#endif

	// Mark hunklevel at end of startup
	Hunk_AllocName(0, "-HOST_HUNKLEVEL-");
	host_hunklevel = Hunk_LowMark();

	Profile_Init();

	// Mark DLL as active
	giActive = DLL_ACTIVE;
	// Enable rendering
	scr_skipupdate = FALSE;

	// Check for special -dev flag
	if (COM_CheckParm("-dev"))
	{
		Cvar_SetValue("sv_cheats", 1.0);
		Cvar_SetValue("developer", 1.0);
	}

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
	Con_DPrintf("123\n");
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