// cl_parse.c  -- parse a message received from the server

#include "quakedef.h"
#include "pmove.h"
#include "decal.h"
#include "cl_demo.h"
#include "cl_draw.h"


int		last_data[64];
int		msg_buckets[64];

UserMsg* gClientUserMsgs = NULL;

// TODO: Implement

char* svc_strings[] =
{
	"svc_bad",
	"svc_nop",
	"svc_disconnect",
	"svc_updatestat",
	"svc_version",				// [long] server version
	"svc_setview",				// [short] entity number
	"svc_sound",				// <see code>
	"svc_time",					// [float] server time
	"svc_print",				// [string] null terminated string
	"svc_stufftext",			// [string] stuffed into client's console buffer
									// the string should be \n terminated
	"svc_setangle",				// [vec3] set the view angle to this absolute value

	"svc_serverinfo",			// [long] version
									// [string] signon string
									// [string]..[0]model cache [string]...[0]sounds cache
									// [string]..[0]item cache
	"svc_lightstyle",			// [byte] [string]
	"svc_updatename",			// [byte] [string]
	"svc_updatefrags",			// [byte] [short]
	"svc_clientdata",			// <shortbits + data>
	"svc_stopsound",			// <see code>
	"svc_updatecolors",			// [byte] [byte]
	"svc_particle",				// [vec3] <variable>
	"svc_damage",				// [byte] impact [byte] blood [vec3] from

	"svc_spawnstatic",
	"OBSOLETE svc_spawnbinary",
	"svc_spawnbaseline",

	"svc_temp_entity",			// <variable>
	"svc_setpause",				// [byte] on / off
	"svc_signonnum",			// [byte]  used for the signon sequence
	"svc_centerprint",			// [string] to put in center of the screen
	"svc_killedmonster",
	"svc_foundsecret",
	"svc_spawnstaticsound",		// [coord3] [byte] samp [byte] vol [byte] aten
	"svc_intermission",			// [string] music
	"svc_finale",				// [string] music [string] text
	"svc_cdtrack",				// [byte] track [byte] looptrack
	"svc_restore",
	"svc_cutscene",
	"svc_weaponanim",
	"svc_decalname",			// [byte] index [string] name
	"svc_roomtype",				// [byte] roomtype (dsp effect)
	"svc_addangle",				// [angle3] set the view angle to this absolute value
	"svc_newusermsg",
	"svc_download",
	"svc_packetentities",		// [...]  Non-delta compressed entities
	"svc_deltapacketentities",	// [...]  Delta compressed entities
	"svc_playerinfo",
	"svc_choke",				// # of packets held back on channel because too much data was flowing.
	"svc_resourcelist",
	"svc_newmovevars",
	"svc_nextupload",
	"svc_resourcerequest",
	"svc_customization",
	"svc_crosshairangle",		// [char] pitch * 5 [char] yaw * 5
	"svc_soundfade",			// char percent, char holdtime, char fadeouttime, char fadeintime
	"svc_clientmaxspeed"
};

int	oldparsecountmod;
int	parsecountmod;
double	parsecounttime;

//=============================================================================

/*
===============
CL_EntityNum

This error checks and tracks the total number of entities
===============
*/
cl_entity_t* CL_EntityNum( int num )
{
	if (num >= cl.num_entities)
	{
		if (num >= cl.max_edicts)
			Host_Error("CL_EntityNum: %i is an invalid number, cl.max_edicts is %i", num, cl.max_edicts);
		while (cl.num_entities <= num)
		{
//			cl_entities[cl.num_entities].colormap = vid.colormap; TODO: Implement
			cl.num_entities++;
		}
	}

	return &cl_entities[num];
}

/*
=====================
AddNewUserMsg

Registers a new user message on the client
=====================
*/
void AddNewUserMsg( void )
{
	UserMsg* pList;
	UserMsg umsg;

	int	i;
	int	fFound = 0;

	umsg.iMsg = MSG_ReadByte();
	umsg.iSize = MSG_ReadByte();
	umsg.pfn = NULL;

	if (umsg.iSize == 255)
		umsg.iSize = -1;

	i = MSG_ReadLong();
	strncpy(&umsg.szName[0], (char*)&i, sizeof(i));
	i = MSG_ReadLong();
	strncpy(&umsg.szName[4], (char*)&i, sizeof(i));
	i = MSG_ReadLong();
	strncpy(&umsg.szName[8], (char*)&i, sizeof(i));
	i = MSG_ReadLong();
	strncpy((char*)&umsg.next, (char*)&i, sizeof(i));

	// Scan all user messages
	for (pList = gClientUserMsgs; pList; pList = pList->next)
	{
		if (!_strcmpi(pList->szName, umsg.szName))
		{
			fFound = 1;
			pList->iMsg = umsg.iMsg;
			pList->iSize = umsg.iSize;
		}
	}

	if (!fFound)
	{
		UserMsg* pumsg;

		pumsg = (UserMsg*)Z_Malloc(sizeof(UserMsg));
		memcpy(pumsg, &umsg, sizeof(UserMsg));
		pumsg->next = gClientUserMsgs;
		gClientUserMsgs = pumsg;
	}
}

/*
=================
DispatchUserMsg
=================
*/
void DispatchUserMsg( int iMsg )
{
	static char buf[MAX_USER_MSG_DATA];
	int MsgSize = 0;
	int fFound = 0;

	UserMsg* pList;

	if (iMsg <= svc_lastmsg || iMsg >= MAX_USERMSGS)
	{
		Con_DPrintf("Illegal User Msg %d\n", iMsg);
		return;
	}

	for (pList = gClientUserMsgs; pList; pList = pList->next)
	{
		if (pList->iMsg == iMsg)
		{
			MsgSize = pList->iSize;
			if (!fFound)
			{
				if (MsgSize == -1)
					MsgSize = MSG_ReadByte();

				MSG_ReadBuf(MsgSize, buf);
			}

			fFound = 1;

//			if (pList->pfn)
//				pList->pfn(pList->szName, MsgSize, buf);
//			else
				Con_DPrintf("UserMsg: No pfn %s %d\n", pList->szName, iMsg);
		}
	}

	if (!fFound)
	{
		Con_DPrintf("UserMsg: Not Present on Client %d\n", iMsg);
	}
}

// TODO: Implement


/*
==================
CL_ParseStartSoundPacket
==================
*/
void CL_ParseStartSoundPacket( void )
{
	int Byte; // ebp
	int v1; // eax
	int v2; // esi
	int v3; // ebx
	int *v4; // edi
	float v6; // [esp+10h] [ebp-68h]
	float v7; // [esp+10h] [ebp-68h]
	int Short; // [esp+14h] [ebp-64h]
	int channel; // [esp+14h] [ebp-64h]
	int v10; // [esp+18h] [ebp-60h]
	float v11; // [esp+1Ch] [ebp-5Ch]
	float v12; // [esp+20h] [ebp-58h]
	int v13[3]; // [esp+24h] [ebp-54h] BYREF
	int Str[18]; // [esp+30h] [ebp-48h] BYREF

	Byte = MSG_ReadByte();
	if ((Byte & 1) != 0)
	{
		v6 = (double)MSG_ReadByte() / 255.0;
		v12 = v6;
	}
	else
	{
		v12 = 1.0;
	}
	if ((Byte & 2) != 0)
	{
		v7 = (double)MSG_ReadByte() / 64.0;
		v11 = v7;
	}
	else
	{
		v11 = 1.0;
	}
	Short = MSG_ReadShort();
	if ((Byte & 4) != 0)
		v1 = MSG_ReadShort();
	else
		v1 = MSG_ReadByte();
	v2 = v1;
	v3 = Short >> 3;
	channel = Short & 7;
	if (v3 >= cl.max_edicts)
		Host_Error("CL_ParseStartSoundPacket: ent = %i", v3);
	v4 = v13;
	do
		*((float *)++v4 - 1) = MSG_ReadCoord();
	while (v4 < Str);
	if ((Byte & 8) != 0)
		v10 = MSG_ReadByte();
	else
		v10 = 100;

	// TODO: Implement
}

// TODO: Implement

/*
==================
CL_ParseStartSoundPacket
==================
*/
void CL_ParseDownload( void )
{
	int read;
	CRC32_t crc;
	char szFileName[128];

	memset(szFileName, 0, sizeof(szFileName));

	read = MSG_ReadShort();
	crc = MSG_ReadShort();

	// TODO: Implement
}

// TODO: Implement

/*
===============
CL_ClearResourceLists
===============
*/
void CL_ClearResourceLists( void )
{
	CL_ClearResourceList(&cl.resourcesneeded);
	CL_ClearResourceList(&cl.resourcesonhand);
}

/*
==================
CL_RegisterResources

Clean up and move to next part of sequence.
==================
*/
void CL_RegisterResources( void )
{
	double	time1, time2, time3, time4;

	if (cls.dl.custom)
	{
		cls.dl.custom = FALSE;
		return;
	}

	cl.worldmodel = cl.model_precache[1];

	cl_entities->model = cl.worldmodel;

	if (!cl.worldmodel)
		Sys_Error("Client world model is NULL\n");

	Con_DPrintf("Setting up renderer...\n");
	time1 = Sys_FloatTime();

	R_NewMap();			// Tell rendering system we have a new set of models.
	time2 = Sys_FloatTime();

	Hunk_Check();		// make sure nothing is hurt
	time3 = Sys_FloatTime();

	noclip_anglehack = FALSE;		// noclip is turned off at start

	if (!sv.active)
	{
		// TODO: Implement
	}

	MSG_WriteByte(&cls.netchan.message, clc_stringcmd);

	// Done with all resources, issue spawn command.
	// Include server count in case server disconnects and changes level during d/l
	MSG_WriteString(&cls.netchan.message, va("prespawn %i 0", cl.servercount));
	time4 = Sys_FloatTime();
}

void CL_MoveToOnHandList( resource_t* pResource )
{
	if (!pResource)
	{
		Con_DPrintf("Null resource passed to CL_MoveToOnHandList\n");
		return;
	}

	switch (pResource->type)
	{
	case t_sound:
		// TODO: Implement
		break;
	case t_skin:
		break;
	case t_model:
		cl.model_precache[pResource->nIndex] = Mod_ForName(pResource->szFileName, FALSE);
		if (!cl.model_precache[pResource->nIndex])
		{
			Con_Printf("Model %s not found\n", pResource->szFileName);
			if (pResource->ucFlags & RES_FATALIFMISSING)
			{
				Con_Printf("Cannot continue without model, disconnecting\n");
				CL_Disconnect();
			}
		}
		break;
	case t_decal:
		if (!(pResource->ucFlags & RES_CUSTOM))
		{
			Draw_DecalSetName(pResource->nIndex, pResource->szFileName);
		}
		break;

	default:
		Con_DPrintf("Unknown resource type\n");
		break;
	}

	CL_RemoveFromResourceList(pResource);
	CL_AddToResourceList(pResource, &cl.resourcesonhand);
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
	resource_t* p;
	FILE* fp;

	int nTotalSize = 0;
	int nSize, nDownloadSize;

	for (p = cl.resourcesneeded.pNext; p != &cl.resourcesneeded; p = p->pNext)
	{
		nSize = 0;
		fp = NULL;

		switch (p->type)
		{
		case t_sound:
			if (p->szFileName[0] != '*')
			{
				nSize = COM_FindFile(va("sound/%s", p->szFileName), NULL, &fp);
			}
			break;
		case t_skin:
			nSize = COM_FindFile(p->szFileName, NULL, &fp);
			break;
		case t_model:
			if (p->szFileName[0] != '*')
			{
				nSize = COM_FindFile(p->szFileName, NULL, &fp);
			}
			break;
		case t_decal:
			if (p->ucFlags & RES_CUSTOM)
			{
				nSize = -1;
			}
			break;
		}

		if (nSize == -1)
		{
			nDownloadSize = p->nDownloadSize;
			p->ucFlags |= 2u;
		}
		else
		{
			nDownloadSize = 0;
		}

		if (fp)
			fclose(fp);

		nTotalSize += nDownloadSize;
	}

	return nTotalSize;
}

/*
================
CL_RequestMissingResources

================
*/
qboolean CL_RequestMissingResources( void )
{
	resource_t* p;

	if (cls.dl.download || cls.dl.isdownloading)
		return FALSE;

	if (!cls.dl.custom && cls.state != ca_uninitialized)
		return FALSE;

	if (cls.dl.doneregistering)
		return FALSE;

	p = cl.resourcesneeded.pNext;
	cls.dl.resource = p;

//	memcpy(&custom_resource, cl.resourcesneeded.pNext, sizeof(custom_resource)); TODO: Implement

	if (p == &cl.resourcesneeded)
	{
		cls.dl.resource = NULL;
		Con_DPrintf("Resource propagation complete.\n");
		CL_RegisterResources();
		cls.dl.doneregistering = TRUE;
		return FALSE;
	}

	if (!(p->ucFlags & RES_WASMISSING))
	{
		CL_MoveToOnHandList(p);
		return TRUE;
	}

	switch (p->type)
	{
	case t_sound:
		// TODO: Implement
		break;
	case t_skin:
		// TODO: Implement
		break;
	case t_model:
		// TODO: Implement
		break;
	case t_decal:
		// TODO: Implement
		break;
	}

	return TRUE;
}

/*
===============
CL_StartResourceDownloading

Begin resource downloading, set incoming transfer data
===============
*/
void CL_StartResourceDownloading( char* pszMessage, qboolean bCustom )
{
	int nWorldSize, nModelsSize, nDecalsSize, nSoundsSize, nSkinsSize;

	if (pszMessage)
		Con_DPrintf(pszMessage);

	cls.dl.nTotalSize = COM_SizeofResourceList(&cl.resourcesneeded, &nWorldSize, &nModelsSize, &nDecalsSize, &nSoundsSize, &nSkinsSize);
	cls.dl.nTotalToTransfer = CL_EstimateNeededResources();

	Con_DPrintf("Resources total %iK\n", cls.dl.nTotalSize / 1024);

	if (nWorldSize > 0)
		Con_DPrintf("  World :  %iK\n", nWorldSize / 1024);
	if (nModelsSize > 0)
		Con_DPrintf("  Models:  %iK\n", nModelsSize / 1024);
	if (nSoundsSize > 0)
		Con_DPrintf("  Sounds:  %iK\n", nSoundsSize / 1024);
	if (nDecalsSize > 0)
		Con_DPrintf("  Decals:  %iK\n", nDecalsSize / 1024);
	if (nSkinsSize > 0)
		Con_DPrintf("  Skins :  %iK\n", nSkinsSize / 1024);

	Con_DPrintf("----------------------\n");
	Con_DPrintf("Resources to request: %iK\n", cls.dl.nTotalToTransfer / 1024);

	if (bCustom)
	{
		cls.dl.custom = TRUE;
	}
	else
	{
		cls.state = ca_uninitialized;
		cls.dl.custom = FALSE;
	}

	cls.dl.doneregistering = FALSE;
	cls.dl.isdownloading = FALSE;

	cls.dl.fLastStatusUpdate = realtime;
	cls.dl.fLastDownloadTime = realtime;

	cls.dl.nRemainingToTransfer = cls.dl.nTotalToTransfer;

	memset(cls.dl.rgStats, 0, sizeof(cls.dl.rgStats));
	cls.dl.nCurStat = 0;
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
	if (cl_entities)
	{
		free(cl_entities);
		cl_entities = NULL;
	}

	// TODO: Implement
}

/*
================
CL_ReallocateDynamicData

================
*/
void CL_ReallocateDynamicData( int nMaxClients )
{
	cl.max_edicts = COM_EntsForPlayerSlots(nMaxClients);

	if (cl.max_edicts <= 0)
		Sys_Error("CL_ReallocateDynamicData allocating 0 entities");

	if (cl_entities)
		Con_Printf("CL_Reallocate cl_entities\n");

	cl_entities = (cl_entity_t*)malloc(sizeof(cl_entity_t) * cl.max_edicts);
	memset(cl_entities, 0, (sizeof(cl_entity_t) * cl.max_edicts));

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
	if (cl.playernum & PN_SPECTATOR)
	{
		cl.spectator = TRUE;
		cl.playernum &= ~PN_SPECTATOR;
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

/*
==================
CL_ParseBaseline
==================
*/
void CL_ParseBaseline( cl_entity_t* ent )
{
	int			i;

	ent->baseline.entityType = MSG_ReadByte();
	ent->baseline.modelindex = MSG_ReadShort();
	ent->baseline.sequence = MSG_ReadByte();
	ent->baseline.frame = MSG_ReadByte();

	if (ent->baseline.entityType != 0)
		ent->baseline.scale = MSG_ReadByte() * (1.0 / 10.0);
	else
		ent->baseline.scale = MSG_ReadWord() * (1.0 / 256.0);

	ent->baseline.colormap = MSG_ReadByte();
	ent->baseline.skin = MSG_ReadShort();
	ent->baseline.solid = MSG_ReadByte();

	for (i = 0; i < 3; i++)
	{
		ent->baseline.origin[i] = MSG_ReadCoord();
		ent->baseline.angles[i] = MSG_ReadFloat();
		ent->baseline.mins[i] = MSG_ReadCoord();
		ent->baseline.maxs[i] = MSG_ReadCoord();
	}

	ent->baseline.rendermode = MSG_ReadByte();
	ent->baseline.renderamt = MSG_ReadByte();
	ent->baseline.rendercolor.r = MSG_ReadByte();
	ent->baseline.rendercolor.g = MSG_ReadByte();
	ent->baseline.rendercolor.b = MSG_ReadByte();
	ent->baseline.renderfx = MSG_ReadByte();
}


/*
==================
CL_ParseClientdata

Server information pertaining to this client only
==================
*/
void CL_ParseClientdata( int bits )
{
	int				i;
	float		latency;
	frame_t* frame;

// calculate simulated time of message
	oldparsecountmod = parsecountmod;

	i = cls.netchan.incoming_acknowledged;
	cl.parsecount = i;
	i &= UPDATE_MASK;
	parsecountmod = i;
	frame = &cl.frames[i];
	parsecounttime = cl.frames[i].senttime;

	frame->receivedtime = realtime;

// calculate latency
	latency = frame->receivedtime - frame->senttime;

	if (latency < 0 || latency > 1.0)
	{
//		Con_Printf("Odd latency: %5.2f\n", latency);
	}
	else
	{
		// drift the average latency towards the observed latency

		if (latency < cls.latency)
			cls.latency = latency;
		else
			cls.latency += 0.001;	// drift up, so correction are needed		
	}

	if (bits & SU_VIEWHEIGHT)
		cl.viewheight = MSG_ReadChar();
	else
		cl.viewheight = DEFAULT_VIEWHEIGHT;

	if (bits & SU_IDEALPITCH)
		cl.idealpitch = MSG_ReadChar();
	else
		cl.idealpitch = 0;

	VectorCopy(cl.mvelocity[0], cl.mvelocity[1]);
	for (i = 0; i < 3; i++)
	{
		if (bits & (SU_PUNCH1 << i))
			cl.punchangle[i] = MSG_ReadHiresAngle();
		else
			cl.punchangle[i] = 0;
		if (bits & (SU_VELOCITY1 << i))
			cl.mvelocity[0][i] = MSG_ReadChar() * 16;
		else
			cl.mvelocity[0][i] = 0;
	}

	if (bits & SU_WEAPONS)
		cl.weapons = MSG_ReadLong();

	cl.onground = (bits & SU_ONGROUND) != 0;
	cl.inwater = (bits & SU_INWATER) != 0;

	if (cl.inwater)
	{
		if (bits & SU_FULLYINWATER)
			cl.waterlevel = 3;
		else
			cl.waterlevel = 2;
	}
	else
	{
		cl.waterlevel = 0;
	}

	if (bits & SU_ITEMS)
		cl.stats[STAT_WEAPON] = MSG_ReadShort();
	else
		cl.stats[STAT_WEAPON] = 0;

	i = MSG_ReadShort();
	if (i != cl.stats[STAT_HEALTH])
	{
		cl.stats[STAT_HEALTH] = i;
	}
}

// TODO: Implement

/*
===================
CL_ParseStaticSound

===================
*/
void CL_ParseStaticSound( void )
{
	vec3_t		org;
	int			sound_num;
	float		vol, atten;
	int			i;
	int			ent;
	int			pitch;
	int			flags;

	for (i = 0; i < 3; i++)
		org[i] = MSG_ReadCoord();
	sound_num = MSG_ReadShort();
	vol = MSG_ReadByte() / 255.0;		// reduce back to 0.0 - 1.0 range
	atten = MSG_ReadByte() / 64.0;
	ent = MSG_ReadShort();
	pitch = MSG_ReadByte();
	flags = MSG_ReadByte();

	// TODO: Implement
}


// TODO: Implement


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


// TODO: Implement


void CL_PlayerDropped( int nPlayerNumber )
{
	COM_ClearCustomizationList(&cl.players[nPlayerNumber].customdata, TRUE);
}


// TODO: Implement


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
	int	i, j;
	// For determining data parse sizes
	int bufStart, bufEnd;

	int	slot, spectator;

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

//
// parse the message
//
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
			msg_buckets[63]++;
			DispatchUserMsg(cmd);

			// Mark end position
			bufEnd = msg_readcount;
			last_data[63] += bufEnd - bufStart;
			continue;
		}
		
		SHOWNET(svc_strings[cmd]);

		// TODO: Implement

		if (cmd <= 63)
			msg_buckets[cmd]++;

	// other commands
		switch (cmd)
		{
		default:
			Con_DPrintf("Last 3 messages parsed.\n");
			// TODO: Implement
			Con_DPrintf("BAD:  %3i:%s\n", msg_readcount - 1, svc_strings[cmd]);
			Host_Error("CL_ParseServerMessage: Illegible server message\n");
			break;

		case svc_nop:
//			Con_Printf("svc_nop\n");
			break;

		case svc_disconnect:
			Host_EndGame("Server disconnected\n");

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

		case svc_sound:
			CL_ParseStartSoundPacket();
			break;

		case svc_time:
			cl.mtime[1] = cl.mtime[0];
			cl.mtime[0] = MSG_ReadFloat();
			break;

		case svc_print:
			Con_Printf("%s", MSG_ReadString());
			break;

		case svc_stufftext:
			Cbuf_AddText(MSG_ReadString());
			break;

		case svc_setangle:
			for (i = 0; i < 3; i++)
			{
				cl.viewangles[i] = MSG_ReadHiresAngle();
			}
			break;

		case svc_serverinfo:
			CL_ParseServerInfo();
			vid.recalc_refdef = TRUE;	// leave intermission full screen
			break;

		case svc_lightstyle:
			i = MSG_ReadByte();
			if (i >= MAX_LIGHTSTYLES)
				Sys_Error("svc_lightstyle > MAX_LIGHTSTYLES");
			Q_strcpy(cl_lightstyle[i].map, MSG_ReadString());
			cl_lightstyle[i].length = Q_strlen(cl_lightstyle[i].map);
			break;

		case svc_updatename:
			i = MSG_ReadByte();
			slot = i & ~PN_SPECTATOR;
			spectator = (i & PN_SPECTATOR) != 0;
			if (slot >= cl.maxclients)
				Host_Error("CL_ParseServerMessage: svc_updatename > MAX_SCOREBOARD");
			strcpy(cl.players[slot].name, MSG_ReadString());
			if (spectator)
			{
				CL_PlayerDropped(slot);
			}
			break;

		case svc_updatefrags:
			i = MSG_ReadByte();
			MSG_ReadShort();
			break;

		case svc_clientdata:
			i = MSG_ReadShort();
			CL_ParseClientdata(i);
			break;

		case svc_stopsound:
			i = MSG_ReadShort();
			// TODO: Implement
			break;

		case svc_updatecolors:
			i = MSG_ReadByte();
			if (i >= cl.maxclients)
				Host_Error("CL_ParseServerMessage: svc_updatecolors > MAX_SCOREBOARD");
			cl.players[i].color = MSG_ReadByte();
			break;

		case svc_particle:
			R_ParseParticleEffect();
			break;

		case svc_damage:
			break;

		// TODO: Implement

		case svc_spawnbaseline:
			i = MSG_ReadShort();
			// must use CL_EntityNum() to force cl.num_entities up
			CL_ParseBaseline(CL_EntityNum(i));
			break;

		// TODO: Implement

		case svc_signonnum:
			i = MSG_ReadByte();
			if (i <= cls.signon)
				Host_Error("Received signon %i when at %i", i, cls.signon);
			cls.signon = i;
			CL_SignonReply();
			break;

		// TODO: Implement

		case svc_killedmonster:
			break;

		case svc_foundsecret:
			break;

		case svc_spawnstaticsound:
			CL_ParseStaticSound();
			break;

		case svc_intermission:
			cl.intermission = 1;
			vid.recalc_refdef = TRUE;	// go to full screen
			cl.completed_time = cl.time;
			break;

		case svc_finale:
			cl.intermission = 2;
			vid.recalc_refdef = TRUE;	// go to full screen
			cl.completed_time = cl.time;
			SCR_CenterPrint(MSG_ReadString());
			break;

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
		
		case svc_weaponanim:
			cl.weaponstarttime = 0.0f;
			cl.weaponsequence = MSG_ReadByte();
			cl.viewent.baseline.body = MSG_ReadByte();
			break;

		// TODO: Implement

		case svc_roomtype:
			Cvar_SetValue("room_type", MSG_ReadShort());
			break;

		case svc_addangle:
			cl.viewangles[YAW] += MSG_ReadHiresAngle();
			break;

		case svc_newusermsg:
			AddNewUserMsg();
			break;

		case svc_download:
			CL_ParseDownload();
			break;

		case svc_packetentities:
			CL_ParsePacketEntities(FALSE);
			break;

		case svc_deltapacketentities:
			CL_ParsePacketEntities(TRUE);
			break;

		case svc_playerinfo:
			CL_ParsePlayerinfo();
			break;

		case svc_chokecount:
			i = MSG_ReadByte();
			for (j = 0; j < i; j++)
				cl.frames[(cls.netchan.incoming_acknowledged - 1 - j) & UPDATE_MASK].receivedtime = -2;
			break;

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

		case svc_crosshairangle:
			cl.crosshairangle[PITCH] = MSG_ReadChar() * 0.2f;
			cl.crosshairangle[YAW] = MSG_ReadChar() * 0.2f;
			break;

		// TODO: Implement
		}

		// Mark end position
		bufEnd = msg_readcount;
		last_data[cmd] += bufEnd - bufStart;
	}

	// end of message
	SHOWNET("END OF MESSAGE");

	// TODO: Implement
}