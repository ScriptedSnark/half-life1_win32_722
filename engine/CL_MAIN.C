// cl_main.c  -- client main loop

#include "quakedef.h"
#include "winquake.h"
#include "crc.h"


// Only send this many requests before timing out.
#define CL_CONNECTION_RETRIES		4




cvar_t	cl_timeout = { "cl_timeout", "305", TRUE };
cvar_t	cl_shownet = { "cl_shownet", "0" };



cvar_t	lookspring = { "lookspring", "0", TRUE };
cvar_t	lookstrafe = { "lookstrafe", "0", TRUE };
cvar_t	sensitivity = { "sensitivity", "3", TRUE };

// TODO: Implement

cvar_t	m_pitch = { "m_pitch", "0.022", TRUE };
cvar_t	m_yaw = { "m_yaw", "0.022", TRUE };
cvar_t	m_forward = { "m_forward", "1", TRUE };
cvar_t	m_side = { "m_side", "0.8", TRUE };





cvar_t	cl_resend = { "cl_resend", "3.0" };





client_static_t	cls;
client_state_t cl;

qboolean cl_inmovie;

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


/*
=====================
CL_Disconnect

Sends a disconnect message to the server
This is also called on Host_Error, so it shouldn't cause any errors
=====================
*/
void CL_Disconnect( void )
{
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
	// TODO: Implement
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
	cls.spectator = FALSE;

	if (Cmd_Argc() < 2)
	{
		Con_Printf("usage: connect <server> [server password]\n");
		return;
	}

	// TODO: Implement
}




/*
===============
CL_DecayLights

===============
*/
void CL_DecayLights( void )
{
	// TODO: Implement
}


/*
=================
CL_SendCmd
=================
*/
void CL_SendCmd( void )
{
	// TODO: Implement
}


/*
=================
CL_Init
=================
*/
void CL_Init( void )
{
	// TODO: Implement
}