// cl_parse.c  -- parse a message received from the server

#include "quakedef.h"
#include "pmove.h"
#include "decal.h"
#include "r_trans.h"
#include "cl_demo.h"
#include "cl_draw.h"
#include "hashpak.h"

int		last_data[MAX_DATA_HISTORY];
int		msg_buckets[MAX_DATA_HISTORY];

UserMsg* gClientUserMsgs = NULL;

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
resource_t custom_resource;

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
			cl_entities[cl.num_entities].colormap = vid.colormap;
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
		if (!_stricmp(pList->szName, umsg.szName))
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

			if (pList->pfn)
				pList->pfn(pList->szName, MsgSize, buf);
			else
				Con_DPrintf("UserMsg: No pfn %s %d\n", pList->szName, iMsg);
		}
	}

	if (!fFound)
	{
		Con_DPrintf("UserMsg: Not Present on Client %d\n", iMsg);
	}
}

/*
===============
DispatchDirectUserMsg

Returns true on success
===============
*/
int DispatchDirectUserMsg( const char* pszName, int iSize, void* pBuf )
{
	int	fFound;
	UserMsg* pList;
	pfnUserMsgHook pfnRet;
	int	iMsgSize;

	fFound = 0;

	for (pList = gClientUserMsgs; pList; pList = pList->next)
	{
		if (!_stricmp(pszName, pList->szName))
		{
			fFound = 1;

			iMsgSize = pList->iSize;
			if (iMsgSize == -1)
				iMsgSize = iSize;

			pfnRet = pList->pfn;
			if (!pfnRet)
			{
				Con_DPrintf("UserMsg: No pfn %s %d\n", pList->szName, pList->iMsg);
				continue;
			}

			pfnRet(pList->szName, iMsgSize, pBuf);
		}
	}

	return fFound;
}

pfnUserMsgHook HookServerMsg( const char* pszName, pfnUserMsgHook pfn )
{
	UserMsg* pList, * pLastMatch;
	pfnUserMsgHook pfnRet;

	pLastMatch = NULL;

	for (pList = gClientUserMsgs; pList; pList = pList->next)
	{
		if (!_stricmp(pszName, pList->szName))
		{
			pfnRet = pList->pfn;
			if (pfnRet == pfn)
				return pfnRet;

			pLastMatch = pList;
		}
	}

	pList = (UserMsg*)malloc(sizeof(UserMsg));
	memset(pList, 0, sizeof(UserMsg));

	if (pLastMatch)
	{
		memcpy(pList, pLastMatch, sizeof(*pList));
	}
	else
	{
		strcpy(pList->szName, pszName);
	}

	pList->pfn = pfn;
	pList->next = gClientUserMsgs;
	gClientUserMsgs = pList;
	
	return NULL;
}

void CL_UserMsgs_f( void )
{
	UserMsg* pMsg;

	pMsg = gClientUserMsgs;
	while (pMsg)
	{
		Con_Printf("%i:%s Sz %i\n", pMsg->iMsg, pMsg->szName, pMsg->iSize);
		pMsg = pMsg->next;
	}
}

///////////////////////////////////////////////////
//
// SOUND
//
//

/*
==================
CL_ParseStartSoundPacket
==================
*/
void CL_ParseStartSoundPacket( void )
{
	vec3_t	pos;
	int		channel, ent;
	int		sound_num;
	float	volume;
	int		field_mask;
	float 	attenuation;
	int		pitch;
	int		i;
	sfx_t* sfx;
	sfx_t sfxsentence;

	field_mask = MSG_ReadByte();

	if (field_mask & SND_VOLUME)
		volume = MSG_ReadByte() / 255.0;		// reduce back to 0.0 - 1.0 range
	else
		volume = DEFAULT_SOUND_PACKET_VOLUME / 255.0;

	if (field_mask & SND_ATTENUATION)
		attenuation = MSG_ReadByte() / 64.0;
	else
		attenuation = DEFAULT_SOUND_PACKET_ATTENUATION;

	channel = MSG_ReadShort();
	if (field_mask & SND_LARGE_INDEX)
		sound_num = MSG_ReadShort();
	else
		sound_num = MSG_ReadByte();

	ent = channel >> 3;
	channel &= 7;

	if (ent >= cl.max_edicts)
		Host_Error("CL_ParseStartSoundPacket: ent = %i", ent);

	for (i = 0; i < 3; i++)
		pos[i] = MSG_ReadCoord();

	if (field_mask & SND_PITCH)
		pitch = MSG_ReadByte();
	else
		pitch = DEFAULT_SOUND_PACKET_PITCH;

	if (field_mask & SND_SENTENCE)
	{
		sfx = &sfxsentence;
		strcpy(sfx->name, "!");
		strcat(sfx->name, rgpszrawsentence[sound_num]);
	}
	else
	{
		sfx = cl.sound_precache[sound_num];
	}

	if (channel == CHAN_STATIC)
	{
		S_StartStaticSound(ent, channel, sfx, pos, volume, attenuation, field_mask, pitch);
	}
	else
	{
		S_StartDynamicSound(ent, channel, sfx, pos, volume, attenuation, field_mask, pitch);
	}
}

/*
==================
CL_CheckFile

Checks if the file exists or if we can download it
==================
*/
qboolean CL_CheckFile( char* filename )
{
	// TODO: Reimplement

	qboolean	bDoWeHaveOnlyAPartOfTheFile = FALSE;
	int			nNumChunksRead;
	int			filesize;
	FILE*		pfHandle;
	CRC32_t		crc;
	char		szName[MAX_QPATH];
	resource_t	res;
	char		buffer[1024];

	// Filter out the junkies, who wanna abuse our pretty system.
	if (strstr(filename, ".."))
	{
		Con_Printf("Refusing to download a path with '..'\n");

		return TRUE;
	}

	// You shall not pass!
	if (!cl_allowdownload.value)
	{
		Con_Printf("Download refused, cl_allow_download is 0\n");

		return TRUE;
	}

	// That bitch is way too heavy, I ain't downloading it over my sweaty dial-up.
	if (cl_download_max.value && cls.dl.resource)
	{
		if (cls.dl.resource->nDownloadSize > cl_download_max.value)
		{
			Con_Printf("Download refused, cl_download_maxsize is %i, file is %i bytes\n", (int)cl_download_max.value, cls.dl.resource->nDownloadSize);

			return TRUE;
		}
	}

	// I don't wanna have lag spikes when I'm playing just because some bozo decided to join, so GTFO and take your file back!
	if (cls.state == ca_active && !cl_download_ingame.value)
	{
		Con_Printf("In-game download refused, cl_download_ingame is 0\n");

		return TRUE;
	}

	sprintf(szName, filename);

	// This is a custom resource (MD5), handle it differently
	if (strlen(filename) == 36 && !_strnicmp(filename, "!MD5", 4))
	{
		memset(&res, 0, sizeof(res));

		COM_HexConvert(&filename[4] /* skip !MD5 */, 32, res.rgucMD5_hash);

		if (HPAK_GetDataPointer("custom.hpk", &res, &pfHandle))
		{
			fclose(pfHandle);
			return TRUE;
		}

		sprintf(cls.dl.szFileName, "cust.dat");
	}
	else
	{
		if (COM_FindFile(szName, NULL, &pfHandle) == -1)
		{
			strcpy(cls.dl.szFileName, szName); // It doesn't exist, so we need to download it
		}
		else
		{
			if (pfHandle)
			{
				fclose(pfHandle);
			}

			// It exists, return  T R U E
			return TRUE;
		}
	}
	
	// sub_59B3550(); // TODO: Implement
	//FF: ^ this is somehow related to demos idk what the hell is this

	// Strip the extension of the file we're going to download and place either .cst or .tmp there.
	COM_StripExtension(cls.dl.szFileName, cls.dl.extension);
	strcat(&cls.dl.extension[strlen(cls.dl.extension)], cls.dl.custom ? ".cst" : ".tmp");

	cls.dl.crcFile = 0;

	filesize = COM_FindFile(cls.dl.extension, NULL, &pfHandle);
	if (filesize != -1 && !cls.dl.custom)
	{
		bDoWeHaveOnlyAPartOfTheFile = TRUE;
		nNumChunksRead = 0;
		CRC32_Init(&crc);
		if ((filesize / 1024) > 0)
		{
			while (TRUE)
			{
				// Let's see if we can successfully read the first 1024 bytes chunk
				if (fread(buffer, sizeof(buffer), sizeof(byte), pfHandle) == 1)
				{
					// We'll try reading another 1024 bytes chunk, but the file may run out of bytes, so we need to keep an eye on that
					nNumChunksRead++;
					CRC32_ProcessBuffer(&crc, buffer, sizeof(buffer));
					if (nNumChunksRead >= (filesize / 1024))
						break;
				}
				else
				{
					bDoWeHaveOnlyAPartOfTheFile = TRUE;
					break;
				}
			}
		}

		cls.dl.crcFile = crc = CRC32_Final(crc);

		if (pfHandle)
		{
			fclose(pfHandle);
		}
	}

	MSG_WriteByte(&cls.netchan.message, clc_stringcmd);
	if (!cls.dl.custom && bDoWeHaveOnlyAPartOfTheFile && !(filesize % 1024))
	{
		MSG_WriteString(&cls.netchan.message, va("download %s %i %i", szName, (filesize / 1024), crc));
	}
	else if (!cls.dl.custom)
	{
		MSG_WriteString(&cls.netchan.message, va("download %s", szName));
	}
	else
	{
		MSG_WriteString(&cls.netchan.message, va("download \"!MD5%s\"", MD5_Print(cls.dl.resource->rgucMD5_hash)));
	}

	cls.dl.downloading = TRUE;
	cls.dl.nNumFileChunksRead++;

	if (pfHandle)
	{
		fclose(pfHandle);
	}

	return FALSE;
}

/*
==================
CL_ParseDownload
==================
*/
void CL_UpdateDownloadStats( void )
{
	// TODO: Implement
}

/*
==================
CL_ParseDownload
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

/*
================
CL_PrintResourceInfo

Prints which data the resource passed contains.
================
*/
void CL_PrintResourceInfo( int index, resource_t* pResource )
{
	// TODO: Reimplement

	static char fatal[8];
	static char type[12];

	// If the resource was missing, let the player know about that
	if (pResource->ucFlags & RES_WASMISSING)
		sprintf(fatal, "Y");
	else
		sprintf(fatal, "N");

	// Now let the player know which type this resource has
	switch (pResource->type)
	{
		case t_sound:
			sprintf(type, "sound");
			break;
		case t_skin:
			sprintf(type, "skin");
			break;
		case t_model:
			sprintf(type, "model");
			break;
		case t_decal:
			sprintf(type, "decal");
			break;
		default:
			sprintf(type, "unknown");
			break;
	}

	// Here we spit out the whole resource data
	Con_Printf(
	  "%3i %i %s:%15s %i %s\n",
	  index,
	  pResource->nDownloadSize,
	  type,
	  pResource->szFileName,
	  pResource->nIndex,
	  fatal);

	// If the resource is a custom resource uploaded by somebody, print its MD5 hash
	if (pResource->ucFlags & RES_CUSTOM)
	{
		Con_Printf("MD5:  %s\n", MD5_Print(pResource->rgucMD5_hash));
	}
}

/*
===============
CL_ClearResourceLists

===============
*/
void CL_Resources_f( void )
{
	// TODO: Implement
}

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
		if (pResource->ucFlags & RES_WASMISSING)
		{
			cl.sound_precache[pResource->nIndex] = NULL;
		}
		else
		{
			S_BeginPrecaching();
			cl.sound_precache[pResource->nIndex] = S_PrecacheSound(pResource->szFileName);
			S_EndPrecaching();
			if (!cl.sound_precache[pResource->nIndex] && (pResource->ucFlags & RES_FATALIFMISSING))
			{
				Con_Printf("Cannot continue without sound %s, disconnecting\n", pResource->szFileName);
				CL_Disconnect();
			}
		}
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
	int nSize;

	nSize = 0;

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
			p->ucFlags |= RES_WASMISSING;
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

This is used to perform repeated checks on the local player to see
if it has loaded all the required resources
================
*/
qboolean CL_RequestMissingResources( void )
{
	char	szCust[MAX_OSPATH];
	resource_t* p;

	if (cls.dl.download || cls.dl.downloading)
		return FALSE;

	if (!cls.dl.custom && cls.state != ca_uninitialized)
		return FALSE;

	if (cls.dl.doneregistering)
		return FALSE;

	p = cl.resourcesneeded.pNext;
	cls.dl.resource = p;

	memcpy(&custom_resource, cl.resourcesneeded.pNext, sizeof(custom_resource));

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
		if (p->szFileName[0] == '*' || CL_CheckFile(va("sound/%s", p->szFileName)))
		{
			CL_MoveToOnHandList(p);
			return TRUE;
		}
		break;
	case t_skin:
		CL_MoveToOnHandList(p);
		break;
	case t_model:
		if (p->szFileName[0] == '*' || CL_CheckFile(va("sound/%s", p->szFileName)))
		{
			CL_MoveToOnHandList(p);
			return TRUE;
		}
		break;
	case t_decal:
		if (p->ucFlags & RES_CUSTOM)
		{
			sprintf(szCust, "!MD5%s", MD5_Print(p->rgucMD5_hash));

			if (CL_CheckFile(szCust))
			{
				CL_MoveToOnHandList(p);
				return TRUE;
			}
		}
		else
		{
			CL_MoveToOnHandList(p);
		}
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
	int		worldSize, modelsSize, decalsSize, soundsSize, skinsSize;

	if (pszMessage)
		Con_DPrintf(pszMessage);

	cls.dl.nTotalSize = COM_SizeofResourceList(&cl.resourcesneeded, &worldSize, &modelsSize, &decalsSize, &soundsSize, &skinsSize);
	cls.dl.nTotalToTransfer = CL_EstimateNeededResources();

	Con_DPrintf("Resources total %iK\n", cls.dl.nTotalSize / 1024);

	if (worldSize > 0)
		Con_DPrintf("  World :  %iK\n", worldSize / 1024);
	if (modelsSize > 0)
		Con_DPrintf("  Models:  %iK\n", modelsSize / 1024);
	if (soundsSize > 0)
		Con_DPrintf("  Sounds:  %iK\n", soundsSize / 1024);
	if (decalsSize > 0)
		Con_DPrintf("  Decals:  %iK\n", decalsSize / 1024);
	if (skinsSize > 0)
		Con_DPrintf("  Skins :  %iK\n", skinsSize / 1024);

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
	int		i, total;
	int		totalsize;
	resource_t* resource;

	totalsize = MSG_ReadShort();
	i = MSG_ReadShort();
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

	if (total < totalsize)
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
CL_PlayerHasCustomization

Sees if the specified customization type exists for the nPlayerNum
================
*/
customization_t* CL_PlayerHasCustomization( int nPlayerNum, resourcetype_t type )
{
	customization_t* pList;

	pList = cl.players[nPlayerNum].customdata.pNext;
	while (pList)
	{
		if (pList->resource.type == type)
			return pList;

		pList = pList->pNext;
	}

	return NULL;
}

/*
================
CL_RemoveCustomization

Removes the specified customization for the nPlayerNum
================
*/
void CL_RemoveCustomization( int nPlayerNum, customization_t* pRemove )
{
	customization_t* pList;
	int	i;
	customization_t* pNext;

	pList = cl.players[nPlayerNum].customdata.pNext;
	while (pList)
	{
		pNext = pList->pNext;

		if (pRemove == pList)
		{
			if (pList->bInUse && pList->pBuffer)
				free(pList->pBuffer);

			if (pList->bInUse && pList->pInfo)
			{
				if (pRemove->resource.type == t_decal)
				{
					cachewad_t* pWad;

					if (cls.state == ca_active)
						R_DecalRemoveAll(-1 - nPlayerNum);

					pWad = (cachewad_t*)pRemove->pInfo;
					free(pWad->lumps);

					for (i = 0; i < pWad->cacheCount; i++)
					{
#if defined ( GLQUAKE )
						cacheentry_t* pic = &pWad->cache[i];
#else
						cachepic_t* pic = &pWad->cache[i];
#endif
						if (Cache_Check(&pic->cache))
							Cache_Free(&pic->cache);
					}

					free(pWad->cache);
				}

				free(pRemove->pInfo);
			}

			free(pRemove);
			cl.players[nPlayerNum].customdata.pNext = pNext;
		}

		pList = pNext;
	}
}

/*
================
CL_ParseCustomization

================
*/
void CL_ParseCustomization( void )
{
	resource_t* resource;
	int	i;
	customization_t* pExistingCustomization;
	customization_t* pList;
	FILE* pFile;
	qboolean bFound;

	i = MSG_ReadByte();
	if (i >= MAX_CLIENTS)
		Host_Error("Bogus player index during customization parsing.\n");

	resource = (resource_t*)malloc(sizeof(resource_t));
	memset(resource, 0, sizeof(resource_t));
	resource->type = MSG_ReadByte();
	strcpy(resource->szFileName, MSG_ReadString());
	resource->nIndex = MSG_ReadShort();
	resource->nDownloadSize = MSG_ReadLong();
	resource->pNext = resource->pPrev = NULL;
	resource->ucFlags = MSG_ReadByte();
	resource->ucFlags &= ~RES_WASMISSING;

	if (resource->ucFlags & RES_CUSTOM)
		MSG_ReadBuf(sizeof(resource->rgucMD5_hash), resource->rgucMD5_hash);

	Con_DPrintf("New resource from player %i\n", i);

	if (developer.value)
		CL_PrintResourceInfo(i, resource);

	resource->playernum = i;

	if (cls.demoplayback)
	{
		Con_DPrintf("Custom resources do not function during demo playback.\n");
		free(resource);
		return;
	}

	if (!cl_allowdownload.value)
	{
		Con_DPrintf("Refusing new resource, cl_allow_download set to 0\n");
		free(resource);
		return;
	}

	if (cls.state == ca_active && !cl_download_ingame.value)
	{
		Con_Printf("Refusing new resource, cl_download_ingame set to 0\n");
		free(resource);
		return;
	}

	if (cl_download_max.value && resource->nDownloadSize > cl_download_max.value)
	{
		Con_Printf("Refusing new resource, cl_download_max is %i, resource is %i bytes\n",
			cl_download_max.value, resource->nDownloadSize);
		free(resource);
		return;
	}

	pExistingCustomization = CL_PlayerHasCustomization(resource->playernum, resource->type);
	if (pExistingCustomization)
	{
		CL_RemoveCustomization(resource->playernum, pExistingCustomization);
	}

	if (!HPAK_GetDataPointer(HASHPAK_FILENAME, resource, &pFile))
	{
		resource->ucFlags |= RES_WASMISSING;
		CL_AddToResourceList(resource, &cl.resourcesneeded);
		Con_Printf("Requesting %s from server\n", resource->szFileName);
		memcpy(&custom_resource, resource, sizeof(custom_resource));
		cls.dl.resource = &custom_resource;
		CL_StartResourceDownloading("Custom resource propagation...\n", TRUE);
		return;
	}

	// Search if this resource is already in customizations list
	bFound = FALSE;
	pList = cl.players[resource->playernum].customdata.pNext;
	while (pList)
	{
		if (memcmp(pList->resource.rgucMD5_hash, resource->rgucMD5_hash, sizeof(pList->resource.rgucMD5_hash)) == 0)
		{
			bFound = TRUE;
			break;
		}

		pList = pList->pNext;
	}

	if (bFound)
	{
		Con_DPrintf("Duplicate resource ignored for local client\n");
	}
	else
	{
		customization_t* pCust;
		cachewad_t* pWad;

		pCust = (customization_t*)malloc(sizeof(customization_t));
		memset(pCust, 0, sizeof(customization_t));
		pCust->bInUse = TRUE;

		memcpy(&pCust->resource, resource, sizeof(pCust->resource));

		if (resource->nDownloadSize <= 0)
			Host_EndGame("Error:  Customization with download size < 0\n");

		pCust->pBuffer = malloc(resource->nDownloadSize);
		fread(pCust->pBuffer, resource->nDownloadSize, 1, pFile);
		fclose(pFile);

		pCust->pNext = cl.players[resource->playernum].customdata.pNext;
		cl.players[resource->playernum].customdata.pNext = pCust;

		if ((pCust->resource.ucFlags & RES_CUSTOM) && pCust->resource.type == t_decal)
		{
			pWad = (cachewad_t*)malloc(sizeof(cachewad_t));
			pCust->pInfo = pWad;
			memset(pWad, 0, sizeof(cachewad_t));
			CustomDecal_Init(pWad, pCust->pBuffer, pCust->resource.nDownloadSize);

			pCust->bTranslated = TRUE;
			pCust->nUserData1 = 0;
			pCust->nUserData2 = pWad->lumpCount;
		}
	}

	free(resource);
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

	R_DestroyObjects();
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

	R_AllocObjects(cl.max_edicts);
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

	// receive level name
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

	if (ent->baseline.entityType == ENTITY_NORMAL)
		ent->baseline.scale = MSG_ReadWord() * (1.0 / 256.0);
	else
		ent->baseline.scale = MSG_ReadByte() * (1.0 / 10.0);	

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

/*
=====================
CL_ParseStatic
=====================
*/
void CL_ParseStatic( void )
{
	cl_entity_t* ent;
	int		i;

	i = cl.num_statics;
	if (i >= MAX_STATIC_ENTITIES)
		Host_Error("Too many static entities");
	ent = &cl_static_entities[i];
	cl.num_statics++;
	CL_ParseBaseline(ent);

// copy it to the current state
	ent->model = cl.model_precache[ent->baseline.modelindex];
	ent->frame = ent->baseline.frame;
	ent->colormap = (byte*)vid.colormap;
	ent->skin = ent->baseline.skin;
	ent->effects = ent->baseline.effects;
	ent->scale = ent->baseline.scale;
	ent->rendermode = ent->baseline.rendermode;
	ent->renderamt = ent->baseline.renderamt;
	ent->rendercolor.r = ent->baseline.rendercolor.r;
	ent->rendercolor.g = ent->baseline.rendercolor.g;
	ent->rendercolor.b = ent->baseline.rendercolor.b;
	ent->renderfx = ent->baseline.renderfx;

	VectorCopy(ent->baseline.origin, ent->origin);
	VectorCopy(ent->baseline.angles, ent->angles);
	R_AddEfrags(ent);
}

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
	sfx_t* sfx;
	sfx_t sfxsentence;

	for (i = 0; i < 3; i++)
		org[i] = MSG_ReadCoord();
	sound_num = MSG_ReadShort();
	vol = MSG_ReadByte() / 255.0;		// reduce back to 0.0 - 1.0 range
	atten = MSG_ReadByte() / 64.0;
	ent = MSG_ReadShort();
	pitch = MSG_ReadByte();
	flags = MSG_ReadByte();

	if (flags & SND_SENTENCE)
	{
		// make dummy sfx for sentences
		sfx = &sfxsentence;
		strcpy(sfx->name, "!");
		strcat(sfx->name, rgpszrawsentence[sound_num]);
	}
	else
	{
		sfx = cl.sound_precache[sound_num];
	}

	S_StartStaticSound(ent, CHAN_STATIC, sfx, org, vol, atten, flags, pitch);
}

/*
===================
CL_ParseMovevars
===================
*/
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

#if defined( GLQUAKE )
	if (movevars.zmax != gl_zmax.value)
		Cvar_SetValue("gl_zmax", movevars.zmax);
	if (gl_wateramp.value != movevars.waveHeight)
		Cvar_SetValue("gl_wateramp", movevars.waveHeight);

	cl_entities->scale = gl_wateramp.value;
#endif
}

/*
===============
CL_ParseSoundFade

===============
*/
void CL_ParseSoundFade( void )
{
	int percent;
	int inTime, holdTime, outTime;

	percent = MSG_ReadByte();
	holdTime = MSG_ReadByte();
	inTime = MSG_ReadByte();
	outTime = MSG_ReadByte();

	cls.soundfade.soundFadeStartTime = realtime;
	cls.soundfade.nStartPercent = percent;
	cls.soundfade.soundFadeHoldTime = holdTime;
	cls.soundfade.soundFadeInTime = inTime;
	cls.soundfade.soundFadeOutTime = outTime;
}

/*
===============
CL_Restore

Restores a saved game.
===============
*/
void CL_Restore( char* fileName )
{
	DECALLIST decalList;
	int i, decalCount, temp, mapCount;
	FILE* pFile;
	char name[16];
	char* pMapName;

	pFile = fopen(fileName, "rb");
	if (pFile)
	{
		setvbuf(pFile, NULL, _IOFBF, 0x1000);
		fread(&temp, sizeof(int), 1, pFile);
		fread(&i, sizeof(int), 1, pFile);

		if (temp == SAVEFILE_HEADER)
		{
			fread(&decalCount, sizeof(int), 1, pFile);

			for (i = 0; i < decalCount; i++)
			{
				fread(name, sizeof(char), 16, pFile);
				fread(&decalList.entityIndex, sizeof(short), 1, pFile);
				fread(&decalList.depth, sizeof(byte), 1, pFile);
				fread(&decalList.flags, sizeof(byte), 1, pFile);
				fread(decalList.position, sizeof(vec3_t), 1, pFile);

				if (r_decals.value)
				{
					temp = Draw_DecalIndexFromName(name);

					// Spawn decals
					R_DecalShoot(Draw_DecalIndex(temp), decalList.entityIndex, 0, decalList.position, decalList.flags);
				}
			}
		}

		fclose(pFile);
	}

	mapCount = MSG_ReadByte();

	for (i = 0; i < mapCount; i++)
	{
		pMapName = MSG_ReadString();

		// JAY UNDONE:  Actually load decals that transferred through the transition!!!
		Con_Printf("Loading decals from %s\n", pMapName);
	}
}

void CL_PlayerDropped( int nPlayerNumber )
{
	COM_ClearCustomizationList(&cl.players[nPlayerNumber].customdata, TRUE);
}

int total_data[MAX_DATA_HISTORY];

/*
=================
CL_DumpMessageLoad_f
=================
*/
void CL_DumpMessageLoad_f( void )
{
	int		i, total;

	total = 0;

	Con_Printf("-------- Message Load ---------\n");

	for (i = 0; i < MAX_DATA_HISTORY - 1; i++)
	{
		if (i > svc_lastmsg)
		{
			Con_Printf("%i:%s: %i msgs:%.2fK\n", i, "bogus #", msg_buckets[i], total_data[i] / 1024.0);
		}
		else
		{
			Con_Printf("%i:%s: %i msgs:%.2fK\n", i, svc_strings[i], msg_buckets[i], total_data[i] / 1024.0);
		}

		total += msg_buckets[i];
	}

	Con_Printf("User messages:  %i:%.2fK\n", msg_buckets[MAX_DATA_HISTORY - 1], total_data[MAX_DATA_HISTORY - 1] / 1024.0);
	Con_Printf("------ End:  %i Total----\n", msg_buckets[MAX_DATA_HISTORY - 1] + total);
}

/*
=================
CL_BitCounts_f
=================
*/
void CL_BitCounts_f( void )
{
	int		i, bits;

	bits = 0;

	Con_Printf("------- Bit Counts -------\n");
	Con_Printf("Bit    Delta   Player  Custom\n");

	for (i = 0; i < (32 + 8); i++)
	{
		if (i >= 32)
		{
			Con_Printf("(1<<%2i) %6.6i\n", bits, bitcounts[i]);
		}
		else
		{
			Con_Printf("(1<<%2i) %6.6i  %6.6i  %6.6i\n", bits, bitcounts[i], playerbitcounts[i], custombitcounts[i]);
		}
		bits++;
	}

	Con_Printf("--------------------------\n");
}

/*
=================
CL_TransferMessageData
=================
*/
void CL_TransferMessageData( void )
{
	int i;

	for (i = 0; i < MAX_DATA_HISTORY; i++)
	{
		total_data[i] += last_data[i];
	}
}

int data_history[UPDATE_BACKUP][MAX_DATA_HISTORY];
int history_index = 0;

/*
=================
CL_ShowSizes

=================
*/
void CL_ShowSizes( void )
{
	int		i, j;
	int		peak;
	int		x, offset;
	float	flScale;
	float	flMax, flNormalized;
	vrect_t rcFill;
	byte	color[3];

	if (!cl_showsizes.value)
		return;

	memcpy(data_history[history_index & UPDATE_MASK], last_data, sizeof(last_data));
	history_index++;

	x = scr_vrect.x;

	color[0] = 200;
	color[1] = 150;
	color[2] = 63;

	rcFill.x = x;
	rcFill.y = scr_vrect.y;
	rcFill.width = 256;
	rcFill.height = 64;
	SCR_DrawOutlineRect(&rcFill, color);

	// draw scale markers
	for (i = 0; i < MAX_DATA_HISTORY; i++)
	{
		if ((i % 5) == 0)
		{
			if ((i % 10) == 0)
			{
				color[0] = 255;
				color[1] = 200;
				color[2] = 127;
			}
			else
			{
				color[0] = 200;
				color[1] = 150;
				color[2] = 63;
			}

			rcFill.x = x + (i * 4) + 1;
			rcFill.y = scr_vrect.y + 64;
			rcFill.width = 2;
			rcFill.height = 2;
			D_FillRect(&rcFill, color);
		}
	}

	// draw the actual data graph
	for (i = 0; i < MAX_DATA_HISTORY; i++)
	{
		peak = 0;
		for (j = 0; j < UPDATE_BACKUP; j++)
		{
			if (peak < data_history[(history_index - j - 1) & UPDATE_MASK][i])
				peak = data_history[(history_index - j - 1) & UPDATE_MASK][i];
		}
		flMax = 0.0f;
		for (j = 0; j < 128; j++)
		{
			if (flMax < data_history[(history_index - j - 1) & UPDATE_MASK][i])
				flMax = data_history[(history_index - j - 1) & UPDATE_MASK][i];
		}

		if (flMax != 0.0)
		{
			// draw bar showing extended history maximum
			flNormalized = flMax / 255.0;
			flScale = flNormalized;
			if (flScale > 1.0)
				flScale = (511.0 - flMax) / 255.0;

			if (flNormalized > 1.0 && flScale > 1.0)
			{
				color[0] = 255;
				color[1] = 63 + (int)(flScale * 192.0 + 0.5);
				color[2] = 0;
			}
			else
			{
				color[0] = 0;
				color[1] = 63 + (int)(flScale * 192.0 + 0.5);
				color[2] = 0;
			}

			if (flNormalized > 1.0)
				flNormalized = 1.0;

			offset = (flNormalized * 64.0) + 0.5;
			rcFill.x = x;
			rcFill.y = scr_vrect.y + 64 - offset;
			rcFill.width = 3;
			rcFill.height = offset;
			D_FillRect(&rcFill, color);

			// draw marker showing recent history peak
			flNormalized = (float)peak / 255.0;
			flScale = flNormalized;
			if (flNormalized > 1.0)
				flScale = (511.0 - (float)peak) / 255.0;

			if (flNormalized > 1.0 && flScale > 1.0)
			{
				color[0] = 255;
				color[1] = 0;
				color[2] = 63 + (int)(flScale * 192.0 + 0.5);
			}
			else
			{
				color[0] = 0;
				color[1] = 0;
				color[2] = 63 + (int)(flScale * 192.0 + 0.5);
			}

			if (flNormalized > 1.0)
				flNormalized = 1.0;

			offset = (flNormalized * 64.0) + 0.5;
			rcFill.x = x;
			rcFill.y = scr_vrect.y + 64 - offset;
			rcFill.width = 3;
			rcFill.height = 2;
			D_FillRect(&rcFill, color);
		}
		x += 4;
	}
}

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
	static int lastcmds[3];
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

		lastcmds[0] = lastcmds[1];
		lastcmds[1] = lastcmds[2];
		lastcmds[2] = cmd;

		if (cmd <= 63)
			msg_buckets[cmd]++;

	// other commands
		switch (cmd)
		{
		default:
			Con_DPrintf("Last 3 messages parsed.\n");
			Con_DPrintf("%s\n", svc_strings[lastcmds[0]]);
			Con_DPrintf("%s\n", svc_strings[lastcmds[1]]);
			Con_DPrintf("%s\n", svc_strings[lastcmds[2]]);
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
			S_StopSound(i >> 3, i & 7);
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

		case svc_spawnstatic:
			CL_ParseStatic();
			break;

		case svc_spawnbaseline:
			i = MSG_ReadShort();
			// must use CL_EntityNum() to force cl.num_entities up
			CL_ParseBaseline(CL_EntityNum(i));
			break;

		case svc_tempentity:
			CL_ParseTEnt();
			break;

		case svc_setpause:
			cl.paused = MSG_ReadByte();
			if (cl.paused)
				CDAudio_Pause();
			else
				CDAudio_Resume();
			break;

		case svc_signonnum:
			i = MSG_ReadByte();
			if (i <= cls.signon)
				Host_Error("Received signon %i when at %i", i, cls.signon);
			cls.signon = i;
			CL_SignonReply();
			break;

		case svc_centerprint:
			SCR_CenterPrint(MSG_ReadString());
			break;

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

		case svc_restore:
			CL_Restore(MSG_ReadString());
			break;
		
		case svc_weaponanim:
			cl.weaponstarttime = 0.0f;
			cl.weaponsequence = MSG_ReadByte();
			cl.viewent.baseline.body = MSG_ReadByte();
			break;

		case svc_decalname:
			i = MSG_ReadByte();
			Draw_DecalSetName(i, MSG_ReadString());
			break;

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
			CL_WriteDemoHeader(cls.dl.szFileName);
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

		case svc_nextupload:
			CL_ParseNextUpload();
			break;

		case svc_resourcerequest:
			CL_SendResourceListBlock();
			break;

		case svc_customization:
			CL_WriteDemoHeader(cls.dl.szFileName);
			CL_ParseCustomization();
			break;

		case svc_crosshairangle:
			cl.crosshairangle[PITCH] = MSG_ReadChar() * 0.2;
			cl.crosshairangle[YAW] = MSG_ReadChar() * 0.2;
			break;

		case svc_soundfade:
			CL_ParseSoundFade();
			break;

		case svc_clientmaxspeed:
			i = MSG_ReadByte();
			if (i >= cl.maxclients)
				Host_Error("CL_ParseServerMessage: svc_clientmaxspeed >= cl.maxclients");

			cl.players[i].maxspeed = MSG_ReadFloat();
			if (cl.players[i].maxspeed > movevars.maxspeed)
				cl.players[i].maxspeed = movevars.maxspeed;
			break;
		}

		// Mark end position
		bufEnd = msg_readcount;
		last_data[cmd] += bufEnd - bufStart;
	}

	// end of message
	SHOWNET("END OF MESSAGE");

	CL_TransferMessageData();

	//
	// we don't know if it is ok to save a demo message until
	// after we have parsed the frame
	//
	if (!cls.demoplayback)
	{
		if (cls.state != ca_active)
			CL_WriteDemoStartup(&net_message);

		if (cls.demorecording && !cls.demowaiting && cls.state == ca_active)
			CL_WriteDemoMessage(&net_message);
	}

	CL_SetSolidEntities();
}