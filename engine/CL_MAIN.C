// cl_main.c  -- client main loop

#include "quakedef.h"
#include "winquake.h"


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
CL_ConnectionlessPacket

Responses to broadcasts, etc
=================
*/
void CL_ConnectionlessPacket( void )
{
//	int		c;
//	char	data[6];
//
//	MSG_BeginReading();
//	MSG_ReadLong();        // skip the -1 marker
//
//	c = MSG_ReadByte();
//
//	switch (c)
//	{
//		// TODO: Implement
//
//	default:
//		Con_Printf("Unknown command:\n%c\n", c);
//		break;
//	}
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



/*
=================
CL_CheckForResend

Resend a connect message if the last one has timed out
=================
*/
void CL_CheckForResend( void )
{
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