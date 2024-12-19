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
double		host_time;
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

float g_fFrameTime = 0.0f;

cvar_t	host_framerate = { "host_framerate", "0" };
cvar_t	host_speeds = { "host_speeds", "0" };			// set for running times

cvar_t	serverprofile = { "serverprofile", "0" };




// TODO: Implement


cvar_t	developer = { "developer", "0" };




void Profile_Init( void );


// TODO: Implement


/*
================
Host_Error

This shuts down both the client and server
================
*/
void Host_Error( char* error, ... )
{
	va_list		argptr;
	char		string[1024];
	static	qboolean inerror = FALSE;

	if (inerror)
		Sys_Error("Host_Error: recursively entered");
	inerror = TRUE;

	SCR_EndLoadingPlaque();		// reenable screen updates

	va_start(argptr, error);
	vsprintf(string, error, argptr);
	va_end(argptr);
	Con_Printf("Host_Error: %s\n", string);

	if (sv.active)
		Host_ShutdownServer(FALSE);

	if (cls.state == ca_dedicated)
		Sys_Error("Host_Error: %s\n", string);	// dedicated servers exit

	CL_Disconnect();
	cls.demonum = -1;

	inerror = FALSE;

	longjmp(host_abortserver, 1);
}


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
	Host_InitCommands();

	Cvar_RegisterVariable(&maxfps);
	Cvar_RegisterVariable(&host_framerate);
	Cvar_RegisterVariable(&host_speeds);

	// TODO: Implement

	Cvar_RegisterVariable(&serverprofile);

	// TODO: Implement

	Cvar_RegisterVariable(&developer);

	// TODO: Implement

	Host_FindMaxClients();

	host_time = 1.0;		// so a think at time 0 won't get called
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



/*
==================
Host_ShutdownServer

This only happens at the end of a game, not between levels
==================
*/
void Host_ShutdownServer( qboolean crash )
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


/*
==================
Host_ServerFrame

==================
*/
void Host_ServerFrame( void )
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


void Host_PostFrameRate( float frameTime )
{
	g_fFrameTime = frameTime;
}

/*
==================
Host_GetHostInfo
==================
*/
DLL_EXPORT void Host_GetHostInfo( float* fps, int* nActive, int* nBots, int* nMaxPlayers, char* pszMap )
{
	*fps = g_fFrameTime;

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
	static double		time1 = 0;
	static double		time2 = 0;
	static double		time3 = 0;
	float pass1, pass2, pass3;
	float frameTime;
	float fps;

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

// if running the server locally, make intentions now
	if (sv.active)
		CL_SendCmd();

//-------------------
//
// server operations
//
//-------------------

	if (sv.active)
		Host_ServerFrame();

//-------------------
//
// client operations
//
//-------------------

// if running the server remotely, send intentions now after
// the incoming messages have been read
	if (!sv.active)
		CL_SendCmd();

	// fetch results from server
	CL_ReadPackets();

	if (cls.state == ca_active)
	{
		CL_SetUpPlayerPrediction(FALSE);
		CL_PredictMove();
		CL_SetUpPlayerPrediction(TRUE);
	}

	CL_EmitEntities();

	// Resend connection request if needed.
	CL_CheckForResend();

	while (CL_RequestMissingResources());

// check timeouts
	SV_CheckTimeouts();

	host_time += host_frametime;

	CAM_Think();

// update video
	time1 = Sys_FloatTime();
	if (!gfBackground)
	{
		// Refresh the screen
		SCR_UpdateScreen();

		// If recording movie and the console is totally up, then write out this frame to movie file.
		if (cl_inmovie && !scr_con_current)
		{
			VID_WriteBuffer(NULL);
		}
	}
	time2 = Sys_FloatTime();

	CL_UpdateSoundFade();

// update audio
	if (!gfBackground)
	{
		if (cls.signon == SIGNONS)
		{
			vec3_t vSoundForward, vSoundRight, vSoundUp;
			AngleVectors(r_playerViewportAngles, vSoundForward, vSoundRight, vSoundUp);

			S_Update(r_soundOrigin, vSoundForward, vSoundRight, vSoundUp);
			CL_DecayLights();
		}
		else
		{
			S_Update(vec3_origin, vec3_origin, vec3_origin, vec3_origin);
		}
	}

	CDAudio_Update();

	pass1 = (float)((time1 - time3) * 1000.0);
	time3 = Sys_FloatTime();
	pass2 = (float)((time2 - time1) * 1000.0);
	pass3 = (float)((time3 - time2) * 1000.0);

	frameTime = pass3 + pass2 + pass1;
	if (frameTime == 0.0)
	{
		fps = 100.0;
		Host_PostFrameRate(fps);
	}
	else
	{
		fps = 1000.0 / frameTime;
		Host_PostFrameRate(fps);
	}

	if (host_speeds.value)
	{
		int ent_count = 0;
		int i;

		// count used entities
		for (i = 0; i < sv.num_edicts; i++)
		{
			if (!sv.edicts[i].free)
				ent_count++;
		}

		Con_DPrintf("%3i fps tot %3.0f server %3.0f gfx %3.0f snd %d ents\n",
		  (int)fps,
		  pass1,
		  pass2,
		  pass3,
		  ent_count);
	}

	if ((giSubState & 4) && cls.state == ca_disconnected)
	{
		giActive = DLL_PAUSED;
	}

	host_framecount++;
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