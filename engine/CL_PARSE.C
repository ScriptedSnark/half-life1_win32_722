// cl_parse.c  -- parse a message received from the server

#include "quakedef.h"
#include "pmove.h"
#include "decal.h"
#include "cl_demo.h"
#include "cl_draw.h"


int		last_data[64];
int		msg_buckets[64];



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

//=============================================================================

/*
===============
CL_EntityNum

This error checks and tracks the total number of entities
===============
*/
//cl_entity_t* CL_EntityNum( int num )
//{
//	if (num >= cl.num_entities)
//	{
//		if (num >= cl.max_edicts)
//			Host_Error("CL_EntityNum: %i is an invalid number, cl.max_edicts is %i", num, cl.max_edicts);
//		while (cl.num_entities <= num)
//		{
//			cl_entities[cl.num_entities].colormap = vid.colormap;
//			cl.num_entities++;
//		}
//	}
//
//	return &cl_entities[num];
//}

// TODO: Implement


/*
==================
CL_ParseStartSoundPacket
==================
*/
void CL_ParseStartSoundPacket( void )
{
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

	// TODO: Implement

	Con_DPrintf("Setting up renderer...\n");
	time1 = Sys_FloatTime();

//	R_NewMap();			// Tell rendering system we have a new set of models. (TODO: Implement)
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
		// TODO: Implement
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

	if (cls.dl.file || cls.dl.downloading)
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
	cls.dl.downloading = FALSE;


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

void CL_ParseBaseline( cl_entity_t* ent )
{
	// TODO: Implement
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

		// TODO: Implement

		case svc_serverinfo:
			CL_ParseServerInfo();
			vid.recalc_refdef = TRUE;	// leave intermission full screen
			break;

		// TODO: Implement

		case svc_updatefrags:
			i = MSG_ReadByte();
			MSG_ReadShort();
			break;

		// TODO: Implement

		case svc_damage:
			break;

		// TODO: Implement

		case svc_spawnbaseline:
			i = MSG_ReadShort();
			// must use CL_EntityNum() to force cl.num_entities up
//			CL_ParseBaseline(CL_EntityNum(i)); TODO: Implement
			break;

		// TODO: Implement

		case svc_killedmonster:
			break;

		case svc_foundsecret:
			break;

		case svc_spawnstaticsound:
			CL_ParseStaticSound();
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

		case svc_newusermsg:
			// TODO: Implement
			MSG_ReadByte();
			MSG_ReadByte();
			MSG_ReadLong();
			MSG_ReadLong();
			MSG_ReadLong();
			MSG_ReadLong();
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