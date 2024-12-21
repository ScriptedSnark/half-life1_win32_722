// cl_parse.c  -- parse a message received from the server

#include "quakedef.h"
#include "pmove.h"
#include "cl_demo.h"
#include "cl_draw.h"


int		last_data[64];
int		msg_buckets[64];



// TODO: Implement

/*
==================
CL_RegisterResources

Clean up and move to next part of sequence.
==================
*/
void CL_RegisterResources( sizebuf_t* msg )
{
	// TODO: Implement
}

void CL_MoveToOnHandList( resource_t* pResource )
{
	if (!pResource)
	{
		Con_DPrintf("Null resource passed to CL_MoveToOnHandList\n");
		return;
	}

	// TODO: Implement
}

/*
===============
COM_SizeofResourceList

===============
*/
int COM_SizeofResourceList( resource_t* pList, int* nWorldSize, int* nModelsSize, int* nDecalsSize, int* nSoundsSize, int* nSkinsSize )
{
	resource_t* p;
	int nSize = 0;

	*nModelsSize = 0;
	*nWorldSize = 0;
	*nDecalsSize = 0;
	*nSoundsSize = 0;
	*nSkinsSize = 0;

	for (p = pList->pNext; p != pList; p = p->pNext)
	{
		nSize += p->nDownloadSize;

		switch (p->type)
		{
		case t_sound:
			*nSoundsSize += p->nDownloadSize;
			break;
		case t_skin:
			*nSkinsSize += p->nDownloadSize;
			break;
		case t_model:
			if (p->nIndex == 1) // worldmodel always take 1 slot
			{
				*nWorldSize = p->nDownloadSize;
			}
			else
			{
				*nModelsSize += p->nDownloadSize;
			}
			break;
		case t_decal:
			*nDecalsSize += p->nDownloadSize;
			break;
		}
	}

	return nSize;
}

void CL_AddToResourceList( resource_t* pResource, resource_t* pList )
{
	if (pResource->pPrev || pResource->pNext)
	{
		Con_Printf("Resource already linked\n");
		return;
	}

	if (!pList->pPrev || !pList->pNext)
	{
		Sys_Error("Resource list corrupted.\n");
	}

	pResource->pPrev = pList->pPrev;
	pList->pPrev->pNext = pResource;
	pList->pPrev = pResource;
	pResource->pNext = pList;
}

/*
===============
CL_ClearResourceList
===============
*/
void CL_ClearResourceList( resource_t* pList )
{
	resource_t* p, * n = NULL;

	for (p = pList->pNext; p && p != pList; p = n)
	{
		n = p->pNext;

		CL_RemoveFromResourceList(p);
		free(p);
	}

	pList->pPrev = pList;
	pList->pNext = pList;
}

void CL_RemoveFromResourceList( resource_t* pResource )
{
	if (!pResource->pPrev || !pResource->pNext)
	{
		Sys_Error("Mislinked resource in CL_RemoveFromResourceList");
	}

	if (pResource->pNext == pResource || pResource->pPrev == pResource)
	{
		Sys_Error("Attempt to free last entry in list.");
	}

	pResource->pPrev->pNext = pResource->pNext;
	pResource->pNext->pPrev = pResource->pPrev;
	pResource->pPrev = NULL;
	pResource->pNext = NULL;
}

/*
===============
CL_EstimateNeededResources

Returns the size of needed resources to download
===============
*/
int CL_EstimateNeededResources( void )
{
	// TODO: Implement
	return 0;
}

/*
================
CL_RequestMissingResources

================
*/
qboolean CL_RequestMissingResources( void )
{
	// TODO: Implement
	return FALSE;
}

/*
===============
CL_StartResourceDownloading

Begin resource downloading, set incoming transfer data
===============
*/
void CL_StartResourceDownloading( char* pszMessage, qboolean bCustom )
{
	if (pszMessage)
		Con_DPrintf(pszMessage);

	// TODO: Implement
//	cls.dl.nTotalSize = COM_SizeofResourceList(&cl.resourcesneeded, &nWorldSize, &nModelsSize, &nDecalsSize, &nSoundsSize, &nSkinsSize);
//	cls.dl.nTotalToTransfer = CL_EstimateNeededResources();
//
//	Con_DPrintf("Resources total %iK\n", cls.dl.nTotalSize / 1024);

	// TODO: Implement
}

/*
===============
CL_ParseResourceList

Parse the list of resources received from the server
===============
*/
void CL_ParseResourceList( void )
{
	int		total, total_blocks;
	int		i;
	resource_t* resource;

	// Get the number of resource blocks
	total_blocks = MSG_ReadShort();
	// Get the start index
	i = MSG_ReadShort();
	// Get the total number of resources
	total = MSG_ReadShort();

	for (; i < total; i++)
	{
		resource = (resource_t*)malloc(sizeof(resource_t));
		memset(resource, 0, sizeof(resource_t));

		resource->type = MSG_ReadByte();
		strcpy(resource->szFileName, MSG_ReadString());
		resource->nIndex = MSG_ReadShort();
		resource->nDownloadSize = MSG_ReadLong();
		resource->pNext = resource->pPrev = NULL;
		resource->ucFlags = MSG_ReadByte() & ~RES_WASMISSING;

		if (resource->ucFlags & RES_CUSTOM)
		{
			MSG_ReadBuf(sizeof(resource->rgucMD5_hash), resource->rgucMD5_hash);
		}

		// Add new entry in the linked list
		CL_AddToResourceList(resource, &cl.resourcesneeded);
	}

	// Check if there are still unparsed blocks
	if (total < total_blocks)
	{
		cls.state = ca_connected;
	}
	else
	{
		CL_StartResourceDownloading("Verifying and downloading resources...\n", FALSE);
	}
}

/*
================
CL_DeallocateDynamicData

================
*/
void CL_DeallocateDynamicData( void )
{
	// TODO: Implement
}

/*
================
CL_ReallocateDynamicData

================
*/
void CL_ReallocateDynamicData( int nMaxClients )
{
	// TODO: Implement
}

/*
=================
CL_Parse_ServerInfo

Read in server info packet.
=================
*/
void CL_ParseServerInfo( void )
{
	char* str;
	int		i;

	Con_DPrintf("Serverinfo packet received.\n");
//
// wipe the client_state_t struct
//
	CL_ClearState(FALSE);
	
	SPR_Init();
	
	// Re-init hud video, especially if we changed game directories
	ClientDLL_HudVidInit();
	
	cls.demowaiting = FALSE;
	
	CL_BeginDemoStartup();
	
	// parse protocol version number
	i = MSG_ReadLong();
	if (i != PROTOCOL_VERSION)
	{
		Con_Printf("Server returned version %i, not %i\n", i, PROTOCOL_VERSION);
		return;
	}

	// Parse servercount (i.e., # of servers spawned since server .exe started)
	// So that we can detect new server startup during download, etc.
	cl.servercount = MSG_ReadLong();

	// Because a server doesn't run during
	//  demoplayback, but the decal system relies on this...
	if (cls.demoplayback)
	{
		cl.servercount = gHostSpawnCount;
	}

	// The CRC of the server map must match the CRC of the client map. or else
	//  the client is probably cheating.
	cl.serverCRC = MSG_ReadLong();
	cl.mapCRC = MSG_ReadLong();

	cl.maxclients = MSG_ReadByte();

	if (cl.maxclients < 1 || cl.maxclients > MAX_CLIENTS)
	{
		Con_Printf("Bad maxclients (%u) from server\n", cl.maxclients);
		return;
	}

	CL_DeallocateDynamicData();
	CL_ReallocateDynamicData(cl.maxclients);

	cl.playernum = MSG_ReadByte();

	// See if we're just spectating
	if (cl.playernum & 128)
	{
		cl.spectator = TRUE;
		cl.playernum &= ~128;
	}

	// Clear customization for all clients
	for (i = 0; i < MAX_CLIENTS; i++)
		COM_ClearCustomizationList(&cl.players[i].customdata, TRUE);

	CL_CreateCustomizationList();

	// parse gametype
	cl.gametype = MSG_ReadByte();

	// Recieve level name
	str = MSG_ReadString();
	strncpy(cl.levelname, str, sizeof(cl.levelname) - 1);

	MSG_WriteByte(&cls.netchan.message, clc_stringcmd);
	MSG_WriteString(&cls.netchan.message, va("resourcelist %i 0", cl.servercount));

	// During a level transition the client remained active which could cause problems.
	// knock it back down to 'connected'
	cls.state = ca_connected;

	gHostSpawnCount = cl.servercount;
}


void CL_ParseMovevars( void )
{
	movevars.gravity			= MSG_ReadFloat();
	movevars.stopspeed			= MSG_ReadFloat();
	movevars.maxspeed			= MSG_ReadFloat();
	movevars.spectatormaxspeed	= MSG_ReadFloat();
	movevars.accelerate			= MSG_ReadFloat();
	movevars.airaccelerate		= MSG_ReadFloat();
	movevars.wateraccelerate	= MSG_ReadFloat();
	movevars.friction			= MSG_ReadFloat();
	movevars.edgefriction		= MSG_ReadFloat();
	movevars.waterfriction		= MSG_ReadFloat();
	movevars.entgravity			= MSG_ReadFloat();
	movevars.bounce				= MSG_ReadFloat();
	movevars.stepsize			= MSG_ReadFloat();
	movevars.maxvelocity		= MSG_ReadFloat();
	movevars.zmax				= MSG_ReadFloat();
	movevars.waveHeight			= MSG_ReadFloat();

	strcpy(movevars.skyName, MSG_ReadString());

	if (strcmp(movevars.skyName, cl_skyname.string))
		Cvar_Set("cl_skyname", movevars.skyName);

	// TODO: Implement
}






int	total_data[64];




#define SHOWNET(x) \
	if (cl_shownet.value == 2.0 && Q_strlen(x) > 1) \
		Con_Printf("%3i:%s\n", msg_readcount - 1, x);

/*
=================
CL_ParseServerMessage

Parse incoming message from server.
=================
*/
void CL_ParseServerMessage( void )
{
	// Index of svc_ or user command to issue.
	int	cmd;
	int	i;
	// For determining data parse sizes
	int bufStart, bufEnd;

//
// if recording demos, copy the message out
//
	if (cl_shownet.value == 1.0)
	{
		Con_Printf("%i ", net_message.cursize);
	}
	else if (cl_shownet.value == 2.0)
	{
		Con_Printf("------------------\n");
	}

	cl.onground = FALSE;	// unless the server says otherwise

	memset(last_data, 0, sizeof(last_data));

	while (1)
	{
		if (msg_badread)
			Host_Error("CL_ParseServerMessage: Bad server message");

		// Mark start position
		bufStart = msg_readcount;

		cmd = MSG_ReadByte();

		// Bogus message?
		if (cmd == -1)
			break;

		if (cmd > svc_lastmsg)
		{
			// TODO: Implement

			continue;
		}
		
		switch (cmd)
		{
		default:
			// TODO: Implement
			break;

		case svc_nop:
//			Con_Printf("svc_nop\n");
			break;

		// TODO: Implement

		case svc_updatestat:
			i = MSG_ReadByte();
			if (i >= MAX_CL_STATS)
				Sys_Error("svc_updatestat: %i is invalid", i);
			cl.stats[i] = MSG_ReadLong();
			break;

		case svc_version:
			i = MSG_ReadLong();
			if (i != PROTOCOL_VERSION)
				Host_Error("CL_ParseServerMessage: Server is protocol %i instead of %i\n", i, PROTOCOL_VERSION);
			break;

		case svc_setview:
			cl.viewentity = MSG_ReadShort();
			break;

		// TODO: Implement

		case svc_print:
			Con_Printf("%s", MSG_ReadString());
			break;

		case svc_stufftext:
			Cbuf_AddText(MSG_ReadString());
			break;

		// TODO: Implement

		case svc_serverinfo:
			CL_ParseServerInfo();
			vid.recalc_refdef = TRUE;	// leave intermission full screen
			break;

		// TODO: Implement

		case svc_cdtrack:
			cl.cdtrack = MSG_ReadByte();
			cl.looptrack = MSG_ReadByte();

			if ((cls.demoplayback || cls.demorecording) && cls.forcetrack != -1)
			{
				CDAudio_Play(cls.forcetrack, TRUE);
			}
			else
			{
				CDAudio_Play(cl.cdtrack, TRUE);
			}
			break;

		// TODO: Implement

		case svc_resourcelist:
			CL_ParseResourceList();
			break;

		case svc_newmovevars:
			CL_ParseMovevars();
			break;

		// TODO: Implement

		case svc_resourcerequest:
			CL_SendResourceListBlock();
			break;

		// TODO: Implement

		}

		// TODO: Implement
	}

	// end of message
	SHOWNET("END OF MESSAGE");

	// TODO: Implement
}