#include "quakedef.h"
#include "server.h"
#include "decal.h"
#include "hashpak.h"

cvar_t sv_uploadinterval = { "sv_uploadinterval", "1.0" };

/*
==================
SV_CheckOrUploadFile

Checks if a file exists locally or initiates client-side upload
==================
*/
qboolean SV_CheckOrUploadFile( char* filename )
{
	byte	buffer[1024];
	int		i, size;
	CRC32_t	crc;
	resource_t	p;
	qboolean isMD5;
	qboolean hasRemainingFileSegments;
	FILE* pFile, * pHashFile;
	char* s;
	char	name[MAX_QPATH];

	isMD5 = FALSE;

	pFile = NULL;

	memset(&p, 0, sizeof(p));

	if (strstr(filename, ".."))
	{
		Con_Printf("Refusing to upload a path with '..'\n");
		return TRUE;
	}

	sprintf(name, filename);

	// Handle hashed resources
	if (strlen(filename) == 36 &&
		!_strnicmp(filename, "!MD5", 4))
	{
		isMD5 = TRUE;

		// MD5 signature is correct, lets try to find this resource locally
		COM_HexConvert(filename + 4, 32, p.rgucMD5_hash);

		// See if it's already in HPAK
		if (HPAK_GetDataPointer(HASHPAK_FILENAME, &p, &pHashFile))
		{
			fclose(pHashFile);
			return TRUE;
		}

		sprintf(host_client->uploadfn, "cust%02i.dat ", (int)host_client->pViewEntity);
	}
	else
	{
		// Non custom upload
		size = COM_FindFile(name, NULL, &pFile);
		if (size != -1)
		{
			if (pFile)
				fclose(pFile);

			return TRUE;
		}

		strcpy(host_client->uploadfn, name);
	}

	COM_StripExtension(host_client->uploadfn, host_client->uploadfntmp);

	if (isMD5)
	{
		char szExt[8];
		sprintf(szExt, ".t%02i", (int)host_client->pViewEntity);
		strcat(host_client->uploadfntmp, szExt);
	}
	else
	{
		strcat(host_client->uploadfntmp, ".tmp");
	}

	host_client->uploadfinalCRC = 0;

	if (isMD5)
	{
		size = -1;
	}
	else
	{
		size = COM_FindFile(host_client->uploadfntmp, NULL, &pFile);
	}

	if (size != -1)
	{
		hasRemainingFileSegments = TRUE;

		CRC32_Init(&crc);
		for (i = 0; i < size / 1024; i++)
		{
			if (fread(buffer, sizeof(buffer), 1, pFile) != 1)
			{
				hasRemainingFileSegments = FALSE;
				break;
			}

			CRC32_ProcessBuffer(&crc, buffer, sizeof(buffer));
		}
		crc = CRC32_Final(crc);
		host_client->uploadfinalCRC = crc;

		if (pFile)
			fclose(pFile);
	}

	MSG_WriteByte(&host_client->netchan.message, svc_stufftext);

	if (isMD5)
	{
		s = va("upload \"!MD5%s\"\n", MD5_Print(p.rgucMD5_hash));
	}
	else if (hasRemainingFileSegments && (size % 1024) == 0)
	{
		s = va("upload %s %i %i\n", name, size / 1024, crc); // partial upload with CRC check
	}
	else
	{
		s = va("upload %s\n", name); // full upload required
	}

	MSG_WriteString(&host_client->netchan.message, s);
	host_client->uploadinprogress = TRUE;

	return FALSE;
}

/*
==================
SV_UpdateUploadCount

==================
*/
void SV_UpdateUploadCount( void )
{
	downloadtime_t* pStats;

	if (!sv_uploadinterval.value)
		return;

	if (sv_uploadinterval.value < 0.0)
		Cvar_SetValue("sv_uploadinterval", 1.0);

	if ((realtime - host_client->fLastUploadTime) < sv_uploadinterval.value)
		return;

	host_client->fLastUploadTime = realtime;

	pStats = &host_client->rgUploads[host_client->nCurUpload & (MAX_DL_STATS - 1)];
	host_client->nCurUpload++;

	pStats->bUsed = TRUE;
	pStats->fTime = realtime;
	pStats->nBytesRemaining = host_client->nRemainingToTransfer;
}

/*
==================
SV_ParseUpload

Handles incoming file upload data from client
==================
*/
void SV_ParseUpload( void )
{
	int		percent;
	int		status;
	int		active;
	int		size;
	char	name[MAX_OSPATH];
	char	fullPath[MAX_OSPATH];
	char	finalPath[MAX_OSPATH];

	size = MSG_ReadShort();
	active = MSG_ReadShort();
	status = MSG_ReadLong();
	percent = MSG_ReadByte();

	host_client->uploadcount = percent;

	COM_FileBase(host_client->uploadfn, name);

	if (size == -1)
	{
		if (status == -1)
			Con_Printf("Client refused upload %s.\n", name);
		else
			Con_Printf("File %s not on client.\n", name);

		// Clean up if upload was unexpectedly active
		if (host_client->upload)
		{
			Con_Printf("Error:  host_client->upload shouldn't have been set.\n");
			fclose(host_client->upload);
			host_client->upload = NULL;
		}

		host_client->uploadinprogress = FALSE;
		host_client->uploadresource->ucFlags |= RES_WASMISSING;
		SV_MoveToOnHandList(host_client->uploadresource);
		return;
	}

	if (!host_client->upload)
	{
		sprintf(fullPath, "%s/%s", com_gamedir, host_client->uploadfntmp);

		COM_CreatePath(fullPath);

		if (active)
		{
			// Append mode for active transfers
			host_client->upload = fopen(fullPath, "a+b");
			host_client->uploadcurrentCRC = host_client->uploadfinalCRC;
		}
		else
		{
			// New file
			host_client->upload = fopen(fullPath, "wb");
			CRC32_Init(&host_client->uploadcurrentCRC);
		}

		if (!host_client->upload)
		{
			msg_readcount += size;
			Con_Printf("Failed to open %s\n", host_client->uploadfntmp);
			host_client->uploadinprogress = FALSE;
			return;
		}

		Con_Printf("uploading %s\n", name);
	}

	CRC32_ProcessBuffer(&host_client->uploadcurrentCRC, net_message.data + msg_readcount, size);
	fwrite(net_message.data + msg_readcount, 1, size, host_client->upload);
	msg_readcount += size;
	host_client->nRemainingToTransfer -= size;

	// Update upload stats
	SV_UpdateUploadCount();

	if (percent != 100)
	{
		// Update progress bar
		scr_downloading.value = percent;
		MSG_WriteByte(&host_client->netchan.message, svc_nextupload);
	}
	else
	{
		Con_Printf("100%%\n");
		scr_downloading.value = -1; // Reset progress bar

		fclose(host_client->upload);

		sprintf(finalPath, "%s/%s", com_gamedir, host_client->uploadfntmp);

		// Verify and add to HPAK
		host_client->upload = fopen(finalPath, "rb");
		setvbuf(host_client->upload, NULL, _IOFBF, 4096); // 4K buffer
		if (host_client->upload)
		{
			customization_t* pCust, * pList;
			qboolean bFound = FALSE;

			// Add to HPAK
			HPAK_AddLump(HASHPAK_FILENAME, host_client->uploadresource, NULL, host_client->upload);
			fseek(host_client->upload, 0, SEEK_SET);
			host_client->uploadresource->ucFlags &= ~RES_WASMISSING;

			pCust = (customization_t*)malloc(sizeof(customization_t));
			memset(pCust, 0, sizeof(customization_t));

			pCust->bInUse = TRUE;
			memcpy(&pCust->resource, host_client->uploadresource, sizeof(pCust->resource));

			pCust->pBuffer = malloc(host_client->uploadresource->nDownloadSize);
			fread(pCust->pBuffer, host_client->uploadresource->nDownloadSize, 1, host_client->upload);

			// Search if this resource is already in customizations list
			pList = host_client->customdata.pNext;
			while (pList)
			{
				if (memcmp(pList->resource.rgucMD5_hash, pCust->resource.rgucMD5_hash, sizeof(pList->resource.rgucMD5_hash)) == 0)
				{
					bFound = TRUE;
					break;
				}

				pList = pList->pNext;
			}

			if (bFound)
			{
				Con_DPrintf("Duplicate resource received and ignored.\n");
				free(pCust);
			}
			else
			{
				pCust->pNext = host_client->customdata.pNext;
				host_client->customdata.pNext = pCust;
			}

			fclose(host_client->upload);
		}

		// Clean up temp file
		_unlink(finalPath);

		// Reset upload state
		host_client->upload = NULL;
		host_client->uploadcount = 0;
		host_client->uploadcurrentCRC = 0;
		host_client->uploadinprogress = FALSE;
	}
}

/*
==================
SV_PrintResource

==================
*/
void SV_PrintResource( int index, resource_t* pResource )
{
	static char type[12];
	static char fatal[8];

	sprintf(fatal, "N");

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

	Con_Printf("%3i %i %s:%15s %i %s\n", index, pResource->nDownloadSize, type, pResource->szFileName, pResource->nIndex, fatal);
}

/*
==================
SV_PrintResourceLists_f

==================
*/
void SV_PrintResourceLists_f( void )
{
	int		i;
	resource_t* pResource;

	i = 1;

	Con_Printf("- Needed -------------------------------------------\n");
	Con_Printf("#   Name                  Size Type Index Fatal\n");

	for (pResource = host_client->resourcesneeded.pNext; pResource != &host_client->resourcesneeded; pResource = pResource->pNext)
	{
		SV_PrintResource(i, pResource);
		i++;
	}

	Con_Printf("- On hand ------------------------------------------\n");
	Con_Printf("#   Name                  Size Type Index Fatal\n");

	for (pResource = host_client->resourcesonhand.pNext; pResource != &host_client->resourcesonhand; pResource = pResource->pNext)
	{
		SV_PrintResource(i, pResource);
		i++;
	}

	Con_Printf("--------------------------------------------\n\n");
}

/*
==================
SV_ClearResourceLists

==================
*/
void SV_ClearResourceLists( client_t* cl )
{
	if (!cl)
		Sys_Error("SV_ClearResourceLists with NULL client!");

	SV_ClearResourceList(&cl->resourcesneeded);
	SV_ClearResourceList(&cl->resourcesonhand);
}

/*
==================
SV_PrintCusomizations_f

==================
*/
void SV_PrintCusomizations_f( void )
{
	int		i;
	int		nIndex;
	client_t* cl;
	customization_t* pCust;

	if (cmd_source == src_command && !sv.active)
	{
		Cmd_ForwardToServer();
		return;
	}

	if (!NET_IsLocalAddress(net_from))
		Host_BeginRedirect(RD_CLIENT, &net_from);

	for (i = 0, cl = svs.clients; i < svs.maxclients; i++, cl++)
	{
		if (!cl->active && !cl->spawned)
			continue;
		if (!cl->customdata.pNext)
			continue;

		nIndex = 1;
		Con_DPrintf("SV Customizations:\nPlayer %i:%s\n", nIndex, cl->name);

		for (pCust = cl->customdata.pNext; pCust; pCust = pCust->pNext)
		{
			if (pCust->bInUse)
			{
				SV_PrintResource(nIndex, &pCust->resource);
				nIndex++;
			}
		}

		Con_DPrintf("-----------------\n\n");
	}

	if (!NET_IsLocalAddress(net_from))
		Host_EndRedirect();
}

/*
==================
SV_CreateCustomizationList

==================
*/
void SV_CreateCustomizationList( client_t* pHost )
{
	resource_t* pResource;

	pHost->customdata.pNext = NULL;

	for (pResource = pHost->resourcesonhand.pNext; pResource != &pHost->resourcesonhand; pResource = pResource->pNext)
	{
		customization_t* pList;
		qboolean bFound;

		// Search if this resource is already in customizations list
		bFound = FALSE;
		pList = pHost->customdata.pNext;
		while (pList)
		{
			if (memcmp(pList->resource.rgucMD5_hash, pResource->rgucMD5_hash, sizeof(pList->resource.rgucMD5_hash)) == 0)
			{
				bFound = TRUE;
				break;
			}

			pList = pList->pNext;
		}

		if (bFound)
		{
			Con_DPrintf("SV_CreateCustomization list, ignoring dup. resource for player %s\n", pHost->name);
		}
		else
		{
			customization_t* pCust;
			FILE* pFile;
			cachewad_t* pWad;

			pCust = (customization_t*)malloc(sizeof(customization_t));
			memset(pCust, 0, sizeof(customization_t));
			memcpy(&pCust->resource, pResource, sizeof(pCust->resource));

			if (pResource->nDownloadSize != 0)
			{
				pCust->bInUse = TRUE;

				if (HPAK_GetDataPointer(HASHPAK_FILENAME, pResource, &pFile))
				{
					if (pResource->nDownloadSize <= 0)
						Sys_Error("SV_CreateCustomizationList with resource download size <= 0");

					pCust->pBuffer = malloc(pResource->nDownloadSize);
					fread(pCust->pBuffer, pResource->nDownloadSize, 1, pFile);
					fclose(pFile);

					pCust->resource.playernum = cl.playernum;

					pWad = (cachewad_t*)malloc(sizeof(cachewad_t));
					pCust->pInfo = pWad;
					memset(pWad, 0, sizeof(cachewad_t));
					CustomDecal_Init(pWad, pCust->pBuffer, pResource->nDownloadSize);

					pCust->bTranslated = FALSE;
					pCust->nUserData1 = 0;
					pCust->nUserData2 = pWad->lumpCount;

					free(pWad->cache);
					free(pWad->lumps);
					free(pCust->pInfo);
					pCust->pInfo = NULL;
				}
			}

			pCust->pNext = pHost->customdata.pNext;
			pHost->customdata.pNext = pCust;

			gEntityInterface.pfnPlayerCustomization(pHost->edict, pCust);
		}
	}
}

/*
==================
SV_RegisterResources

Creates customizations list for the current player and sends resources to other players
==================
*/
void SV_RegisterResources( void )
{
	resource_t* pResource;
	client_t* pHost;

	pHost = host_client;
	pHost->uploading = FALSE;

	for (pResource = pHost->resourcesonhand.pNext; pResource != &pHost->resourcesonhand; pResource = pResource->pNext)
	{
		SV_CreateCustomizationList(pHost);
		SV_Customization(pHost, pResource, TRUE);
	}

	host_client = pHost;
}

/*
==================
SV_MoveToOnHandList

==================
*/
void SV_MoveToOnHandList( resource_t* pResource )
{
	if (!pResource)
	{
		Con_DPrintf("Null resource passed to SV_MoveToOnHandList\n");
		return;
	}

	if (pResource->type >= rt_max)
		Con_DPrintf("Unknown resource type\n");

	SV_RemoveFromResourceList(pResource);
	SV_AddToResourceList(pResource, &host_client->resourcesonhand);
}

/*
===============
SV_SizeofResourceList

===============
*/
int SV_SizeofResourceList( resource_t* pList, int* nWorldSize, int* nModelsSize, int* nDecalsSize, int* nSoundsSize, int* nSkinsSize )
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

/*
==================
SV_AddToResourceList

==================
*/
void SV_AddToResourceList( resource_t* pResource, resource_t* pList )
{
	if (pResource->pPrev || pResource->pNext)
	{
		Con_Printf("Resource already linked\n");
		return;
	}

	pResource->pPrev = pList->pPrev;
	pResource->pNext = pList;
	pList->pPrev->pNext = pResource;
	pList->pPrev = pResource;
}

/*
==================
SV_ClearResourceList

==================
*/
void SV_ClearResourceList( resource_t* pList )
{
	resource_t* p, * n = NULL;

	for (p = pList->pNext; p && p != pList; p = n)
	{
		n = p->pNext;

		SV_RemoveFromResourceList(p);
		free(p);
	}

	pList->pPrev = pList;
	pList->pNext = pList;
}

/*
==================
SV_RemoveFromResourceList

==================
*/
void SV_RemoveFromResourceList( resource_t* pResource )
{
	pResource->pPrev->pNext = pResource->pNext;
	pResource->pNext->pPrev = pResource->pPrev;
	pResource->pPrev = NULL;
	pResource->pNext = NULL;
}

/*
==================
SV_EstimateNeededResources

Returns the size of needed resources to download
==================
*/
int SV_EstimateNeededResources( void )
{
	resource_t* p;
	int		missing;
	int		size;
	int		downloadSize;

	missing = 0;

	for (p = host_client->resourcesneeded.pNext; p != &host_client->resourcesneeded; p = p->pNext)
	{
		size = 0;

		if (p->type == t_decal)
		{
			size = -1;
			if (HPAK_ResourceForHash("custom.hpk", p->rgucMD5_hash, NULL))
				size = p->nDownloadSize;
		}

		if (size == -1)
		{
			p->ucFlags |= RES_WASMISSING;
			downloadSize = p->nDownloadSize;
		}
		else
		{
			downloadSize = 0;
		}

		missing += downloadSize;
	}

	return missing;
}

/*
==================
SV_RequestMissingResourcesFromClients

==================
*/
void SV_RequestMissingResourcesFromClients( void )
{
	int		i;

	for (i = 0, host_client = svs.clients; i < svs.maxclients; i++, host_client++)
	{
		if (!host_client->active && !host_client->spawned)
			continue;

		while (SV_RequestMissingResources());
	}
}

/*
==================
SV_RequestMissingResources

This is used to perform repeated checks on the current player to see
if it has uploaded all the required resources
==================
*/
qboolean SV_RequestMissingResources( void )
{
	char	szCust[MAX_OSPATH];
	resource_t* p;

	if (host_client->upload || host_client->uploadinprogress)
		return FALSE;

	if (!host_client->uploading)
		return FALSE;

	if (host_client->uploaddoneregistering)
		return FALSE;

	p = host_client->resourcesneeded.pNext;
	host_client->uploadresource = p;

	if (p == &host_client->resourcesneeded)
	{
		SV_RegisterResources();
		SV_PropagateCustomizations();
		Con_DPrintf("Custom resource propagation complete.\n");
		host_client->uploaddoneregistering = TRUE;
		return FALSE;
	}

	if (!(p->ucFlags & RES_WASMISSING))
	{
		SV_MoveToOnHandList(p);
		return TRUE;
	}

	if (p->type == t_decal)
	{
		if (p->ucFlags & RES_CUSTOM)
		{
			sprintf(szCust, "!MD5%s", MD5_Print(p->rgucMD5_hash));

			if (SV_CheckOrUploadFile(szCust))
			{
				SV_MoveToOnHandList(p);
				return TRUE;
			}
		}
		else
		{
			if (SV_CheckOrUploadFile(p->szFileName))
			{
				SV_MoveToOnHandList(p);
			}
		}
	}

	return TRUE;
}

/*
================
SV_RequestResourceList_f

Request a resource from client
================
*/
void SV_RequestResourceList_f( void )
{
	int		servercount;
	int		index;

	if (Cmd_Argc() != 3)
	{
		Con_Printf("Invalid resource request\n");
		return;
	}

	servercount = atoi(Cmd_Argv(1));
	if (!cls.demoplayback && servercount != svs.spawncount)
	{
		Con_Printf("Resource request with mismatched servercount\n");
		return;
	}

	index = atoi(Cmd_Argv(2));
	if (index < 0)
	{
		Con_Printf("Resource request with bogus starting index %i\n", index);
		return;
	}

	MSG_WriteByte(&host_client->netchan.message, svc_resourcerequest);
	MSG_WriteLong(&host_client->netchan.message, svs.spawncount);
	MSG_WriteLong(&host_client->netchan.message, index);
}

/*
==================
SV_ParseResourceList
==================
*/
void SV_ParseResourceList( void )
{
	int		i, total;
	int		totalsize;
	int		worldSize, modelsSize, decalsSize, soundsSize, skinsSize;
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

		if (i == 0)
		{
			SV_ClearResourceList(&host_client->resourcesneeded);
			SV_ClearResourceList(&host_client->resourcesonhand);
		}

		// Add new entry in the linked list
		SV_AddToResourceList(resource, &host_client->resourcesneeded);
	}

	if (total < totalsize)
	{
		host_client->uploading = FALSE;
	}
	else
	{
		Con_DPrintf("Verifying and uploading resources...\n");

		host_client->nTotalSize = SV_SizeofResourceList(&host_client->resourcesneeded, &worldSize, &modelsSize, &decalsSize, &soundsSize, &skinsSize);
		host_client->nTotalToTransfer = SV_EstimateNeededResources();

		if (host_client->nTotalSize != 0)
		{
			Con_DPrintf("Custom resources total %.2fK\n", host_client->nTotalSize / 1024.0);

			if (modelsSize != 0)
				Con_DPrintf("  Models:  %.2fK\n", modelsSize / 1024.0);
			if (soundsSize != 0)
				Con_DPrintf("  Sounds:  %.2fK\n", soundsSize / 1024.0);
			if (decalsSize != 0)
				Con_DPrintf("  Decals:  %.2fK\n", decalsSize / 1024.0);
			if (skinsSize != 0)
				Con_DPrintf("  Skins :  %.2fK\n", skinsSize / 1024.0);

			Con_DPrintf("----------------------\n");

			if (host_client->nTotalToTransfer > 1024)
				Con_DPrintf("Resources to request: %iK\n", host_client->nTotalToTransfer / 1024);
			else
				Con_DPrintf("Resources to request: %i bytes\n", host_client->nTotalToTransfer);
		}

		host_client->uploading = TRUE;

		host_client->uploaddoneregistering = FALSE;
		host_client->uploadinprogress = FALSE;

		host_client->fLastStatusUpdate = realtime;
		host_client->fLastUploadTime = realtime;

		host_client->nRemainingToTransfer = host_client->nTotalToTransfer;

		memset(host_client->rgUploads, 0, sizeof(host_client->rgUploads));
		host_client->nCurUpload = 0;
	}
}