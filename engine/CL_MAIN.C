// cl_main.c  -- client main loop

#include "quakedef.h"
#include "winquake.h"
#include "crc.h"
#include "clientid.h"
#include "pmove.h"
#include "decal.h"
#include "hashpak.h"
#include "cl_demo.h"
#include "cl_tent.h"
#include "cl_servercache.h"
#include "tmessage.h"

// Only send this many requests before timing out.
#define CL_CONNECTION_RETRIES		4

// these two are not intended to be set directly
cvar_t	cl_name = { "_cl_name", "player", TRUE };
cvar_t	cl_color = { "_cl_color", "0", TRUE };

cvar_t	cl_timeout = { "cl_timeout", "305", TRUE };
cvar_t	cl_shownet = { "cl_shownet", "0" };	// can be 0, 1, or 2
cvar_t	cl_showsizes = { "cl_showsizes", "0" };
cvar_t	cl_nolerp = { "cl_nolerp", "0" };
cvar_t	cl_stats = { "cl_stats", "0" };
cvar_t	cl_spectator_password = { "cl_spectator_password", "0" };

cvar_t	lookspring = { "lookspring", "0", TRUE };
cvar_t	lookstrafe = { "lookstrafe", "0", TRUE };
cvar_t	sensitivity = { "sensitivity", "3", TRUE };

cvar_t	cl_skyname = { "cl_skyname", "desert", TRUE };
cvar_t	cl_skycolor_r = { "cl_skycolor_r", "0" };
cvar_t	cl_skycolor_g = { "cl_skycolor_g", "0" };
cvar_t	cl_skycolor_b = { "cl_skycolor_b", "0" };
cvar_t	cl_skyvec_x = { "cl_skyvec_x", "0" };
cvar_t	cl_skyvec_y = { "cl_skyvec_y", "0" };
cvar_t	cl_skyvec_z = { "cl_skyvec_z", "0" };

cvar_t	cl_predict_players = { "cl_predict_players", "1" };
cvar_t	cl_solid_players = { "cl_solid_players", "1" };
cvar_t	cl_nodelta = { "cl_nodelta", "0" };
cvar_t	cl_printplayers = { "cl_printplayers", "0" };

cvar_t	m_pitch = { "m_pitch", "0.022", TRUE };
cvar_t	m_yaw = { "m_yaw", "0.022", TRUE };
cvar_t	m_forward = { "m_forward", "1", TRUE };
cvar_t	m_side = { "m_side", "0.8", TRUE };

cvar_t	cl_pitchup = { "cl_pitchup", "89" };
cvar_t	cl_pitchdown = { "cl_pitchdown", "89" };

cvar_t	rcon_password = { "rcon_password", "" };
cvar_t	rcon_address = { "rcon_address", "" };
cvar_t	rcon_port = { "rcon_port", "0" };

cvar_t	cl_resend = { "cl_resend", "3.0" };
cvar_t	cl_downloadinterval = { "cl_downloadinterval", "1.0" };
cvar_t	cl_slisttimeout = { "cl_slist", "10.0" };
cvar_t	cl_allowdownload = { "cl_allowdownload", "1" };
cvar_t	cl_allowupload = { "cl_allowupload", "1" };
cvar_t	cl_upload_max = { "cl_upload_max", "0" };
cvar_t	cl_download_max = { "cl_download_max", "0" };
cvar_t	cl_download_ingame = { "cl_download_ingame", "1" };

client_static_t	cls;
client_state_t cl;

static server_cache_t	cached_servers[MAX_LOCAL_SERVERS];

// FIXME: put these on hunk?
efrag_t			cl_efrags[MAX_EFRAGS];
cl_entity_t*	cl_entities = NULL;
cl_entity_t		cl_static_entities[MAX_STATIC_ENTITIES];
lightstyle_t	cl_lightstyle[MAX_LIGHTSTYLES];
dlight_t		cl_dlights[MAX_DLIGHTS];
dlight_t		cl_elights[MAX_ELIGHTS];

// refresh list
// this is double buffered so the last frame
// can be scanned for oldorigins of trailing objects
int				cl_numvisedicts, cl_oldnumvisedicts, cl_numbeamentities;
cl_entity_t* cl_visedicts, * cl_oldvisedicts;
cl_entity_t	cl_visedicts_list[2][MAX_VISEDICTS];



qboolean cl_inmovie;

qboolean g_bSkipDownload = FALSE;
qboolean g_bSkipUpload = FALSE;

int playerbitcounts[MAX_CLIENTS];  // # of bytes of player data for this slot

/*
=================
CL_UpdateSoundFade

Modulates sound volume on the client.
=================
*/
void CL_UpdateSoundFade( void )
{
	// TODO: Implement
}

/*
=================
CL_ParseMOTD

=================
*/
void CL_ParseMOTD( void )
{
	// TODO: Implement
}

/*
=================
CL_ParseServerInfoResponse

=================
*/
void CL_ParseServerInfoResponse( void )
{
	// TODO: Implement
}

/*
=================
CL_Slist_f (void)

Populates the client's server_cache_t structrue
Replaces Slist command
=================
*/
void CL_Slist_f( void )
{
	// TODO: Implement
}

/*
=================
CL_ClearCachedServers_f (void)

=================
*/
void CL_ClearCachedServers_f( void )
{
	// TODO: Implement
}

/*
=================
CL_PingServers_f

Broadcast pings to any servers that we can see on our LAN
=================
*/
void CL_PingServers_f( void )
{
	// TODO: Implement
}

/*
=================
CL_AddToServerCache

Adds the address, name to the server cache
=================
*/
void CL_AddToServerCache( netadr_t adr, char* name, char* map, char* desc, int active, int maxplayers )
{
	// TODO: Implement
}

/*
===================
CL_ListCachedServers_f()

===================
*/
void CL_ListCachedServers_f( void )
{
	// TODO: Implement
}

/*
=================
CL_ConnectionlessPacket

Responses to broadcasts, etc
=================
*/
void CL_ConnectionlessPacket( void )
{
	int		c;
	byte	data[6];

	MSG_BeginReading();
	MSG_ReadLong();        // skip the -1 marker

	c = MSG_ReadByte();

	switch (c)
	{
	case S2C_CONNECTION:
		// Already connected?
		if (cls.state == ca_connected)
		{
			if (!cls.demoplayback)
			{
				Con_Printf("Duplicate connect ack. received.  Ignored.\n");
			}
		}
		else
		{
			// Initiate the network channel
			Netchan_Setup(NS_CLIENT, &cls.netchan, net_from);

			// Signon process will commence now that server ok'd connection.
			MSG_WriteChar(&cls.netchan.message, clc_stringcmd);
			MSG_WriteString(&cls.netchan.message, "new");

			// Report connection success.
			if (_stricmp("loopback", NET_AdrToString(net_from)))
				Con_Printf("Connection accepted by %s\n", NET_AdrToString(net_from));
			else
				Con_DPrintf("Connection accepted.\n");

			// Bump connection time to now so we don't resend a connection
			// Request
			cls.connect_time = realtime;

			// Mark client as connected
			cls.state = ca_connected;

			// Not in the demo loop now
			cls.demonum = -1;
			// Need all the signon messages before playing ( or drawing first frame )
			cls.signon = 0;

			memset(cls.trueaddress, 0, sizeof(cls.trueaddress));
		}
		break;

	case S2C_CHALLENGE:
		cls.challenge = BigLong(MSG_ReadLong());
		CL_SendConnectPacket();
		break;

	case A2C_PRINT:
		Con_Printf(MSG_ReadString());
		break;

	// ping from somewhere
	case A2A_PING:
		data[0] = 0xFF;
		data[1] = 0xFF;
		data[2] = 0xFF;
		data[3] = 0xFF;
		data[4] = A2A_ACK;
		data[5] = 0;
		NET_SendPacket(NS_CLIENT, sizeof(data), data, net_from);
		break;

	case S2A_INFO:
		CL_ParseServerInfoResponse();
		break;

	case M2A_MOTD:
		CL_ParseMOTD();
		break;

	default:
		Con_Printf("Unknown command:\n%c\n", c);
		break;
	}
}

/*
====================
CL_GetMessage

Handles recording and playback of demos, on top of NET_ code
====================
*/
qboolean CL_GetMessage( void )
{
	if (cls.demoplayback)
		return CL_ReadDemoMessage();

	return NET_GetPacket(NS_CLIENT);
}

/*
=================
CL_ReadPackets

Updates the local time and reads/handles messages on client net connection.
=================
*/
void CL_ReadPackets( void )
{
	cl.oldtime = cl.time;
	cl.time += host_frametime;

	while (CL_GetMessage())
	{
		if (*(int*)net_message.data == -1)
		{
			CL_ConnectionlessPacket();
			continue;
		}

		if (cls.state == ca_disconnected ||
			cls.state == ca_connecting)
			continue;

		if (net_message.cursize < 8)
		{
			Con_Printf("%s: Runt packet\n", NET_AdrToString(net_from));
			continue;
		}

		//
		// packet from server, verify source IP address.
		//
		if (!cls.demoplayback && !NET_CompareAdr(net_from, cls.netchan.remote_address))
		{
			Con_Printf("%s:sequenced packet without connection\n", NET_AdrToString(net_from));
			continue;
		}

		if (cls.demoplayback)
		{
			MSG_BeginReading();
			MSG_ReadLong();		// skip the -1 marker
			MSG_ReadLong();

			// Parse out the commands.
			CL_ParseServerMessage();
			continue;
		}

		if (Netchan_Process(&cls.netchan))
		{
			// Parse out the commands.
			CL_ParseServerMessage();
			continue;
		}
	}

	// check timeout, but not if running _DEBUG engine
#if !defined( _DEBUG )
	// Only check on final frame because that's when the server might send us a packet in single player.  This avoids
	//  a bug where if you sit in the game code in the debugger then you get a timeout here on resuming the engine
	//  because the timestep is > 1 tick because of the debugging delay but the server hasn't sent the next packet yet.
	if ((cls.state >= ca_connected) &&
		(!cls.demoplayback) &&
		((realtime - cls.netchan.last_received) > cl_timeout.value))
	{
		Con_Printf("\nServer connection timed out.\n");
		CL_Disconnect();
		return;
	}
#endif

	if (cl_shownet.value)
		Con_Printf("\n");
}

/*
================
CL_PrintCustomizations_f

================
*/
void CL_PrintCustomizations_f( void )
{
	// TODO: Implement
}

/*
================
CL_CreateCustomizationList

================
*/
void CL_CreateCustomizationList( void )
{
	int i;
	customization_t* pCust;
	resource_t* pResource;
	player_info_t* pPlayer;

	pPlayer = &cl.players[cl.playernum];
	pPlayer->customdata.pNext = NULL;

	for (i = 0, pResource = cl.resourcelist; i < cl.num_resources; i++, pResource++)
	{
		pCust = (customization_t*)malloc(sizeof(customization_t));
		memset(pCust, 0, sizeof(customization_t));

		memcpy(&pCust->resource, pResource, sizeof(pCust->resource));

		if (pResource->nDownloadSize)
		{
			pCust->bInUse = TRUE;
			pCust->pBuffer = COM_LoadFile(pResource->szFileName, 5, NULL);

			if ((pCust->resource.ucFlags & RES_CUSTOM) && pCust->resource.type == t_decal)
			{
				cachewad_t* pWad;

				pCust->resource.playernum = cl.playernum;

				pWad = (cachewad_t*)malloc(sizeof(cachewad_t));
				pCust->pInfo = pWad;
				memset(pWad, 0, sizeof(cachewad_t));
				CustomDecal_Init(pWad, pCust->pBuffer, pResource->nDownloadSize);

				pCust->bTranslated = TRUE;
				pCust->nUserData1 = 0;
				pCust->nUserData2 = pWad->lumpCount;
			}
		}

		pCust->pNext = pPlayer->customdata.pNext;
		pPlayer->customdata.pNext = pCust;
	}
}

void CL_ClearClientState( void )
{
	int i;
	packet_entities_t* pClientPack;

	for (i = 0; i < UPDATE_BACKUP; i++)
	{
		pClientPack = &cl.frames[i].packet_entities;
		if (pClientPack->entities)
			free(pClientPack->entities);

		pClientPack->entities = NULL;
		pClientPack->num_entities = 0;
	}

	CL_ClearResourceLists();

	for (i = 0; i < MAX_CLIENTS; i++)
	{
		COM_ClearCustomizationList(&cl.players[i].customdata, FALSE);
	}

	Q_memset(&cl, 0, sizeof(client_state_t));

	cl.resourcesneeded.pPrev = &cl.resourcesneeded;
	cl.resourcesneeded.pNext = &cl.resourcesneeded;
	cl.resourcesonhand.pPrev = &cl.resourcesonhand;
	cl.resourcesonhand.pNext = &cl.resourcesonhand;

	CL_CreateResourceList();
}

/*
=====================
CL_ClearState

=====================
*/
void CL_ClearState( qboolean bQuiet )
{
	int			i;

	if (!sv.active)
		Host_ClearMemory(bQuiet);

	CL_ClearClientState();

	SZ_Clear(&cls.netchan.message);

// clear other arrays
	memset(cl_efrags, 0, sizeof(cl_efrags));
	memset(cl_dlights, 0, sizeof(cl_dlights));
	memset(cl_elights, 0, sizeof(cl_elights));
	memset(cl_lightstyle, 0, sizeof(cl_lightstyle));

	CL_TempEntInit();

//
// allocate the efrags and chain together into a free list
//	
	cl.free_efrags = cl_efrags;
	for (i = 0; i < MAX_EFRAGS - 1; i++)
		cl.free_efrags[i].entnext = &cl.free_efrags[i + 1];

	cl.free_efrags[i].entnext = NULL;
}

/*
=====================
CL_Disconnect

Sends a disconnect message to the server
This is also called on Host_Error, so it shouldn't cause any errors
=====================
*/
void CL_Disconnect( void )
{
	cls.connect_time = -99999.0;
	cls.connect_retry = 0;

	// stop sounds (especially looping!)
	S_StopAllSounds(TRUE);

	// if running a local server, shut it down
	if (cls.demoplayback)
		CL_StopPlayback();
	else if (cls.state == ca_active
			 || cls.state == ca_connected
			 || cls.state == ca_uninitialized
			 || cls.state == ca_connecting)
	{
		if (cls.demorecording)
			CL_Stop_f();

		if (cls.netchan.remote_address.type != NA_UNUSED)
		{
			byte	final[20];

			// Send a drop command.
			final[0] = clc_stringcmd;
			strcpy((char*)(final + 1), "dropclient");
			Netchan_Transmit(&cls.netchan, 12, final);
			Netchan_Transmit(&cls.netchan, 12, final);
			Netchan_Transmit(&cls.netchan, 12, final);
		}

		cls.state = ca_disconnected;

		if (sv.active)
			Host_ShutdownServer(FALSE);
	}

	cls.timedemo = FALSE;
	cls.demoplayback = FALSE;
	cls.signon = 0;

	if (cls.download)
	{
		fclose(cls.download);
		cls.download = NULL;
	}

	if (cls.upload)
	{
		COM_FreeFile(cls.upload);
		cls.upload = NULL;
	}

	CL_ClearState(TRUE);

	CL_DeallocateDynamicData();

	Cam_Reset();
}

/*
=====================
CL_Disconnect_f

Disconnects user from the server
=====================
*/
void CL_Disconnect_f( void )
{
	CL_Disconnect();
	if (sv.active)
		Host_ShutdownServer(FALSE);
}

// HL1 CD Key
#define GUID_LEN 13

/*
=================
CL_GetCDKeyHash

Connections will now use a hashed cd key value
A LAN server will know not to allows more then xxx users with the same CD Key
=================
*/
char* CL_GetCDKeyHash( void )
{
	char szKeyBuffer[256]; // Keys are about 13 chars long.	
	static char szHashedKeyBuffer[256];
	int nKeyLength = GUID_LEN;
	int bDedicated = 0;
	MD5Context_t ctx;
	unsigned char digest[16]; // The MD5 Hash

	// Get the cd key.
	Launcher_GetCDKey(szKeyBuffer, &nKeyLength, &bDedicated);

	// A dedicated server
	if (bDedicated)
	{
		Con_Printf("Key has no meaning on dedicated server...\n");
		return "";
	}

	if (nKeyLength <= 0 ||
		nKeyLength >= 256)
	{
		Con_Printf("Bogus key length on CD Key...\n");
		return "";
	}

	szKeyBuffer[nKeyLength] = 0;

	// Now get the md5 hash of the key
	memset(&ctx, 0, sizeof(ctx));
	memset(digest, 0, sizeof(digest));

	MD5Init(&ctx);
	MD5Update(&ctx, (unsigned char*)szKeyBuffer, nKeyLength);
	MD5Final(digest, &ctx);
	memset(szHashedKeyBuffer, 0, sizeof(szHashedKeyBuffer));
	strcpy(szHashedKeyBuffer, MD5_Print(digest));
	return szHashedKeyBuffer;
}

/*
=================
CL_SendConnectPacket

called by CL_Connect and CL_CheckResend
If we are in ca_connecting state and we have gotten a challenge
  response before the timeout, send another "connect" request.
=================
*/
void CL_SendConnectPacket( void )
{
	netadr_t adr;
	char	data[2048];
	char szServerName[128];

	strncpy(szServerName, cls.servername, sizeof(szServerName));

	// Deal with local connection
	if (!_strcmpi(cls.servername, "local"))
	{
		sprintf(szServerName, "%s", "localhost");
	}

	if (!NET_StringToAdr(szServerName, &adr))
	{
		Con_Printf("Bad server address\n");
		cls.connect_time = -99999.0;
		cls.connect_retry = 0;
		cls.state = ca_disconnected;
		return;
	}

	if (adr.port == (unsigned short)0)
	{
		adr.port = BigShort((unsigned short)atoi(PORT_SERVER));
	}

	if (strlen(cls.trueaddress) == 0)
		strcpy(cls.trueaddress, "0");

	if (cls.spectator)
	{
		sprintf(data, "%c%c%c%cconnect %i %i \"%s\" %i %i \"%s\" \"%s\" \"%s\"", 255, 255, 255, 255,
			PROTOCOL_VERSION,
			cls.challenge,
			cl_name.string,
			2,
			strlen(CL_GetCDKeyHash()),
			CL_GetCDKeyHash(),
			cls.trueaddress,
			cl_spectator_password.string);  // Send protocol and challenge value
	}
	else
	{
		sprintf(data, "%c%c%c%cconnect %i %i \"%s\" %i %i \"%s\" \"%s\"\n", 255, 255, 255, 255,
			PROTOCOL_VERSION,
			cls.challenge,
			cl_name.string,
			2,
			strlen(CL_GetCDKeyHash()),
			CL_GetCDKeyHash(),
			cls.trueaddress);  // Send protocol and challenge value
	}

	NET_SendPacket(NS_CLIENT, strlen(data), data, adr);
}


/*
=================
CL_CheckForResend

Resend a connect message if the last one has timed out
=================
*/
void CL_CheckForResend( void )
{
	netadr_t	adr;
	char	data[2048];
	char szServerName[128];

	if (cls.state == ca_disconnected && sv.active)
	{
		cls.state = ca_connecting;
		strncpy(cls.servername, "localhost", sizeof(cls.servername) - 1);
		CL_SendConnectPacket();
		return;
	}

	// resend if we haven't gotten a reply yet
	// We only resend during the connection process.
	if (cls.state != ca_connecting)
		return;

	if (cl_resend.value < 1.5)
		Cvar_SetValue("cl_resend", 1.5);
	else if (cl_resend.value > 20.0)
		Cvar_SetValue("cl_resend", 20.0);

	// Wait at least the resend # of seconds.
	if ((realtime - cls.connect_time) < cl_resend.value)
		return;

	strncpy(szServerName, cls.servername, sizeof(szServerName));

	// Deal with local connection.
	if (!_stricmp(cls.servername, "local"))
		sprintf(szServerName, "%s", "localhost");

	if (!NET_StringToAdr(szServerName, &adr))
	{
		Con_Printf("Bad server address\n");
		cls.state = ca_disconnected;
		return;
	}

	// Only retry so many times before failure.
	if (cls.connect_retry >= CL_CONNECTION_RETRIES)
	{
		Con_Printf("Connection failed after %i retries.\n", cls.connect_retry);
		cls.connect_time = -99999.0;
		cls.connect_retry = 0;
		cls.state = ca_disconnected;
		return;
	}

	// Mark time of this attempt.
	cls.connect_time = realtime;	// for retransmit requests

	// Display appropriate message
	if (_stricmp(szServerName, "localhost"))
	{
		if (cls.connect_retry == 0)
			Con_Printf("Connecting to %s...\n", szServerName);			
		else
			Con_Printf("Retrying %s...\n", szServerName);
	}

	cls.connect_retry++;

	// Request another challenge value.
	sprintf(data, "%c%c%c%cgetchallenge\n", 255, 255, 255, 255);

	// Send the request.
	NET_SendPacket(NS_CLIENT, strlen(data), data, adr);
}

/*
=====================
CL_Connect_f

User command to connect to server
=====================
*/
void CL_Connect_f( void )
{
	char* server;
	char name[128], * p;
	int num;

	cls.spectator = FALSE;

	if (Cmd_Argc() < 2)
	{
		Con_Printf("usage: connect <server> [server password]\n");
		return;
	}

	server = Cmd_Args();
	if (!server)
		return;

	// Disconnect from current server
	// Don't call Host_Disconnect, because we don't want to shutdown the listen server!
	CL_Disconnect();

	// Get new server name
	memset(name, 0, sizeof(name));
	strncpy(name, server, sizeof(name));

	p = name;
	while (*p++)
	{
		if (*p == ' ')
			break;
	}

	if (p[0] && p[1])
		strcpy(cls.trueaddress, p + 1);
	else
		strcpy(cls.trueaddress, "0");

	num = atoi(server);  // In case it's an index.


	// TODO: Implement


	memset(msg_buckets, 0, sizeof(msg_buckets));
	memset(total_data, 0, sizeof(total_data));

	strncpy(cls.servername, name, sizeof(cls.servername) - 1);

	// For the check for resend timer to fire a connection / getchallenge request.
	cls.state = ca_connecting;
	// Force connection request to fire.
	cls.connect_time = -99999;

	cls.connect_retry = 0;
}

/*
=====================
CL_Spectate_f

User command to connect to server as spectator
=====================
*/
void CL_Spectate_f( void )
{
	char* server;
	char name[128], * p;
	int num;

	cls.spectator = TRUE;

	if (Cmd_Argc() < 2)
	{
		Con_Printf("usage: spectate <server> [server password]\n");
		return;
	}

	server = Cmd_Args();
	if (!server)
		return;

	// Disconnect from current server
	// Don't call Host_Disconnect, because we don't want to shutdown the listen server!
	CL_Disconnect();

	// Get new server name
	memset(name, 0, sizeof(name));
	strncpy(name, server, sizeof(name));

	p = name;
	while (*p++)
	{
		if (*p == ' ')
			break;
	}

	if (p[0] && p[1])
		strcpy(cls.trueaddress, p + 1);
	else
		strcpy(cls.trueaddress, "0");

	num = atoi(server);


	// TODO: Implement


	memset(msg_buckets, 0, sizeof(msg_buckets));
	memset(total_data, 0, sizeof(total_data));

	strncpy(cls.servername, name, sizeof(cls.servername) - 1);

	// For the check for resend timer to fire a connection / getchallenge request.
	cls.state = ca_connecting;
	// Force connection request to fire.
	cls.connect_time = -99999;

	cls.connect_retry = 0;
}

/*
=================
CL_SignonReply

A svc_signonnum has been received, perform a client side setup
=================
*/
void CL_SignonReply( void )
{
	char 	str[8192];

	Con_DPrintf("CL_SignonReply: %i\n", cls.signon);

	switch (cls.signon)
	{
	case 1:
		MSG_WriteByte(&cls.netchan.message, clc_stringcmd);
		MSG_WriteString(&cls.netchan.message, va("name \"%s\"\n", cl_name.string));

		MSG_WriteByte(&cls.netchan.message, clc_stringcmd);
		MSG_WriteString(&cls.netchan.message, va("color %i %i\n", ((int)cl_color.value) >> 4, ((int)cl_color.value) & 15));

		MSG_WriteByte(&cls.netchan.message, clc_stringcmd);
		sprintf(str, "spawn %i %s", cl.servercount, cls.spawnparms);
		MSG_WriteString(&cls.netchan.message, str);
		break;

	case 2:
		MSG_WriteByte(&cls.netchan.message, clc_stringcmd);
		MSG_WriteString(&cls.netchan.message, "begin");
		Cache_Report();		// print remaining memory
		break;

	case 3:
		SCR_EndLoadingPlaque();		// allow normal screen updates
		break;
	}
}

/*
==================
CL_NextDemo

Called to play the next demo in the demo loop
==================
*/
void CL_NextDemo( void )
{
	char    str[1024];

	if (cls.demonum == -1)
		return; // don't play demos

	SCR_BeginLoadingPlaque();

	if (!cls.demos[cls.demonum][0] || cls.demonum == MAX_DEMOS)
	{
		cls.demonum = 0;
		if (!cls.demos[cls.demonum][0])
		{
			scr_disabled_for_loading = FALSE;

			Con_Printf("No demos listed with startdemos\n");
			cls.demonum = -1;
			return;
		}	
	}

	sprintf(str, "playdemo %s\n", cls.demos[cls.demonum]);
	Cbuf_InsertText(str);
	cls.demonum++;
}

/*
==============
CL_PrintEntities_f
==============
*/
void CL_PrintEntities_f( void )
{
	// TODO: Implement
}

/*
===============
CL_TakeSnapshot_f

Generates a filename and calls the vidwin.c function to write a .bmp file
===============
*/
void CL_TakeSnapshot_f( void )
{
	// TODO: Implement
}

/*
===============
CL_StartMovie_f

Sets the engine up to dump frames
===============
*/
void CL_StartMovie_f( void )
{
	// TODO: Implement
}


/*
===============
CL_EndMovie_f

Ends frame dumping
===============
*/

void CL_EndMovie_f( void )
{
	// TODO: Implement
}

/*
=====================
CL_Rcon_f

  Send the rest of the command line over as
  an unconnected command.
=====================
*/
void CL_Rcon_f( void )
{
	// TODO: Reimplement

	int			port;
	int			i;
	netadr_t	to;
	char		message[1024];

	if (!rcon_password.string)
	{
		Con_Printf("You must set 'rcon_password' before\nissuing an rcon command.\n");
		return;
	}

	if (cls.state >= ca_connected)
	{
		to = cls.netchan.remote_address;
	}
	else
	{
		if (!strlen(rcon_address.string))
		{
			Con_Printf("You must either be connected,\n"
					   "or set the 'rcon_address' cvar\n"
					   "to issue rcon commands\n");

			return;
		}

		NET_StringToAdr(rcon_address.string, &to);
	}

	port = rcon_port.value;
	if (port == 0) {
		port = atoi("27015");
	}
	to.port = BigShort(port);

	// construct the message
	sprintf(message, "rcon ");
	strcat(message, rcon_password.string);
	for (i = 1, message[strlen(message)] = ' '; i < Cmd_Argc(); message[strlen(message)] = ' ', i++) {
		strcat(message, Cmd_Argv(i));
	}

	Netchan_OutOfBandPrint(NS_CLIENT, to, "%s", message);
}

/*
==============
CL_View_f

Debugging changes the view entity to the specified index
===============
*/
void CL_View_f( void )
{
	// TODO: Implement
}

/*
===============
SetPal

Debugging tool, just flashes the screen
===============
*/
void SetPal( int i )
{
#if 0
	static int old;
	byte	pal[768];
	int		c;

	if (i == old)
		return;
	old = i;

	if (i == 0)
		VID_SetPalette(host_basepal);
	else if (i == 1)
	{
		for (c = 0; c < 768; c += 3)
		{
			pal[c] = 0;
			pal[c + 1] = 255;
			pal[c + 2] = 0;
		}
		VID_SetPalette(pal);
	}
	else
	{
		for (c = 0; c < 768; c += 3)
		{
			pal[c] = 0;
			pal[c + 1] = 0;
			pal[c + 2] = 255;
		}
		VID_SetPalette(pal);
	}
#endif
}

/*
=================
CL_AllocDlight

=================
*/
dlight_t* CL_AllocDlight( int key )
{
	int	i;
	dlight_t* dl = NULL;

// first look for an exact key match
	if (key)
	{
		dl = cl_dlights;
		for (i = 0; i < MAX_DLIGHTS; i++, dl++)
		{
			if (dl->key == key)
			{
				memset(dl, 0, sizeof(*dl));
				dl->key = key;
				r_dlightchanged |= (1 << i);
				r_dlightactive |= (1 << i);
				return dl;
			}
		}
	}

// then look for anything else
	dl = cl_dlights;
	for (i = 0; i < MAX_DLIGHTS; i++, dl++)
	{
		if (dl->die < cl.time)
		{
			memset(dl, 0, sizeof(*dl));
			dl->key = key;
			r_dlightchanged |= (1 << i);
			r_dlightactive |= (1 << i);
			return dl;
		}
	}

	dl = &cl_dlights[0];
	memset(dl, 0, sizeof(*dl));
	dl->key = key;
	r_dlightchanged |= (1 << 0);
	r_dlightactive |= (1 << 0);
	return dl;
}

/*
=================
CL_AllocElight

=================
*/
dlight_t* CL_AllocElight( int key )
{
	int	i;
	dlight_t* el = NULL;

// first look for an exact key match
	if (key)
	{
		el = cl_elights;
		for (i = 0; i < MAX_ELIGHTS; i++, el++)
		{
			if (el->key == key)
			{
				memset(el, 0, sizeof(*el));
				el->key = key;
				return el;
			}
		}
	}

// then look for anything else
	el = cl_elights;
	for (i = 0; i < MAX_ELIGHTS; i++, el++)
	{
		if (el->die < cl.time)
		{
			memset(el, 0, sizeof(*el));
			el->key = key;
			return el;
		}
	}

	el = &cl_elights[0];
	memset(el, 0, sizeof(*el));
	el->key = key;
	return el;
}

/*
===============
CL_DecayLights

===============
*/
void CL_DecayLights( void )
{
	int			i;
	dlight_t* dl;
	float		time;

	r_dlightchanged = 0;
	r_dlightactive = 0;

	time = cl.time - cl.oldtime;

	dl = cl_dlights;

	for (i = 0; i < MAX_DLIGHTS; i++, dl++)
	{
		if (dl->radius != 0)
		{
			if (dl->die < cl.time)
			{
				r_dlightchanged |= (1 << i);
				dl->radius = 0;
			}
			else if (dl->decay)
			{
				r_dlightchanged |= (1 << i);

				dl->radius -= time * dl->decay;
				if (dl->radius < 0)
					dl->radius = 0;
			}
			if (dl->radius != 0)
			{
				r_dlightactive |= (1 << i);
			}
		}
	}

	dl = cl_elights;
	for (i = 0; i < MAX_ELIGHTS; i++, dl++)
	{
		if (!dl->radius)
			continue;
		if (dl->die < cl.time)
		{
			dl->radius = 0;
			continue;
		}

		dl->radius -= time * dl->decay;
		if (dl->radius < 0)
			dl->radius = 0;
	}
}

/*
===============
CL_TouchLight

===============
*/
void CL_TouchLight( dlight_t* dl )
{
	int i;

	i = dl - cl_dlights;
	if (i >= 0 && i < 32)
		r_dlightchanged |= 1 << i;
}

/*
===============
CL_LerpPoint

Determines the fraction between the last two messages that the objects
should be put at.
===============
*/
float CL_LerpPoint( void )
{
	float	f, frac;

	f = cl.mtime[0] - cl.mtime[1];

	if (!f || cl_nolerp.value || cls.timedemo || sv.active && !fakelag.value)
	{
		float fgap;

		fgap = cl.time - cl.oldtime;

		cl.time = cl.mtime[0];
		if (cls.demoplayback)
		{
			cl.oldtime = cl.time - fgap;
			return 1;
		}
		else
			return 1;
	}

	if (f > 0.1)
	{	// dropped packet, or start of demo
		cl.mtime[1] = cl.mtime[0] - 0.1;
		f = 0.1;
	}
	frac = (cl.time - cl.mtime[1]) / f;
//Con_Printf("frac: %f\n", frac);
	if (frac < 0)
	{
		if (frac < -0.01)
		{
			SetPal(1);
			cl.time = cl.mtime[1];
//				Con_Printf("low frac\n");
		}
		frac = 0;
	}
	else if (frac > 1)
	{
		if (frac > 1.01)
		{
			SetPal(2);
			cl.time = cl.mtime[0];
//				Con_Printf("high frac\n");
		}
		frac = 1;
	}
	else
		SetPal(0);

	return frac;
}

/*
=================
CL_SendCmd
=================
*/
void CL_SendCmd( void )
{
	sizebuf_t	buf;
	byte		data[128];
	int			i;
	usercmd_t* cmd, * oldcmd, nullcmd;
	int			checksumIndex;
	int			seq_hash;

	if (cls.state == ca_dedicated ||
		cls.state == ca_disconnected ||
		cls.state == ca_connecting)
		return;

	// save this command off for prediction
	i = cls.netchan.outgoing_sequence & UPDATE_MASK;
	cmd = &cl.frames[i].cmd;
	cl.frames[i].senttime = realtime;
	cl.frames[i].receivedtime = -1;		// we haven't gotten a reply yet

	memset(cmd, 0, sizeof(*cmd));

//	seq_hash = (cls.netchan.outgoing_sequence & 0xffff) ; // ^ QW_CHECK_HASH;
	seq_hash = cls.netchan.outgoing_sequence;

	// get basic movement from keyboard
	if (!cls.demoplayback && cls.signon == SIGNONS)
	{
		// get basic movement from keyboard
		CL_BaseMove(cmd);

		// allow mice or other external controllers to add to the move
		IN_Move(cmd);
	}

// send this and the previous cmds in the message, so
// if the last packet was dropped, it can be recovered
	buf.maxsize = 128;
	buf.cursize = 0;
	buf.data = data;

	MSG_WriteByte(&buf, clc_move);

	// save the position for a checksum byte
	checksumIndex = buf.cursize;
	MSG_WriteByte(&buf, 0);

	VectorCopy(cl.viewangles, cmd->angles);

	cmd->msec = (int)(host_frametime * 1000.0);
	if (cmd->msec > 250)
		cmd->msec = 100;

	cmd->buttons = CL_ButtonBits(1);
	cmd->impulse = in_impulse;

	// if we are spectator, try autocam
	if (cl.spectator)
	{
		Cam_Track(cmd);
		Cam_FinishMove(cmd);
	}

	memset(&nullcmd, 0, sizeof(nullcmd));

	i = (cls.netchan.outgoing_sequence - 2) & UPDATE_MASK;
	cmd = &cl.frames[i].cmd;
	MSG_WriteDeltaUsercmd(&buf, cmd, &nullcmd);
	oldcmd = cmd;

	i = (cls.netchan.outgoing_sequence - 1) & UPDATE_MASK;
	cmd = &cl.frames[i].cmd;
	MSG_WriteDeltaUsercmd(&buf, cmd, oldcmd);
	oldcmd = cmd;

	i = (cls.netchan.outgoing_sequence) & UPDATE_MASK;
	cmd = &cl.frames[i].cmd;
	MSG_WriteDeltaUsercmd(&buf, cmd, oldcmd);

	// calculate a checksum over the move commands
	buf.data[checksumIndex] = COM_BlockSequenceCRCByte(
		buf.data + checksumIndex + 1, buf.cursize - checksumIndex - 1,
		seq_hash);

	memcpy(&cl.cmd, cmd, sizeof(cl.cmd));

	in_impulse = 0;

	// request delta compression of entities
	if (cls.netchan.outgoing_sequence - cl.validsequence >= UPDATE_BACKUP - 1)
		cl.validsequence = 0;

	if (cl.validsequence && !cl_nodelta.value && cls.state == ca_active &&
		!cls.demorecording && !cls.demowaiting)
	{
		cl.frames[cls.netchan.outgoing_sequence & UPDATE_MASK].delta_sequence = cl.validsequence;
		MSG_WriteByte(&buf, clc_delta);
		MSG_WriteByte(&buf, cl.validsequence & 255);
	}
	else
		cl.frames[cls.netchan.outgoing_sequence & UPDATE_MASK].delta_sequence = -1;


//
// deliver the message
//
	Netchan_Transmit(&cls.netchan, buf.cursize, buf.data);
}

/*
=================
CL_ParseNextUpload

Sends next file chunk to server
=================
*/
void CL_ParseNextUpload( void )
{
	int		r;
	int		percent;
	int		size;

	if (!cls.upload)
		return;

	if (g_bSkipUpload)
	{
		g_bSkipUpload = FALSE;
		COM_FreeFile(cls.upload);
		cls.upload = NULL;
		Con_Printf("Skipping upload...\n");
		return;
	}

	r = cls.uploadsize - cls.uploadpos;
	if (r > 1024)
		r = 1024;
	CRC32_ProcessBuffer(&cls.uploadCRC, (byte*)cls.upload + cls.uploadpos, r);

	MSG_WriteByte(&cls.netchan.message, clc_upload);
	MSG_WriteShort(&cls.netchan.message, r);
	MSG_WriteShort(&cls.netchan.message, cls.uploadpos / 1024);
	MSG_WriteLong(&cls.netchan.message, cls.uploadCRC);

	cls.uploadpos += r;
	size = cls.uploadsize;
	if (!size)
		size = 1;
	percent = cls.uploadpos * 100 / size;
	MSG_WriteByte(&cls.netchan.message, percent);
	SZ_Write(&cls.netchan.message, (byte*)cls.upload + cls.uploadpos - r, r);

	if (cls.uploadpos != cls.uploadsize)
		return;

	COM_FreeFile(cls.upload);
	cls.upload = NULL;
}

/*
==================
CL_SetupResume

==================
*/
void CL_SetupResume( int size, CRC32_t crc )
{
	CRC32_t crcFile;

	if (size < 0)
		return;

	size *= 1024;
	if (cls.uploadsize < size)
		return;

	CRC32_Init(&crcFile);
	CRC32_ProcessBuffer(&crcFile, cls.upload, size);
	crcFile = CRC32_Final(crcFile);
	if (crcFile == crc)
	{
		cls.uploadpos = size;
		cls.uploadCRC = crc;
		cls.uploading = TRUE;
	}
}

/*
=================
CL_BeginUpload_f

Starts file upload to server, handles both normal files and MD5-hashed resources
=================
*/
void CL_BeginUpload_f( void )
{
	char* name;
	FILE* file;

	name = Cmd_Argv(1);

	if (strstr(name, "..") || !cl_allowupload.value)
	{
		MSG_WriteByte(&cls.netchan.message, clc_upload);
		MSG_WriteShort(&cls.netchan.message, -1);
		MSG_WriteShort(&cls.netchan.message, -1);
		MSG_WriteLong(&cls.netchan.message, -1);
		MSG_WriteByte(&cls.netchan.message, 0);
		return;
	}

	if (cls.upload)
	{
		COM_FreeFile(cls.upload);
		cls.upload = NULL;
	}

	file = NULL;

	// Handle customizations
	if (strlen(name) == 36 && !_strnicmp(name, "!MD5", 4))
	{
		resource_t resource;
		unsigned char rgucMD5_hash[16];

		memset(&resource, 0, sizeof(resource));

		COM_HexConvert(name + 4, 32, rgucMD5_hash);

		if (HPAK_ResourceForHash(HASHPAK_FILENAME, rgucMD5_hash, &resource) &&
			HPAK_GetDataPointer(HASHPAK_FILENAME, &resource, &file))
		{
			cls.uploadsize = resource.nDownloadSize;
			cls.upload = (FILE*)malloc(resource.nDownloadSize + 1);
			fread(cls.upload, resource.nDownloadSize, 1, file);
			*((byte*)cls.upload + resource.nDownloadSize) = 0;

			fclose(file);
			file = NULL;
		}
	}
	else
	{
		cls.uploadsize = COM_FindFile(name, NULL, &file);
		if (cls.uploadsize != -1 && file)
		{
			cls.upload = (FILE*)COM_LoadFile(name, 5, NULL);
			fclose(file);
			file = NULL;
		}
	}

	cls.uploadpos = 0;

	if (cls.uploadsize == -1 || !cls.upload || (cl_upload_max.value && (cls.uploadsize > cl_upload_max.value)))
	{
		MSG_WriteByte(&cls.netchan.message, clc_upload);
		MSG_WriteShort(&cls.netchan.message, -1);
		MSG_WriteShort(&cls.netchan.message, -1);
		MSG_WriteLong(&cls.netchan.message, -2);
		MSG_WriteByte(&cls.netchan.message, 0);
		return;
	}

	cls.uploading = FALSE;

	CRC32_Init(&cls.uploadCRC);

	if (Cmd_Argc() == 4)
	{
		CL_SetupResume(atoi(Cmd_Argv(2)), atol(Cmd_Argv(3)));
	}

	CL_ParseNextUpload();
	Con_DPrintf("Uploading %s\n", name);
}

/*
=================
CL_SendResourceListBlock

=================
*/
void CL_SendResourceListBlock( void )
{
	int		i;
	int		arg, size;

	if (cls.state == ca_dedicated || cls.state == ca_disconnected)
	{
		Con_Printf("custom resource list not valid -- not connected\n");
		return;
	}

	arg = MSG_ReadLong();
	if (!cls.demoplayback && arg != cl.servercount)
	{
		Con_Printf("CL_SendResourceListBlock_f from different level\n");
		return;
	}

	i = MSG_ReadLong();
	if (i < 0 || i > cl.num_resources)
	{
		Con_Printf("custom resource list request out of range\n");
		return;
	}

	// Send resource list
	MSG_WriteByte(&cls.netchan.message, clc_resourcelist);
	MSG_WriteShort(&cls.netchan.message, cl.num_resources);
	MSG_WriteShort(&cls.netchan.message, i);
	size = cls.netchan.message.cursize;

	MSG_WriteShort(&cls.netchan.message, 0);

	for (; i < cl.num_resources; i++)
	{
		if (cls.netchan.message.cursize + 128 >= cls.netchan.message.maxsize)
			break;

		MSG_WriteByte(&cls.netchan.message, cl.resourcelist[i].type);
		MSG_WriteString(&cls.netchan.message, cl.resourcelist[i].szFileName);
		MSG_WriteShort(&cls.netchan.message, cl.resourcelist[i].nIndex);
		MSG_WriteLong(&cls.netchan.message, cl.resourcelist[i].nDownloadSize);
		MSG_WriteByte(&cls.netchan.message, cl.resourcelist[i].ucFlags);

		if (cl.resourcelist[i].ucFlags & RES_CUSTOM)
			SZ_Write(&cls.netchan.message, cl.resourcelist[i].rgucMD5_hash, sizeof(cl.resourcelist[i].rgucMD5_hash));
	}

	*(unsigned short*)&cls.netchan.message.data[size] = i;

	if (i < cl.num_resources)
	{
		MSG_WriteByte(&cls.netchan.message, clc_stringcmd);
		MSG_WriteString(&cls.netchan.message, va("customrsrclist %i %i\n", cl.servercount, i));
	}
}

/*
====================
CL_AddResource

Adds a new resource to the client resource list
====================
*/
resource_t* CL_AddResource( resourcetype_t type, char* name, int size, qboolean bFatalIfMissing, int index )
{
	resource_t* r;

	r = &cl.resourcelist[cl.num_resources];
	if (cl.num_resources >= MAX_RESOURCES)
		Sys_Error("Too many resources on client.");
	cl.num_resources++;

	r->type = type;
	strcpy(r->szFileName, name);
	r->nDownloadSize = size;
	r->nIndex = index;

	if (bFatalIfMissing)
	{
		r->ucFlags |= RES_FATALIFMISSING;
	}

	return r;
}

/*
====================
CL_CreateResourceList

====================
*/
void CL_CreateResourceList( void )
{
	FILE* fp;
	int			nSize;
	char		szFileName[128];
	unsigned char	rgucMD5_hash[16];

	resource_t* pNewResource;

	cl.num_resources = 0;

	sprintf(szFileName, "pldecal.wad");

	memset(rgucMD5_hash, 0, sizeof(rgucMD5_hash));

	nSize = COM_FindFile(szFileName, NULL, &fp);
	if (nSize == -1)
	{
		nSize = 0;
	}
	else
	{
		MD5_Hash_File(rgucMD5_hash, szFileName);
	}

	if (nSize)
	{
		pNewResource = CL_AddResource(t_decal, szFileName, nSize, FALSE, 0);
		if (pNewResource)
		{
			pNewResource->ucFlags |= RES_CUSTOM;
			memcpy(pNewResource->rgucMD5_hash, rgucMD5_hash, sizeof(pNewResource->rgucMD5_hash));
			HPAK_AddLump(HASHPAK_FILENAME, pNewResource, NULL, fp);
		}
	}

	if (fp)
		fclose(fp);
}

/*
==================
CL_SkipDownload_f

Skip current download
==================
*/
void CL_SkipDownload_f( void )
{
	if (!cls.download)
		return;

	g_bSkipDownload = TRUE;
}

/*
==================
CL_SkipUpload_f

Skip current upload
==================
*/
void CL_SkipUpload_f( void )
{
	if (!cls.upload)
		return;

	g_bSkipUpload = TRUE;
}

/*
==================
CL_AllowDownload_f

==================
*/
void CL_AllowDownload_f( void )
{
	cl_allowdownload.value = !cl_allowdownload.value;

	if (!cl_allowdownload.value)
		Con_Printf("Client downloading disabled.\n");
	else
		Con_Printf("Client downloading enabled.\n");
}

/*
==================
CL_AllowUpload_f

==================
*/
void CL_AllowUpload_f( void )
{
	cl_allowupload.value = !cl_allowupload.value;

	if (!cl_allowupload.value)
		Con_Printf("Client uploading disabled.\n");
	else
		Con_Printf("Client uploading enabled.\nMax. upload size is %i", (unsigned int)(__int64)cl_upload_max.value);
}

char* CL_HashedClientID( unsigned char* hash, int size )
{
	static char szReturn[128];
	unsigned char c;
	char szChunk[10];
	int i;

	memset(szReturn, 0, sizeof(szReturn));

	for (i = 0; i < size; i++)
	{
		c = (unsigned char)hash[i];
		sprintf(szChunk, "%2x", c);
		strcat(szReturn, szChunk);
	}

	return szReturn;
}

/*
=================
CL_PrintCDKey_f

Print the CD key to the console
=================
*/
void CL_PrintCDKey_f( void )
{
	char szKeyBuffer[256]; // Keys are about 13 chars long.	
	char szHashedKeyBuffer[256];
	char szHashedClientID[2048];
	int nKeyLength = GUID_LEN;
	int bDedicated = 0;
	MD5Context_t ctx;
	clientid_t clientid;
	unsigned char digest[16]; // The MD5 Hash

	// Get the cd key.
	Launcher_GetCDKey(szKeyBuffer, &nKeyLength, &bDedicated);

	// A dedicated server
	if (bDedicated)
	{
		Con_Printf("Key has no meaning on dedicated server...\n");
		return;
	}

	if (nKeyLength <= 0 ||
		nKeyLength >= 256)
	{
		Con_Printf("Bogus key length on CD Key...\n");
		return;
	}

	szKeyBuffer[nKeyLength] = 0;

	// Now get the md5 hash of the key
	memset(&ctx, 0, sizeof(ctx));
	memset(digest, 0, sizeof(digest));

	MD5Init(&ctx);
	MD5Update(&ctx, (unsigned char*)szKeyBuffer, nKeyLength);
	MD5Final(digest, &ctx);
	memset(szHashedKeyBuffer, 0, sizeof(szHashedKeyBuffer));
	strcpy(szHashedKeyBuffer, MD5_Print(digest));

	memset(szHashedClientID, 0, sizeof(szHashedClientID));

	// Get the client ID
	if (Launcher_GetClientID(&clientid))
	{
		sprintf(szHashedClientID, "%s", CL_HashedClientID(clientid.hash, clientid.size));
	}
	else
	{
		strcpy(szHashedClientID, "Unset");
	}

	Con_Printf("CD Key:  %s\nMD5 Hash:  %s\nClient ID:  %s\n\n", szKeyBuffer, szHashedKeyBuffer, szHashedClientID);
}

/*
=================
CL_Init
=================
*/
void CL_Init( void )
{
	CL_InitInput();
	CL_InitTEnts();

	TextMessageInit();

	ClientDLL_HudInit();
	ClientDLL_HudVidInit();

//
// register our commands
//
	Cvar_RegisterVariable(&cl_name);
	Cvar_RegisterVariable(&cl_color);
	Cvar_RegisterVariable(&cl_upspeed);
	Cvar_RegisterVariable(&cl_forwardspeed);
	Cvar_RegisterVariable(&cl_backspeed);
	Cvar_RegisterVariable(&cl_sidespeed);
	Cvar_RegisterVariable(&cl_movespeedkey);
	Cvar_RegisterVariable(&cl_yawspeed);
	Cvar_RegisterVariable(&cl_pitchspeed);
	Cvar_RegisterVariable(&cl_anglespeedkey);
	Cvar_RegisterVariable(&cl_nolerp);
	Cvar_RegisterVariable(&cl_skyname);
	Cvar_RegisterVariable(&cl_skycolor_r);
	Cvar_RegisterVariable(&cl_skycolor_g);
	Cvar_RegisterVariable(&cl_skycolor_b);
	Cvar_RegisterVariable(&cl_skyvec_x);
	Cvar_RegisterVariable(&cl_skyvec_y);
	Cvar_RegisterVariable(&cl_skyvec_z);
	Cvar_RegisterVariable(&lookspring);
	Cvar_RegisterVariable(&lookstrafe);
	Cvar_RegisterVariable(&sensitivity);
	Cvar_RegisterVariable(&cl_stats);

	Cvar_RegisterVariable(&m_pitch);
	Cvar_RegisterVariable(&m_yaw);
	Cvar_RegisterVariable(&m_forward);
	Cvar_RegisterVariable(&m_side);

	Cvar_RegisterVariable(&cl_pitchup);
	Cvar_RegisterVariable(&cl_pitchdown);
	Cvar_RegisterVariable(&cl_appendmixed);
	Cvar_RegisterVariable(&cl_resend);
	Cvar_RegisterVariable(&cl_timeout);
	Cvar_RegisterVariable(&cl_shownet);
	Cvar_RegisterVariable(&cl_showsizes);
	Cvar_RegisterVariable(&rcon_address);
	Cvar_RegisterVariable(&rcon_port);
	Cvar_RegisterVariable(&cl_spectator_password);
	Cvar_RegisterVariable(&cl_predict_players);
	Cvar_RegisterVariable(&cl_solid_players);
	Cvar_RegisterVariable(&cl_nodelta);
	Cvar_RegisterVariable(&cl_printplayers);
	Cvar_RegisterVariable(&cl_slisttimeout);
	Cvar_RegisterVariable(&cl_downloadinterval);
	Cvar_RegisterVariable(&cl_upload_max);
	Cvar_RegisterVariable(&cl_download_max);
	Cvar_RegisterVariable(&cl_download_ingame);
	Cvar_RegisterVariable(&cl_allowdownload);
	Cvar_RegisterVariable(&cl_allowupload);

	Cmd_AddCommand("cdkey", CL_PrintCDKey_f);
	Cmd_AddCommand("disconnect", CL_Disconnect_f);
	Cmd_AddCommand("record", CL_Record_f);
	Cmd_AddCommand("stop", CL_Stop_f);
	Cmd_AddCommand("playdemo", CL_PlayDemo_f);
	Cmd_AddCommand("timedemo", CL_TimeDemo_f);
	Cmd_AddCommand("listdemo", CL_ListDemo_f);
	Cmd_AddCommand("appenddemo", CL_AppendDemo_f);
	Cmd_AddCommand("removedemo", CL_RemoveDemo_f);
	Cmd_AddCommand("swapdemo", CL_SwapDemo_f);
	Cmd_AddCommand("setdemoinfo", CL_SetDemoInfo_f);
	Cmd_AddCommand("snapshot", CL_TakeSnapshot_f);
	Cmd_AddCommand("startmovie", CL_StartMovie_f);
	Cmd_AddCommand("endmovie", CL_EndMovie_f);
	Cmd_AddCommand("entities", CL_PrintEntities_f);
	Cmd_AddCommand("rcon", CL_Rcon_f);
	Cmd_AddCommand("cl_view", CL_View_f);
	Cmd_AddCommand("cl_messages", CL_DumpMessageLoad_f);
	Cmd_AddCommand("cl_bitcounts", CL_BitCounts_f);
	Cmd_AddCommand("cl_usr", CL_UserMsgs_f);
	Cmd_AddCommand("pingservers", CL_PingServers_f);
	Cmd_AddCommand("slist", CL_Slist_f);
	Cmd_AddCommand("list", CL_ListCachedServers_f);
	Cmd_AddCommand("clearlist", CL_ClearCachedServers_f);
	Cmd_AddCommand("resources", CL_PrintResourceLists_f);
	Cmd_AddCommand("cl_allow_upload", CL_AllowUpload_f);
	Cmd_AddCommand("cl_allow_download", CL_AllowDownload_f);
	Cmd_AddCommand("upload", CL_BeginUpload_f);
	Cmd_AddCommand("allowupload", CL_AllowUpload_f);
	Cmd_AddCommand("skipdl", CL_SkipDownload_f);
	Cmd_AddCommand("skipul", CL_SkipUpload_f);
	Cmd_AddCommand("cl_print_custom", CL_PrintCustomizations_f);
	
	CL_InitPrediction();

	CL_InitCam();

	Pmove_Init();

	memset(bitcounts, 0, sizeof(bitcounts));
	memset(playerbitcounts, 0, sizeof(playerbitcounts));
	memset(custombitcounts, 0, sizeof(custombitcounts));

	memset(&cl, 0, sizeof(client_state_t));

	cl.resourcesneeded.pNext = cl.resourcesneeded.pPrev = &cl.resourcesneeded;
	cl.resourcesonhand.pNext = cl.resourcesonhand.pPrev = &cl.resourcesonhand;
}