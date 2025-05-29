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

int Rcon_Validate( void )
{
	if (!strlen(rcon_password.string))
		return 0;

	if (strcmp(Cmd_Argv(1), rcon_password.string))
		return 0;

	return 1;
}

/*
===============
Host_RemoteCommand

A client issued an rcon command.
Shift down the remaining args
Redirect all printfs
===============
*/
void Host_RemoteCommand( netadr_t* net_from )
{
	int		i;
	int		invalid;
	char	remaining[1024];

	// Verify this user has access rights.
	invalid = Rcon_Validate();
	if (!invalid)
	{
		Con_Printf("Bad rcon from %s:\n%s\n", NET_AdrToString(*net_from), net_message.data + 4);
	}
	else
	{
		Con_Printf("Rcon from %s:\n%s\n", NET_AdrToString(*net_from), net_message.data + 4);
	}

	invalid = Rcon_Validate();
	if (!invalid)
	{
		Con_Printf("Bad rcon_password.\n");
		return;
	}

	remaining[0] = 0;

	for (i = 2; i < Cmd_Argc(); i++)
	{
		strcat(remaining, Cmd_Argv(i));
		strcat(remaining, " ");
	}

	Host_BeginRedirect(RD_PACKET, net_from);
	Cmd_ExecuteString(remaining, src_command);
	Host_EndRedirect();
}

/*
==================
Host_Quit_f
==================
*/
void Host_Quit_f( void )
{
	if (Cmd_Argc() == 1)
	{
		giActive = DLL_CLOSE;

		if (cls.state != ca_dedicated)
			CL_Disconnect();

		Host_ShutdownServer(FALSE);
		Sys_Quit();
	}

	giActive = DLL_PAUSED;
	giStateInfo = 4;
}

/*
==================
Host_Status_PrintClient

Print client info to console
==================
*/
void Host_Status_PrintClient( char* pszState, qboolean fromcbuf, void (*print) ( char* fmt, ... ), int playernum, client_t* client )
{
	int			seconds;
	int			minutes;
	int			hours = 0;

	seconds = (int)(realtime - client->netchan.connect_time);
	minutes = seconds / 60;
	if (minutes)
	{
		seconds %= 60;
		hours = minutes / 60;
		if (hours)
			minutes %= 60;
	}
	else
		hours = 0;

	print("#%-2u %-12.12s\n", playernum + 1, client->name);
	print("   frags:  %3i\n", client->edict->v.frags);

	if (hours)
		print("   time :  %2i:%02i:%02i\n", hours, minutes, seconds);
	else
		print("   time :  %02i:%02i\n", minutes, seconds);

	print("   frame rate :  %4i\n", (int)(1000.0f * client->netchan.frame_rate));
	print("   frame latency :  %4i\n", (int)(1000.0f * client->netchan.frame_latency));
	print("   ping :  %4i\n", SV_CalcPing(client));
	print("   drop :  %5.2f %%\n", 100.0f * client->netchan.drop_count / client->netchan.incoming_sequence);

	if (pszState && pszState[0])
		print("   %s\n", pszState);

	if (client->spectator)
	{
		print("  (spectator) %s\n", NET_BaseAdrToString(client->netchan.remote_address));
	}
	else if (client->fakeclient)
	{
		print("  (fake)\n");
	}
	else if (fromcbuf)
	{
		print("   %s\n", NET_BaseAdrToString(client->netchan.remote_address));
	}
	print("\n");
}


/*
==================
Host_Status_f
==================
*/
void Host_Status_f( void )
{
	client_t* client;
	int			j;
	int			players, spectators;
	void		(*print) ( char* fmt, ... );

	if (cmd_source == src_command)
	{
		if (sv.active == FALSE)
		{
			Cmd_ForwardToServer();
			return;
		}
		print = Con_Printf;
	}
	else
		print = SV_ClientPrintf;

	// ============================================================
	// Server status information.
	print("hostname:  %s\n", Cvar_VariableString("hostname"));
	print("build   :  %d\n", build_number());
	print("tcp/ip  :  %s\n", NET_AdrToString(net_local_adr));
	print(" map     :  %s\n", sv.name);

	SV_CountPlayers(&players, &spectators);

	if (spectators)
		print(" players: %i active (%i spectators) (%i max)\n\n", players, spectators, svs.maxclients);
	else
		print(" players: %i active (%i max)\n\n", players, svs.maxclients);

	for (j = 0, client = svs.clients; j < svs.maxclients; j++, client++)
	{
		if (!client->active)
		{
			if (client->connected)
				Host_Status_PrintClient("CONNECTING", cmd_source == src_command, print, j, client);		
			continue;
		}

		Host_Status_PrintClient(NULL, cmd_source == src_command, print, j, client);
	}
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
	
	Cmd_AddCommand("status", Host_Status_f);

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