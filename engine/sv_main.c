// sv_main.c -- server main program

#include "quakedef.h"
#include "decal.h"

server_t		sv;
server_static_t	svs;


// TODO: Implement


globalvars_t gGlobalVariables;

char	localmodels[MAX_MODELS][5];			// inline model names for precache

qboolean	allow_cheats;

decalname_t	sv_decalnames[MAX_BASE_DECALS];


// TODO: Implement

int			sv_decalnamecount;



// TODO: Implement

char* gNullString = "";

// TODO: Implement

float g_LastScreenUpdateTime;
float scr_centertime_off;

cvar_t sv_cheats = { "sv_cheats", "0", FALSE, TRUE };

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

void SV_BroadcastCommand(char *fmt, ...)
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
void SV_SendReconnect(void)
{
	// TODO: Implement
}

// TODO: Implement

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
	sv.num_edicts = svs.maxclients + 1;

	// TODO: Implement
	//FF: client_t needed

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
	memset(ent, 0, sizeof(ent->v));
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