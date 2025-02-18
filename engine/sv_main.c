// sv_main.c -- server main program

#include "quakedef.h"
#include "pr_cmds.h"
#include "pmove.h"
#include "decal.h"

server_t		sv;
server_static_t	svs;


// TODO: Implement


char* pr_strings = NULL, *gNullString = "";
globalvars_t gGlobalVariables;

char	localmodels[MAX_MODELS][5];			// inline model names for precache

qboolean	allow_cheats;

decalname_t	sv_decalnames[MAX_BASE_DECALS];


// TODO: Implement

int			sv_decalnamecount;

UserMsg* sv_gpNewUserMsgs;
UserMsg* sv_gpUserMsgs;

int		nReliableBytesSent = 0;
int		nDatagramBytesSent = 0;
int		nReliables = 0;
int		nDatagrams = 0;
qboolean bUnreliableOverflow = FALSE;

// TODO: Implement

float g_LastScreenUpdateTime;
float scr_centertime_off;

qboolean bAddDeltaFlag = FALSE;

cvar_t sv_password = { "sv_password", "" };
cvar_t sv_spectator_password = { "sv_spectator_password", "" };
cvar_t sv_maxspectators = { "sv_maxspectators", "8", FALSE, TRUE };

cvar_t sv_cheats = { "sv_cheats", "0", FALSE, TRUE };

cvar_t sv_zmax = { "sv_zmax", "4096" };
cvar_t sv_wateramp = { "sv_wateramp", "0" };
cvar_t sv_skyname = { "sv_skyname", "desert" };
cvar_t sv_maxvelocity = { "sv_maxvelocity", "2000" };

/*
================
SV_RejectConnection

Rejects connection request and sends back a message
================
*/
void SV_RejectConnection( netadr_t* adr, char* reason )
{
	SZ_Clear(&net_message);
	MSG_WriteLong(&net_message, 0xFFFFFFFF);
	MSG_WriteByte(&net_message, S2C_CONNREJECT);
	MSG_WriteString(&net_message, reason);
	NET_SendPacket(NS_SERVER, net_message.cursize, net_message.data, *adr);
	SZ_Clear(&net_message);
}

// TODO: Implement


// MAX_CHALLENGES is made large to prevent a denial
//  of service attack that could cycle all of them
//  out before legitimate users connected
#define	MAX_CHALLENGES	1024
typedef struct
{
	netadr_t    adr;       // Address where challenge value was sent to.
	int			challenge; // To connect, adr IP address must respond with this #
	int			time;      // # is valid for only a short duration.
} challenge_t;

challenge_t	g_rg_sv_challenges[MAX_CHALLENGES];	// to prevent spoofed IPs from connecting

/*
================
SV_ConnectClient

Initializes a client_t for a new net connection.  This will only be called
once for a player each game, not once for each level change.
================
*/
void SV_ConnectClient( void )
{
	char*		s;
	int			i, nChallengeValue;
	netadr_t	adr;
	client_t*	cl;
	char		name[128];
	int			nCDKeyLength, cKeyUsages;
	char		rgszCDKey[512];
	qboolean	bSpectator = FALSE;
	int			clients, spectators;
	edict_t*	ent;
	int			edictnum;

	cKeyUsages = 0;

	adr = net_from;
	s = Cmd_Argv(1); //proto version
	if (atoi(s) != PROTOCOL_VERSION)
	{
		SV_RejectConnection(&adr, "Bad protocol\n");
		return;
	}
	if (Cmd_Argc() < 8)
	{
		SV_RejectConnection(&adr, "Insufficient connection info\n");
		return;
	}

	s = Cmd_Argv(2); //challenge
	nChallengeValue = atoi(s);
	if (!NET_IsLocalAddress(adr))
	{
		for (i = 0; i < MAX_CHALLENGES; i++)
		{
			if (NET_CompareClassBAdr(net_from, g_rg_sv_challenges[i].adr))
			{
				// FIXME:  Compare time gap and don't allow too long.
				if (nChallengeValue == g_rg_sv_challenges[i].challenge)
					break;		// good
				SV_RejectConnection(&adr, "Bad challenge.\n");
				return;
			}
		}
		if (i == MAX_CHALLENGES)
		{
			SV_RejectConnection(&adr, "No challenge for your address.\n");
			return;
		}
	}

	//check if the client is reconnecting
	for (i = 0, cl = svs.clients; i < svs.maxclientslimit; i++, cl++)
	{
		if (cl->active || cl->spawned || cl->connected)
		{
			if (NET_CompareAdr(cl->netchan.remote_address, adr))
			{
				Con_Printf("%s:reconnect\n", NET_AdrToString(cl->netchan.remote_address));
				SV_DropClient(cl, FALSE);
			}
		}
	}

	s = Cmd_Argv(3);
	memset(name, 0, sizeof(name));
	if (s)
	{
		strncpy(name, s, sizeof(name));
		name[sizeof(name) - 1] = 0;
	}
	else
	{
		strcpy(name, "unconnected");
	}
	s = Cmd_Argv(4);
	if (!s || !s[0])
	{
		SV_RejectConnection(&adr, "Invalid connection.\n");
		return;
	}

	i = atoi(s);
	if (i <= 0)
	{
		SV_RejectConnection(&adr, "Invalid connection.\n");
		return;
	}
	if (i > 2)
	{
		SV_RejectConnection(&adr, "Invalid connection.\n");
		return;
	}
	s = Cmd_Argv(5); //cd key hash
	if (!s || !s[0])
	{
		SV_RejectConnection(&adr, "Invalid connection.\n");
		return;
	}

	nCDKeyLength = atoi(s);
	if (nCDKeyLength <= 0)
	{
		SV_RejectConnection(&adr, "Invalid connection.\n");
		return;
	}
	if (nCDKeyLength > 1024)
	{
		SV_RejectConnection(&adr, "Invalid connection.\n");
		return;
	}

	s = Cmd_Argv(6); //the cd key itself
	if (!s)
	{
		SV_RejectConnection(&adr, "Invalid connection protocol.\n");
		return;
	}
	memset(rgszCDKey, 0, sizeof(rgszCDKey));
	strcpy(rgszCDKey, s);
	if (i != 2)
	{
		SV_RejectConnection(&adr, "Invalid connection protocol.\n");
		return;
	}

	// HASHED CD KEY VALIDATION
	if (strlen(rgszCDKey) != 32 || nCDKeyLength != 32)
	{
		SV_RejectConnection(&adr, "Invalid CD Key.\n");
		return;
	}
	for (i = 0, cl = svs.clients; i < svs.maxclientslimit; i++, cl++)
	{
		if ((cl->active || cl->spawned || cl->connected) && !_strnicmp(rgszCDKey, cl->hashedcdkey, sizeof(cl->hashedcdkey) - 1))
			++cKeyUsages;
	}
	if (cKeyUsages >= 5)
	{
		SV_RejectConnection(&adr, "CD Key already in use.\n");
		return;
	}

	if (Cmd_Argc() >= 8) //the player has specified a password
	{
		s = Cmd_Argv(7); //server password
		if (s[0])
		{
			if (sv_password.string[0] && _strcmpi(sv_password.string, "none") != 0 && strcmp(sv_password.string, s) != 0)
			{
				Con_Printf("%s:  password failed\n", NET_AdrToString(net_from));
				SV_RejectConnection(&net_from, "Invalid server password.\n");
				return;
			}
		}
	}
	if (Cmd_Argc() >= 9) //the player has specified a spectator password
	{
		s = Cmd_Argv(8);
		if (s[0])
		{
			if (sv_spectator_password.string[0] && _strcmpi(sv_spectator_password.string, "none") != 0 && strcmp(sv_spectator_password.string, s) != 0)
			{
				Con_Printf("%s:spectator password failed\n", NET_AdrToString(net_from));
				SV_RejectConnection(&net_from, "Invalid spectator password.\n");
				return;
			}
			bSpectator = TRUE;
		}
	}

	spectators = 0;
	clients = 0;
	SV_CountPlayers(&clients, &spectators);
	clients -= spectators;
	if (bSpectator && spectators >= sv_maxspectators.value)
	{
		Con_Printf("%s:No space for spectator\n", NET_AdrToString(net_from));
		SV_RejectConnection(&net_from, "No more spectators allowed.\n");
		return;
	}

	// find a free client
	for (i = 0, cl = svs.clients; i < svs.maxclients; i++, cl++)
	{
		if (!cl->active && !cl->spawned && !cl->connected)
			break;
	}
	if (i == svs.maxclients)
	{
		SV_RejectConnection(&net_from, "Server is full.");
		return;
	}

////////////////////////////////////////////////
// Client can connect
//

	// Store off the potential client number
	edictnum = i + 1;
	ent = EDICT_NUM(edictnum);
	host_client = cl;
	SV_ClearResourceLists(cl);
	memset(cl, 0, sizeof(*cl));
	cl->resourcesneeded.pPrev = &cl->resourcesneeded;
	cl->resourcesneeded.pNext = &cl->resourcesneeded;
	cl->resourcesonhand.pPrev = &cl->resourcesonhand;
	cl->resourcesonhand.pNext = &cl->resourcesonhand;
	// Set up the network channel.
	Netchan_Setup(1, &cl->netchan, adr);

	// Tell client connection worked.
	Netchan_OutOfBandPrint(NS_SERVER, adr, "%c", S2C_CONNECTION);

	if (_strcmpi(NET_AdrToString(adr), "loopback") != 0)
	{
		if (bSpectator)
		{
			Con_DPrintf("Spectator %s connected\nAdr: %s\n", name, NET_AdrToString(adr));
		}
		else
		{
			Con_DPrintf("Client %s connected\nAdr: %s\n", name, NET_AdrToString(adr));
		}
	} else {
		Con_DPrintf("Local connection.\n");
	}

	// Set up client structure.
	strcpy(cl->name, name);
	strncpy(cl->hashedcdkey, rgszCDKey, sizeof(cl->hashedcdkey) - 1);
	cl->hashedcdkey[sizeof(cl->hashedcdkey) - 1] = 0;
	cl->active = FALSE;
	cl->spawned = FALSE;
	cl->uploading = FALSE;
	cl->edict = ent;
	cl->maxspeed = 0.0;
	cl->spectator = bSpectator;
	cl->connected = TRUE;
	if (bSpectator)
		cl->edict->v.flags |= FL_SPECTATOR;
	cl->datagram.allowoverflow = TRUE;
	cl->datagram.maxsize = MAX_DATAGRAM;
	cl->privileged = FALSE;
	cl->datagram.data = cl->datagram_buf;

	if (!sv.loadgame)
		gEntityInterface.pfnParmsNewLevel();
}

/*
================
SVC_Ping

Just responds with an acknowledgement
================
*/
void SVC_Ping( void )
{
	char	data;

	data = A2A_ACK;

	NET_SendPacket(NS_SERVER, 1, &data, net_from);
}

/*
================
SVC_GetChallenge
================
*/
void SVC_GetChallenge( void )
{
	int		i, oldest, oldestTime = INT_MAX;
	char	data[9]; // -1 mark + S2C_CHALLENGE + challenge value

	for (i = 0; i < MAX_CHALLENGES; i++)
	{
		if (NET_CompareClassBAdr(net_from, g_rg_sv_challenges[i].adr))
			break;
		if (g_rg_sv_challenges[i].time < oldestTime)
		{
			oldestTime = g_rg_sv_challenges[i].time;
			oldest = i;
		}
	}

	if (i == MAX_CHALLENGES)
	{
		i = oldest;
		g_rg_sv_challenges[i].challenge = RandomLong(0, 0xFFFF) | (RandomLong(0, 0xFFFF) << 16);
		g_rg_sv_challenges[i].adr = net_from;
		g_rg_sv_challenges[i].time = (int) realtime;
	}

	sprintf(data, "%c%c%c%c%c", 255, 255, 255, 255, S2C_CHALLENGE);
	(*(int*)&data[5]) = BigLong(g_rg_sv_challenges[i].challenge);
	NET_SendPacket(NS_SERVER, sizeof(data), data, net_from);
}

/*
================
SVC_Info
================
*/
void SVC_Info( void )
{
	sizebuf_t	sb;
	char		buf[2048];
	int			i, cNumActiveClients;
	client_t*	cl;

	cNumActiveClients = 0;
	sb.data = buf;

	if (sv.active && svs.maxclients > 1 
		&& (noip.value || !NET_CompareClassBAdr(net_local_adr, net_from))
#ifdef _WIN32
		&& (noipx.value || !NET_CompareClassBAdr(net_local_ipx_adr, net_from))
#endif //_WIN32
		)
	{
		for (i = 0, cl = svs.clients; i < svs.maxclients; i++, cl++)
		{
			if (cl->active)
				cNumActiveClients++;
		}

		MSG_WriteLong(&sb, 0xFFFFFFFF); // -1 mark
		MSG_WriteByte(&sb, S2A_INFO);
		MSG_WriteString(&sb, NET_AdrToString(net_local_adr));
		MSG_WriteString(&sb, host_name.string);
		MSG_WriteString(&sb, sv.name);
		MSG_WriteString(&sb, com_gamedir);
		MSG_WriteString(&sb, gEntityInterface.pfnGetGameDescription());
		MSG_WriteByte(&sb, cNumActiveClients);
		MSG_WriteByte(&sb, svs.maxclients);
		MSG_WriteByte(&sb, 7); // TODO: Wtf is 7?
		NET_SendPacket(NS_SERVER, sb.cursize, sb.data, net_from);
	}
}

/*
================
SVC_Heartbeat
================
*/
void SVC_Heartbeat( void )
{
	// TODO: Implement
	MSG_ReadLong();
	MSG_ReadByte();
}

/*
=================
SV_ConnectionlessPacket

A connectionless packet has four leading 0xff
characters to distinguish it from a game channel.
Clients that are in the game can still send
connectionless packets.
=================
*/
void SV_ConnectionlessPacket( void )
{
	char	*s;
	char	*c;

	MSG_BeginReading();
	MSG_ReadLong();		// skip the -1 marker

	s = MSG_ReadStringLine();

	Cmd_TokenizeString(s);

	c = Cmd_Argv(0);

	if (!strcmp(c, "ping") || (c[0] == A2A_PING && (c[1] == 0 || c[1] == '\n')))
	{
		SVC_Ping();
		return;
	}
	if (c[0] == A2A_ACK && (c[1] == 0 || c[1] == '\n'))
	{
		Con_Printf("A2A_ACK from %s\n", NET_AdrToString(net_from));
		return;
	}
	else if (c[0] == M2A_CHALLENGE && (c[1] == 0 || c[1] == '\n'))
	{
		SVC_Heartbeat();
	}
	else if (!strcmp(c, "getchallenge"))
	{
		SVC_GetChallenge();
	}
	else if (!strcmp(c, "connect"))
	{
		SV_ConnectClient();
	}

	// TODO: Implement
}

/*
=================
SV_ReadPackets
=================
*/
void SV_ReadPackets( void )
{
	int			i;
	client_t*	cl;
	float		time1, time2, time3, time4, time5, time6;
	float		timetotal1, timetotal2, timetotal3;

	timetotal1 = 0;
	timetotal2 = 0;
	timetotal3 = 0;

	if (host_speeds.value == 2)
		Sys_FloatTime();

	while (NET_GetPacket(NS_SERVER))
	{
		time1 = Sys_FloatTime();

		if (SV_FilterPacket())
		{
			SV_SendBan();	// tell them we aren't listening...
			continue;
		}

		time2 = Sys_FloatTime();

		if (host_speeds.value == 2)
			timetotal1 += time2 - time1;

		// check for connectionless packet (0xffffffff) first
		if (*(int *)net_message.data == -1)
		{
			SV_ConnectionlessPacket();
			continue;
		}

		i = 0;
		cl = svs.clients;
		if (svs.maxclientslimit > 0)
		{
			// check for packets from connected clients
			while (!cl->connected && !cl->active && !cl->spawned
			   || NET_CompareAdr(net_from, cl->netchan.remote_address) == FALSE)
			{
				++i;
				++cl;
				if (svs.maxclientslimit <= i)
					break;
			}
			time3 = Sys_FloatTime();
			if (Netchan_Process(&cl->netchan))
			{	// this is a valid, sequenced packet, so process it
				svs.stats.packets++;
				cl->send_message = TRUE;	// reply at end of frame

				time4 = Sys_FloatTime();
				SV_ExecuteClientMessage(cl);
				time5 = Sys_FloatTime();

				if (host_speeds.value == 2)
					timetotal2 += time5 - time4;
			}
			time6 = Sys_FloatTime();
			if (host_speeds.value == 2)
				timetotal3 += time6 - time3;
		}
	}

	if (host_speeds.value == 2)
		Sys_FloatTime();
}

/*
==================
SV_CheckTimeouts

If a packet has not been received from a client in sv_timeout.value
seconds, drop the conneciton.

When a client is normally dropped, the client_t goes into a zombie state
for a few seconds to make sure any final reliable message gets resent
if necessary
==================
*/
void SV_CheckTimeouts( void )
{
	// TODO: Implement
}

// TODO: Implement

/*
===================
SV_FullClientUpdate

Writes all update values to a sizebuf
===================
*/
void SV_FullClientUpdate( client_t* cl, sizebuf_t* sb )
{
	int			i, nClientNum;
	client_t*	client;

	for (i = 0, client = svs.clients; i < svs.maxclients; i++, client++)
	{
		// skip inactive, fake clients, and the specified client
		if (!client->active || client->fakeclient || cl == client)
			continue;

		MSG_WriteByte(sb, svc_updatename);
		nClientNum = cl - svs.clients;
		if (!cl->active && !cl->spawned && cl->name[0] == 0)
			nClientNum |= PN_SPECTATOR;
		MSG_WriteByte(sb, nClientNum);
		MSG_WriteString(sb, cl->name);

		MSG_WriteByte(sb, svc_updatecolors);
		MSG_WriteByte(sb, cl - svs.clients);
		MSG_WriteByte(sb, 0);
	}
}

/*
==================
SV_Init

General initialization of the server
==================
*/
void SV_Init( void )
{
	Cvar_RegisterVariable(&sv_password);

	// TODO: Implement
	
	Cvar_RegisterVariable(&sv_gravity);
	Cvar_RegisterVariable(&sv_friction);
	Cvar_RegisterVariable(&sv_edgefriction);
	Cvar_RegisterVariable(&sv_stopspeed);
	Cvar_RegisterVariable(&sv_maxspeed);
	Cvar_RegisterVariable(&sv_accelerate);
	Cvar_RegisterVariable(&sv_stepsize);
	Cvar_RegisterVariable(&sv_clipmode);
	Cvar_RegisterVariable(&sv_bounce);
	Cvar_RegisterVariable(&sv_airmove);
	Cvar_RegisterVariable(&sv_airaccelerate);
	Cvar_RegisterVariable(&sv_wateraccelerate);
	Cvar_RegisterVariable(&sv_waterfriction);

	// TODO: Implement
	Cvar_RegisterVariable(&sv_zmax);
	Cvar_RegisterVariable(&sv_wateramp);
	Cvar_RegisterVariable(&sv_skyname);
	Cvar_RegisterVariable(&sv_maxvelocity);
	Cvar_RegisterVariable(&sv_spectator_password);
	Cvar_RegisterVariable(&sv_maxspectators);

	// TODO: Implement

	Cvar_RegisterVariable(&sv_cheats);
	Cvar_RegisterVariable(&sv_spectatormaxspeed);

	// TODO: Implement
}

/*
==================
void SV_CountPlayers

Counts number of connections.  Clients includes regular connections, while spectators counts the spectators count.
==================
*/
void SV_CountPlayers( int* clients, int* spectators )
{
	int			i;
	client_t*	cl;

	*clients = 0;
	if (spectators)
		*spectators = 0;

	for (i = 0, cl = svs.clients; i < svs.maxclients; i++, cl++)
	{
		if (cl->active || cl->spawned || cl->connected)
		{
			(*clients)++;
			if (cl->spectator)
			{
				if (spectators)
					(*spectators)++;
			}
		}
	}
}

void SV_ClearChannel( qboolean forceclear )
{
	// TODO: Implement; not sure if this function belongs here.
}

/*
================
SV_DeallocateDynamicData

================
*/
void SV_DeallocateDynamicData( void )
{
	// TODO: Implement
}

/*
================
SV_DeallocateDynamicData

================
*/
void SV_ReallocateDynamicData( void )
{
	// TODO: Implement
}

// TODO: Implement

/*
=================
SV_BroadcastCommand

Sends text to all active clients
=================
*/

void SV_BroadcastCommand( char *fmt, ... )
{
	// TODO: Implement
	//FF: what does this do here? it must be in sv_send.c
}

/*
================
SV_SendReconnect

Tell all the clients that the server is changing levels
================
*/
void SV_SendReconnect( void )
{
	// TODO: Implement
}

// TODO: Implement

/*
================
SV_WriteMovevarsToClient

Send the movevars to the specified client's netchan
================
*/
void SV_WriteMovevarsToClient( sizebuf_t* sb )
{
	MSG_WriteByte(sb, svc_newmovevars);
	MSG_WriteFloat(sb, movevars.gravity);
	MSG_WriteFloat(sb, movevars.stopspeed);
	MSG_WriteFloat(sb, movevars.maxspeed);
	MSG_WriteFloat(sb, movevars.spectatormaxspeed);
	MSG_WriteFloat(sb, movevars.accelerate);
	MSG_WriteFloat(sb, movevars.airaccelerate);
	MSG_WriteFloat(sb, movevars.wateraccelerate);
	MSG_WriteFloat(sb, movevars.friction);
	MSG_WriteFloat(sb, movevars.edgefriction);
	MSG_WriteFloat(sb, movevars.waterfriction);
	MSG_WriteFloat(sb, movevars.entgravity);
	MSG_WriteFloat(sb, movevars.bounce);
	MSG_WriteFloat(sb, movevars.stepsize);
	MSG_WriteFloat(sb, movevars.maxvelocity);
	MSG_WriteFloat(sb, movevars.zmax);
	MSG_WriteFloat(sb, movevars.waveHeight);
	MSG_WriteString(sb, movevars.skyName);
}

/*
================
SV_QueryMovevarsChanged

Tell all the clients about new movevars, if any
================
*/
void SV_QueryMovevarsChanged( void )
{
	// TODO: FF: Move me

	int			i;
	client_t*	cl;

	if (movevars.maxspeed != sv_maxspeed.value
	  || movevars.gravity != sv_gravity.value
	  || movevars.stopspeed != sv_stopspeed.value
	  || sv_spectatormaxspeed.value != movevars.spectatormaxspeed
	  || movevars.accelerate != sv_accelerate.value
	  || movevars.airaccelerate != sv_airaccelerate.value
	  || movevars.wateraccelerate != sv_wateraccelerate.value
	  || movevars.friction != sv_friction.value
	  || movevars.edgefriction != sv_edgefriction.value
	  || movevars.waterfriction != sv_waterfriction.value
	  || movevars.entgravity != 1
	  || movevars.bounce != sv_bounce.value
	  || movevars.stepsize != sv_stepsize.value
	  || movevars.maxvelocity != sv_maxvelocity.value
	  || movevars.zmax != sv_zmax.value
	  || movevars.waveHeight != sv_wateramp.value
	  || strcmp(sv_skyname.string, movevars.skyName) != 0)
	{
		SV_SetMoveVars();
		for (i = 0, cl = svs.clients; i < svs.maxclients; i++, cl++)
		{
			if (cl->active || cl->spawned || cl->connected)
				SV_WriteMovevarsToClient(&cl->netchan.message);
		}
	}
}

/*
================
SV_SpawnServer

This is called at the start of each level
================
*/
int SV_SpawnServer( qboolean bIsDemo, char* server, char* startspot )
{
	edict_t*		ent;
	int				i;
	char			szDllName[MAX_QPATH];

	// let's not have any servers with no name
	if (host_name.string[0] == 0)
		Cvar_Set("hostname", "Half-Life");

	scr_centertime_off = 0;

	if (startspot)
		Con_DPrintf("Spawn Server %s: [%s]\n", server, startspot);
	else
		Con_DPrintf("Spawn Server %s\n", server);

	g_LastScreenUpdateTime = 0;

	// Any partially connected client will be restarted if the spawncount is not matched.
	// in "spawn" command
	gHostSpawnCount = ++svs.spawncount;

//
// tell all connected clients that we are going to a new level
//
	if (sv.active && svs.maxclients > 1)
	{
		SV_SendReconnect();
	}

//
// make cvars consistant
//
	if (coop.value)
		Cvar_SetValue("deathmatch", 0);

	current_skill = (int)(skill.value + 0.5);
	if (current_skill < 0)
		current_skill = 0;
	if (current_skill > 3)
		current_skill = 3;

	Cvar_SetValue("skill", (float)current_skill);

//
// set up the new server
//
	Host_ClearMemory(FALSE);

	memset(&sv, 0, sizeof(sv));

	strcpy(sv.name, server);

	if (startspot)
		strcpy(sv.startspot, startspot);
	else
		sv.startspot[0] = 0;

// load progs to get entity field count
	pr_strings = gNullString;
	gGlobalVariables.pStringBase = gNullString;

	// Force normal player collisions for single player
	if (svs.maxclients == 1)
		Cvar_SetValue("sv_clienttrace", 1);

// allocate server memory
	sv.max_edicts = COM_EntsForPlayerSlots(svs.maxclients);

	Host_DeallocateDynamicData();
	Host_ReallocateDynamicData();

	// Assume no entities beyond world and client slots
	gGlobalVariables.maxEntities = sv.max_edicts;

	sv.edicts = Hunk_AllocName(sizeof(edict_t) * sv.max_edicts, "edicts");

	sv.datagram.maxsize = sizeof(sv.datagram_buf);
	sv.datagram.cursize = 0;
	sv.datagram.data = sv.datagram_buf;

	sv.reliable_datagram.maxsize = sizeof(sv.reliable_datagram_buf);
	sv.reliable_datagram.cursize = 0;
	sv.reliable_datagram.data = sv.reliable_datagram_buf;

	sv.master.maxsize = sizeof(sv.master_buf);
	sv.master.cursize = 0;
	sv.master.data = sv.master_buf;

	sv.signon.maxsize = sizeof(sv.master_buf);
	sv.signon.cursize = 0;
	sv.signon.data = sv.signon_buffers[0];

	sv.num_signon_buffers = 1;

	// leave slots at start for clients only
	sv.num_edicts = svs.maxclients + 1;
	for (i = 0; i < MAX_CLIENTS; i++)
	{
		svs.clients[i].edict = &sv.edicts[i + 1];
	}

	gGlobalVariables.maxClients = svs.maxclients;
	sv.paused = FALSE;
	sv.state = ss_loading;

	// Set initial time values.
	sv.time = 1.0;
	gGlobalVariables.time = 1.0f;

	strcpy(sv.name, server);

	// Load the world model.
	sprintf(sv.modelname, "maps/%s.bsp", server);
	sv.worldmodel = Mod_ForName(sv.modelname, FALSE);
	if (!sv.worldmodel)
	{
		Con_Printf("Couldn't spawn server %s\n", sv.modelname);
		sv.active = FALSE;
		return 0;
	}

	if (svs.maxclients > 1)
	{
		// Server map CRC check.
		CRC32_Init(&sv.worldmapCRC);
		if (!CRC_MapFile(&sv.worldmapCRC, sv.modelname))
		{
			Con_Printf("Couldn't CRC server map:  %s\n", sv.modelname);
			sv.active = FALSE;
			return 0;
		}

		// DLL CRC check.
		sprintf(szDllName, "cl_dlls\\client.dll");
		CRC32_Init(&sv.clientSideDllCRC);

		if (!CRC_File(&sv.clientSideDllCRC, szDllName))
		{
			Con_Printf("Couldn't CRC client side dll:  %s\n", szDllName);
			sv.active = FALSE;
			return 0;
		}
	}
	else
	{
		sv.worldmapCRC = 0;
		sv.clientSideDllCRC = 0;
	}

	sv.models[1] = sv.worldmodel;

	//
	// clear world interaction links
	//
	SV_ClearWorld();
	sv.sound_precache[0] = pr_strings;
	sv.model_precache[0] = pr_strings;
	sv.model_precache[1] = sv.modelname;

	if (sv.worldmodel->numsubmodels > 1)
	{
		for (i = 1; i < sv.worldmodel->numsubmodels; i++)
		{
			sv.model_precache[1 + i] = localmodels[i];
			sv.models[i + 1] = Mod_ForName(localmodels[i], FALSE);
		}
	}

//
// load the rest of the entities
//
	ent = &sv.edicts[0];
	memset(&ent->v, 0, sizeof(ent->v));
	ent->free = FALSE;
	ent->v.model = sv.worldmodel->name - pr_strings;
	ent->v.modelindex = 1; // world model
	ent->v.solid = SOLID_BSP;
	ent->v.movetype = MOVETYPE_PUSH;

	if (!coop.value)
	{
		gGlobalVariables.deathmatch = deathmatch.value;
	}
	else
	{
		gGlobalVariables.coop = coop.value;
	}

	gGlobalVariables.mapname = sv.name - pr_strings;
	gGlobalVariables.startspot = sv.startspot - pr_strings;
	gGlobalVariables.serverflags = svs.serverflags;

	allow_cheats = sv_cheats.value;

	SV_SetMoveVars();

	return 1;
}