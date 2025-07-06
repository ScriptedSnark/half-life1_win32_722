// sv_main.c -- server main program

#include "quakedef.h"
#include "pr_cmds.h"
#include "pr_edict.h"
#include "pmove.h"
#include "decal.h"
#include "cmodel.h"
#include "customentity.h"

// This many players on a Lan with same key, is ok.
#define MAX_IDENTICAL_CDKEYS	5

server_t		sv;
server_static_t	svs;

char* pr_strings = NULL, * gNullString = "";
globalvars_t gGlobalVariables;

qboolean	allow_cheats;

decalname_t	sv_decalnames[MAX_BASE_DECALS];

// netusage info
int		nReliables = 0;
int		nDatagrams = 0;
int		nReliableBytesSent = 0;
int		nDatagramBytesSent = 0;
qboolean bUnreliableOverflow = FALSE;

int		num_servers;
int		gPacketSuppressed = 0;
int		sv_decalnamecount = 0;
char	localmodels[MAX_MODELS][5];			// inline model names for precache

// User messages
UserMsg* sv_gpNewUserMsgs = NULL;
UserMsg* sv_gpUserMsgs = NULL;
int giNextUserMsg = 64;

float g_fLastPingUpdateTime = 0.0f;
qboolean g_bShouldUpdatePing = FALSE;

cvar_t	sv_language = { "sv_language", "0" };
cvar_t	violence_hblood = { "violence_hblood", "1" };
cvar_t	violence_ablood = { "violence_ablood", "1" };
cvar_t	violence_hgibs = { "violence_hgibs", "1" };
cvar_t	violence_agibs = { "violence_agibs", "1" };

cvar_t	sv_newunit = { "sv_newunit", "0" };

cvar_t	showtriggers = { "showtriggers", "0" };
cvar_t	laddermode = { "laddermode", "0" };
cvar_t	sv_clienttrace = { "sv_clienttrace", "1", FALSE, TRUE };

cvar_t	timeout = { "sv_timeout", "65", FALSE, TRUE };
cvar_t	sv_challengetime = { "sv_challengetime", "15.0" };

cvar_t	sv_cheats = { "sv_cheats", "0", FALSE, TRUE };

cvar_t	spectator_password = { "sv_spectator_password", "" };	// password for entering as a sepctator
cvar_t	max_spectators = { "sv_maxspectators", "8", FALSE, TRUE };
cvar_t	sv_spectalk = { "sv_spectalk", "1", FALSE, TRUE };
cvar_t	sv_password = { "sv_password", "" };	// password for entering the game

cvar_t	sv_masterprint = { "sv_masterprint", "1" };
cvar_t	sv_masterprinttime = { "sv_masterprinttime", "5.0" };

cvar_t	sv_netsize = { "sv_netsize", "0" };

cvar_t	sv_allow_download = { "sv_allowdownload", "1", FALSE, TRUE };
cvar_t	sv_allow_upload = { "sv_allowupload", "1", FALSE, TRUE };
cvar_t	sv_upload_maxsize = { "sv_upload_maxsize", "0", FALSE, TRUE };

cvar_t	sv_showcmd = { "sv_showcmd", "0" };

int sv_playermodel;

//=============================================================================

/*
===============
SV_Init
===============
*/
void SV_Init( void )
{
	int i;
	client_t* client;

	Cvar_RegisterVariable(&sv_password);
	Cvar_RegisterVariable(&sv_masterprint);
	Cvar_RegisterVariable(&sv_masterprinttime);
	Cvar_RegisterVariable(&sv_idealpitchscale);
	Cvar_RegisterVariable(&sv_aim);
	Cvar_RegisterVariable(&sv_language);
	Cvar_RegisterVariable(&violence_hblood);
	Cvar_RegisterVariable(&violence_ablood);
	Cvar_RegisterVariable(&violence_hgibs);
	Cvar_RegisterVariable(&violence_agibs);
	Cvar_RegisterVariable(&sv_newunit);
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
	Cvar_RegisterVariable(&sv_challengetime);
	Cvar_RegisterVariable(&timeout);
	Cvar_RegisterVariable(&sv_clienttrace);
	Cvar_RegisterVariable(&sv_zmax);
	Cvar_RegisterVariable(&sv_wateramp);
	Cvar_RegisterVariable(&sv_skyname);
	Cvar_RegisterVariable(&sv_maxvelocity);
	Cvar_RegisterVariable(&spectator_password);
	Cvar_RegisterVariable(&max_spectators);
	Cvar_RegisterVariable(&sv_spectalk);
	Cvar_RegisterVariable(&showtriggers);
	Cvar_RegisterVariable(&sv_cheats);
	Cvar_RegisterVariable(&sv_spectatormaxspeed);
	Cvar_RegisterVariable(&sv_netsize);
	Cvar_RegisterVariable(&sv_showcmd);

	Cvar_RegisterVariable(&pm_pushfix);

	Cvar_RegisterVariable(&sv_upload_maxsize);
	Cvar_RegisterVariable(&sv_allow_download);
	Cvar_RegisterVariable(&sv_allow_upload);

	Pmove_Init();

	for (i = 0; i < MAX_MODELS; i++)
	{
		sprintf(localmodels[i], "*%i", i);
	}

	for (i = 0; i < svs.maxclients; i++)
	{
		client = &svs.clients[i];
		memset(client, 0, sizeof(client_t));

		client->resourcesonhand.pPrev = &client->resourcesonhand;
		client->resourcesonhand.pNext = &client->resourcesonhand;
		client->resourcesneeded.pPrev = &client->resourcesneeded;
		client->resourcesneeded.pNext = &client->resourcesneeded;
	}
}

/*
==================
void SV_CountPlayers

Counts number of connections.  Clients includes regular connections
==================
*/
void SV_CountPlayers( int* clients, int* spectators )
{
	int			i;
	client_t* cl;

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

void SV_FindModelNumbers( void )
{
	int		i;

	sv_playermodel = -1;

	for (i = 0; i < MAX_MODELS; i++)
	{
		if (!sv.model_precache[i])
			break;
		if (!strcmp(sv.model_precache[i], "models/player.mdl"))
			sv_playermodel = i;
	}
}

/*
=============================================================================

EVENT MESSAGES

=============================================================================
*/

/*
==================
SV_StartParticle

Make sure the event gets sent to all clients
==================
*/
void SV_StartParticle( const vec_t* org, const vec_t* dir, int color, int count )
{
	int		i, v;

	if (sv.datagram.cursize > MAX_DATAGRAM - 16)
		return;
	MSG_WriteByte(&sv.datagram, svc_particle);
	MSG_WriteCoord(&sv.datagram, org[0]);
	MSG_WriteCoord(&sv.datagram, org[1]);
	MSG_WriteCoord(&sv.datagram, org[2]);
	for (i = 0; i < 3; i++)
	{
		v = dir[i] * 16;
		if (v > 127)
			v = 127;
		else if (v < -128)
			v = -128;
		MSG_WriteChar(&sv.datagram, v);
	}
	MSG_WriteByte(&sv.datagram, count);
	MSG_WriteByte(&sv.datagram, color);
}

/*
==================
SV_StartSound

Each entity can have eight independant sound sources, like voice,
weapon, feet, etc.

Channel 0 is an auto-allocate channel, the others override anything
allready running on that entity/channel pair.

An attenuation of 0 will play full volume everywhere in the level.
Larger attenuations will drop off.  (max 4 attenuation)

Pitch should be PITCH_NORM (100) for no pitch shift. Values over 100 (up to 255)
shift pitch higher, values lower than 100 lower the pitch.
==================
*/
void SV_StartSound( edict_t* entity, int channel, const char* sample, int volume, float attenuation, int fFlags, int pitch )
{
	int         sound_num;
	int			field_mask;
	int			i;
	int			ent;
	vec3_t		origin;

	if (volume < 0 || volume > 255)
		Sys_Error("SV_StartSound: volume = %i", volume);

	if (attenuation < 0 || attenuation > 4)
		Sys_Error("SV_StartSound: attenuation = %f", attenuation);

	if (channel < CHAN_AUTO || channel > CHAN_NETWORKVOICE_BASE)
		Sys_Error("SV_StartSound: channel = %i", channel);

	if (pitch < 0 || pitch > 255)
		Sys_Error("SV_StartSound: pitch = %i", pitch);

	// if this is a sentence, get sentence number
	if (sample[0] == CHAR_SENTENCE)
	{
		fFlags |= SND_SENTENCE;
		sound_num = atoi(sample + 1);
		if (sound_num >= CVOXFILESENTENCEMAX)
		{
			Con_Printf("invalid sentence number: %s", sample + 1);
			return;
		}
	}
	else
	{
// find precache number for sound
		for (sound_num = 1; sound_num < MAX_SOUNDS
			&& sv.sound_precache[sound_num]; sound_num++)
			if (!strcmp(sample, sv.sound_precache[sound_num]))
				break;

		if (sound_num == MAX_SOUNDS || !sv.sound_precache[sound_num])
		{
			Con_Printf("SV_StartSound: %s not precached (%d)\n", sample, sound_num);
			return;
		}
	}

	ent = NUM_FOR_EDICT(entity);

	for (i = 0; i < 3; i++)
		origin[i] = entity->v.origin[i] + (entity->v.mins[i] + entity->v.maxs[i]) * 0.5;

	channel = (ent << 3) | channel;

	field_mask = fFlags;

	if (volume != DEFAULT_SOUND_PACKET_VOLUME)
		field_mask |= SND_VOLUME;
	if (attenuation != DEFAULT_SOUND_PACKET_ATTENUATION)
		field_mask |= SND_ATTENUATION;
	if (pitch != DEFAULT_SOUND_PACKET_PITCH)
		field_mask |= SND_PITCH;
	if (sound_num > 255)
		field_mask |= SND_LARGE_INDEX;

// directed messages go only to the entity the are targeted on
	MSG_WriteByte(&sv.multicast, svc_sound);
	MSG_WriteByte(&sv.multicast, field_mask);
	if (field_mask & SND_VOLUME)
		MSG_WriteByte(&sv.multicast, volume);
	if (field_mask & SND_ATTENUATION)
		MSG_WriteByte(&sv.multicast, attenuation * 64);
	MSG_WriteShort(&sv.multicast, channel);
	if (sound_num > 255)
		MSG_WriteShort(&sv.multicast, sound_num);
	else
		MSG_WriteByte(&sv.multicast, sound_num);
	for (i = 0; i < 3; i++)
		MSG_WriteCoord(&sv.multicast, origin[i]);
	if (field_mask & SND_PITCH)
		MSG_WriteByte(&sv.multicast, pitch);
	if (channel == CHAN_STATIC)
		SV_Multicast(origin, MSG_BROADCAST, FALSE);
	else
		SV_Multicast(origin, MSG_ALL, FALSE);
}

/*
=============================================================================

MULTICAST MESSAGE

=============================================================================
*/

qboolean IsSinglePlayerGame( void )
{
	return svs.maxclients == 1;
}

/*
================
SV_ValidClientMulticast

Checks if a client should receive a multicast message
================
*/
qboolean SV_ValidClientMulticast( client_t* client, int soundLeaf, int to )
{
	unsigned char* mask;

	mask = NULL;

	if (IsSinglePlayerGame())
		return TRUE;

	if (to == MSG_FL_NONE)
	{
	}
	else if (to == MSG_FL_PVS)
	{
		mask = CM_LeafPVS(soundLeaf);
	}
	else if (to == MSG_FL_PAS)
	{
		mask = CM_LeafPAS(soundLeaf);
	}
	else
	{
		Con_Printf("MULTICAST: Error %d!\n", to);
		return FALSE;
	}

	if (!mask)
		return TRUE;

	int bitNumber;

	// Get the leaf number the client is on.
	bitNumber = SV_PointLeafnum(client->edict->v.origin);

	if (mask[(bitNumber - 1) >> 3] & (1 << ((bitNumber - 1) & 7)))
		return TRUE;

	return FALSE;
}

/*
=================
SV_Multicast

Sends the contents of sv.multicast to a subset of the clients,
then clears sv.multicast.
=================
*/
void SV_Multicast( vec_t* origin, int to, qboolean reliable )
{
	client_t* client;
	int		leafnum;
	int		j;
	sizebuf_t* pBuffer;

	leafnum = SV_PointLeafnum(origin);

	// send the data to all relevent clients
	for (j = 0, client = svs.clients; j < svs.maxclients; j++, client++)
	{
		if (!client->active)
			continue;

		if (SV_ValidClientMulticast(client, leafnum, to))
		{
			if (reliable)
			{
				pBuffer = &client->netchan.message;
			}
			else
			{
				pBuffer = &client->datagram;
			}

			if (pBuffer->maxsize - pBuffer->cursize > sv.multicast.cursize)
				SZ_Write(pBuffer, sv.multicast.data, sv.multicast.cursize);
		}
		else
		{
			gPacketSuppressed += sv.multicast.cursize;
		}
	}

	SZ_Clear(&sv.multicast);
}

/*
==============================================================================

CLIENT SPAWNING

==============================================================================
*/

/*
==================
SV_WriteMovevarsToClient

==================
*/
void SV_WriteMovevarsToClient( sizebuf_t* message )
{
	MSG_WriteByte(message, svc_newmovevars);
	MSG_WriteFloat(message, movevars.gravity);
	MSG_WriteFloat(message, movevars.stopspeed);
	MSG_WriteFloat(message, movevars.maxspeed);
	MSG_WriteFloat(message, movevars.spectatormaxspeed);
	MSG_WriteFloat(message, movevars.accelerate);
	MSG_WriteFloat(message, movevars.airaccelerate);
	MSG_WriteFloat(message, movevars.wateraccelerate);
	MSG_WriteFloat(message, movevars.friction);
	MSG_WriteFloat(message, movevars.edgefriction);
	MSG_WriteFloat(message, movevars.waterfriction);
	MSG_WriteFloat(message, movevars.entgravity);
	MSG_WriteFloat(message, movevars.bounce);
	MSG_WriteFloat(message, movevars.stepsize);
	MSG_WriteFloat(message, movevars.maxvelocity);
	MSG_WriteFloat(message, movevars.zmax);
	MSG_WriteFloat(message, movevars.waveHeight);
	MSG_WriteString(message, movevars.skyName);
}

/*
==================
SV_QueryMovevarsChanged

Check for changes in movevars and update them if necessary
==================
*/
void SV_QueryMovevarsChanged( void )
{
	int i;
	client_t* cl;

	if (movevars.maxspeed			== sv_maxspeed.value &&
		movevars.gravity			== sv_gravity.value &&
		movevars.stopspeed			== sv_stopspeed.value &&
		sv_spectatormaxspeed.value	== movevars.spectatormaxspeed &&
		movevars.accelerate			== sv_accelerate.value &&
		movevars.airaccelerate		== sv_airaccelerate.value &&
		movevars.wateraccelerate	== sv_wateraccelerate.value &&
		movevars.friction			== sv_friction.value &&
		movevars.edgefriction		== sv_edgefriction.value &&
		movevars.waterfriction		== sv_waterfriction.value &&
		movevars.entgravity			== 1.0 &&
		movevars.bounce				== sv_bounce.value &&
		movevars.stepsize			== sv_stepsize.value &&
		movevars.maxvelocity		== sv_maxvelocity.value &&
		movevars.zmax				== sv_zmax.value &&
		movevars.waveHeight			== sv_wateramp.value &&
		!strcmp(sv_skyname.string, movevars.skyName))
		return;

	SV_SetMoveVars();

	for (i = 0, cl = svs.clients; i < svs.maxclients; i++, cl++)
	{
		if (!cl->active && !cl->spawned && !cl->connected)
			continue;

		SV_WriteMovevarsToClient(&cl->netchan.message);
	}
}

/*
================
SV_SendServerinfo

Sends the first message from the server to a connected client.
This will be sent on the initial connection and upon each server load.
================
*/
void SV_SendServerinfo( client_t *client )
{
	char			message[2048];
	int				playernum;

	// Only send this message to developer console, or multiplayer clients.
	if ((developer.value) || (svs.maxclients > 1))
	{
		MSG_WriteByte(&client->netchan.message, svc_print);
		sprintf(message, "%c\nBUILD %d SERVER (%i CRC)\nServer # %i\n", 0x2, build_number(), 0, svs.spawncount);
		MSG_WriteString(&client->netchan.message, message);
	}

	MSG_WriteByte(&client->netchan.message, svc_serverinfo);
	MSG_WriteLong(&client->netchan.message, PROTOCOL_VERSION);
	MSG_WriteLong(&client->netchan.message, svs.spawncount);
	MSG_WriteLong(&client->netchan.message, sv.worldmapCRC);       // To prevent cheating with hacked maps
	MSG_WriteLong(&client->netchan.message, sv.clientSideDllCRC);  // To prevent cheating with hacked client dlls
	MSG_WriteByte(&client->netchan.message, svs.maxclients);

	playernum = NUM_FOR_EDICT(client->edict) - 1;
	if (client->spectator)
		playernum |= PN_SPECTATOR;
	MSG_WriteByte(&client->netchan.message, playernum);

	if (!coop.value && deathmatch.value)
		MSG_WriteByte(&client->netchan.message, GAME_DEATHMATCH);
	else
		MSG_WriteByte(&client->netchan.message, GAME_COOP);

	sprintf(message, pr_strings + sv.edicts->v.message);

	MSG_WriteString(&client->netchan.message, message);

	SV_WriteMovevarsToClient(&host_client->netchan.message);

// send music
	MSG_WriteByte(&client->netchan.message, svc_cdtrack);
	MSG_WriteByte(&client->netchan.message, gGlobalVariables.cdAudioTrack);
	MSG_WriteByte(&client->netchan.message, gGlobalVariables.cdAudioTrack);

// set view
	MSG_WriteByte(&client->netchan.message, svc_setview);
	MSG_WriteShort(&client->netchan.message, NUM_FOR_EDICT(client->edict));

// send resource request
	MSG_WriteByte(&client->netchan.message, svc_resourcerequest);
	MSG_WriteLong(&client->netchan.message, svs.spawncount);
	MSG_WriteLong(&client->netchan.message, 0);

	client->connected = TRUE;
	client->sendsignon = TRUE;
	client->spawned = FALSE;
}

/*
================
SV_New_f

Sends the first message from the server to a connected client.
This will be sent on the initial connection and upon each server load.
================
*/
void SV_New_f( void )
{
	int		size1, size2, size3, size4;
	edict_t* ent;

	if (host_client->spawned && !host_client->active)
		return;

	ent = host_client->edict;

	host_client->connected = TRUE;
	host_client->connection_started = realtime;

	SZ_Clear(&host_client->netchan.message);

	SV_SendServerinfo(host_client);

	size1 = host_client->netchan.message.cursize;
	if (sv_netsize.value)
		Con_DPrintf("SINFO=%i\n", size1);

	size2 = host_client->netchan.message.cursize;
	if (sv_netsize.value)
		Con_DPrintf("DECALS=%i\n", size2 - size1);

	// Send user messages
	if (sv_gpUserMsgs)
	{
		UserMsg* pTemp;

		pTemp = sv_gpNewUserMsgs;
		sv_gpNewUserMsgs = sv_gpUserMsgs;
		SV_SendUserReg(&host_client->netchan.message);
		sv_gpNewUserMsgs = pTemp;
	}

	size3 = host_client->netchan.message.cursize;
	if (sv_netsize.value)
		Con_DPrintf("USR=%i\n", size3 - size2);

	if (host_client->spectator)
	{
		SV_SpawnSpectator();

		// call the spawn function
		gGlobalVariables.time = sv.time;
		gEntityInterface.pfnParmsNewLevel();
		gEntityInterface.pfnSpectatorConnect(ent);
	}
	else
	{
		// call the spawn function
		gEntityInterface.pfnClientConnect(ent);
	}

	size4 = host_client->netchan.message.cursize;
	if (sv_netsize.value)
		Con_DPrintf("CLSIZE = %i\n", size4 - size3);

	net_activeconnections++;
}

/*
=================
SV_PTrack_f

Change the bandwidth estimate for a client
=================
*/
void SV_PTrack_f( void )
{
	int i;

	if (Cmd_Argc() != 2)
	{
		// turn off tracking
		host_client->spec_track = 0;
		return;
	}

	i = atoi(Cmd_Argv(1));
	if (i < 0 || i >= svs.maxclientslimit || !svs.clients[i].active ||
		svs.clients[i].spectator)
	{
		SV_ClientPrintf("Invalid client to track\n");
		host_client->spec_track = 0;
		return;
	}
	host_client->spec_track = i + 1; // now tracking
}

/*
================
SV_RejectConnection

Rejects connection request and sends back a message
================
*/
void SV_RejectConnection( netadr_t* adr, char* text )
{
	SZ_Clear(&net_message);
	MSG_WriteLong(&net_message, -1); // -1 -1 -1 -1 signal
	MSG_WriteByte(&net_message, A2C_PRINT);
	MSG_WriteString(&net_message, text);
	NET_SendPacket(NS_SERVER, net_message.cursize, net_message.data, *adr);
	SZ_Clear(&net_message);
}

/*
==================
SV_SpawnSpectator
==================
*/
void SV_SpawnSpectator( void )
{
	int		i;
	edict_t* e;

	VectorCopy(vec3_origin, sv_player->v.origin);
	VectorCopy(vec3_origin, sv_player->v.view_ofs);
	sv_player->v.view_ofs[2] = 22;

	// search for an info_playerstart to spawn the spectator at
	for (i = MAX_CLIENTS - 1; i < sv.num_edicts; i++)
	{
		e = EDICT_NUM(i);
		if (!strcmp(pr_strings + e->v.classname, "info_player_start"))
		{
			VectorCopy(e->v.origin, sv_player->v.origin);
			return;
		}
	}
}

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
	edict_t* ent;
	client_t* client;
	netadr_t		adr;
	int				edictnum;
	int				i;
	char* s;
	char			name[128];
	char			szRawCertificate[512];
	int				clients, spectators;
	qboolean		spectator = FALSE;
	int				nAuthProtocol;
	int				nCDKeyLength;
	int				nHashCount = 0;
	int				version;
	int				challenge;

	adr = net_from;

	version = atoi(Cmd_Argv(1));
	if (version != PROTOCOL_VERSION)
	{
		SV_RejectConnection(&adr, "Bad protocol\n");
		return;
	}

	if (Cmd_Argc() < 8)
	{
		SV_RejectConnection(&adr, "Insufficient connection info\n");
		return;
	}

	challenge = atoi(Cmd_Argv(2));

	// see if the challenge is valid
	if (!NET_IsLocalAddress(adr))
	{
		for (i = 0; i < MAX_CHALLENGES; i++)
		{
			if (NET_CompareClassBAdr(net_from, g_rg_sv_challenges[i].adr))
			{
				if (challenge == g_rg_sv_challenges[i].challenge)
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

	// Client connection has passed challenge response.
	// if there is allready a slot for this ip, drop it
	for (i = 0, client = svs.clients; i < svs.maxclientslimit; i++, client++)
	{
		if (!client->active && !client->spawned && !client->connected)
			continue;

		if (NET_CompareAdr(adr, client->netchan.remote_address))
		{
			Con_Printf("%s:reconnect\n", NET_AdrToString(adr));
			SV_DropClient(client, FALSE);
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

	// Check protocol ID
	nAuthProtocol = atoi(s);
	if ((nAuthProtocol <= 0) || (nAuthProtocol > PROTOCOL_LASTVALID))
	{
		SV_RejectConnection(&adr, "Invalid connection.\n");
		return;
	}

	s = Cmd_Argv(5);
	if (!s || !s[0])
	{
		SV_RejectConnection(&adr, "Invalid connection.\n");
		return;
	}

	nCDKeyLength = atoi(s);
	if ((nCDKeyLength <= 0) || (nCDKeyLength > 1024))
	{
		SV_RejectConnection(&adr, "Invalid connection.\n");
		return;
	}

	// Now check auth information
	s = Cmd_Argv(6);
	if (!s)
	{
		SV_RejectConnection(&adr, "Invalid connection protocol.\n");
		return;
	}

	memset(szRawCertificate, 0, sizeof(szRawCertificate));
	strcpy(szRawCertificate, s);

	if (nAuthProtocol != PROTOCOL_HASHEDCDKEY)
	{
		SV_RejectConnection(&adr, "Invalid connection protocol.\n");
		return;
	}

	// HASHED CD KEY VALIDATION
	if (strlen(szRawCertificate) != 32 || nCDKeyLength != 32)
	{
		SV_RejectConnection(&adr, "Invalid CD Key.\n");
		return;
	}

	// Now make sure that this hash isn't "overused"
	for (i = 0, client = svs.clients; i < svs.maxclientslimit; i++, client++)
	{
		if (!client->active && !client->spawned && !client->connected)
			continue;

		if (_strnicmp(szRawCertificate, client->hashedcdkey, 32))
			continue;

		nHashCount++;
	}

	if (nHashCount >= MAX_IDENTICAL_CDKEYS)
	{
		SV_RejectConnection(&adr, "CD Key already in use.\n");
		return;
	}

	// check for password
	if (Cmd_Argc() >= 8)
	{
		s = Cmd_Argv(7);
		if (s[0])
		{
			if (sv_password.string[0] &&
				_stricmp(sv_password.string, "none") &&
				strcmp(sv_password.string, s))
			{
				Con_Printf("%s:  password failed\n", NET_AdrToString(net_from));
				SV_RejectConnection(&net_from, "Invalid server password.\n");
				return;
			}
		}
	}

	// check for spectator_password
	if (Cmd_Argc() >= 9)
	{
		s = Cmd_Argv(8);
		if (s[0])
		{
			if (spectator_password.string[0] &&
				_stricmp(spectator_password.string, "none") &&
				strcmp(spectator_password.string, s))
			{	// failed
				Con_Printf("%s:spectator password failed\n", NET_AdrToString(net_from));
				SV_RejectConnection(&net_from, "Invalid spectator password.\n");
				return;
			}
			spectator = TRUE;
		}
	}

	// count up the clients and spectators
	spectators = 0;
	clients = 0;
	SV_CountPlayers(&clients, &spectators);
	clients -= spectators;

	if (spectator && (spectators >= max_spectators.value))
	{
		Con_Printf("%s:No space for spectator\n", NET_AdrToString(net_from));
		SV_RejectConnection(&net_from, "No more spectators allowed.\n");
		return;
	}

	for (i = 0, client = svs.clients; i < svs.maxclients; i++, client++)
	{
		if (!client->active && !client->spawned && !client->connected)
			break;
	}

	if (i == svs.maxclients)
	{
		SV_RejectConnection(&net_from, "Server is full.");
		return;
	}

	// Store off the potential client number
	edictnum = i + 1;
	ent = EDICT_NUM(edictnum);
	host_client = client;

	SV_ClearResourceLists(client);
	memset(client, 0, sizeof(client_t));

	client->resourcesneeded.pPrev = &client->resourcesneeded;
	client->resourcesneeded.pNext = &client->resourcesneeded;
	client->resourcesonhand.pPrev = &client->resourcesonhand;
	client->resourcesonhand.pNext = &client->resourcesonhand;

////////////////////////////////////////////////
// Client can connect
//
	// Set up the network channel.
	Netchan_Setup(NS_SERVER, &client->netchan, adr);

	// Tell client connection worked.
	Netchan_OutOfBandPrint(NS_SERVER, adr, "%c", S2C_CONNECTION);

	// Display debug message.
	if (!_stricmp(NET_AdrToString(client->netchan.remote_address), adr.ip))
	{
		Con_DPrintf("Local connection.\n");
	}
	else
	{
		if (spectator)
			Con_DPrintf("Spectator %s connected\nAdr: %s\n", name, NET_AdrToString(client->netchan.remote_address));
		else
			Con_DPrintf("Client %s connected\nAdr: %s\n", name, NET_AdrToString(client->netchan.remote_address));
	}

	// Set up client structure.
	strcpy(client->name, name);
	strncpy(client->hashedcdkey, szRawCertificate, sizeof(client->hashedcdkey) - 1);
	client->hashedcdkey[sizeof(client->hashedcdkey) - 1] = 0;

	client->active = FALSE;
	client->spawned = FALSE;
	client->uploading = FALSE;

	client->edict = ent;
	client->maxspeed = 0;
	client->spectator = spectator;
	client->connected = TRUE;

	if (client->spectator)
		client->edict->v.flags |= FL_SPECTATOR;

	client->privileged = FALSE;

	// Set up the datagram buffer for this client.
	client->datagram.allowoverflow = TRUE;
	client->datagram.maxsize = MAX_DATAGRAM;
	client->datagram.data = client->datagram_buf;

	if (!sv.loadgame)
		gEntityInterface.pfnParmsNewLevel();
}


/*
==============================================================================

CONNECTIONLESS COMMANDS

==============================================================================
*/

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
SVC_Status

Responds with all the info that qplug or qspy can see
This message can be up to around 5k with worst case string lengths.
================
*/
void SVC_Status( void )
{
}

/*
=================
SVC_GetChallenge

Returns a challenge number that can be used
in a subsequent client_connect command.
We do this to prevent denial of service attacks that
flood the server with invalid connection IPs.  With a
challenge, they must give a valid IP address.
=================
*/
void SVC_GetChallenge( void )
{
	int		i;
	int		oldest;
	int		oldestTime;
	char	data[8 + 1];

	oldest = 0;
	oldestTime = 0x7fffffff;

	// see if we already have a challenge for this ip
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
		// overwrite the oldest
		g_rg_sv_challenges[oldest].challenge = (RandomLong(0, 0xFFFF) << 16 | RandomLong(0, 0xFFFF));
		g_rg_sv_challenges[oldest].adr = net_from;
		g_rg_sv_challenges[oldest].time = realtime;
		i = oldest;
	}

	// send it back
	sprintf(data, "%c%c%c%c%c", 255, 255, 255, 255, S2C_CHALLENGE);
	*(int*)(data + 5) = BigLong(g_rg_sv_challenges[i].challenge);
	NET_SendPacket(NS_SERVER, 9, data, net_from);
}

/*
================
SVC_Info

Responds with short or long info for broadcast scans
================
*/
void SVC_Info( void )
{
	int		i, count;
	sizebuf_t buf;

	if (!sv.active)            // Must be running a server.
		return;

	if (svs.maxclients <= 1)   // ignore in single player
		return;

	if (!noip.value && NET_CompareClassBAdr(net_local_adr, net_from))
		return;
#ifdef _WIN32
	if (!noipx.value && NET_CompareClassBAdr(net_local_ipx_adr, net_from))
		return;
#endif //_WIN32

	count = 0;
	for (i = 0; i < svs.maxclients; i++)
	{
		if (svs.clients[i].active)
			count++;
	}

	MSG_WriteLong(&buf, 0xffffffff);
	MSG_WriteByte(&buf, S2A_INFO);

	// Send the IP address
	MSG_WriteString(&buf, NET_AdrToString(net_local_adr));

	MSG_WriteString(&buf, host_name.string);
	MSG_WriteString(&buf, sv.name);

	MSG_WriteString(&buf, com_gamedir);
	MSG_WriteString(&buf, gEntityInterface.pfnGetGameDescription());

	MSG_WriteByte(&buf, count);
	MSG_WriteByte(&buf, svs.maxclients);
	MSG_WriteByte(&buf, 7);

	NET_SendPacket(NS_SERVER, buf.cursize, buf.data, net_from);
}

/*
=================
SVC_PlayerInfo

Returns info about requested player.
=================
*/
void SVC_PlayerInfo( void )
{
	int		i, count;
	client_t* client;
	sizebuf_t buf;
	byte	data[2048];

	buf.data = data;
	buf.maxsize = sizeof(data);
	buf.cursize = 0;

	if (!sv.active)            // Must be running a server.
		return;

	if (svs.maxclients <= 1)   // ignore in single player
		return;

	// Find Player
	MSG_WriteLong(&buf, -1);
	MSG_WriteByte(&buf, S2A_PLAYER);

	count = 0;
	for (i = 0, client = svs.clients; i < svs.maxclients; i++, client++)
	{
		if (!svs.clients[i].active)
			continue;

		count++;
	}

	MSG_WriteByte(&buf, count);
	count = 0;
	for (i = 0, client = svs.clients; i < svs.maxclients; i++, client++)
	{
		if (!svs.clients[i].active)
			continue;

		count++;

		MSG_WriteByte(&buf, count);
		MSG_WriteString(&buf, client->name);
		MSG_WriteLong(&buf, client->edict->v.frags);
		MSG_WriteFloat(&buf, realtime - client->netchan.connect_time);
	}

	NET_SendPacket(NS_SERVER, buf.cursize, buf.data, net_from);
}

/*
=================
SVC_RuleInfo

More detailed server information
=================
*/
void SVC_RuleInfo( void )
{
	int		nNumRules;
	cvar_t* var;
	sizebuf_t buf;
	byte	data[2048];

	buf.data = data;
	buf.maxsize = sizeof(data);
	buf.cursize = 0;

	if (!sv.active)            // Must be running a server.
		return;
	
	if (svs.maxclients <= 1)   // ignore in single player
		return;

	nNumRules = Cvar_CountServerVariables();
	if (nNumRules <= 0)        // No server rules active.
		return;

	// Find Player
	MSG_WriteLong(&buf, -1);
	MSG_WriteByte(&buf, S2A_RULES);  // All players coming now.
	MSG_WriteShort(&buf, nNumRules);

	// Need to respond with game directory, game name, and any server variables that have been set that
	//  effect rules.  Also, probably need a hook into the .dll to respond with additional rule information.
	for (var = cvar_vars; var; var = var->next)
	{
		if (!var->server)
			continue;

		MSG_WriteString(&buf, var->name);   // Cvar Name
		MSG_WriteString(&buf, var->string); // Value
	}

	NET_SendPacket(NS_SERVER, buf.cursize, buf.data, net_from);
}

/*
=================
SV_SetMasterPeeringMessage

Set master peering message
=================
*/
char g_szMasterMsg[1024];
int g_iMasterMsgSize;
void SV_SetMasterPeeringMessage( qboolean skipHeader )
{
	char	pBuf[1024];
	int		nSize;

	if (skipHeader)
	{
		msg_readcount = 0;
		MSG_ReadLong();
		MSG_ReadByte();
		MSG_ReadByte();
	}

	nSize = MSG_ReadLong();
	if (nSize <= 0 || nSize >= sizeof(pBuf))
		return;

	if (MSG_ReadBuf(nSize, pBuf) == -1)
		return;

	g_iMasterMsgSize = nSize;
	memcpy(g_szMasterMsg, pBuf, g_iMasterMsgSize);
}

/*
=================
SVC_Heartbeat

=================
*/
void SVC_Heartbeat( void )
{
	gfHeartbeatWaiting = FALSE;                   // // Kill timer
	gHeartbeatChallenge = MSG_ReadLong();

	if (MSG_ReadByte() == M2A_ACTIVEMODS)
		SV_SetMasterPeeringMessage(FALSE);

	// Send the actual heartbeat request to this master server.
	Master_RequestHeartbeat();
}

/*
=================
SVC_MasterPrint

Master message
=================
*/
float gfLastMasterPrintTime = 0.0f;
void SVC_MasterPrint( void )
{
	char	pBuf[1024];
	byte	pSign[1024];
	int		nSize, nSignSize;
	char* pMsg;

	if (!g_iMasterMsgSize)
		return;

	msg_readcount = 0;
	MSG_ReadLong();
	MSG_ReadByte();
	MSG_ReadByte();

	nSize = MSG_ReadLong();
	if (nSize <= 0 || nSize >= sizeof(pBuf))
		return;

	if (MSG_ReadBuf(nSize, pBuf) == -1)
		return;

	nSignSize = MSG_ReadLong();
	if (nSignSize <= 0 || nSignSize >= sizeof(pSign))
		return;

	if (MSG_ReadBuf(nSignSize, pSign) == -1)
		return;

	// Verify the message
	pMsg = Launcher_VerifyMessage(g_iMasterMsgSize, g_szMasterMsg, nSize, pBuf, nSignSize, pSign);
	if (!pMsg)
		return;

	if (sv_masterprint.value)
	{
		if ((realtime - gfLastMasterPrintTime) >= sv_masterprinttime.value)
		{
			gfLastMasterPrintTime = realtime;
			Con_Printf("%s\n", pMsg);
		}
	}
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
	char* s;
	char* c;

	MSG_BeginReading();
	MSG_ReadLong();		// skip the -1 marker

	s = MSG_ReadStringLine();
	Cmd_TokenizeString(s);

	c = Cmd_Argv(0);

	if (!strcmp(c, "ping") ||
		(c[0] == A2A_PING && (c[1] == 0 || c[1] == '\n')))
	{
		SVC_Ping();
		return;
	}
	else if (c[0] == A2A_ACK && (c[1] == 0 || c[1] == '\n'))
	{
		Con_Printf("A2A_ACK from %s\n", NET_AdrToString(net_from));
		return;
	}
	else if (c[0] == M2A_CHALLENGE && (c[1] == 0 || c[1] == '\n'))
	{
		SVC_Heartbeat();
		return;
	}
	else if (c[0] == M2M_MSG && (c[1] == 0 || c[1] == '\n'))
	{
		if (NET_CompareClassBAdr(gMasterAddress, net_from))
			SVC_MasterPrint();
		return;
	}
	else if (!strcmp(c, "status"))
	{
		SVC_Status();
		return;
	}
	else if (!strcmp(c, "getchallenge"))
	{
		SVC_GetChallenge();
		return;
	}
	else if (!_stricmp(c, "info"))
	{
		SVC_Info();
		return;
	}
	else if (!_stricmp(c, "players")) // Player info request.
	{
		SVC_PlayerInfo();
		return;
	}
	else if (!_stricmp(c, "rules"))   // Rule info request.
	{
		SVC_RuleInfo();
		return;
	}
	else if (!strcmp(c, "connect"))  // Must include correct challenge #
	{
		SV_ConnectClient();
		return;
	}
	else if (!strcmp(c, "rcon"))
	{
		Host_RemoteCommand(&net_from);
	}
	else
	{
		// Just ignore it.
		Con_Printf("bad connectionless packet from %s:\n%s\n",
			NET_AdrToString(net_from), s);
	}
}

//============================================================================

/*
=================
SV_ReadPackets

Read's packets from clients and executes messages as appropriate.
=================
*/
void SV_ReadPackets( void )
{
	int			i;
	client_t* cl;
	float		start, end;
	float		time1, time2, time3, time4, time5, time6;
	float		packettime, exectime, processtime;

	packettime = 0;
	exectime = 0;
	processtime = 0;

	if (host_speeds.value == 2)
		start = Sys_FloatTime();

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
			packettime += time2 - time1;

		// check for connectionless packet (0xffffffff) first
		if (*(int*)net_message.data == -1)
		{
			SV_ConnectionlessPacket();
			continue;
		}

		// check for packets from connected clients
		for (i = 0, cl = svs.clients; i < svs.maxclientslimit; i++, cl++)
		{
			if (!cl->connected && !cl->active && !cl->spawned)
				continue;
			if (!NET_CompareAdr(net_from, cl->netchan.remote_address))
				continue;

			time3 = Sys_FloatTime();

			if (Netchan_Process(&cl->netchan))
			{	// this is a valid, sequenced packet, so process it
				svs.stats.packets++;
				cl->send_message = TRUE;	// reply at end of frame

				time4 = Sys_FloatTime();
				SV_ExecuteClientMessage(cl);
				time5 = Sys_FloatTime();

				if (host_speeds.value == 2)
					exectime += time5 - time4;
			}

			time6 = Sys_FloatTime();

			if (host_speeds.value == 2)
				processtime += time6 - time3;
		}

		if (i != MAX_CLIENTS)
			continue;

		// packet is not from a known client
		//	Con_Printf("%s:sequenced packet without connection\n"
		// , NET_AdrToString(net_from));
	}

	if (host_speeds.value == 2)
		end = Sys_FloatTime();
}

/*
==================
SV_CheckTimeouts

If a packet has not been received from a client in sv_timeout.GetFloat()
seconds, drop the conneciton.

When a client is normally dropped, the client_t goes into a zombie state
for a few seconds to make sure any final reliable message gets resent
if necessary
==================
*/
void SV_CheckTimeouts( void )
{
	int		    i;
	client_t* cl;
	float	    droptime;

	droptime = realtime - timeout.value;

	for (i = 0, cl = svs.clients; i < svs.maxclientslimit; i++, cl++)
	{
		if (cl->fakeclient)
			continue;

		if ((cl->connected || cl->active || cl->spawned) &&
			(cl->netchan.last_received < droptime))
		{
			SV_BroadcastPrintf("%s timed out\n", cl->name);
			SV_DropClient(cl, FALSE);
			cl->active = FALSE;
			cl->spawned = FALSE;
			cl->connected = FALSE;
		}
	}
}

int SV_CalcPing( client_t* cl )
{
	float		ping;
	int			i;
	int			count;
	client_frame_t* frame;
	int idx;

	idx = cl - svs.clients;

	ping = 0;
	count = 0;

	for (i = 0; i < UPDATE_BACKUP; i++)
	{
		frame = &cl->frames[i];
		if (frame->ping_time > 0)
		{
			ping += frame->ping_time;
			count++;
		}
	}

	if (!count)
		return 9999;
	ping /= count;
	return (ping * 1000);
}

/*
===================
SV_FullClientUpdate

sends all the info about *cl to *sb
===================
*/
void SV_FullClientUpdate( client_t* cl, sizebuf_t* sb )
{
	int		i;
	int		playernum;
	client_t* client;

// send notification to all clients
	for (i = 0, client = svs.clients; i < svs.maxclients; i++, client++)
	{
		if (!client->active)
			continue;
		if (client->fakeclient)
			continue;
		if (cl == client)
			continue;

		MSG_WriteByte(sb, svc_updatename);
		playernum = cl - svs.clients;
		if (!cl->active && !cl->spawned && !cl->name[0])
			playernum |= PN_SPECTATOR;
		MSG_WriteByte(sb, playernum);
		MSG_WriteString(sb, cl->name);
		MSG_WriteByte(sb, svc_updatecolors);
		MSG_WriteByte(sb, cl - svs.clients);
		MSG_WriteByte(sb, 0);
	}
}

/*
===============================================================================

FRAME UPDATES

===============================================================================
*/

/*
==================
SV_ClearDatagram

==================
*/
void SV_ClearDatagram( void )
{
	SZ_Clear(&sv.datagram);
}

/*
=============================================================================

The PVS must include a small area around the client to allow head bobbing
or other small motion on the client side.  Otherwise, a bob might cause an
entity that should be visible to not show up, especially when the bob
crosses a waterline.

=============================================================================
*/

int		fatbytes;
byte	fatpvs[MAX_MAP_LEAFS / 8];
void SV_AddToFatPVS( vec_t* org, mnode_t* node )
{
	int		i;
	byte* pvs;
	mplane_t* plane;
	float	d;

	while (1)
	{
	// if this is a leaf, accumulate the pvs bits
		if (node->contents < 0)
		{
			if (node->contents != CONTENTS_SOLID)
			{
				pvs = Mod_LeafPVS((mleaf_t*)node, sv.worldmodel);
				for (i = 0; i < fatbytes; i++)
					fatpvs[i] |= pvs[i];
			}
			return;
		}

		plane = node->plane;
		d = DotProduct(org, plane->normal) - plane->dist;
		if (d > 8)
			node = node->children[0];
		else if (d < -8)
			node = node->children[1];
		else
		{	// go down both
			SV_AddToFatPVS(org, node->children[0]);
			node = node->children[1];
		}
	}
}

/*
=============
SV_FatPVS

Calculates a PVS that is the inclusive or of all leafs within 8 pixels of the
given point.
=============
*/
byte* SV_FatPVS( float* org )
{
	fatbytes = (sv.worldmodel->numleafs + 31) >> 3;
	Q_memset(fatpvs, 0, fatbytes);
	SV_AddToFatPVS(org, sv.worldmodel->nodes);
	return fatpvs;
}

//=============================================================================

int SV_PointLeafnum( vec_t* p )
{
	mleaf_t* pLeaf;

	pLeaf = Mod_PointInLeaf(p, sv.worldmodel);
	if (pLeaf)
		return pLeaf - sv.worldmodel->leafs;

	return 0;
}

/*
===============
SV_MemPrediction_f

Show memory usage stats
===============
*/
void SV_MemPrediction_f( void )
{
	int		i, j;
	int		totalSize;
	int		totalEnts;
	int		totalClients;
	int		totalPackets = 0;
	int		entCount, entSize;
	float	avgEnts, avgSize;
	client_t* cl;
	client_frame_t* frame;

	if (!sv.active)
		return;

	totalSize = 0;
	totalEnts = 0;
	totalClients = 0;

	// Scan through all client slots
	for (i = 0, cl = svs.clients; i < svs.maxclients; i++, cl++)
	{
		entSize = 0;

		if (!cl->active)
			continue;
		
		entCount = 0;
		totalClients++;

		// Analyze last frames
		for (j = 0; j < UPDATE_BACKUP; j++)
		{
			frame = &cl->frames[j & UPDATE_MASK];
			// Only count frames with entities
			if (frame->entities.num_entities > 0)
			{
				entCount += frame->entities.num_entities;
				entSize += sizeof(entity_state_t) * frame->entities.num_entities;
				totalPackets++;
			}
		}

		totalSize += entSize;
		totalEnts += entCount;
	}

	// Calculate averages
	if (totalPackets)
	{
		avgSize = totalSize / (float)totalPackets;
		avgEnts = totalEnts / (float)totalPackets;
	}
	else
	{
		avgEnts = 0.0f;
		avgSize = 0.0f;
	}

	Con_Printf("--- Prediction Memory Usage ---\n");
	Con_Printf("# Clients  :  %i\n", totalClients);
	Con_Printf("# Packets  :  %i\n", totalPackets);
	Con_Printf("# Entities :  %i\n", totalEnts);
	Con_Printf("Total Size :  %i\n", totalSize);
	Con_Printf("Avg. Ents  :  %.2f\n", avgEnts);
	Con_Printf("Avg. Size  :  %.2f\n", avgSize);
	Con_Printf("\n");
}

//=============================================================================

/*
==================
SV_WriteCustomEntityDeltaToClient

Writes part of a custom entity message.
Can delta from either a baseline or a previous packet_entity
==================
*/
void SV_WriteCustomEntityDeltaToClient( entity_state_t* from, entity_state_t* to, sizebuf_t* msg, qboolean force )
{
	int		bits;
	int		i;
	int		rendermode;
	float	miss;

// send an update
	if (to->rendermode == from->rendermode)
		bits = (U_MOREBITS | U_EVENMOREBITS | U_CUSTOM);
	else
		bits = (U_MOREBITS | U_EVENMOREBITS | U_BEAM_TYPE | U_CUSTOM);		

	rendermode = (to->rendermode & 15);
	if (rendermode == kRenderNormal || rendermode == kRenderTransColor)
	{
		for (i = 0; i < 3; i++)
		{
			miss = to->origin[i] - from->origin[i];
			if (miss < -0.1 || miss > 0.1)
				bits |= U_BEAM_STARTX << i;
		}

		if (rendermode == kRenderNormal)
		{
			for (i = 0; i < 3; i++)
			{
				miss = to->angles[i] - from->angles[i];
				if (miss < -0.1 || miss > 0.1)
					bits |= U_BEAM_ENDX << i;
			}
		}
	}

	if ((rendermode == kRenderTransColor || rendermode == kRenderTransTexture) &&
		(to->sequence != from->sequence || to->skin != from->skin))
		bits |= U_BEAM_ENTS;

	if (from->modelindex != to->modelindex)
		bits |= U_BEAM_MODEL;

	if (from->scale != to->scale)
		bits |= U_BEAM_WIDTH;

	if (from->body != to->body)
		bits |= U_BEAM_NOISE;

	if (to->rendercolor.r != from->rendercolor.r ||
		to->rendercolor.g != from->rendercolor.g ||
		to->rendercolor.b != from->rendercolor.b ||
		to->renderfx != from->renderfx)
	{
		bits |= U_BEAM_RENDER;
	}

	if (from->renderamt != to->renderamt)
		bits |= U_BEAM_BRIGHTNESS;

	if (from->frame != to->frame)
		bits |= U_BEAM_FRAME;

	if ((int)from->animtime != (int)to->animtime)
		bits |= U_BEAM_SCROLL;

	if (to->number > 255)
		bits |= U_LONGENTITY;

	if (bits > 0xFF)
		bits |= U_MOREBITS;
	if (bits > 0xFFFF)
		bits |= U_EVENMOREBITS;
	if (bits > 0xFFFFFF)
		bits |= U_YETMOREBITS;

	//
	// write the message
	//
	if (!to->number)
		Sys_Error("Unset entity number");

	if (!bits && !force)
		return;		// nothing to send!

	MSG_WriteByte(msg, bits & 255);

	if (bits & U_MOREBITS)
		MSG_WriteByte(msg, (bits >> 8) & 255);
	if (bits & U_EVENMOREBITS)
		MSG_WriteByte(msg, (bits >> 16) & 255);
	if (bits & U_YETMOREBITS)
		MSG_WriteByte(msg, (bits >> 24) & 255);

	if (bits & U_LONGENTITY)
		MSG_WriteShort(msg, to->number);
	else
		MSG_WriteByte(msg, to->number);

	if (bits & U_BEAM_STARTX)
		MSG_WriteCoord(msg, to->origin[0]);
	if (bits & U_BEAM_STARTY)
		MSG_WriteCoord(msg, to->origin[1]);
	if (bits & U_BEAM_STARTZ)
		MSG_WriteCoord(msg, to->origin[2]);
	if (bits & U_BEAM_ENDX)
		MSG_WriteCoord(msg, to->angles[0]);
	if (bits & U_BEAM_ENDY)
		MSG_WriteCoord(msg, to->angles[1]);
	if (bits & U_BEAM_ENDZ)
		MSG_WriteCoord(msg, to->angles[2]);

	if (bits & U_BEAM_ENTS)
	{
		MSG_WriteShort(msg, to->sequence);
		MSG_WriteShort(msg, to->skin);
	}

	if (bits & U_BEAM_TYPE)
		MSG_WriteByte(msg, to->rendermode);

	if (bits & U_BEAM_MODEL)
		MSG_WriteShort(msg, to->modelindex);

	if (bits & U_BEAM_WIDTH)
		MSG_WriteByte(msg, to->scale);

	if (bits & U_BEAM_NOISE)
		MSG_WriteByte(msg, to->body);

	if (bits & U_BEAM_RENDER)
	{
		MSG_WriteByte(msg, to->rendercolor.r);
		MSG_WriteByte(msg, to->rendercolor.g);
		MSG_WriteByte(msg, to->rendercolor.b);
		MSG_WriteByte(msg, to->renderfx);
	}

	if (bits & U_BEAM_BRIGHTNESS)
		MSG_WriteByte(msg, to->renderamt);

	if (bits & U_BEAM_FRAME)
		MSG_WriteByte(msg, to->frame);

	if (bits & U_BEAM_SCROLL)
		MSG_WriteByte(msg, to->animtime);
}

/*
==================
SV_WriteDelta

Writes part of a packetentities message.
Can delta from either a baseline or a previous packet_entity
==================
*/
void SV_WriteDelta( entity_state_t* from, entity_state_t* to, sizebuf_t* msg, qboolean force )
{
	int     bits, bboxbits;
	int		i;
	float   miss;

	if (to->entityType != ENTITY_NORMAL)
	{
		SV_WriteCustomEntityDeltaToClient(from, to, msg, force);
		return;
	}

// send an update
	bits = 0;
	bboxbits = 0;

	for (i = 0; i < 3; i++)
	{
		miss = to->origin[i] - from->origin[i];
		if (miss < -0.1 || miss > 0.1)
			bits |= (U_ORIGIN1 << i);
	}

	if (to->angles[0] != from->angles[0])
		bits |= U_ANGLE1;

	if (to->angles[1] != from->angles[1])
		bits |= U_ANGLE2;

	if (to->angles[2] != from->angles[2])
		bits |= U_ANGLE3;

	if (to->movetype != from->movetype)
		bits |= U_MOVETYPE;

	if (to->colormap != from->colormap)
		bits |= U_COLORMAP;

	if (to->skin != from->skin || to->solid != from->solid)
		bits |= U_CONTENTS;

	if (to->frame != from->frame)
		bits |= U_FRAME;

	if (to->scale != from->scale)
		bits |= U_SCALE;

	if (to->effects != from->effects)
		bits |= U_EFFECTS;

	if (to->modelindex != from->modelindex )
		bits |= U_MODELINDEX;

	if (to->animtime != from->animtime || to->sequence != from->sequence)
		bits |= U_SEQUENCE;

	if (to->framerate != from->framerate)
		bits |= U_FRAMERATE;

	if (to->body != from->body)
		bits |= U_BODY;

	if (to->controller[0] != from->controller[0])
		bits |= U_CONTROLLER1;
	if (to->controller[1] != from->controller[1])
		bits |= U_CONTROLLER2;
	if (to->controller[2] != from->controller[2])
		bits |= U_CONTROLLER3;
	if (to->controller[3] != from->controller[3])
		bits |= U_CONTROLLER4;

	if (to->blending[0] != from->blending[0])
		bits |= U_BLENDING1;
	if (to->blending[1] != from->blending[1])
		bits |= U_BLENDING2;

	if (to->rendermode != from->rendermode ||
		to->renderamt != from->renderamt ||
		to->renderfx != from->renderfx ||
		to->rendercolor.r != from->rendercolor.r ||
		to->rendercolor.g != from->rendercolor.g ||
		to->rendercolor.b != from->rendercolor.b)
	{
		bits |= U_RENDER;
	}

	if (to->animtime != 0.0 && to->velocity[0] == 0 && to->velocity[1] == 0 && to->velocity[2] == 0)
		bits |= U_MONSTERMOVE;

	for (i = 0; i < 3; i++)
	{
		miss = to->mins[i] - from->mins[i];
		if (miss < -0.1 || miss > 0.1)
			bboxbits |= (U_BBOXMINS1 << i);

		miss = to->maxs[i] - from->maxs[i];
		if (miss < -0.1 || miss > 0.1)
			bboxbits |= (U_BBOXMAXS1 << i);
	}

	if (to->number > 255)
		bits |= U_LONGENTITY;

	if (bits > 0xFF)
		bits |= U_MOREBITS;
	if (bits > 0xFFFF)
		bits |= U_EVENMOREBITS;
	if (bits > 0xFFFFFF)
		bits |= U_YETMOREBITS;

	if (bboxbits)
		bits |= (U_YETMOREBITS | U_EVENMOREBITS | U_MOREBITS | U_BBOXBITS);

	//
	// write the message
	//
	if (!to->number)
		Sys_Error("Unset entity number");

	if (!bits && !bboxbits && !force)
		return;		// nothing to send!

	MSG_WriteByte(msg, bits & 255);

	if (bits & U_MOREBITS)
		MSG_WriteByte(msg, (bits >> 8) & 255);
	if (bits & U_EVENMOREBITS)
		MSG_WriteByte(msg, (bits >> 16) & 255);
	if (bits & U_YETMOREBITS)
		MSG_WriteByte(msg, (bits >> 24) & 255);

	if (bits & U_BBOXBITS)
		MSG_WriteByte(msg, bboxbits);

	if (bits & U_LONGENTITY)
		MSG_WriteShort(msg, to->number);
	else
		MSG_WriteByte(msg, to->number);

	if (bits & U_MODELINDEX)
		MSG_WriteShort(msg, to->modelindex);
	if (bits & U_FRAME)
		MSG_WriteWord(msg, to->frame * 256);
	if (bits & U_MOVETYPE)
		MSG_WriteByte(msg, to->movetype);
	if (bits & U_COLORMAP)
		MSG_WriteByte(msg, to->colormap);

	if (bits & U_CONTENTS)
	{
		MSG_WriteShort(msg, to->skin);
		MSG_WriteByte(msg, to->solid);
	}

	if (bits & U_SCALE)
		MSG_WriteWord(msg, to->scale * 256);

	if (bits & U_EFFECTS)
		MSG_WriteByte(msg, to->effects);

	if (bits & U_ORIGIN1)
		MSG_WriteCoord(msg, to->origin[0]);
	if (bits & U_ANGLE1)
		MSG_WriteHiresAngle(msg, to->angles[0]);
	if (bits & U_ORIGIN2)
		MSG_WriteCoord(msg, to->origin[1]);
	if (bits & U_ANGLE2)
		MSG_WriteHiresAngle(msg, to->angles[1]);
	if (bits & U_ORIGIN3)
		MSG_WriteCoord(msg, to->origin[2]);
	if (bits & U_ANGLE3)
		MSG_WriteHiresAngle(msg, to->angles[2]);

	if (bits & U_SEQUENCE)
	{
		MSG_WriteByte(msg, to->sequence);
		MSG_WriteByte(msg, (byte)(to->animtime * 100));
	}

	if (bits & U_FRAMERATE)
		MSG_WriteChar(msg, to->framerate * 16);

	if (bits & U_CONTROLLER1)
		MSG_WriteByte(msg, to->controller[0]);
	if (bits & U_CONTROLLER2)
		MSG_WriteByte(msg, to->controller[1]);
	if (bits & U_CONTROLLER3)
		MSG_WriteByte(msg, to->controller[2]);
	if (bits & U_CONTROLLER4)
		MSG_WriteByte(msg, to->controller[3]);

	if (bits & U_BLENDING1)
		MSG_WriteByte(msg, to->blending[0]);
	if (bits & U_BLENDING2)
		MSG_WriteByte(msg, to->blending[1]);

	if (bits & U_BODY)
		MSG_WriteByte(msg, to->body);

	if (bits & U_RENDER)
	{
		MSG_WriteByte(msg, to->rendermode);
		MSG_WriteByte(msg, to->renderamt);
		MSG_WriteByte(msg, to->rendercolor.r);
		MSG_WriteByte(msg, to->rendercolor.g);
		MSG_WriteByte(msg, to->rendercolor.b);
		MSG_WriteByte(msg, to->renderfx);
	}

	if (bboxbits & U_BBOXMINS1)
		MSG_WriteCoord(msg, to->mins[0]);
	if (bboxbits & U_BBOXMINS2)
		MSG_WriteCoord(msg, to->mins[1]);
	if (bboxbits & U_BBOXMINS3)
		MSG_WriteCoord(msg, to->mins[2]);
	if (bboxbits & U_BBOXMAXS1)
		MSG_WriteCoord(msg, to->maxs[0]);
	if (bboxbits & U_BBOXMAXS2)
		MSG_WriteCoord(msg, to->maxs[1]);
	if (bboxbits & U_BBOXMAXS3)
		MSG_WriteCoord(msg, to->maxs[2]);
}

/*
=============
SV_EmitPacketEntities

Writes a delta update of a packet_entities_t to the message.
=============
*/
void SV_EmitPacketEntities( client_t* client, packet_entities_t* to, sizebuf_t* msg )
{
	edict_t* ent;
	client_frame_t* fromframe;
	packet_entities_t* from;

	int         oldnum, newnum;
	int         oldindex;
	int         newindex;
	int         oldmax;
	unsigned int        bits;

	// this is the frame that we are going to delta update from
	if (client->delta_sequence != -1)
	{
		fromframe = &client->frames[client->delta_sequence & UPDATE_MASK];
		from = &fromframe->entities;
		oldmax = from->num_entities;

		MSG_WriteByte(msg, svc_deltapacketentities);
		MSG_WriteShort(msg, to->num_entities);
		MSG_WriteByte(msg, client->delta_sequence);
	}
	else
	{
		oldmax = 0;	// no delta update
		from = NULL;

		MSG_WriteByte(msg, svc_packetentities);
		MSG_WriteShort(msg, to->num_entities);
	}

	newindex = 0;
	oldindex = 0;
//Con_Printf("---%i to %i ----\n", client->delta_sequence & UPDATE_MASK
//			, client->netchan.outgoing_sequence & UPDATE_MASK);
	while (newindex < to->num_entities || oldindex < oldmax)
	{
		newnum = newindex >= to->num_entities ? 9999 : to->entities[newindex].number;
		oldnum = oldindex >= oldmax ? 9999 : from->entities[oldindex].number;

		if (newnum == oldnum)
		{	// delta update from old position
//Con_Printf("delta %i\n", newnum);
			SV_WriteDelta(&from->entities[oldindex], &to->entities[newindex], msg, FALSE);
			newindex++;
			oldindex++;
			continue;
		}

		if (newnum < oldnum)
		{	// this is a new entity, send it from the baseline
			ent = EDICT_NUM(newnum);
//Con_Printf("baseline %i\n", newnum);
			SV_WriteDelta(&ent->baseline, &to->entities[newindex], msg, TRUE);
			newindex++;
			continue;
		}

		if (newnum > oldnum)
		{	// the old entity isn't present in the new message
//Con_Printf("remove %i\n", oldnum);
			bits = U_REMOVE;

			if (oldnum > 255)
				bits |= U_LONGENTITY;

			if (bits > 0xFF)
				bits |= U_MOREBITS;
			if (bits > 0xFFFF)
				bits |= U_EVENMOREBITS;
			if (bits > 0xFFFFFF)
				bits |= U_YETMOREBITS;

			MSG_WriteByte(msg, bits & 255);

			if (bits & U_MOREBITS)
				MSG_WriteByte(msg, (bits >> 8) & 255);
			if (bits & U_EVENMOREBITS)
				MSG_WriteByte(msg, (bits >> 16) & 255);
			if (bits & U_YETMOREBITS)
				MSG_WriteByte(msg, (bits >> 24) & 255);

			oldindex++;

			if (bits & U_LONGENTITY)
				MSG_WriteShort(msg, oldnum);
			else
				MSG_WriteByte(msg, oldnum);

			continue;
		}
	}

	MSG_WriteLong(msg, 0);	// end of packetentities
}

/*
=============
SV_WritePlayersToClient

=============
*/
void SV_WritePlayersToClient( client_t* client, byte* pvs, sizebuf_t* msg )
{
	int			i, j;
	int			playernum;
	client_t* cl;
	edict_t* ent, * clent;
	int			msec;
	usercmd_t	cmd, nullcmd;
	int			pflags;

	clent = client->edict;

	for (j = 0, cl = svs.clients; j < svs.maxclientslimit; j++, cl++)
	{
		if (!cl->spawned && !cl->active)
			continue;

		ent = cl->edict;

		// ZOID visibility tracking
		if (ent != clent &&
			!(client->spec_track && client->spec_track - 1 == j))
		{
			if (cl->spectator)
				continue;

			// ignore if not touching a PV leaf
			for (i = 0; i < ent->num_leafs; i++)
				if (pvs[ent->leafnums[i] >> 3] & (1 << (ent->leafnums[i] & 7)))
					break;
			if (i == ent->num_leafs)
				continue;		// not visible
		}

		pflags = PF_MSEC | PF_COMMAND;

		if (g_bShouldUpdatePing)
			pflags |= PF_PING;

		if (ent->v.modelindex != sv_playermodel)
			pflags |= PF_MODEL;
		for (i = 0; i < 3; i++)
			if (ent->v.velocity[i])
				pflags |= PF_VELOCITY1 << i;
		if (ent->v.effects)
			pflags |= PF_EFFECTS;
		if (ent->v.skin)
			pflags |= PF_SKINNUM;
		if (ent->v.health <= 0)
			pflags |= PF_DEAD;
		if (ent->v.mins[2] != -24)
			pflags |= PF_GIB;

		if (cl->spectator)
		{	// only sent origin and velocity to spectators
			pflags &= PF_VELOCITY1 | PF_VELOCITY2 | PF_VELOCITY3;
		}
		else if (ent == clent)
		{	// don't send a lot of data on personal entity
			pflags &= ~PF_MSEC;
		}

		if (ent->v.weaponmodel)
			pflags |= PF_WEAPONMODEL;
		if (ent->v.movetype)
			pflags |= PF_MOVETYPE;
		if (ent->v.sequence || ent->v.animtime)
			pflags |= PF_SEQUENCE;

		if (ent->v.rendermode ||
			ent->v.renderamt ||
			ent->v.renderfx ||
			ent->v.rendercolor[0] ||
			ent->v.rendercolor[1] ||
			ent->v.rendercolor[2])
		{
			pflags |= PF_RENDER;
		}

		if (ent->v.framerate != 1.0)
			pflags |= PF_FRAMERATE;

		if (ent->v.body)
			pflags |= PF_BODY;

		for (i = 0; i < 4; i++)
		{
			if (ent->v.controller[i])
				pflags |= PF_CONTROLLER1 << i;
		}

		for (i = 0; i < 2; i++)
		{
			if (ent->v.blending[i])
				pflags |= PF_BLENDING1 << i;
		}

		if (ent->v.clbasevelocity[0] != 0 || ent->v.clbasevelocity[1] != 0 || ent->v.clbasevelocity[2] != 0)
			pflags |= PF_BASEVELOCITY;

		if (ent->v.friction != 1.0)
			pflags |= PF_FRICTION;

		MSG_WriteByte(msg, svc_playerinfo);
		playernum = j;
		if (cl->spectator)
			playernum |= PN_SPECTATOR;
		MSG_WriteByte(msg, playernum);
		MSG_WriteLong(msg, pflags);
		MSG_WriteLong(msg, ent->v.flags);

		for (i = 0; i < 3; i++)
			MSG_WriteCoord(msg, ent->v.origin[i]);

		MSG_WriteByte(msg, ent->v.frame);

		if (pflags & PF_MSEC)
		{
			msec = 1000 * (sv.time - cl->localtime);
			if (msec > 255)
				msec = 255;
			MSG_WriteByte(msg, msec);
		}

		if (pflags & PF_COMMAND)
		{
			cmd = cl->lastcmd;

			if (ent->v.health <= 0)
			{	// don't show the corpse looking around...
				cmd.angles[0] = 0;
				cmd.angles[1] = ent->v.angles[1];
				cmd.angles[0] = 0;
			}

			cmd.buttons = 0;	// never send buttons
			cmd.impulse = 0;	// never send impulses

			memset(&nullcmd, 0, sizeof(nullcmd));
			MSG_WriteDeltaUsercmd(msg, &cmd, &nullcmd);
		}

		for (i = 0; i < 3; i++)
		{
			if (pflags & (PF_VELOCITY1 << i))
				MSG_WriteShort(msg, ent->v.velocity[i]);
		}

		if (pflags & PF_MODEL)
			MSG_WriteShort(msg, ent->v.modelindex);

		if (pflags & PF_SKINNUM)
			MSG_WriteByte(msg, ent->v.skin);

		if (pflags & PF_EFFECTS)
			MSG_WriteByte(msg, ent->v.effects);

		if (pflags & PF_WEAPONMODEL)
			MSG_WriteShort(msg, SV_ModelIndex(pr_strings + ent->v.weaponmodel));

		if (pflags & PF_MOVETYPE)
			MSG_WriteByte(msg, ent->v.movetype);

		if (pflags & PF_SEQUENCE)
		{
			MSG_WriteByte(msg, ent->v.sequence);
			MSG_WriteByte(msg, ent->v.animtime * 100);
		}

		if (pflags & PF_RENDER)
		{
			MSG_WriteByte(msg, ent->v.rendermode);
			MSG_WriteByte(msg, ent->v.renderamt);
			MSG_WriteByte(msg, ent->v.rendercolor[0]);
			MSG_WriteByte(msg, ent->v.rendercolor[1]);
			MSG_WriteByte(msg, ent->v.rendercolor[2]);
			MSG_WriteByte(msg, ent->v.renderfx);
		}

		if (pflags & PF_FRAMERATE)
			MSG_WriteChar(msg, ent->v.framerate * 16);

		if (pflags & PF_BODY)
			MSG_WriteByte(msg, ent->v.body);

		if (pflags & PF_CONTROLLER1)
			MSG_WriteByte(msg, ent->v.controller[0]);
		if (pflags & PF_CONTROLLER2)
			MSG_WriteByte(msg, ent->v.controller[1]);
		if (pflags & PF_CONTROLLER3)
			MSG_WriteByte(msg, ent->v.controller[2]);
		if (pflags & PF_CONTROLLER4)
			MSG_WriteByte(msg, ent->v.controller[3]);

		if (pflags & PF_BLENDING1)
			MSG_WriteByte(msg, ent->v.blending[0]);
		if (pflags & PF_BLENDING2)
			MSG_WriteByte(msg, ent->v.blending[1]);

		if (pflags & PF_BASEVELOCITY)
		{
			MSG_WriteShort(msg, ent->v.clbasevelocity[0]);
			MSG_WriteShort(msg, ent->v.clbasevelocity[1]);
			MSG_WriteShort(msg, ent->v.clbasevelocity[2]);
		}

		if (pflags & PF_FRICTION)
			MSG_WriteShort(msg, ent->v.friction);

		if (pflags & PF_PING)
			MSG_WriteShort(msg, SV_CalcPing(cl));
	}
}

typedef struct full_packet_entities_s
{
	int num_entities;
	entity_state_t entities[MAX_PACKET_ENTITIES];
} full_packet_entities_t;

/*
==================
SV_AddToFullPack

Return 1 if the entity state has been filled in for the ent and the entity will be propagated to the client, 0 otherwise

state is the server maintained copy of the state info that is transmitted to the client
a MOD could alter values copied into state to send the "host" a different look for a particular entity update, etc.
e and ent are the entity that is being added to the update, if 1 is returned
host is the player's edict of the player whom we are sending the update to
player is 1 if the ent/e is a player and 0 otherwise
pSet is either the PAS or PVS that we previous set up.  We can use it to ask the engine to filter the entity against the PAS or PVS.
we could also use the pas/ pvs that we set in SetupVisibility, if we wanted to.  Caching the value is valid in that case, but still only for the current frame
==================
*/
void SV_AddToFullPack( full_packet_entities_t* pack, int e, unsigned char* pSet )
{
	int					i;
	edict_t* ent;
	entity_state_t*	state;

	ent = &sv.edicts[e];

	// don't send if flagged for NODRAW
	if (ent->v.effects == EF_NODRAW)
		return;

	// Ignore ents without valid / visible models
	if (!ent->v.modelindex || !(pr_strings + ent->v.model))
		return;

	// If pSet is NULL, then the test will always succeed and the entity will be added to the update
	if (pSet)
	{
		if (ent->num_leafs < 0)
			if (!CM_HeadnodeVisible(&sv.worldmodel->nodes[ent->leafnums[0]], pSet))
				return;

		// ignore if not touching a PV leaf
		for (i = 0; i < ent->num_leafs; i++)
			if (pSet[ent->leafnums[i] >> 3] & (1 << (ent->leafnums[i] & 7)))
				break;
		if (i == ent->num_leafs)
			return;		// not visible
	}

	if (pack->num_entities >= MAX_PACKET_ENTITIES)
	{
		Con_DPrintf("Too many entities in visible packet list.\n");
		return;
	}
	state = &pack->entities[pack->num_entities];
	pack->num_entities++;

	state->number = e;
	state->flags = 0;
	state->entityType = ENTITY_NORMAL;

	// flag custom entities
	if (ent->v.flags & FL_CUSTOMENTITY)
		state->entityType = ENTITY_BEAM;
		
	VectorCopy(ent->v.origin, state->origin);
	VectorCopy(ent->v.angles, state->angles);
	VectorCopy(ent->v.mins, state->mins);
	VectorCopy(ent->v.maxs, state->maxs);
	VectorCopy(ent->v.velocity, state->velocity);

	state->modelindex = ent->v.modelindex;
	state->frame = ent->v.frame;
	state->skin = ent->v.skin;
	state->colormap = ent->v.colormap;
	state->solid = ent->v.solid;
	state->effects = ent->v.effects;
	state->scale = ent->v.scale;
	state->movetype = ent->v.movetype;
	state->animtime = ent->v.animtime;
	state->sequence = ent->v.sequence;
	state->framerate = ent->v.framerate;
	state->body = ent->v.body;

	for (i = 0; i < 4; i++)
	{
		state->controller[i] = ent->v.controller[i];
	}

	for (i = 0; i < 2; i++)
	{
		state->blending[i] = ent->v.blending[i];
	}

	state->rendermode = ent->v.rendermode;
	state->renderamt = ent->v.renderamt;
	state->renderfx = ent->v.renderfx;
	state->rendercolor.r = ent->v.rendercolor[0];
	state->rendercolor.g = ent->v.rendercolor[1];
	state->rendercolor.b = ent->v.rendercolor[2];
}

/*
=============
SV_WriteEntitiesToClient

Encodes the current state of the world as
a svc_packetentities messages and
svc_playerinfo messages
=============
*/
void SV_WriteEntitiesToClient( client_t* client, sizebuf_t* msg )
{
	int		i;
	byte* pvs;
	vec3_t	org;
	packet_entities_t* pack;
	int		entsinpacket;
	edict_t* clent;
	client_frame_t* frame;
	full_packet_entities_t fullpack;

	// this is the frame we are creating
	frame = &client->frames[client->netchan.incoming_sequence & UPDATE_MASK];

	// find the client's PVS
	clent = client->edict;
	VectorAdd(clent->v.origin, clent->v.view_ofs, org);
	pvs = SV_FatPVS(org);

	// send over the players in the PVS
	SV_WritePlayersToClient(client, pvs, msg);

	// put other visible entities into either a packet_entities or a nails message
	pack = &frame->entities;
	pack->num_entities = 0;

	if (pack->entities)
		free(pack->entities);

	pack->entities = NULL;

	fullpack.num_entities = 0;
	for (i = svs.maxclients + 1; i < sv.num_edicts; i++)
	{
		// add to the packetentities
		SV_AddToFullPack(&fullpack, i, pvs);
	}

	entsinpacket = fullpack.num_entities;
	pack->num_entities = entsinpacket;

	if (entsinpacket == 0)
		entsinpacket = 1;

	pack->entities = (entity_state_t*)malloc(sizeof(entity_state_t) * entsinpacket);
	if (!pack->entities)
		Sys_Error("Failed to allocate space for %i packet entities\n", entsinpacket);

	if (pack->num_entities)
		memcpy(pack->entities, fullpack.entities, sizeof(entity_state_t) * pack->num_entities);
	else
		memset(pack->entities, 0, sizeof(entity_state_t));

	// encode the packet entities as a delta from the
	// last packetentities acknowledged by the client

	SV_EmitPacketEntities(client, pack, msg);
}

/*
=============
SV_CleanupEnts

=============
*/
void SV_CleanupEnts( void )
{
	int		e;
	edict_t* ent;

	for (e = 1; e < sv.num_edicts; e++)
	{
		ent = &sv.edicts[e];

		ent->v.effects &= ~EF_MUZZLEFLASH;
		ent->v.effects &= ~EF_NOINTERP;
	}
}

/*
==================
SV_WriteClientdataToMessage

==================
*/
void SV_WriteClientdataToMessage( client_t* client, sizebuf_t* msg )
{
	int		bits;
	int		i;
	edict_t* ent;

	ent = client->edict;

	// send the chokecount for r_netgraph
	if (client->chokecount)
	{
		MSG_WriteByte(msg, svc_chokecount);
		MSG_WriteByte(msg, client->chokecount);
		client->chokecount = 0;
	}

//
// send the current viewpos offset from the view entity
//
	SV_SetIdealPitch();		// how much to look up / down ideally

// a fixangle might get lost in a dropped packet.  Oh well.
	if (ent->v.fixangle)
	{
		if (ent->v.fixangle == 2)
		{
			MSG_WriteByte(msg, svc_addangle);
			MSG_WriteHiresAngle(msg, ent->v.avelocity[1]);
			ent->v.avelocity[1] = 0;
		}
		else
		{
			MSG_WriteByte(msg, svc_setangle);
			for (i = 0; i < 3; i++)
				MSG_WriteHiresAngle(msg, ent->v.angles[i]);
		}
		ent->v.fixangle = 0;
	}

	bits = 0;

	if (ent->v.view_ofs[2] != DEFAULT_VIEWHEIGHT)
		bits |= SU_VIEWHEIGHT;

	if (ent->v.idealpitch)
		bits |= SU_IDEALPITCH;

	if (ent->v.weapons)
		bits |= SU_WEAPONS;

	if (ent->v.flags & FL_ONGROUND)
		bits |= SU_ONGROUND;

	if (ent->v.waterlevel >= 2)
		bits |= SU_INWATER;

	if (ent->v.waterlevel >= 3)
		bits |= SU_FULLYINWATER;

	if (ent->v.viewmodel)
		bits |= SU_ITEMS;

	for (i = 0; i < 3; i++)
	{
		if (ent->v.punchangle[i])
			bits |= (SU_PUNCH1 << i);
		if (ent->v.velocity[i])
			bits |= (SU_VELOCITY1 << i);
	}

// send the data

	MSG_WriteByte(msg, svc_clientdata);
	MSG_WriteShort(msg, bits);

	if (bits & SU_VIEWHEIGHT)
		MSG_WriteChar(msg, ent->v.view_ofs[2]);

	if (bits & SU_IDEALPITCH)
		MSG_WriteChar(msg, ent->v.idealpitch);

	for (i = 0; i < 3; i++)
	{
		if (bits & (SU_PUNCH1 << i))
			MSG_WriteHiresAngle(msg, ent->v.punchangle[i]);
		if (bits & (SU_VELOCITY1 << i))
			MSG_WriteChar(msg, ent->v.velocity[i] / 16);
	}

	if (bits & SU_WEAPONS)
		MSG_WriteLong(msg, ent->v.weapons);

	if (bits & SU_ITEMS)
		MSG_WriteShort(msg, SV_ModelIndex(pr_strings + ent->v.viewmodel));

	MSG_WriteShort(msg, ent->v.health);
}

/*
=======================
SV_SendClientDatagram
=======================
*/
qboolean SV_SendClientDatagram( client_t* client )
{
	byte		buf[MAX_DATAGRAM];
	sizebuf_t	msg;

	msg.data = buf;
	msg.maxsize = sizeof(buf);
	msg.cursize = 0;
	msg.allowoverflow = TRUE;
	msg.overflowed = FALSE;

	MSG_WriteByte(&msg, svc_time);
	MSG_WriteFloat(&msg, sv.time);

// add the client specific data to the datagram
	SV_WriteClientdataToMessage(client, &msg);

	// send over all the objects that are in the PVS
	// this will include clients, a packetentities, and
	// possibly a nails update
	SV_WriteEntitiesToClient(client, &msg);

	// copy the accumulated multicast datagram
	// for this client out to the message
	if (client->datagram.overflowed)
		Con_Printf("WARNING: datagram overflowed for %s\n", client->name);
	else
		SZ_Write(&msg, client->datagram.data, client->datagram.cursize);
	SZ_Clear(&client->datagram);

	if (scr_graphmean.value)
	{
		nDatagrams++;
		nDatagramBytesSent += msg.cursize;
	}
	else
	{
		if (nDatagramBytesSent < msg.cursize)
			nDatagramBytesSent = msg.cursize;
	}

	if (msg.overflowed)
	{
		Con_Printf("WARNING: msg overflowed for %s\n", client->name);
		SZ_Clear(&msg);
	}

// send the datagram
	Netchan_Transmit(&client->netchan, msg.cursize, buf);

	return TRUE;
}

/*
=======================
SV_UpdateToReliableMessages
=======================
*/
void SV_UpdateToReliableMessages( void )
{
	int			i, j;
	client_t* client;

// check for changes to be sent over the reliable streams to all clients
	for (i = 0, host_client = svs.clients; i < svs.maxclients; i++, host_client++)
	{
		if (!host_client->edict)
			continue;
		if (host_client->fakeclient)
			continue;
		if (!host_client->active && !host_client->connected)
			continue;
		if (!sv_gpNewUserMsgs)
			continue;

		SV_SendUserReg(&host_client->netchan.message);
	}

	// Link new user messages to the global messages chain
	if (sv_gpNewUserMsgs)
	{
		if (sv_gpUserMsgs)
		{
			UserMsg* pMsg = sv_gpUserMsgs;
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

	if (sv.datagram.overflowed)
	{
		Con_DPrintf("sv.datagram overflowed!\n");
		SZ_Clear(&sv.datagram);
	}

	// append the broadcast messages to each client messages
	for (j = 0, client = svs.clients; j < svs.maxclients; j++, client++)
	{
		if (!client->active || client->fakeclient)
			continue;

		SZ_Write(&client->netchan.message, sv.reliable_datagram.data, sv.reliable_datagram.cursize);
		SZ_Write(&client->datagram, sv.datagram.data, sv.datagram.cursize);
	}

	SZ_Clear(&sv.reliable_datagram);
	SZ_Clear(&sv.datagram);
}

/*
=======================
SV_SendClientMessages
=======================
*/
void SV_SendClientMessages( void )
{
	int			i;

// update frags, names, etc
	SV_UpdateToReliableMessages();

	nReliableBytesSent = 0;
	nDatagramBytesSent = 0;
	nReliables = 0;
	nDatagrams = 0;
	bUnreliableOverflow = FALSE;

	if (g_fLastPingUpdateTime <= sv.time)
	{
		g_bShouldUpdatePing = TRUE;
		g_fLastPingUpdateTime = sv.time + 1.0f;
	}

// build individual updates
	for (i = 0, host_client = svs.clients; i < svs.maxclients; i++, host_client++)
	{
		if (!host_client->active && !host_client->connected && !host_client->spawned)
			continue;

		if (host_client->fakeclient)
			continue;

		// if the reliable message overflowed,
		// drop the client
		if (host_client->netchan.message.overflowed)
		{
			SZ_Clear(&host_client->netchan.message);
			SZ_Clear(&host_client->datagram);
			SV_BroadcastPrintf("%s overflowed\n", host_client->name);
			Con_Printf("WARNING: reliable overflow for %s\n", host_client->name);
			SV_DropClient(host_client, FALSE);
			host_client->send_message = TRUE;
			host_client->netchan.cleartime = 0;	// don't choke this message
		}

		// only send messages if the client has sent one
		// and the bandwidth is not choked
		if (!host_client->send_message)
			continue;

		host_client->send_message = FALSE;	// try putting this after choke?

		if (!Netchan_CanPacket(&host_client->netchan))
		{
			host_client->chokecount++;
			continue;		// bandwidth choke
		}

		if (host_client->active)
			SV_SendClientDatagram(host_client);
		else
			Netchan_Transmit(&host_client->netchan, 0, NULL);
	}

	SCR_UpdateNetUsage(nReliableBytesSent, nReliables, FALSE);

	if (bUnreliableOverflow)
		SCR_UpdateNetUsage(nDatagramBytesSent, MAX_MSGLEN + 1, TRUE);
	else
		SCR_UpdateNetUsage(nDatagramBytesSent, nDatagrams, TRUE);

	g_bShouldUpdatePing = FALSE;

// clear muzzle flashes
	SV_CleanupEnts();
}


/*
==============================================================================

SERVER SPAWNING

==============================================================================
*/

/*
================
SV_ModelIndex

================
*/
int SV_ModelIndex( char* name )
{
	int		i;

	if (!name || !name[0])
		return 0;

	for (i = 0; i < MAX_MODELS && sv.model_precache[i]; i++)
		if (!strcmp(sv.model_precache[i], name))
			return i;
	if (i == MAX_MODELS || !sv.model_precache[i])
		Sys_Error("SV_ModelIndex: model %s not precached", name);
	return i;
}

/*
================
SV_FlushSignon

Moves to the next signon buffer if needed
================
*/
void SV_FlushSignon( void )
{
	if (sv.signon.cursize < sv.signon.maxsize - 100)
		return;

	if (sv.num_signon_buffers == MAX_SIGNON_BUFFERS - 1)
		Sys_Error("sv.num_signon_buffers == MAX_SIGNON_BUFFERS-1");

	sv.signon_buffer_size[sv.num_signon_buffers - 1] = sv.signon.cursize;
	sv.signon.data = sv.signon_buffers[sv.num_signon_buffers];
	sv.num_signon_buffers++;
	sv.signon.cursize = 0;
}

/*
================
SV_SendResourceListBlock_f

================
*/
void SV_SendResourceListBlock_f( void )
{
	int		n;
	int		savepos;

	if (cmd_source == src_command)
	{
		Cmd_ForwardToServer();
		return;
	}

	if (!host_client->connected)
	{
		Con_Printf("resourcelist not valid -- already spawned\n");
		return;
	}

	// handle the case of a level changing while a client was connecting
	if (!cls.demoplayback && atoi(Cmd_Argv(1)) != svs.spawncount)
	{
		Con_Printf("SV_SendResourceListBlock_f from different level\n");
		SV_New_f();
		return;
	}

	n = atoi(Cmd_Argv(2));

	MSG_WriteByte(&host_client->netchan.message, svc_resourcelist);
	MSG_WriteShort(&host_client->netchan.message, sv.num_resources);
	MSG_WriteShort(&host_client->netchan.message, n);

	savepos = host_client->netchan.message.cursize;
	MSG_WriteShort(&host_client->netchan.message, 0);

	for (n; n < sv.num_resources; n++)
	{
		if (host_client->netchan.message.cursize >= host_client->netchan.message.maxsize - 512)
			break;

		MSG_WriteByte(&host_client->netchan.message, sv.resources[n].type);
		MSG_WriteString(&host_client->netchan.message, sv.resources[n].szFileName);
		MSG_WriteShort(&host_client->netchan.message, sv.resources[n].nIndex);
		MSG_WriteLong(&host_client->netchan.message, sv.resources[n].nDownloadSize);
		MSG_WriteByte(&host_client->netchan.message, sv.resources[n].ucFlags);

		if (sv.resources[n].ucFlags & RES_CUSTOM)
			SZ_Write(&cls.netchan.message, sv.resources[n].rgucMD5_hash, sizeof(sv.resources[n].rgucMD5_hash));
	}

	*(unsigned short*)&host_client->netchan.message.data[savepos] = n;

	if (sv.num_resources > n)
	{
		MSG_WriteByte(&host_client->netchan.message, svc_stufftext);
		MSG_WriteString(&host_client->netchan.message, va("cmd resourcelist %i %i\n", svs.spawncount, n));
	}
}

/*
================
SV_AddResource

Adds a new resource to the server resource list
================
*/
void SV_AddResource( resourcetype_t type, const char *name, int size, byte flags, int index )
{
	resource_t*	r;

	r = &sv.resources[sv.num_resources];
	if (sv.num_resources >= MAX_RESOURCES)
		Sys_Error("Too many resources on server.");
	sv.num_resources++;

	r->type = type;
	strcpy(r->szFileName, name);
	r->nDownloadSize = size;
	if (flags)
		r->ucFlags |= RES_FATALIFMISSING;
	r->nIndex = index;
}

/*
================
SV_CreateResourceList

Creates a common list of all server resources
================
*/
void SV_CreateResourceList( void )
{
	char** s;
	int		ffirstsent = 0;
	FILE* pFile;

	int			i, nSize;

	sv.num_resources = 0;

	// Add sound files to svc_resourcelist
	i = 1;
	s = &sv.sound_precache[i];
	while (*s)
	{
		if (**s == CHAR_SENTENCE)
		{
			if (!ffirstsent)
			{
				ffirstsent = 1;
				SV_AddResource(t_sound, "!", 0, RES_FATALIFMISSING, i);
			}
		}
		else
		{
			if (svs.maxclients <= 1)
			{
				nSize = 0;
			}
			else
			{
				nSize = COM_FindFile(va("sound/%s", *s), NULL, &pFile);
				if (pFile)
					fclose(pFile);
				if (nSize == -1)
					nSize = 0;
			}

			SV_AddResource(t_sound, *s, nSize, 0, i);
		}

		i++;
		s++;
	}

	// Add model files to svc_resourcelist
	i = 1;
	s = &sv.model_precache[i];
	while (*s)
	{
		if (svs.maxclients <= 1)
		{
			nSize = 0;
		}
		else
		{
			nSize = COM_FindFile(*s, NULL, &pFile);
			if (pFile)
				fclose(pFile);
			if (nSize == -1)
				nSize = 0;
		}

		SV_AddResource(t_model, *s, nSize, RES_FATALIFMISSING, i);

		i++;
		s++;
	}

	// Add decals to svc_resourcelist
	for (i = 0; i < sv_decalnamecount; i++)
	{
		SV_AddResource(t_decal, sv_decalnames[i].name, Draw_DecalSize(i), 0, i);
	}
}

/*
==================
SV_Customization

Sends resource to all other players, optionally skipping originating player.
==================
*/
void SV_Customization( client_t* pPlayer, resource_t* pResource, qboolean bSkipPlayer )
{
	int		i;
	int		nPlayerNumber;

	nPlayerNumber = -1;

	// Get originating player id
	for (i = 0, host_client = svs.clients; i < svs.maxclients; i++, host_client++)
	{
		if (host_client == pPlayer)
		{
			nPlayerNumber = i;
			break;
		}
	}

	if (nPlayerNumber == -1)
	{
		Sys_Error("Couldn't find player index for customization.");
		return;
	}

	// Send resource to all other active players
	for (i = 0, host_client = svs.clients; i < svs.maxclients; i++, host_client++)
	{
		if (!host_client->active && !host_client->spawned)
			continue;

		if (host_client == pPlayer && bSkipPlayer)
			continue;

		MSG_WriteByte(&host_client->netchan.message, svc_customization);
		MSG_WriteByte(&host_client->netchan.message, nPlayerNumber);
		MSG_WriteByte(&host_client->netchan.message, pResource->type);
		MSG_WriteString(&host_client->netchan.message, pResource->szFileName);
		MSG_WriteShort(&host_client->netchan.message, pResource->nIndex);
		MSG_WriteLong(&host_client->netchan.message, pResource->nDownloadSize);
		MSG_WriteByte(&host_client->netchan.message, pResource->ucFlags);

		if (pResource->ucFlags & RES_CUSTOM)
			SZ_Write(&host_client->netchan.message, pResource->rgucMD5_hash, sizeof(pResource->rgucMD5_hash));
	}
}

/*
====================
SV_PropagateCustomizations

Sends customizations from all active players to the current player.
====================
*/
void SV_PropagateCustomizations( void )
{
	client_t* pHost;
	customization_t* pCust;
	resource_t* pResource;
	int		i;

	pHost = host_client;

	// For each active player
	for (i = 0, host_client = svs.clients; i < svs.maxclients; i++, host_client++)
	{
		if (!host_client->active && !host_client->spawned)
			continue;

		// Send each customization to current player
		pCust = host_client->customdata.pNext;
		while (pCust)
		{
			if (pCust->bInUse)
			{
				pResource = &pCust->resource;

				MSG_WriteByte(&pHost->netchan.message, svc_customization);
				MSG_WriteByte(&pHost->netchan.message, i);
				MSG_WriteByte(&pHost->netchan.message, pResource->type);
				MSG_WriteString(&pHost->netchan.message, pResource->szFileName);
				MSG_WriteShort(&pHost->netchan.message, pResource->nIndex);
				MSG_WriteLong(&pHost->netchan.message, pResource->nDownloadSize);
				MSG_WriteByte(&pHost->netchan.message, pResource->ucFlags);

				if (pResource->ucFlags & RES_CUSTOM)
					SZ_Write(&pHost->netchan.message, pResource->rgucMD5_hash, sizeof(pResource->rgucMD5_hash));
			}

			pCust = pCust->pNext;
		}
	}

	host_client = pHost;
}

/*
================
SV_CreateBaseline

================
*/
void SV_CreateBaseline( void )
{
	int			i;
	edict_t* svent;
	int			entnum;

	for (entnum = 0; entnum < sv.num_edicts; entnum++)
	{
	// get the current server version
		svent = &sv.edicts[entnum];
		if (svent->free)
			continue;
		if (entnum > svs.maxclients && !svent->v.modelindex)
			continue;

		svent->baseline.entityType = ENTITY_NORMAL;
		if (svent->v.flags & FL_CUSTOMENTITY)
			svent->baseline.entityType = ENTITY_BEAM;

	//
	// create entity baseline
	//
		VectorCopy(svent->v.origin, svent->baseline.origin);
		VectorCopy(svent->v.angles, svent->baseline.angles);
		VectorCopy(svent->v.mins, svent->baseline.mins);
		VectorCopy(svent->v.maxs, svent->baseline.maxs);
		svent->baseline.frame = svent->v.frame;
		svent->baseline.skin = svent->v.skin;
		svent->baseline.scale = svent->v.scale;
		svent->baseline.solid = svent->v.solid;
		if (entnum > 0 && entnum <= svs.maxclients)
		{
			svent->baseline.colormap = entnum;
			svent->baseline.modelindex = SV_ModelIndex("models/player.mdl");
		}
		else
		{
			svent->baseline.colormap = 0;
			svent->baseline.modelindex =
				SV_ModelIndex(pr_strings + svent->v.model);
		}

		svent->baseline.rendermode = svent->v.rendermode;
		svent->baseline.renderamt = svent->v.renderamt;
		svent->baseline.rendercolor.r = svent->v.rendercolor[0];
		svent->baseline.rendercolor.g = svent->v.rendercolor[1];
		svent->baseline.rendercolor.b = svent->v.rendercolor[2];
		svent->baseline.renderfx = svent->v.renderfx;

		SV_FlushSignon();

	//
	// add to the message
	//
		MSG_WriteByte(&sv.signon, svc_spawnbaseline);
		MSG_WriteShort(&sv.signon, entnum);

		MSG_WriteByte(&sv.signon, svent->baseline.entityType);
		MSG_WriteShort(&sv.signon, svent->baseline.modelindex);
		MSG_WriteByte(&sv.signon, svent->baseline.sequence);
		MSG_WriteByte(&sv.signon, svent->baseline.frame);

		if (svent->baseline.entityType == ENTITY_NORMAL)
			MSG_WriteWord(&sv.signon, svent->baseline.scale * 256);
		else
			MSG_WriteByte(&sv.signon, svent->baseline.scale);

		MSG_WriteByte(&sv.signon, svent->baseline.colormap);
		MSG_WriteShort(&sv.signon, svent->baseline.skin);
		MSG_WriteByte(&sv.signon, svent->baseline.solid);

		for (i = 0; i < 3; i++)
		{
			MSG_WriteCoord(&sv.signon, svent->baseline.origin[i]);
			MSG_WriteFloat(&sv.signon, svent->baseline.angles[i]);
			MSG_WriteCoord(&sv.signon, svent->baseline.mins[i]);
			MSG_WriteCoord(&sv.signon, svent->baseline.maxs[i]);
		}

		MSG_WriteByte(&sv.signon, svent->v.rendermode);
		MSG_WriteByte(&sv.signon, svent->v.renderamt);
		MSG_WriteByte(&sv.signon, svent->v.rendercolor[0]);
		MSG_WriteByte(&sv.signon, svent->v.rendercolor[1]);
		MSG_WriteByte(&sv.signon, svent->v.rendercolor[2]);
		MSG_WriteByte(&sv.signon, svent->v.renderfx);
	}
}

/*
=================
SV_BroadcastCommand

Sends text to all active clients
=================
*/
void SV_BroadcastCommand( char* fmt, ... )
{
	va_list		argptr;
	char		string[1024];
	char		data[128];
	sizebuf_t	msg;
	int			i;
	client_t* cl;

	msg.data = data;
	msg.maxsize = sizeof(data);
	msg.cursize = 0;

	if (!sv.active)
		return;

	va_start(argptr, fmt);
	vsprintf(string, fmt, argptr);
	va_end(argptr);

	MSG_WriteByte(&msg, svc_stufftext);
	MSG_WriteString(&msg, string);

	for (i = 0, cl = svs.clients; i < svs.maxclients; i++, cl++)
	{
		if ((!cl->active && !cl->connected && !cl->spawned) || cl->fakeclient)
			continue;

		SZ_Write(&cl->netchan.message, msg.data, msg.cursize);
	}
}

/*
================
SV_SendReconnect

Tell all the clients that the server is changing levels
================
*/
void SV_SendReconnect( void )
{
	SV_BroadcastCommand("reconnect\n");

	/*if (cls.state != ca_dedicated)
		Cbuf_InsertText("reconnect\n");
	*/
}


/*
================
SV_SaveSpawnparms

Grabs the current state of each client for saving across the
transition to another level
================
*/
void SV_SaveSpawnparms( SAVERESTOREDATA* pSaveData )
{
	int		i;

	svs.serverflags = gGlobalVariables.serverflags;

	gEntityInterface.pfnParmsChangeLevel();

	for (i = 0, host_client = svs.clients; i < svs.maxclients; i++, host_client++)
	{
		if (!host_client->active && !host_client->connected)
			continue;

	// call the progs to get default spawn parms for the new client
		host_client->saveSize = pSaveData->size;
		pSaveData->currentIndex = NUM_FOR_EDICT(host_client->edict);
		gEntityInterface.pfnSave(host_client->edict, pSaveData);
	}
}

/*
================
SV_ActivateServer

================
*/
void SV_ActivateServer( int runPhysics )
{
	int i;

	Cvar_Set("sv_newunit", "0");

	// Activate the DLL server code
	gEntityInterface.pfnServerActivate(sv.edicts, sv.num_edicts, svs.maxclients);

	sv.active = TRUE;
	// all setup is completed, any further precache statements are errors
	sv.state = ss_active;

	if (runPhysics)
	{
		host_frametime = 0.1;
		SV_Physics();
	}
	else
	{
		host_frametime = 0.001;
	}

	SV_Physics();

// create a baseline for more efficient communications
	SV_CreateBaseline();

	// update signon buffer size
	sv.signon_buffer_size[sv.num_signon_buffers - 1] = sv.signon.cursize;

	SV_CreateResourceList();

// send serverinfo to all connected clients
	for (i = 0, host_client = svs.clients; i < svs.maxclients; i++, host_client++)
	{
		if (host_client->active || host_client->connected)
		{
			SV_SendServerinfo(host_client);

			// Send usermsgs
			if (sv_gpUserMsgs)
			{
				UserMsg* pTemp = sv_gpNewUserMsgs;
				sv_gpNewUserMsgs = sv_gpUserMsgs;
				SV_SendUserReg(&host_client->netchan.message);
				sv_gpNewUserMsgs = pTemp;
			}
		}
	}

	// Tell what kind of server has been started.
	if (svs.maxclients > 1)
	{
		Con_DPrintf("%i player server started\n", svs.maxclients);
	}
	else
	{
		Con_DPrintf("Game started\n");
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
	edict_t* ent;
	int			i;

	// let's not have any servers with no name
	if (host_name.string[0] == 0)
		Cvar_Set("hostname", "Half-Life");
	scr_centertime_off = 0;

	if (startspot)
		Con_DPrintf("Spawn Server %s: [%s]\n", server, startspot);
	else
		Con_DPrintf("Spawn Server %s\n", server);

	g_fLastPingUpdateTime = 0.0f;

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

	sv.edicts = (edict_t*)Hunk_AllocName(sizeof(edict_t) * sv.max_edicts, "edicts");

	sv.datagram.maxsize = sizeof(sv.datagram_buf);
	sv.datagram.cursize = 0;
	sv.datagram.data = sv.datagram_buf;

	sv.reliable_datagram.maxsize = sizeof(sv.reliable_datagram_buf);
	sv.reliable_datagram.cursize = 0;
	sv.reliable_datagram.data = sv.reliable_datagram_buf;

	sv.master.maxsize = sizeof(sv.master_buf);
	sv.master.data = sv.master_buf;

	sv.signon.maxsize = sizeof(sv.signon_buffers[0]);
	sv.signon.cursize = 0;
	sv.signon.data = sv.signon_buffers[0];
	sv.num_signon_buffers = 1;

	sv.multicast.maxsize = sizeof(sv.multicast_buf);
	sv.multicast.data = sv.multicast_buf;

// leave slots at start for clients only
	sv.num_edicts = svs.maxclients + 1;
	for (i = 0; i < svs.maxclients; i++)
	{
		ent = &sv.edicts[i + 1];
		svs.clients[i].edict = ent;
	}

	gGlobalVariables.maxClients = svs.maxclients;

	sv.state = ss_loading;
	sv.paused = FALSE;

	// Set initial time values.
	sv.time = 1.0;
	gGlobalVariables.time = 1.0;

	strcpy(sv.name, server);
	sprintf(sv.modelname, "maps/%s.bsp", server);
	sv.worldmodel = Mod_ForName(sv.modelname, FALSE);
	if (!sv.worldmodel)
	{
		Con_Printf("Couldn't spawn server %s\n", sv.modelname);
		sv.active = FALSE;
		return FALSE;
	}

	if (svs.maxclients > 1)
	{
		char szDllName[MAX_QPATH];

		// Server map CRC check.
		CRC32_Init(&sv.worldmapCRC);
		if (!CRC_MapFile(&sv.worldmapCRC, sv.modelname))
		{
			Con_Printf("Couldn't CRC server map:  %s\n", sv.modelname);
			sv.active = FALSE;
			return FALSE;
		}

		// DLL CRC check.
		sprintf(szDllName, "cl_dlls\\client.dll");
		CRC32_Init(&sv.clientSideDllCRC);

		if (!CRC_File(&sv.clientSideDllCRC, szDllName))
		{
			Con_Printf("Couldn't CRC client side dll:  %s\n", szDllName);
			sv.active = FALSE;
			return FALSE;
		}

		CM_CalcPAS(sv.worldmodel);
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
	for (i = 1; i < sv.worldmodel->numsubmodels; i++)
	{
		sv.model_precache[1 + i] = localmodels[i];
		sv.models[i + 1] = Mod_ForName(localmodels[i], FALSE);
	}

//
// load the rest of the entities
//
	ent = sv.edicts;
	memset(&ent->v, 0, sizeof(entvars_t));
	ent->free = FALSE;
	ent->v.model = sv.worldmodel->name - pr_strings;
	ent->v.modelindex = 1;		// world model
	ent->v.solid = SOLID_BSP;
	ent->v.movetype = MOVETYPE_PUSH;

	if (coop.value)
		gGlobalVariables.coop = coop.value;
	else
		gGlobalVariables.deathmatch = deathmatch.value;

	gGlobalVariables.mapname = sv.name - pr_strings;
	gGlobalVariables.startspot = sv.startspot - pr_strings;

// serverflags are for cross level information (sigils)
	gGlobalVariables.serverflags = svs.serverflags;

	allow_cheats = sv_cheats.value;

	SV_SetMoveVars();

	return TRUE;
}

void SV_LoadEntities( void )
{
	ED_LoadFromFile(sv.worldmodel->entities);
}

// Clears all entities
void SV_ClearEntities( void )
{
	int i;

	for (i = 0; i < sv.num_edicts; i++)
	{
		edict_t* pEdict = &sv.edicts[i];
		if (pEdict->free)
			continue;

		ReleaseEntityDLLFields(pEdict);
	}
}

/*
=================
RegUserMsg

Registers a user message
=================
*/
int RegUserMsg( const char* pszName, int iSize )
{
	UserMsg* pUserMsgs;
	UserMsg* pNewMsg;
	int iFound = 0;

	if (giNextUserMsg > MAX_USERMSGS)
		return 0;
	if (!pszName)
		return 0;
	if (strlen(pszName) > 11)
		return 0;
	if (iSize > MAX_USER_MSG_DATA)
		return 0;

	pUserMsgs = sv_gpUserMsgs;
	while (pUserMsgs)
	{
		if (!strcmp(pszName, pUserMsgs->szName))
		{
			iFound = 1;
			break;
		}
		pUserMsgs = pUserMsgs->next;
	}

	if (iFound)
		return pUserMsgs->iMsg;

	pNewMsg = (UserMsg*)malloc(sizeof(UserMsg));
	pNewMsg->iSize = iSize;
	pNewMsg->iMsg = giNextUserMsg;
	giNextUserMsg++;

	strcpy(pNewMsg->szName, pszName);

	if (pNewMsg)
	{
		pNewMsg->next = sv_gpNewUserMsgs;
		sv_gpNewUserMsgs = pNewMsg;
		return pNewMsg->iMsg;
	}

	return 0;
}

/*
==================
SV_SendUserReg

==================
*/
void SV_SendUserReg( sizebuf_t* sb )
{
	UserMsg* pMsg;

	pMsg = sv_gpNewUserMsgs;
	while (pMsg)
	{
		MSG_WriteByte(sb, svc_newusermsg);
		MSG_WriteByte(sb, pMsg->iMsg);
		MSG_WriteByte(sb, pMsg->iSize);
		MSG_WriteLong(sb, *(int*)(pMsg->szName + 0));
		MSG_WriteLong(sb, *(int*)(pMsg->szName + 4));
		MSG_WriteLong(sb, *(int*)(pMsg->szName + 8));
		MSG_WriteLong(sb, (int)pMsg->next);
		pMsg = pMsg->next;
	}
}

/*
==============================================================================

PACKET FILTERING


You can add or remove addresses from the filter list with:

addip <ip>
removeip <ip>

The ip address is specified in dot format, and any unspecified digits will match any value, so you can specify an entire class C network with "addip 192.246.40".

Removeip will only remove an address specified exactly the same way.  You cannot addip a subnet, then removeip a single host.

listip
Prints the current list of filters.

writeip
Dumps "addip <ip>" commands to listip.cfg so it can be execed at a later date.  The filter lists are not saved and restored by default, because I beleive it would cause too much confusion.

filterban <0 or 1>

If 1 (the default), then ip addresses matching the current list will be prohibited from entering the game.  This is the default setting.

If 0, then only addresses matching the list will be allowed.  This lets you easily set up a private game, or a game that only allows players from your local network.


==============================================================================
*/


typedef struct
{
	unsigned	mask;
	unsigned	compare;
	float		banEndTime; // 0 for permanent ban
	float		banTime;
} ipfilter_t;

#define	MAX_IPFILTERS	1024

ipfilter_t	ipfilters[MAX_IPFILTERS];
int			numipfilters;

cvar_t	filterban = { "filterban", "1" };

/*
=================
SV_SendBan
=================
*/
void SV_SendBan( void )
{
	char		data[16];

	sprintf(data, "banned.\n");

	SZ_Clear(&net_message);
	MSG_WriteLong(&net_message, 0xFFFFFFFF); // -1 -1 -1 -1 signal
	MSG_WriteByte(&net_message, A2C_PRINT);
	MSG_WriteString(&net_message, data);
	NET_SendPacket(NS_SERVER, net_message.cursize, net_message.data, net_from);
	SZ_Clear(&net_message);
}

/*
=================
SV_FilterPacket
=================
*/
qboolean SV_FilterPacket( void )
{
	int		i, j;
	unsigned	in;

	in = *(unsigned*)net_from.ip;

	// Handle timeouts 
	for (i = numipfilters - 1; i >= 0; i--)
	{
		if ((ipfilters[i].compare != 0xffffffff) &&
			 (ipfilters[i].banEndTime != 0.0f) &&
			 (ipfilters[i].banEndTime <= realtime))
		{
			for (j = i + 1; j < numipfilters; j++)
			{
				ipfilters[j - 1] = ipfilters[j];
			}
			numipfilters--;
			continue;
		}

		// Only get here if ban is still in effect.
		if ((in & ipfilters[i].mask) == ipfilters[i].compare)
			return filterban.value;
	}
	return !filterban.value;
}

/*
=================
StringToFilter
=================
*/
qboolean StringToFilter( char* s, ipfilter_t* f )
{
	char	num[128];
	int		i, j;
	byte	b[4];
	byte	m[4];

	for (i = 0; i < 4; i++)
	{
		b[i] = 0;
		m[i] = 0;
	}

	for (i = 0; i < 4; i++)
	{
		if (*s < '0' || *s > '9')
		{
			Con_Printf("Bad filter address: %s\n", s);
			return FALSE;
		}

		j = 0;
		while (*s >= '0' && *s <= '9')
		{
			num[j++] = *s++;
		}
		num[j] = 0;
		b[i] = atoi(num);
		if (b[i] != 0)
			m[i] = 255;

		if (!*s)
			break;
		s++;
	}

	f->mask = *(unsigned*)m;
	f->compare = *(unsigned*)b;

	return TRUE;
}

/*
=================
SV_AddIP_f
=================
*/
void SV_AddIP_f( void )
{
	int		i;
	float banTime;

	if (Cmd_Argc() != 3)
	{
		Con_Printf("Usage:  addip <minutes> <ipaddress>\nUse 0 minutes for permanent\n");
		return;
	}

	for (i = 0; i < numipfilters; i++)
	{
		if (ipfilters[i].compare == 0xffffffff)
			break;		// free spot
	}

	if (i == numipfilters)
	{
		if (numipfilters == MAX_IPFILTERS)
		{
			Con_Printf("IP filter list is full\n");
			return;
		}
		numipfilters++;
	}

	banTime = atof(Cmd_Argv(1));
	if (banTime < 0.01f)
		banTime = 0.0f;

	ipfilters[i].banTime = banTime;

	if (banTime)
	{
		banTime *= 60.0f;
		banTime += realtime; // Time when we are done banning.
	}

	// Time to unban.
	ipfilters[i].banEndTime = banTime;

	if (!StringToFilter(Cmd_Argv(2), &ipfilters[i]))
		ipfilters[i].compare = 0xffffffff;
}

/*
=================
SV_RemoveIP_f
=================
*/
void SV_RemoveIP_f( void )
{
	ipfilter_t	f;
	int			i, j;

	if (!StringToFilter(Cmd_Argv(1), &f))
		return;

	for (i = 0; i < numipfilters; i++)
	{
		if ((ipfilters[i].mask == f.mask) &&
			 (ipfilters[i].compare == f.compare))
		{
			for (j = i + 1; j < numipfilters; j++)
			{
				ipfilters[j - 1] = ipfilters[j];
			}
			numipfilters--;
			Con_Printf("Filter removed.\n");
			return;
		}
	}
	Con_Printf("Didn't find %s.\n", Cmd_Argv(1));
}

/*
=================
SV_ListIP_f
=================
*/
void SV_ListIP_f( void )
{
	int		i;
	byte	b[4];

	Con_Printf("Filter list:\n");
	for (i = 0; i < numipfilters; i++)
	{
		*(unsigned*)b = ipfilters[i].compare;
		if (ipfilters[i].banTime != 0.0f)
			Con_Printf("%3i.%3i.%3i.%3i : %.3f min\n", b[0], b[1], b[2], b[3], ipfilters[i].banTime);
		else
			Con_Printf("%3i.%3i.%3i.%3i : permanent\n", b[0], b[1], b[2], b[3]);
	}
}

/*
=================
SV_WriteIP_f
=================
*/
void SV_WriteIP_f( void )
{
	FILE* f;
	char	name[MAX_OSPATH];
	byte	b[4];
	int		i;
	float banTime;

	sprintf(name, "%s/listip.cfg", com_gamedir);

	Con_Printf("Writing %s.\n", name);

	f = fopen(name, "wb");
	if (!f)
	{
		Con_Printf("Couldn't open %s\n", name);
		return;
	}

	for (i = 0; i < numipfilters; i++)
	{
		*(unsigned*)b = ipfilters[i].compare;

		// Only store out the permanent bad guys from this server.
		banTime = ipfilters[i].banTime;
		if (banTime != 0.0f)
			continue;

		fprintf(f, "addip 0.0 %i.%i.%i.%i\n", b[0], b[1], b[2], b[3]);
	}

	fclose(f);
}

//============================================================================

// Print CD keys
void SV_Keys_f( void )
{
	int		i;
	char	szKey[1024];
	client_t* cl;

	if (!sv.active)
	{
		Con_Printf("Not running a server\n");
		return;
	}

	Con_Printf("CD Keys =================\n");
	for (i = 0, cl = svs.clients; i < svs.maxclients; i++, cl++)
	{
		if (cl->active || cl->connected || cl->spawned)
		{
			sprintf(szKey, "%3i %s : %s\n", i + 1, cl->name, cl->hashedcdkey);
			Con_Printf(szKey);
		}
	}
	Con_Printf("==========================\n");
}