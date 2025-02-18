// sv_main.c -- server main program

#include "quakedef.h"

#define CHAN_AUTO   0
#define CHAN_WEAPON 1
#define CHAN_VOICE  2
#define CHAN_ITEM   3
#define CHAN_BODY   4

/*
=============================================================================

Con_Printf redirection

=============================================================================
*/

char	outputbuf[8000];

redirect_t	sv_redirected;

/*
==================
SV_FlushRedirect
==================
*/
void SV_FlushRedirect( void )
{
	// TODO: Implement
}

/*
==================
SV_SendUserReg
==================
*/
void SV_SendUserReg( sizebuf_t* sb )
{
	UserMsg* pMsg;

	for (pMsg = sv_gpNewUserMsgs; pMsg; pMsg = pMsg->next)
	{
		MSG_WriteByte(sb, svc_newusermsg);
		MSG_WriteByte(sb, pMsg->iMsg);
		MSG_WriteByte(sb, pMsg->iSize);
		MSG_WriteLong(sb, *(int*)&pMsg->szName[0]);
		MSG_WriteLong(sb, *(int*)&pMsg->szName[4]);
		MSG_WriteLong(sb, *(int*)&pMsg->szName[8]);
		MSG_WriteLong(sb, (int)pMsg->next);
	}
}

/*
=================
SV_SendBan
=================
*/
void SV_SendBan( void )
{
	// TODO: Implement
}

/*
=================
SV_FilterPacket
=================
*/
qboolean SV_FilterPacket( void )
{
	// TODO: Implement

	return FALSE;
}

/*
=======================
SV_SendClientDatagram

Send datagram to the client who need one
=======================
*/
static qboolean SV_SendClientDatagram( client_t* cl )
{
	sizebuf_t	msg;
	byte		data[MAX_DATAGRAM];

	msg.maxsize = MAX_DATAGRAM;
	msg.data = data;
	msg.cursize = 0;
	msg.allowoverflow = TRUE;
	msg.overflowed = FALSE;

	MSG_WriteByte(&msg, svc_time);
	MSG_WriteFloat(&msg, sv.time);

	// Did we overflow the message buffer?  If so, just ignore it.
	if (cl->datagram.overflowed)
	{
		Con_Printf("WARNING: datagram overflowed for %s\n", cl->name);
	}
	else
	{
		// Otherwise, copy the client->datagram into the message stream, too.
		SZ_Write(&msg, cl->datagram.data, cl->datagram.cursize);
	}

	// Clear up the client datagram because we've just copied it.
	SZ_Clear(&cl->datagram);

	if (!scr_graphmean.value)
	{
		if (nDatagramBytesSent < msg.cursize)
			nDatagramBytesSent = msg.cursize;
	}
	else
	{
		++nDatagrams;
		nDatagramBytesSent += msg.cursize;
	}

	// send deltas over reliable stream
	if (msg.overflowed)
	{
		Con_Printf("WARNING: msg overflowed for %s\n", cl->name);
		SZ_Clear(&msg);
	}
	
	// Send the datagram
	Netchan_Transmit(&cl->netchan, msg.cursize, data);

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
	client_t*	client;
	UserMsg*	pMsg;

// check for changes to be sent over the reliable streams
	for (i = 0, host_client = svs.clients; i < svs.maxclients; i++, host_client++)
	{
		if (!host_client->edict)
			continue;

		if (host_client->fakeclient) //this is a client controlled entirely by the game .dll, let it handle the stuff
			continue;

		if (!sv_gpNewUserMsgs) //no new UserMsgs were registered
			continue;

		if (host_client->active || host_client->connected)
		{
			SV_SendUserReg(&host_client->netchan.message);
		}
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
		sv_gpNewUserMsgs = NULL; //nullify it so we don't send "svc_newusermsg" each frame
	}

	// Clear the server datagram if it overflowed.
	if (sv.datagram.overflowed)
	{
		Con_DPrintf("sv.datagram overflowed!\n");
		SZ_Clear(&sv.datagram);
	}

	// Now send the reliable and server datagrams to all clients.
	for (j = 0, client = svs.clients; j < svs.maxclients; j++, client++)
	{
		if (!client->active || client->fakeclient)
			continue;

		SZ_Write(&client->netchan.message, sv.reliable_datagram.data, sv.reliable_datagram.cursize);
		SZ_Write(&client->datagram, sv.datagram.data, sv.datagram.cursize);
	}

	// Now clear the reliable and datagram buffers.
	SZ_Clear(&sv.reliable_datagram);
	SZ_Clear(&sv.datagram);
}

/*
=======================
SV_SendClientMessages
=======================
*/
void SV_SendClientMessages(void)
{
	int			i;

	// update frags, names, etc
	SV_UpdateToReliableMessages();

	nReliableBytesSent = 0;
	nDatagramBytesSent = 0;
	nReliables = 0;
	nDatagrams = 0;
	bUnreliableOverflow = FALSE;

	if (g_LastScreenUpdateTime <= sv.time)
	{
		bAddDeltaFlag = TRUE;
		g_LastScreenUpdateTime = sv.time + 1;
	}

	for (i = 0, host_client = svs.clients; i < svs.maxclients; i++, host_client++)
	{
		if ((host_client->active || host_client->connected || host_client->spawned) && !host_client->fakeclient)
		{
			// Clear the client datagram if it overflowed, as well as disconnect him/her.
			if (host_client->netchan.message.overflowed)
			{
				SZ_Clear(&host_client->netchan.message);
				SZ_Clear(&host_client->datagram);

				SV_BroadcastPrintf("%s overflowed\n", host_client->name);
				Con_Printf("WARNING: reliable overflow for %s\n", host_client->name);

				SV_DropClient(host_client, FALSE);

				// we must notify them about the disconnect
				host_client->send_message = TRUE;

				host_client->netchan.cleartime = 0;	// don't choke this message
			}

			// only send messages if the client has sent one
			// and the bandwidth is not choked
			if (!host_client->send_message)
				continue;

			host_client->send_message = FALSE; // try putting this after choke?

			if (!Netchan_CanPacket(&host_client->netchan))
			{
				host_client->chokecount++;
				continue;		// bandwidth choke
			}

			// Append the unreliable data (player updates and packet entities)
			if (host_client->active)
			{
				SV_SendClientDatagram(host_client);
			}
			else
			{
				// Connected, but inactive, just send reliable, sequenced info.
				Netchan_Transmit(&host_client->netchan, 0, NULL);
			}
		}
	}
}
