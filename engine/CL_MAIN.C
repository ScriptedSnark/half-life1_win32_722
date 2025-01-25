// cl_main.c  -- client main loop

#include "quakedef.h"
#include "winquake.h"
#include "crc.h"
#include "hashpak.h"
#include "cl_demo.h"
#include "cl_tent.h"


// Only send this many requests before timing out.
#define CL_CONNECTION_RETRIES		4



// these two are not intended to be set directly
cvar_t	cl_name = { "_cl_name", "player", TRUE };
cvar_t	cl_color = { "_cl_color", "0", TRUE };

cvar_t	cl_timeout = { "cl_timeout", "305", TRUE };
cvar_t	cl_shownet = { "cl_shownet", "0" };



cvar_t	cl_spectator_password = { "cl_spectator_password", "0" };




cvar_t	lookspring = { "lookspring", "0", TRUE };
cvar_t	lookstrafe = { "lookstrafe", "0", TRUE };
cvar_t	sensitivity = { "sensitivity", "3", TRUE };

cvar_t	cl_skyname = { "cl_skyname", "desert", TRUE };

// TODO: Implement

cvar_t	m_pitch = { "m_pitch", "0.022", TRUE };
cvar_t	m_yaw = { "m_yaw", "0.022", TRUE };
cvar_t	m_forward = { "m_forward", "1", TRUE };
cvar_t	m_side = { "m_side", "0.8", TRUE };





cvar_t	cl_resend = { "cl_resend", "3.0" };





client_static_t	cls;
client_state_t cl;
// FIXME: put these on hunk?
efrag_t			cl_efrags[MAX_EFRAGS];
cl_entity_t*	cl_entities;
//cl_entity_t		cl_static_entities[MAX_STATIC_ENTITIES]; TODO: Implement
lightstyle_t	cl_lightstyle[MAX_LIGHTSTYLES];
dlight_t		cl_dlights[MAX_DLIGHTS];
dlight_t		cl_elights[MAX_ELIGHTS];



qboolean cl_inmovie;

int g_playerbits[MAX_CLIENTS];

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


// TODO: Implement


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
//	if (cls.demoplayback) TODO: Implement
//		return CL_ReadDemoMessage();

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


// TODO: Implement



/*
================
CL_CreateCustomizationList

================
*/
void CL_CreateCustomizationList( void )
{
	// TODO: Implement
}

void CL_ClearClientState( void )
{
	int i;

	// TODO: Implement

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

	if (cls.dl.download)
	{
		fclose(cls.dl.download);
		cls.dl.download = NULL;
	}

	if (cls.dl.upload)
	{
		COM_FreeFile(cls.dl.upload);
		cls.dl.upload = NULL;
	}

	CL_ClearState(TRUE);

	CL_DeallocateDynamicData();

	// TODO: Implement
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

	int num = atoi(server);  // In case it's an index.


	// TODO: Implement


	memset(msg_buckets, 0, sizeof(msg_buckets));
	memset(total_data, 0, sizeof(total_data));

	strncpy(cls.servername, name, sizeof(cls.servername) - 1);

	// For the check for resend timer to fire a connection / getchallenge request.
	cls.state = ca_connecting;
	// Force connection request to fire.
	cls.connect_time = -99999.0;

	cls.connect_retry = 0;
}

// TODO: Implement


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
		sprintf(str, "spawn %i %s", cl.servercount, cls.spawnparams);
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


// TODO: Implement

/*
===============
CL_DecayLights

===============
*/
void CL_DecayLights( void )
{
	// TODO: Implement
}

// TODO: cl_input.c
cvar_t	cl_nodelta = { "cl_nodelta","0" };

void CL_BaseMove( usercmd_t *cmd );
int CL_ButtonBits( int bResetState );
byte COM_BlockSequenceCRCByte( byte* base, int length, int sequence );

/*
=================
CL_SendCmd
=================
*/
void CL_SendCmd( void )
{
	// TODO: Reimplement

	sizebuf_t	buf;
	byte		data[128];
	int			i;
	usercmd_t* cmd, * oldcmd;
	int			checksumIndex;
	int			seq_hash;
	usercmd_t nullcmd; // guarenteed to be zero

	if (cls.state < ca_connected)
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

	VectorCopy(cl.viewangles, cl.frames[i].cmd.angles);

	cl.frames[i].cmd.msec = (int)(host_frametime * 1000.0);
	if (cl.frames[i].cmd.msec > 250)
		cl.frames[i].cmd.msec = 100;

	cl.frames[i].cmd.buttons = CL_ButtonBits(1);
	cl.frames[i].cmd.impulse = 0;//dword_10577E70;

	if (cl.spectator)
	{
		// TODO: Implement
	}

	memset(&nullcmd, 0, sizeof(nullcmd));

	i = (cls.netchan.outgoing_sequence - 2) & UPDATE_MASK;
	cmd = &cl.frames[i].cmd;
	MSG_WriteUsercmd(&buf, cmd, &nullcmd);
	oldcmd = cmd;

	i = (cls.netchan.outgoing_sequence - 1) & UPDATE_MASK;
	cmd = &cl.frames[i].cmd;
	MSG_WriteUsercmd(&buf, cmd, oldcmd);
	oldcmd = cmd;

	i = (cls.netchan.outgoing_sequence) & UPDATE_MASK;
	cmd = &cl.frames[i].cmd;
	MSG_WriteUsercmd(&buf, cmd, oldcmd);

	// calculate a checksum over the move commands
	buf.data[checksumIndex] = COM_BlockSequenceCRCByte(
		buf.data + checksumIndex + 1, buf.cursize - checksumIndex - 1,
		seq_hash);

	memcpy(&cl.cmd, cmd, sizeof(cl.cmd));

	// request delta compression of entities
	if (cls.netchan.outgoing_sequence - cl.validsequence >= UPDATE_BACKUP - 1)
		cl.validsequence = 0;

	if (cl.validsequence && !cl_nodelta.value && cls.state == ca_active &&
		!cls.demorecording)
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
==================
Host_NextDemo

Skip to next demo
==================
*/
void Host_NextDemo( void )
{
	char    str[1024];

	if (cls.demonum == -1)
		return;

	SCR_BeginLoadingPlaque();

	if (!cls.demos[cls.demonum][0] || cls.demonum == MAX_DEMOS)
	{
		cls.demonum = 0;

		if (!cls.demos[cls.demonum][0])
		{
			scr_drawloading = FALSE;
			Con_Printf( "No demos listed with startdemos\n" );
			cls.demonum = -1;
		}
		return;
	}

	snprintf( str, sizeof( str ), "playdemo %s\n", cls.demos[cls.demonum] );
	Cbuf_InsertText( str );
	cls.demonum++;
}

// TODO: Implement

/*
=====================
CL_Disconnect_f

Disconnects user from the server
=====================
*/
void CL_Disconnect_f( void )
{
	// TODO: Implement
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
		MD5_Hash_File(rgucMD5_hash, szFileName, FALSE, FALSE, NULL);
	}

	if (nSize)
	{
		pNewResource = CL_AddResource(t_decal, szFileName, nSize, FALSE, 0);
		if (pNewResource)
		{
			pNewResource->ucFlags |= RES_CUSTOM;
			memcpy(pNewResource->rgucMD5_hash, rgucMD5_hash, sizeof(pNewResource->rgucMD5_hash));
			HPAK_AddLump("custom.hpk", pNewResource, NULL, fp);
		}
	}

	if (fp)
		fclose(fp);
}




// TODO: Implement


/*
=================
CL_Init
=================
*/
void CL_Init( void )
{
	CL_InitInput();

	// TODO: Implement

	ClientDLL_HudInit();
	ClientDLL_HudVidInit();

	Cvar_RegisterVariable(&cl_name);

	// TODO: Implement

	Cvar_RegisterVariable(&cl_sidespeed);
	Cvar_RegisterVariable(&cl_movespeedkey);

	// TODO: Implement

	Cvar_RegisterVariable(&cl_pitchspeed);

	// TODO: Implement

	Cvar_RegisterVariable(&cl_skyname);

	// TODO: Implement

	Cvar_RegisterVariable(&lookspring);
	Cvar_RegisterVariable(&lookstrafe);
	Cvar_RegisterVariable(&sensitivity);

	// TODO: Implement

	Cvar_RegisterVariable(&m_pitch);
	Cvar_RegisterVariable(&m_yaw);
	Cvar_RegisterVariable(&m_forward);
	Cvar_RegisterVariable(&m_side);
	Cvar_RegisterVariable(&cl_pitchup);
	Cvar_RegisterVariable(&cl_pitchdown);

	// TODO: Implement

	Cvar_RegisterVariable(&cl_resend);
	Cvar_RegisterVariable(&cl_timeout);
	Cvar_RegisterVariable(&cl_shownet);

	// TODO: Implement

	Cvar_RegisterVariable(&cl_spectator_password);

	// TODO: Implement
	
	CL_InitPrediction();

	// TODO: Implement

	memset(&cl, 0, sizeof(client_state_t));

	cl.resourcesneeded.pPrev = &cl.resourcesneeded;
	cl.resourcesneeded.pNext = &cl.resourcesneeded;
	cl.resourcesonhand.pPrev = &cl.resourcesonhand;
	cl.resourcesonhand.pNext = &cl.resourcesonhand;
}