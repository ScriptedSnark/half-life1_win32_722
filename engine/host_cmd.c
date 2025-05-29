#include "quakedef.h"
#include "winquake.h"
#include "pr_cmds.h"
#include "cl_demo.h"
#include "pr_edict.h"

int	current_skill;
int	gHostSpawnCount = 0;
qboolean noclip_anglehack;

cvar_t rcon_password = { "rcon_password", "" };

/*
====================
SV_InactivateClients

Prepare for level transition, etc.
====================
*/
void SV_InactivateClients( void )
{
	int i;
	client_t* cl;

	for (i = 0, cl = svs.clients; i < svs.maxclients; i++, cl++)
	{
		if (!cl->active && !cl->connected && !cl->spawned)
			continue;

		SZ_Clear(&cl->netchan.message);

		cl->active = FALSE;
		cl->connected = TRUE;
		cl->spawned = FALSE;

		COM_ClearCustomizationList(&cl->customdata, FALSE);
		cl->maxspeed = 0.0;
	}
}

/*
=============================================================================

Con_Printf redirection

=============================================================================
*/

char	outputbuf[8000];

redirect_t	sv_redirected;

netadr_t sv_redirectto;

/*
==================
Host_FlushRedirect
==================
*/
void Host_FlushRedirect( void )
{
	if (sv_redirected == RD_PACKET)
	{
		SZ_Clear(&net_message);
		MSG_WriteLong(&net_message, 0xffffffff); // -1 -1 -1 -1 signal
		MSG_WriteByte(&net_message, A2C_PRINT);
		MSG_WriteString(&net_message, outputbuf);
		NET_SendPacket(NS_SERVER, net_message.cursize, net_message.data, sv_redirectto);
		SZ_Clear(&net_message);
		
	}
	else if (sv_redirected == RD_CLIENT)   // Send to client on message stream.
	{
		MSG_WriteByte(&host_client->netchan.message, svc_print);
		MSG_WriteString(&host_client->netchan.message, outputbuf);
	}

	// clear it
	outputbuf[0] = 0;
}


/*
==================
Host_BeginRedirect

  Send Con_Printf data to the remote client
  instead of the console
==================
*/
void Host_BeginRedirect( redirect_t rd, netadr_t* addr )
{
	sv_redirected = rd;
	sv_redirectto = *addr;
	outputbuf[0] = 0;
}

void Host_EndRedirect( void )
{
	Host_FlushRedirect();
	sv_redirected = RD_NONE;
}






//============================================================================= FINISH LINE


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

	if (cmd_source != src_command)
		return;

	for (name[0] = 0, i = 0; i < Cmd_Argc(); i++, *(unsigned short*) &name[strlen(name)] = ' ' /* write space and the zero terminator so if this is the last cycle, strlen would not crash us */)
	{
		strcat(name, Cmd_Argv(i));
	}
	// Write line feed and the zero terminator at the same time
	*(unsigned short*) &name[strlen(name)] = '\n';

	strcpy(mapstring, Cmd_Argv(1));

	// If there is a .bsp on the end, strip it off!
	i = strlen(mapstring);
	if ( i > 4 && !_strcmpi(mapstring + i - 4, ".bsp") )
	{
		mapstring[i-4] = 0;
	}

	if (!PF_IsMapValid_I(mapstring))
	{
		Con_Printf("map change failed: '");
		Con_Printf(mapstring);
		Con_Printf("' not found on server\n");
		return;
	}

	Cvar_Set("HostMap", mapstring);
	Host_Map(FALSE, name, mapstring, 0);
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

// TODO: Implement

/*
==================
Host_PreSpawn_f
==================
*/
void Host_PreSpawn_f(void)
{
	unsigned	buf;

	if (cmd_source == src_command)
	{
		Con_Printf("prespawn is not valid from the console\n");
		return;
	}

	if (host_client->spawned)
	{
		Con_Printf("prespawn not valid -- already spawned\n");
		return;
	}

	// handle the case of a level changing while a client was connecting
	if (!cls.demoplayback && atoi(Cmd_Argv(1)) != svs.spawncount)
	{
		Con_Printf("SV_PreSpawn_f from different level\n");
		SV_New_f();
		return;
	}

	buf = atoi(Cmd_Argv(2));
	if (buf >= sv.num_signon_buffers)
		buf = 0;

	SZ_Write(&host_client->netchan.message, sv.signon_buffers[buf], sv.signon_buffer_size[buf]);

	buf++;
	if (buf == sv.num_signon_buffers)
	{	// all done prespawning
		MSG_WriteByte(&host_client->netchan.message, svc_signonnum);
		MSG_WriteByte(&host_client->netchan.message, 1);
		host_client->sendsignon = TRUE;
	}
	else
	{	// need to prespawn more
		MSG_WriteByte(&host_client->netchan.message, svc_stufftext);
		MSG_WriteString(&host_client->netchan.message,
			va("cmd prespawn %i %i\n", svs.spawncount, buf));
	}
}

/*
==================
Host_Spawn_f
==================
*/
void Host_Spawn_f( void )
{
	int		i;
	client_t*	client;
	edict_t* ent;

	if (cmd_source == src_command)
	{
		Con_Printf("spawn is not valid from the console\n");
		return;
	}

	if (!host_client->connected)
	{
		Con_Printf("Spawn not valid -- already spawned\n");
		return;
	}

	if (!cls.demoplayback && svs.spawncount != atoi(Cmd_Argv(1)))
	{
		Con_Printf("SV_Spawn_f from different level\n");
		SV_New_f();
		return;
	}

	if (sv.loadgame)
	{	// loaded games are fully inited allready
		// if this is the last client to be connected, unpause
		sv.paused = FALSE;
	}
	else
	{
		// set up the edict
		ent = host_client->edict;

		sv.state = ss_loading;

		ReleaseEntityDLLFields(ent);
		memset(&ent->v, 0, sizeof(ent->v));
		InitEntityDLLFields(ent);

		ent->v.colormap = NUM_FOR_EDICT(ent);
		ent->v.netname = host_client->name - pr_strings;
		ent->v.team = (host_client->colors & 15) + 1;
		gGlobalVariables.time = sv.time;
		gEntityInterface.pfnClientPutInServer(ent);
		sv.state = ss_active;
	}


// send all current names, colors, and frag counts
	SZ_Clear(&host_client->netchan.message);

// send time of update
	MSG_WriteByte(&host_client->netchan.message, svc_time);
	MSG_WriteFloat(&host_client->netchan.message, sv.time);

	for (i = 0, client = svs.clients; i < svs.maxclients; i++, client++)
	{
		MSG_WriteByte(&host_client->netchan.message, svc_updatename);
		MSG_WriteByte(&host_client->netchan.message, i);
		MSG_WriteString(&host_client->netchan.message, client->name);
		MSG_WriteByte(&host_client->netchan.message, svc_updatecolors);
		MSG_WriteByte(&host_client->netchan.message, i);
		MSG_WriteByte(&host_client->netchan.message, client->colors);

		if (host_client->maxspeed)
		{
			MSG_WriteByte(&host_client->netchan.message, svc_clientmaxspeed);
			MSG_WriteByte(&host_client->netchan.message, i);
			MSG_WriteByte(&host_client->netchan.message, client->maxspeed);
		}
	}

// send all current light styles
	for (i = 0; i < MAX_LIGHTSTYLES; i++)
	{
		MSG_WriteByte(&host_client->netchan.message, svc_lightstyle);
		MSG_WriteByte(&host_client->netchan.message, (char)i);
		MSG_WriteString(&host_client->netchan.message, sv.lightstyles[i]);
	}

//
// send a fixangle
// Never send a roll angle, because savegames can catch the server
// in a state where it is expecting the client to correct the angle
// and it won't happen if the game was just loaded, so you wind up
// with a permanent head tilt
	ent = &sv.edicts[1 + (host_client - svs.clients)];
	MSG_WriteByte(&host_client->netchan.message, svc_setangle);
	for (i = 0; i < 2; i++)
		MSG_WriteHiresAngle(&host_client->netchan.message, ent->v.v_angle[i]);
	MSG_WriteHiresAngle(&host_client->netchan.message, 0);

	SV_WriteClientdataToMessage(host_client, &host_client->netchan.message);

	MSG_WriteByte(&host_client->netchan.message, svc_signonnum);
	MSG_WriteByte(&host_client->netchan.message, 2);
	host_client->sendsignon = TRUE;

	if (sv.loadgame)
	{
		// TODO: Implement
		sv.loadgame = FALSE;
		gGlobalVariables.pSaveData = NULL;
	}
}

/*
==================
Host_Begin_f
==================
*/
void Host_Begin_f( void )
{
	if (cmd_source == src_command)
	{
		Con_Printf("begin is not valid from the console\n");
		return;
	}

	host_client->active = TRUE;
	host_client->connected = FALSE;
	host_client->netchan.frame_latency = 0;
	host_client->netchan.frame_rate = 0;
	host_client->netchan.drop_count = 0;
	host_client->netchan.good_count = 0;
	host_client->spawned = TRUE;
}

// TODO: Implement

/*
==============================
Host_EndSection

Signals the engine that a section has ended
Possible values:
	_oem_end_training
	_oem_end_logo
	_oem_end_demo
==============================
*/
void Host_EndSection( const char* pszSection )
{
	// TODO: Implement
}

// TODO: Implement

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

	Cmd_AddCommand("spawn", Host_Spawn_f);
	Cmd_AddCommand("begin", Host_Begin_f);
	Cmd_AddCommand("prespawn", Host_PreSpawn_f);

	// TODO: Implement

	Cmd_AddCommand("resourcelist", SV_SendResourceListBlock_f);
	Cvar_RegisterVariable(&rcon_password);

	// TODO: Implement

	Cmd_AddCommand("new", SV_New_f);

	// TODO: Implement
}