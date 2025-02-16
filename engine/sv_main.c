// sv_main.c -- server main program

#include "quakedef.h"
#include "decal.h"

server_t		sv;
server_static_t	svs;


// TODO: Implement


char* pr_strings = NULL, * gNullString = "";
globalvars_t gGlobalVariables;

char	localmodels[MAX_MODELS][5];			// inline model names for precache

qboolean	allow_cheats;

decalname_t	sv_decalnames[MAX_BASE_DECALS];


// TODO: Implement

int			sv_decalnamecount;



// TODO: Implement

float g_LastScreenUpdateTime;
float scr_centertime_off;

cvar_t sv_cheats = { "sv_cheats", "0", FALSE, TRUE };

/*
================
SVC_Ping

Just responds with an acknowledgement
================
*/
void SVC_Ping(void)
{
	char	data;

	data = A2A_ACK;

	NET_SendPacket(NS_SERVER, 1, &data, net_from);
}

/*
================
SVC_Heartbeat
================
*/
void SVC_Heartbeat(void)
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
==================
SV_Init

General initialization of the server
==================
*/
void SV_Init( void )
{
	// TODO: Implement
	
	Cvar_RegisterVariable(&sv_gravity);

	// TODO: Implement

	Cvar_RegisterVariable(&sv_cheats);

	// TODO: Implement
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
SV_QueryMovevarsChanged

Tell all the clients about new movevars, if any
================
*/
void SV_QueryMovevarsChanged( void )
{
	//FF: Move me
	// TODO: Implement
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