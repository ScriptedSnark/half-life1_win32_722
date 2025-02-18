#include "quakedef.h"
#include "winquake.h"
#include "pr_cmds.h"
#include "cl_demo.h"



int	gHostSpawnCount = 0;
int	current_skill;
qboolean noclip_anglehack;

void Host_ClearGameState( void );


/*
=====================
Host_Connect_f

User command to connect to server
=====================
*/
void Host_Connect_f( void )
{
	char	name[MAX_QPATH];

	if (cls.demoplayback)
	{
		CL_StopPlayback();
		CL_Disconnect();
	}

	if (Cmd_Argc() < 2 || Cmd_Args() == 0)
	{
		Con_Printf("Usage:  connect <server>\n");
		return;
	}

	strcpy(name, Cmd_Args());
	strncpy(cls.servername, name, sizeof(cls.servername) - 1);

	CL_Connect_f();
}

/*
======================
Host_Map
======================
*/
void Host_Map( qboolean bIsDemo, const char *szMapString, char *szMapName, int nSpawnParms )
{
	UserMsg*	pMsg;

	CL_Disconnect();
	Host_ShutdownServer(FALSE);

	key_dest = key_game;		// remove console or menu

	SCR_BeginLoadingPlaque();
	if (!nSpawnParms)
	{
		Host_ClearGameState();
		SV_InactivateClients();
		svs.serverflags = 0;	// haven't completed an episode yet
	}
	strcpy(cls.mapstring, szMapString);

	if (SV_SpawnServer(bIsDemo, szMapName, NULL))
	{
		if (nSpawnParms)
		{
			// TODO: Implement

			//if (!LoadGamestate(szMapName, 1))
				//SV_LoadEntities();
			sv.paused = TRUE;
			sv.loadgame = TRUE;
			SV_ActivateServer(0);
		}
		else
		{
			SV_LoadEntities();
			SV_ActivateServer(1);
		}

		// Link new user messages to the global messages chain
		if (sv_gpNewUserMsgs)
		{
			pMsg = sv_gpUserMsgs;
			if (pMsg != NULL)
			{
				while (pMsg->next)
				{
					pMsg = pMsg->next;
				}
				pMsg->next = sv_gpNewUserMsgs;
			}
			else
			{
				sv_gpUserMsgs = sv_gpNewUserMsgs;
			}
			sv_gpNewUserMsgs = NULL;
		}

		// connect to the listen server if we aren't in the dedicated mode
		if (cls.state != ca_dedicated)
			Cmd_ExecuteString("connect local", src_command);
	}
}

/*
======================
Host_Map_f

handle a
map <servername>
command from the console.  Active clients are kicked off.
======================
*/
void Host_Map_f( void )
{
	int		i;
	char	name[MAX_QPATH];
	char	mapstring[MAX_QPATH];
	char*	s;

	if (cmd_source != src_command)
		return;

	mapstring[0] = 0;
	for (i = 0; i < Cmd_Argc(); i++, mapstring[strlen(mapstring)] = ' ')
	{
		strcat(mapstring, Cmd_Argv(i));
	}
	mapstring[strlen(mapstring)] = '\n';

	strcpy(mapstring, Cmd_Argv(1));

	if (strlen(mapstring) > 4)
	{
		s = &mapstring[strlen(mapstring)];
		if (!_strcmpi(s, ".bsp"))
			*s = 0;
	}
	if (PF_IsMapValid_I(mapstring))
	{
		Cvar_Set("HostMap", mapstring);
		Host_Map(FALSE, name, mapstring, 0);
	}
	else
	{
		Con_Printf("map change failed: '");
		Con_Printf(mapstring);
		Con_Printf("' not found on server\n");
	}
}

// TODO: Implement

/*
==================
Host_Quit_f

Shutdown the engine/program as soon as possible

NOTE: The game must be shutdown before a new game can start,
before a game can load, and before the system can be shutdown.
==================
*/
void Host_Quit_f(void)
{
	if (Cmd_Argc() == 1)
	{
		giActive = DLL_CLOSE;

		// disconnect if we are connected
		if (cls.state)
			CL_Disconnect();

		// shutdown the server
		Host_ShutdownServer(FALSE);

		// exit the game
		Sys_Quit();
	}

	// either argc isn't 1 or we haven't quitted, well just pause then
	giActive = DLL_PAUSED;
	giStateInfo = 4;
}

DLL_EXPORT BOOL SaveGame( char* pszSlot, char* pszComment )
{
	// TODO: Implement
	return 0;
}

DLL_EXPORT int LoadGame( char* pName )
{
	// TODO: Implement
	return 0;
}


void Host_ClearSaveDirectory( void )
{
	// TODO: Implement
}

void Host_ClearGameState( void )
{
	S_StopAllSounds(TRUE);
	Host_ClearSaveDirectory();
	gEntityInterface.pfnResetGlobalState();
}

void Profile_Init( void )
{
	// TODO: Implement
}


/*
==================
Host_Version_f

Print current protocol version and build date
==================
*/
void Host_Version_f( void )
{
	Con_Printf("Build %d\n", build_number());
	Con_Printf("Exe: " __TIME__ " " __DATE__ "\n");
}





//=============================================================================

/*
==================
Host_InitCommands
==================
*/
void Host_InitCommands( void )
{
	// TODO: Implement

	Cmd_AddCommand("quit", Host_Quit_f);
	Cmd_AddCommand("exit", Host_Quit_f);
	Cmd_AddCommand("map", Host_Map_f);

	// TODO: Implement

	Cmd_AddCommand("connect", Host_Connect_f);


	// TODO: Implement


	Cmd_AddCommand("version", Host_Version_f);


	// TODO: Implement
}