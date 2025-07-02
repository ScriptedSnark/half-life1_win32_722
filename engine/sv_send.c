// sv_main.c -- server main program

#include "quakedef.h"
#include "cmodel.h"

#define CHAN_AUTO   0
#define CHAN_WEAPON 1
#define CHAN_VOICE  2
#define CHAN_ITEM   3
#define CHAN_BODY   4

extern int sv_playermodel;

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
==================
SV_WriteClientdataToMessage

==================
*/
void SV_WriteClientdataToMessage( client_t* client, sizebuf_t* msg )
{
	int		i, flags;
	edict_t* ent;

	flags = 0;

	ent = client->edict;

	// send the chokecount for r_netgraph
	if (client->chokecount)
	{
		MSG_WriteByte(msg, svc_chokecount);
		MSG_WriteByte(msg, client->chokecount);
		client->chokecount = 0;
	}

	SV_SetIdealPitch();

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

	if (ent->v.view_ofs[2] != 22)
		flags |= SU_VIEWHEIGHT;
	if (ent->v.idealpitch != 0.0)
		flags |= SU_IDEALPITCH;
	if (ent->v.weapons)
		flags |= SU_WEAPONS;
	if (ent->v.flags & FL_ONGROUND)
		flags |= SU_ONGROUND;
	if (ent->v.waterlevel >= 2)
		flags |= SU_INWATER;
	if (ent->v.waterlevel >= 3)
		flags |= SU_FULLYINWATER;
	if (ent->v.viewmodel)
		flags |= SU_ITEMS;

	for (i = 0; i < 3; i++)
	{
		if (ent->v.punchangle[i])
		{
			flags |= (SU_PUNCH1 << i);
		}
		if (ent->v.velocity[i])
		{
			flags |= (SU_VELOCITY1 << i);
		}
	}

	MSG_WriteByte(msg, svc_clientdata);
	MSG_WriteShort(msg, flags);
	if (flags & SU_VIEWHEIGHT)
		MSG_WriteChar(msg, ent->v.view_ofs[2]);
	if (flags & SU_IDEALPITCH)
		MSG_WriteChar(msg, ent->v.idealpitch);
	for (i = 0; i < 3; ++i)
	{
		if (flags & (SU_PUNCH1 << i))
			MSG_WriteHiresAngle(msg, ent->v.punchangle[i]);
		if (flags & (SU_VELOCITY1 << i))
			MSG_WriteChar(msg, ent->v.velocity[i]);
	}
	if (flags & SU_WEAPONS)
		MSG_WriteLong(msg, ent->v.weapons);
	if (flags & SU_ITEMS)
		MSG_WriteShort(msg, SV_ModelIndex(pr_strings + ent->v.viewmodel));
	MSG_WriteShort(msg, ent->v.health);
}

/*
=======================
SV_SendClientDatagram

Send datagram to the client who need one
=======================
*/
void SV_WriteEntitiesToClient( client_t* client, sizebuf_t* msg ); // TODO: Remove me!!!
qboolean SV_SendClientDatagram( client_t* client )
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

	// add the client specific data to the datagram
	SV_WriteClientdataToMessage(client, &msg);

	// send over all the objects that are in the PVS
	// this will include clients, a packetentities, and
	// possibly a nails update
	SV_WriteEntitiesToClient(client, &msg);

	// Did we overflow the message buffer?  If so, just ignore it.
	if (client->datagram.overflowed)
	{
		Con_Printf("WARNING: datagram overflowed for %s\n", client->name);
	}
	else
	{
		// Otherwise, copy the client->datagram into the message stream, too.
		SZ_Write(&msg, client->datagram.data, client->datagram.cursize);
	}

	// Clear up the client datagram because we've just copied it.
	SZ_Clear(&client->datagram);

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
		Con_Printf("WARNING: msg overflowed for %s\n", client->name);
		SZ_Clear(&msg);
	}
	
	// Send the datagram
	Netchan_Transmit(&client->netchan, msg.cursize, data);

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

		if (host_client->fakeclient) // this is a client controlled entirely by the game .dll, let it handle the stuff
			continue;

		if (!sv_gpNewUserMsgs) // no new UserMsgs were registered
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
		sv_gpNewUserMsgs = NULL; // nullify it so we don't send "svc_newusermsg" each frame
	}

	// Clear the server datagram if it overflowed.
	if (sv.datagram.overflowed)
	{
		//bUnreliableOverflow = TRUE;
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
void SCR_UpdateNetUsage( int numbytes, int numlisteners, qboolean bIsDatagram ); // TODO: Remove me!!!
void SV_CleanupEnts( void ); // TODO: Remove me!!!
void SV_SendClientMessages(void)
{
	int			i;

	// update frags, names, etc
	SV_UpdateToReliableMessages();

#ifndef SWDS
	nReliableBytesSent = 0;
	nDatagramBytesSent = 0;
	nReliables = 0;
	nDatagrams = 0;
	bUnreliableOverflow = FALSE;
#endif //SWDS

	if (g_LastScreenUpdateTime <= sv.time)
	{
		//bShouldUpdatePing = TRUE;
		g_LastScreenUpdateTime = sv.time + 1.0f;
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

	SCR_UpdateNetUsage(nReliableBytesSent, nReliables, FALSE);
	SCR_UpdateNetUsage(nDatagramBytesSent, !bUnreliableOverflow ? nDatagrams : (MAX_MSGLEN + 1), TRUE);
	//bShouldUpdatePing = FALSE;

	// Allow game .dll to run code, including unsetting EF_MUZZLEFLASH and EF_NOINTERP on effects fields
	SV_CleanupEnts();
}
