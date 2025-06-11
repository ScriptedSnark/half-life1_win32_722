// sv_main.c -- server main program

#include "quakedef.h"
#include "pr_cmds.h"
#include "pmove.h"
#include "decal.h"
#include "cmodel.h"

server_t		sv;
server_static_t	svs;


// TODO: Implement


char* pr_strings = NULL, *gNullString = "";
globalvars_t gGlobalVariables;

int sv_playermodel;

char	localmodels[MAX_MODELS][5];			// inline model names for precache

qboolean	allow_cheats;

decalname_t	sv_decalnames[MAX_BASE_DECALS];


// TODO: Implement

int gPacketSuppressed = 0;
int			sv_decalnamecount;

// Usermsg
UserMsg* sv_gpNewUserMsgs;
UserMsg* sv_gpUserMsgs;
int giNextUserMsg = 64;

int		nReliableBytesSent = 0;
int		nDatagramBytesSent = 0;
int		nReliables = 0;
int		nDatagrams = 0;
qboolean bUnreliableOverflow = FALSE;

// TODO: Implement

float g_LastScreenUpdateTime;
float scr_centertime_off;

qboolean bShouldUpdatePing = FALSE;

cvar_t sv_password = { "sv_password", "" };
cvar_t sv_spectator_password = { "sv_spectator_password", "" };
cvar_t sv_maxspectators = { "sv_maxspectators", "8", FALSE, TRUE };

cvar_t sv_newunit = { "sv_newunit", "0" };

cvar_t sv_cheats = { "sv_cheats", "0", FALSE, TRUE };




cvar_t sv_netsize = { "sv_netsize", "0" };
cvar_t sv_allow_download = { "sv_allowdownload", "1", FALSE, TRUE };
cvar_t sv_allow_upload = { "sv_allowupload", "1", FALSE, TRUE };
cvar_t sv_upload_maxsize = { "sv_upload_maxsize", "0", FALSE, TRUE };
cvar_t sv_showcmd = { "sv_showcmd", "0" };

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

/*
==================
SV_SpawnSpectator
==================
*/
void SV_SpawnSpectator( void )
{
	_asm { int 3 };
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
	int		i, oldest, oldestTime = 0x7FFFFFFF; //INT_MAX
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
	// TODO: Implement
	else if (!strcmp(c, "getchallenge"))
	{
		SVC_GetChallenge();
	}
	else if (!strcmp(c, "connect"))
	{
		SV_ConnectClient();
	}
	else if (!strcmp(c, "rcon"))
	{
		Host_RemoteCommand(&net_from);
	}
	else
	{
		Con_Printf("bad connectionless packet from %s:\n%s\n", NET_AdrToString(net_from), s);
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

/*
===================
SV_CalcPing

===================
*/
int SV_CalcPing( client_t* cl )
{
	float		ping;
	int			i;
	int			count;
	register	client_frame_t* frame;

	ping = 0;
	count = 0;
	for (frame = cl->frames, i = 0; i < UPDATE_BACKUP; i++, frame++)
	{
		if (frame->ping_time > 0)
		{
			ping += frame->ping_time;
			count++;
		}
	}
	if (!count)
		return 9999;
	ping /= count;

	return ping * 1000;
}

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
===============================================================================

FRAME UPDATES

===============================================================================
*/

/*
==================
SV_ClearDatagram

==================
*/
void SV_ClearDatagram(void)
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

void SV_AddToFatPVS(vec3_t org, mnode_t* node)
{
	int		i;
	byte*	pvs;
	mplane_t*	plane;
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
byte* SV_FatPVS(vec3_t org)
{
	fatbytes = (sv.worldmodel->numleafs + 31) >> 3;
	Q_memset(fatpvs, 0, fatbytes);
	SV_AddToFatPVS(org, sv.worldmodel->nodes);
	return fatpvs;
}

//=============================================================================

/*
==================
SV_Init

General initialization of the server
==================
*/
void SV_Init( void )
{
	int		i;

	Cvar_RegisterVariable(&sv_password);

	// TODO: Implement
	
	Cvar_RegisterVariable(&sv_idealpitchscale);
	Cvar_RegisterVariable(&sv_aim);

	// TODO: Implement
	
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

// TODO: Implement

int SV_PointLeafnum( vec_t* p )
{
	mleaf_t* pLeaf;

	pLeaf = Mod_PointInLeaf(p, sv.worldmodel);
	if (pLeaf)
		return pLeaf - sv.worldmodel->leafs;

	return 0;
}

// TODO: Implement

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
	// TODO: Implement
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
	if (sample[0] == '!')
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
	if ((field_mask & 8) != 0)
		MSG_WriteByte(&sv.multicast, pitch);
	if (channel == 6)
		SV_Multicast(origin, MSG_BROADCAST, FALSE);
	else
		SV_Multicast(origin, MSG_ALL, FALSE);
}

// TODO: Implement

/*
=============================================================================

MULTICAST MESSAGE

=============================================================================
*/

qboolean IsSinglePlayerGame( void )
{
	return svs.maxclients == 1;
}

BOOL SV_ValidClientMulticast(client_t *a1, int leafnum, int ArgList)
{
	// TODO: Refactor

	byte *v3; // esi
	byte *v5; // eax
	int v6; // eax

	v3 = 0;
	if (IsSinglePlayerGame())
		return 1;
	if (ArgList)
	{
		if (ArgList == 1)
		{
			v5 = CM_LeafPVS(leafnum);
		}
		else
		{
			if (ArgList != 2)
			{
				Con_Printf("MULTICAST: Error %d!\n", ArgList);
				return 0;
			}
			v5 = CM_LeafPAS(leafnum);
		}
		v3 = v5;
	}
	if (!v3)
		return 1;
	v6 = SV_PointLeafnum(a1->edict->v.origin);
	return (v3[(v6 - 1) >> 3] & (1 << ((v6 - 1) & 7))) != 0;
}

/*
==================
SV_Multicast

Write client buffers based on valid multicast recipients
==================
*/
void SV_Multicast( vec_t* origin, int to, qboolean reliable )
{
	// TODO: Refactor

	int i; // ebp
	int v4; // ebx
	client_t *client; // edi
	sizebuf_t *p_message; // ecx

	i = 0;
	v4 = SV_PointLeafnum(origin);
	for (client = svs.clients; svs.maxclients > i; ++client)
	{
		if (client->active)
		{
			if (SV_ValidClientMulticast(client, v4, to))
			{
				p_message = &client->netchan.message;
				if (reliable == FALSE)
					p_message = &client->datagram;
				if (p_message->maxsize - p_message->cursize > sv.multicast.cursize)
					SZ_Write(p_message, sv.multicast.data, sv.multicast.cursize);
			}
			else
			{
				gPacketSuppressed += sv.multicast.cursize;
			}
		}
		++i;
	}
	SZ_Clear(&sv.multicast);
}

// TODO: Implement

/*
================
SV_CreateBaseline

================
*/
void SV_CreateBaseline( void )
{
	int				i;
	edict_t*		svent;
	int				entnum;

	if (!sv.num_edicts)
		return;

	for (entnum = 0; entnum < sv.num_edicts; entnum++)
	{
		// get the current server version
		svent = &sv.edicts[entnum];
		if (svent->free)
			continue;
		if (entnum > svs.maxclients && !svent->v.modelindex)
			continue;

		svent->baseline.entityType = ENTITY_NORMAL;
		if ((svent->v.flags & FL_CUSTOMENTITY) != 0)
			svent->baseline.entityType = ENTITY_BEAM;

		//
		// create entity baseline
		//
		VectorCopy(svent->v.origin, svent->baseline.origin);
		VectorCopy(svent->v.angles, svent->baseline.angles);
		VectorCopy(svent->v.mins, svent->baseline.mins);
		VectorCopy(svent->v.maxs, svent->baseline.maxs);
		svent->baseline.skin = svent->v.skin;
		svent->baseline.frame = svent->v.frame;
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

		if (svent->baseline.entityType != ENTITY_NORMAL)
			MSG_WriteByte(&sv.signon, svent->baseline.scale);
		else
			MSG_WriteWord(&sv.signon, svent->baseline.scale * 256);

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
SV_SendServerinfo

Sends the first message from the server to a connected client.
This will be sent on the initial connection and upon each server load.
================
*/
void SV_SendServerinfo( client_t *client )
{
	client_t*		cl;
	char			message[2048];
	int				i;

	if (developer.value == 0 && svs.maxclients <= 1)
	{
		cl = client;
	}
	else
	{
		cl = client;
		MSG_WriteByte(&client->netchan.message, svc_print);
		sprintf(message, "%c\nBUILD %d SERVER (%i CRC)\nServer # %i\n", 2, build_number(), 0, svs.spawncount);
		MSG_WriteString(&client->netchan.message, message);
	}

	MSG_WriteByte(&cl->netchan.message, svc_serverinfo);
	MSG_WriteLong(&cl->netchan.message, PROTOCOL_VERSION);
	MSG_WriteLong(&cl->netchan.message, svs.spawncount);
	MSG_WriteLong(&cl->netchan.message, sv.worldmapCRC);
	MSG_WriteLong(&cl->netchan.message, sv.clientSideDllCRC);
	MSG_WriteByte(&cl->netchan.message, svs.maxclients);
	i = NUM_FOR_EDICT(cl->edict) - 1;
	if (cl->spectator)
		i |= PN_SPECTATOR;
	MSG_WriteByte(&cl->netchan.message, i);

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
	UserMsg*	pMsg;
	int			size;

	if (host_client->spawned && !host_client->active)
		return;

	host_client->connection_started = realtime;
	host_client->connected = TRUE;
	SZ_Clear(&host_client->netchan.message);
	SV_SendServerinfo(host_client);

	size = host_client->netchan.message.cursize;
	if (sv_netsize.value != 0.0)
	{
		Con_DPrintf("SINFO=%i\n", size);
	}

	size = host_client->netchan.message.cursize;
	if (sv_netsize.value != 0.0)
	{
		Con_DPrintf("DECALS=%i\n", host_client->netchan.message.cursize - size);
	}

	size = host_client->netchan.message.cursize;
	if (sv_gpUserMsgs)
	{
		pMsg = sv_gpNewUserMsgs;
		sv_gpNewUserMsgs = sv_gpUserMsgs;
		SV_SendUserReg(&host_client->netchan.message);
		sv_gpNewUserMsgs = pMsg;
	}

	if (sv_netsize.value != 0.0)
	{
		Con_DPrintf("USR=%i\n", host_client->netchan.message.cursize - size);
	}

	if (host_client->spectator)
	{
		SV_SpawnSpectator();
		gGlobalVariables.time = sv.time;
		gEntityInterface.pfnParmsNewLevel();
		gEntityInterface.pfnSpectatorConnect(host_client->edict);
	}
	else
	{
		gEntityInterface.pfnClientConnect(host_client->edict);
	}

	if (sv_netsize.value != 0.0)
		Con_DPrintf("CLSIZE = %i\n", host_client->netchan.message.cursize - size);

	++net_activeconnections;
}

/*
================
SV_ActivateServer

================
*/
void SV_ActivateServer( qboolean runPhysics )
{
	int			i;
	UserMsg*	pMsg;

	Cvar_Set("sv_newunit", "0");

	// Activate the DLL server code
	gEntityInterface.pfnServerActivate(sv.edicts, sv.num_edicts, svs.maxclients);

	sv.active = TRUE;
	// all setup is completed, any further precache statements are errors
	sv.state = ss_active;

	if (runPhysics)
	{
		// run two frames to allow everything to settle
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

	sv.signon_buffer_size[sv.num_signon_buffers - 1] = sv.signon.cursize;

	SV_CreateResourceList();

	// Send serverinfo to all connected clients
	for (i = 0, host_client = svs.clients; i < svs.maxclients; i++, host_client++)
	{
		if (host_client->active || host_client->connected)
		{
			SV_SendServerinfo(host_client);
			if (sv_gpUserMsgs)
			{
				pMsg = sv_gpNewUserMsgs;
				sv_gpNewUserMsgs = sv_gpUserMsgs;
				SV_SendUserReg(&host_client->netchan.message);
				sv_gpNewUserMsgs = pMsg;
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

	sv.multicast.maxsize = sizeof(sv.multicast_buf);
	sv.multicast.data = sv.multicast_buf;

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

// TODO: Implement

/*
=================
RegUserMsg

Registers a user message
The name of the message is used to find an exported function in the client library
The format is MsgFunc_<name>
=================
*/
int RegUserMsg( const char* pszName, int iSize )
{
	UserMsg* pUserMsgs;
	UserMsg* pNewMsg;
	int iFound = 0;
	int iRet;

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
	{
		return pUserMsgs->iMsg;
	}

	pNewMsg = (UserMsg*)malloc(sizeof(UserMsg));
	pNewMsg->iSize = iSize;
	pNewMsg->iMsg = giNextUserMsg;
	giNextUserMsg++;

	strcpy(pNewMsg->szName, pszName);

	if (pNewMsg)
	{
		pNewMsg->next = sv_gpNewUserMsgs;
		sv_gpNewUserMsgs = pNewMsg;
		iRet = pNewMsg->iMsg;
		return iRet;
	}

	return 0;
}

// TODO: Implement

/*
================
SV_LoadEntities

Load up the entities from the bsp
=======
*/
void SV_LoadEntities( void )
{
	ED_LoadFromFile(sv.worldmodel->entities);
}