#include "quakedef.h"
#include "server.h"
#include "decal.h"
#include "hashpak.h"

// Lookup the comments after SV_ParseResourceList.
// TODO: Implement

cvar_t	sv_uploadinterval = { "sv_uploadinterval", "1.0" };

/*
================
SV_CheckOrUploadFile

================
*/
extern void COM_HexConvert( const char* pszInput, int nInputLength, unsigned char* pOutput );
qboolean SV_CheckOrUploadFile( char* filename )
{
	qboolean	bDoWeHaveOnlyAPartOfTheFile = FALSE;
	qboolean	bIsCustom = FALSE;
	int			nNumChunksRead;
	char*		pszStuffText;
	char		extension[8];
	int			filesize;
	FILE*		f = NULL;
	CRC32_t		crc;
	char		fbuf[MAX_QPATH];
	resource_t	nullres;
	char		buffer[1024];

	memset(&nullres, 0, sizeof(nullres));

	if (strstr(filename, ".."))
	{
		Con_Printf("Refusing to upload a path with '..'\n");
		return 1;
	}

	sprintf(fbuf, filename);

	// If this is an MD5 hash, handle it differently.
	if (strlen(filename) == 36 && !_strnicmp(filename, "!MD5", 4))
	{
		bIsCustom = TRUE;
		COM_HexConvert(filename + 4, 32, nullres.rgucMD5_hash);
		if (HPAK_GetDataPointer("custom.hpk", &nullres, &filesize))
		{
			//Con_DPrintf("SV_CheckOrUploadFile:  file was on hand\n");
			fclose(filesize);
			return TRUE;
		}

		//Con_DPrintf("SV_CheckOrUploadFile:  file was missing\n");
		sprintf(host_client->extension, "cust%02i.dat ", host_client->pViewEntity);
		//Con_DPrintf("SV_CheckOrUploadFile:  upload will be put in %s\n", host_client->extension);
	}
	else
	{
		//Con_DPrintf("Non custom upload!!!!\n");
		// check if the file exists on our side
		if (COM_FindFile(fbuf, NULL, &f) != -1)
		{
			if (f)
				fclose(f);

			// the file exists, return TRUE
			return TRUE;
		}

		// if it doesn't, save the name
		strcpy(host_client->extension, fbuf);
	}

	COM_StripExtension(host_client->extension, host_client->uploadfn);

	if (bIsCustom)
	{
		sprintf(extension, ".t%02i", host_client->pViewEntity);
		memcpy(&host_client->uploadfn[strlen(host_client->uploadfn)], extension, strlen(extension) + 1);
	}
	else
	{
		strcat(host_client->uploadfn, ".tmp");
	}

	// reset the crc
	host_client->uploadCRC = 0;

	if (bIsCustom)
	{
		filesize = -1;
	}
	else
	{
		filesize = COM_FindFile(host_client->uploadfn, NULL, &f);
	}

	if (filesize != -1) {
		bDoWeHaveOnlyAPartOfTheFile = TRUE;
		nNumChunksRead = 0;
		CRC32_Init(&crc);
		if ((filesize / 1024) > 0)
		{
			while (TRUE)
			{
				// let's see if we can successfully read the first 1024 bytes chunk
				if (fread(buffer, sizeof(buffer), sizeof(byte), f) == 1)
				{
					// we'll try reading another 1024 bytes chunk, but the file may run out of bytes, so we need to keep an eye on that
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

		host_client->uploadCRC = crc = CRC32_Final(crc);
		if (f)
		{
			fclose(f);
		}
	}

	MSG_WriteByte(&host_client->netchan.message, svc_stufftext);
	if (bIsCustom)
	{
		pszStuffText = va("upload \"!MD5%s\"\n", MD5_Print(nullres.rgucMD5_hash));
	}
	else if (!bDoWeHaveOnlyAPartOfTheFile || filesize % 1024)
	{
		pszStuffText = va("upload %s\n", buffer);
	}
	else
	{
		pszStuffText = va("upload %s %i %i\n", buffer, filesize / 1024, crc);
	}
	MSG_WriteString(&host_client->netchan.message, pszStuffText);

	host_client->isuploading = TRUE;

	// the file doesn't exist, initiate an upload
	return FALSE;
}

/*
================
SV_UpdateUploadCount

Rate-limits the client if they're spamming with upload, as well as increments the uploads count.
================
*/
void SV_UpdateUploadCount( void )
{
	downloadtime_t*		pStat;

	if (sv_uploadinterval.value)
	{
		if (sv_uploadinterval.value < 0)
			Cvar_SetValue("sv_uploadinterval", 1);

		if (realtime - host_client->lastuploadtime >= sv_uploadinterval.value)
		{
			host_client->lastuploadtime = realtime;
			host_client->numuploads++;

			pStat = &host_client->uploads[host_client->numuploads & (MAX_DL_STATS - 1)]; // Mask the numuploads so we don't go over MAX_DL_STATS (like "cap")
			pStat->bUsed = TRUE;
			pStat->fTime = realtime;
			pStat->nBytesRemaining = host_client->uploadsize;
		}
	}
}

/*
================
SV_NextUpload
================
*/
extern cvar_t		scr_downloading;
void SV_ParseUpload( void )
{
	int					size, append, reason, percent;
	customization_t*	pLocal;
	customization_t*	pCust;
	qboolean			bDuplicate = FALSE;
	char				file[MAX_OSPATH];
	char				buffer[MAX_OSPATH];
	char				filename[MAX_OSPATH];

	size = MSG_ReadShort();
	append = MSG_ReadShort();
	reason = MSG_ReadLong();
	percent = MSG_ReadByte();

	host_client->uploadcount = percent;
	COM_FileBase(host_client->extension, filename);

	if (size == -1)
	{
		if (reason == -1)
			Con_Printf("Client refused upload %s.\n", filename);
		else
			Con_Printf("File %s not on client.\n", filename);

		if (host_client->upload)
		{
			Con_Printf("Error:  host_client->upload shouldn't have been set.\n");
			fclose(host_client->upload);
			host_client->upload = NULL;
		}

		host_client->isuploading = FALSE;
		host_client->uploadingresource->ucFlags |= RES_WASMISSING;
		SV_MoveToOnHandList(host_client->uploadingresource);
	}
	else
	{
		if (!host_client->upload)
		{
			sprintf(buffer, "%s/%s", com_gamedir, host_client->uploadfn);
			COM_CreatePath(buffer);

			if (append)
			{
				host_client->upload = fopen(buffer, "a+b");
				host_client->tempuploadCRC = host_client->uploadCRC;
			}
			else
			{
				host_client->upload = fopen(buffer, "wb");
				CRC32_Init(&host_client->tempuploadCRC);
			}

			if (!host_client->upload)
			{
				// suck out rest of packet
				msg_readcount += size;

				Con_Printf("Failed to open %s\n", host_client->uploadfn);
				host_client->isuploading = FALSE;

				return;
			}

			Con_Printf("uploading %s\n", filename);
		}

		CRC32_ProcessBuffer(&host_client->tempuploadCRC, &net_message.data[msg_readcount], size);
		fwrite(&net_message.data[msg_readcount], sizeof(byte), size, host_client->upload);

		msg_readcount += size;
		host_client->uploadsize -= size;

		SV_UpdateUploadCount();

		// woohoo finished
		if (percent == 100)
		{
			Con_Printf("100%%\n");

			scr_downloading.value = -1;

			fclose(host_client->upload);

			sprintf(file, "%s/%s", com_gamedir, host_client->uploadfn);
			host_client->upload = fopen(file, "rb");
			setvbuf(host_client->upload, NULL, _IOFBF, 4096);

			if (host_client->upload)
			{
				HPAK_AddLump("custom.hpk", host_client->uploadingresource, NULL, host_client->upload);

				fseek(host_client->upload, 0, SEEK_SET);

				host_client->uploadingresource->ucFlags &= ~RES_WASMISSING;

				pLocal = (customization_t *)malloc(sizeof(customization_t));
				memset(pLocal, 0, sizeof(*pLocal));

				pLocal->bInUse = TRUE;

				memcpy(&pLocal->resource, host_client->uploadingresource, sizeof(pLocal->resource));

				pLocal->pBuffer = malloc(host_client->uploadingresource->nDownloadSize);
				fread(pLocal->pBuffer, host_client->uploadingresource->nDownloadSize, sizeof(byte), host_client->upload);

				pCust = host_client->customdata.pNext;
				if (pCust)
				{
					while (TRUE)
					{
						if (memcmp(pCust->resource.rgucMD5_hash, pLocal->resource.rgucMD5_hash, sizeof(pLocal->resource.rgucMD5_hash)))
						{
							pCust = pCust->pNext;
							if (!pCust)
							{
								break;
							}
						}
						else
						{
							bDuplicate = TRUE;
							break;
						}
					}
				}

				if (bDuplicate)
				{
					Con_DPrintf("Duplicate resource received and ignored.\n");
					free(pLocal);
				}
				else
				{
					pLocal->pNext = host_client->customdata.pNext;
					host_client->customdata.pNext = pLocal;
				}

				fclose(host_client->upload);
			}

			// Erase the file
			_unlink(file);

			// Reset stuff
			host_client->upload = NULL;
			host_client->uploadcount = 0;
			host_client->tempuploadCRC = 0;
			host_client->isuploading = FALSE;
		}
		else
		{
			scr_downloading.value = (float)percent;
			// Tell the client we're still listening
			MSG_WriteByte(&host_client->netchan.message, 47); // TODO: Find out the message number
		}
	}
}

/*
================
SV_PrintResource

Prints which data the resource passed contains.
================
*/
void SV_PrintResource( int index, resource_t* pResource )
{
	static char fatal[8];
	static char type[12];

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

	Con_Printf(
	  "%3i %i %s:%15s %i %s\n",
	  index,
	  pResource->nDownloadSize,
	  type,
	  pResource->szFileName,
	  pResource->nIndex,
	  fatal);
}

/*
================
SV_PrintResourceLists_f

================
*/
void SV_PrintResourceLists_f( void )
{
	int				i;
	resource_t*		res;

	Con_Printf("- Needed -------------------------------------------\n");
	Con_Printf("#   Name                  Size Type Index Fatal\n");
	for (i = 1, res = host_client->resourcesneeded.pNext; res != &host_client->resourcesneeded; res = res->pNext, i++)
	{
		SV_PrintResource(i, res);
	}
	Con_Printf("- On hand ------------------------------------------\n");
	Con_Printf("#   Name                  Size Type Index Fatal\n");
	for (res = host_client->resourcesonhand.pNext; res != &host_client->resourcesonhand; res = res->pNext, i++)
	{
		SV_PrintResource(i, res);
	}
	Con_Printf("--------------------------------------------\n\n");
}

/*
==================
SV_ClearResourceLists

Clears the resource lists of the client specified.
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
	int					i;
	client_t*			cl;
	int					j;
	customization_t*	pCust;

	if (cmd_source == src_command && !sv.active)
	{
		Cmd_ForwardToServer();
		return;
	}

	// Redirect the output so we print the stuff to the client's console, not ours.
	if (!NET_IsLocalAddress(net_from))
		Host_BeginRedirect(RD_CLIENT, &net_from);

	for (i = 0, cl = svs.clients; i < svs.maxclients; ++cl)
	{
		if ((cl->active || cl->spawned) && cl->customdata.pNext)
		{
			Con_DPrintf("SV Customizations:\nPlayer %i:%s\n", i + 1, cl->name);
			for (j = 1, pCust = cl->customdata.pNext; pCust; pCust = pCust->pNext, j++)
			{
				if (pCust->bInUse)
				{
					SV_PrintResource(j, &pCust->resource);
				}
			}
			Con_DPrintf("-----------------\n\n");
		}
	}

	// End redirecting in case we were doing that.
	if (!NET_IsLocalAddress(net_from))
		Host_EndRedirect();
}

/*
==================
SV_CreateCustomizationList

Reinitializes customizations list. Tries to create customization for each resource in on-hands list.
==================
*/
void SV_CreateCustomizationList( client_t* pHost )
{
	// TODO: Implement
}

/*
==================
SV_RegisterResources

Creates customizations list for the current player and sends resources to other players.
==================
*/
void SV_RegisterResources( void )
{
	client_t* pHost = host_client;
	resource_t* pResource;

	pHost->uploading = FALSE;
	for (pResource = pHost->resourcesonhand.pNext; pResource != &pHost->resourcesonhand; pResource = pResource->pNext)
	{
		SV_CreateCustomizationList(pHost);
		SV_Customization(pHost, pResource, TRUE);
	}
}

/*
==================
SV_MoveToOnHandList

Adds pResource to the resourcesonhand in host_client
==================
*/
void SV_MoveToOnHandList( resource_t* pResource )
{
	if (pResource)
	{
		if (pResource->type >= rt_max)
			Con_DPrintf("Unknown resource type\n");

		SV_RemoveFromResourceList(pResource);
		SV_AddToResourceList(pResource, &host_client->resourcesonhand);
	}
	else
	{
		Con_DPrintf("Null resource passed to SV_MoveToOnHandList\n");
	}
}

/*
==================
SV_SizeofResourceList

Counts total size of the resources inside pList.
==================
*/
int SV_SizeofResourceList( resource_t* pList, int* nWorldSize, int* nModelsSize, int* nDecalsSize, int* nSoundsSize, int* nSkinsSize )
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

/*
==================
SV_AddToResourceList

Adds pResource into the pList linked list.
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
	resource_t* p, *n;

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

For each t_decal and RES_CUSTOM resource the player had shown to us, tries to find it locally or count size required to be downloaded.
==================
*/
int SV_EstimateNeededResources( void )
{
	resource_t* p;
	int			nTotalSize = 0;
	int			nSize, nDownloadSize;

	for (p = host_client->resourcesneeded.pNext, nTotalSize = 0; p != &host_client->resourcesneeded; p = p->pNext)
	{
		nSize = 0;

		if (p->type == t_decal)
		{
			nSize = -1;
			if (HPAK_ResourceForHash("custom.hpk", p->rgucMD5_hash, NULL))
				nSize = p->nDownloadSize;
		}

		if (nSize == -1)
		{
			p->ucFlags |= RES_CUSTOM;
			nDownloadSize = p->nDownloadSize;
		}
		else
		{
			nDownloadSize = 0;
		}

		nTotalSize += nDownloadSize;
	}

	return nTotalSize;
}

/*
==================
SV_RequestMissingResourcesFromClients

This is called each frame to do checks on players if they uploaded all files that where requested from them.
==================
*/
void SV_RequestMissingResourcesFromClients( void )
{
	int i;

	for (i = 0, host_client = svs.clients; i < svs.maxclients; host_client++, i++)
	{
		if (host_client->active || host_client->spawned)
		{
			while (SV_RequestMissingResources());
		}
	}
}

/*
==================
SV_RequestMissingResources

This is called each frame to do checks on players if they uploaded all files that where requested from them.
==================
*/
qboolean SV_RequestMissingResources( void )
{
	resource_t*		res;
	char			buffer[MAX_OSPATH];

	// If the client is uploading a resource right now, wait for the them to finish uploading.
	if (host_client->upload || host_client->isuploading)
		return FALSE;
	if (!host_client->uploading)
		return FALSE;

	// If the client has finished uploading all the needed resources, just return FALSE.
	if (host_client->uploaddoneregistering)
		return FALSE;

	res = host_client->resourcesneeded.pNext;
	host_client->uploadingresource = res;

	if (res == &host_client->resourcesneeded) {
		SV_RegisterResources();
		SV_PropagateCustomizations();
		Con_DPrintf("Custom resource propagation complete.\n");
		host_client->uploaddoneregistering = TRUE;
		return FALSE;
	}

	// If the resource isn't missing on our side, just add it into the list of resources on hand.
	if (!(res->ucFlags & RES_WASMISSING))
	{
		SV_MoveToOnHandList(res);
		return 1;
	}

	if (res->type == t_decal)
	{
		if (res->ucFlags & RES_CUSTOM)
		{
			sprintf(buffer, "!MD5%s", MD5_Print(res->rgucMD5_hash));
			if (SV_CheckOrUploadFile(buffer))
			{
				SV_MoveToOnHandList(res);
				return TRUE;
			}
		}
		else if (SV_CheckOrUploadFile(res->szFileName))
		{
			SV_MoveToOnHandList(res);
		}
	}

	return TRUE;
}

/*
================
SV_RequestResourceList_f

Request resource with i index from host_client.
================
*/
void SV_RequestResourceList_f( void )
{
	char*	s;
	int		i;

	if (Cmd_Argc() == 3)
	{
		s = Cmd_Argv(1);
		i = atoi(s);
		if (cls.demoplayback || i == svs.spawncount)
		{
			s = Cmd_Argv(2);
			i = atoi(s);
			if (i >= 0)
			{
				MSG_WriteByte(&host_client->netchan.message, svc_resourcerequest);
				MSG_WriteLong(&host_client->netchan.message, svs.spawncount);
				MSG_WriteLong(&host_client->netchan.message, i);
			}
			else
			{
				Con_Printf("Resource request with bogus starting index %i\n", i);
			}
		}
		else
		{
			Con_Printf("Resource request with mismatched servercount\n");
		}
	}
	else
	{
		Con_Printf("Invalid resource request\n");
	}
}

/*
==================
SV_ParseResourceList
==================
*/
void SV_ParseResourceList( void )
{
	int				total, acknowledged;
	resource_t*		res;
	int				nWorldSize, nModelsSize, nDecalsSize, nSoundsSize, nSkinsSize;

	nWorldSize = MSG_ReadShort();
	total = MSG_ReadShort();
	acknowledged = MSG_ReadShort();

	while (total < acknowledged)
	{
		res = (resource_t*)malloc(sizeof(resource_t));
		memset(res, 0, sizeof(*res));
		res->type = MSG_ReadByte();
		strcpy(res->szFileName, MSG_ReadString());
		res->nIndex = MSG_ReadShort();
		res->nDownloadSize = MSG_ReadLong();
		res->ucFlags = MSG_ReadByte() & (~RES_WASMISSING);
		res->pNext = NULL;
		res->pPrev = NULL;

		if (res->ucFlags & RES_CUSTOM)
		{
			MSG_ReadBuf(sizeof(res->rgucMD5_hash), res->rgucMD5_hash);
		}
		if (total == 0)
		{
			SV_ClearResourceList(&host_client->resourcesneeded);
			SV_ClearResourceList(&host_client->resourcesonhand);
		}

		++total;
		SV_AddToResourceList(res, &host_client->resourcesneeded);
	}
	if (acknowledged < nWorldSize)
	{
		host_client->uploading = FALSE;
	}
	else
	{
		Con_DPrintf("Verifying and uploading resources...\n");

		host_client->nResourcesTotalSize = SV_SizeofResourceList(&host_client->resourcesneeded, &nWorldSize, &nModelsSize, &nDecalsSize, &nSoundsSize, &nSkinsSize);
		host_client->nRemainingToTransfer = SV_EstimateNeededResources();

		if (host_client->nResourcesTotalSize != 0)
		{
			Con_DPrintf("Custom resources total %.2fK\n", (float)host_client->nResourcesTotalSize / 1024.0f);
			if (nModelsSize > 0)
			{
				Con_DPrintf("  Models:  %.2fK\n", (float)nModelsSize / 1024.f);
			}
			if (nSoundsSize)
			{
				Con_DPrintf("  Sounds:  %.2fK\n", (float)nSoundsSize / 1024.0f);
			}
			if (nDecalsSize)
			{
				Con_DPrintf("  Decals:  %.2fK\n", (float)nDecalsSize / 1024.0f);
			}
			if (nSkinsSize)
			{
				Con_DPrintf("  Skins :  %.2fK\n", (float)nSkinsSize / 1024.0f);
			}
			Con_DPrintf("----------------------\n");
			if (host_client->nRemainingToTransfer > 1024)
				Con_DPrintf("Resources to request: %iK\n", host_client->nRemainingToTransfer / 1024);
			else
				Con_DPrintf("Resources to request: %i bytes\n", host_client->nRemainingToTransfer);
		}
		host_client->uploading = TRUE;
		host_client->uploaddoneregistering = FALSE;
		host_client->isuploading = FALSE; // TODO: See if the name is really correct
										  // TODO: Implement
		host_client->uploadstarttime = realtime;
		host_client->lastuploadtime = realtime;
		host_client->uploadsize = host_client->nRemainingToTransfer;
		memset(host_client->uploads, 0, sizeof(host_client->uploads));
		host_client->numuploads = 0;
	}
}

// Functions below do NOT belong to sv_upld.c, move them away from here.
// TODO: Implement

/*
================
SV_ModelIndex

================
*/
int SV_ModelIndex( char* name )
{
	int		i;

	if (!name || !name[0])
		return 0;

	for (i = 0; i < MAX_MODELS && sv.model_precache[i]; i++)
		if (!strcmp(sv.model_precache[i], name))
			return i;
	if (i == MAX_MODELS || !sv.model_precache[i])
		Sys_Error("SV_ModelIndex: model %s not precached", name);
	return i;
}

/*
================
SV_FlushSignon

Moves to the next signon buffer if needed
================
*/
void SV_FlushSignon( void )
{
	if (sv.signon.cursize < sv.signon.maxsize - 100)
		return;

	if (sv.num_signon_buffers == MAX_SIGNON_BUFFERS - 1)
		Sys_Error("sv.num_signon_buffers == MAX_SIGNON_BUFFERS-1");

	sv.signon_buffer_size[sv.num_signon_buffers - 1] = sv.signon.cursize;
	sv.signon.data = sv.signon_buffers[sv.num_signon_buffers];
	sv.num_signon_buffers++;
	sv.signon.cursize = 0;
}

/*
================
SV_SendResourceListBlock_f
================
*/
void SV_SendResourceListBlock_f( void )
{
	int			i, n;
	resource_t* res;

	if (cmd_source == src_command)
	{
		Cmd_ForwardToServer();
		return;
	}

	if (!host_client->connected) {
		Con_Printf("resourcelist not valid -- already spawned\n");
		return;
	}

	// handle the case of a level changing while a client was connecting
	if (!cls.demoplayback && atoi(Cmd_Argv(1)) != svs.spawncount)
	{
		Con_Printf("SV_SendResourceListBlock_f from different level\n");
		SV_New_f();
		return;
	}

	n = atoi(Cmd_Argv(2));

	MSG_WriteByte(&host_client->netchan.message, svc_resourcelist);
	MSG_WriteShort(&host_client->netchan.message, sv.num_resources);
	MSG_WriteShort(&host_client->netchan.message, n);

	// save index
	i = host_client->netchan.message.cursize;

	MSG_WriteShort(&host_client->netchan.message, 0);

	if (sv.num_resources <= n)
	{
		*(unsigned short*)&host_client->netchan.message.data[i] = n;
	}
	else
	{
		for (res = &sv.resources[n]; n < sv.num_resources; n++, res++)
		{
			//leave a small window for other messages
			if (host_client->netchan.message.maxsize - 512 <= host_client->netchan.message.cursize)
				break;

			MSG_WriteByte(&host_client->netchan.message, res->type);
			MSG_WriteString(&host_client->netchan.message, res->szFileName);
			MSG_WriteShort(&host_client->netchan.message, res->nIndex);
			MSG_WriteLong(&host_client->netchan.message, res->nDownloadSize);
			MSG_WriteByte(&host_client->netchan.message, res->ucFlags);
			if (res->ucFlags & RES_CUSTOM)
			{
				SZ_Write(&cls.netchan.message, res->rgucMD5_hash, sizeof(res->rgucMD5_hash));
			}
		}

		*(unsigned short*)&host_client->netchan.message.data[i] = n;
	}

	if (sv.num_resources > n)
	{
		MSG_WriteByte(&host_client->netchan.message, svc_stufftext);
		MSG_WriteString(&host_client->netchan.message,
						va("cmd resourcelist %i %i\n", svs.spawncount, n));
	}
}

/*
================
SV_AddResource

Put this very resource into resources array
================
*/
void SV_AddResource( resourcetype_t type, const char *name, int size, byte flags, int index )
{
	resource_t*		res;

	res = &sv.resources[sv.num_resources];

	if (sv.num_resources >= MAX_RESOURCES)
		Sys_Error("Too many resources on server.");

	sv.num_resources++;

	res->type = type;
	strcpy(res->szFileName, name);
	res->nDownloadSize = size;
	res->nIndex = index;
	res->ucFlags = flags;
}

/*
================
SV_CreateResourceList

Add resources to precache list
================
*/
void SV_CreateResourceList( void )
{
	int			i, nSize;
	char*		s;
	qboolean	bAddedFirst = FALSE;
	FILE*		pf;

	sv.num_resources = 0;

	for (i = 1; i < MAX_SOUNDS; i++)
	{
		s = sv.sound_precache[i];

		if (!s || s[0] == 0)
			break;

		if (s[0] == '!')
		{
			if (!bAddedFirst)
			{
				SV_AddResource(t_sound, "!", 0, RES_FATALIFMISSING, i);
				bAddedFirst = TRUE;
			}
		}
		else
		{
			if (svs.maxclients <= 1)
			{
				SV_AddResource(t_sound, s, 0, 0, i);
			}
			else
			{
				nSize = COM_FindFile(va("sound/%s", s), NULL, &pf);
				if (pf)
					fclose(pf);
				if (nSize == -1)
					nSize = 0;

				SV_AddResource(t_sound, s, nSize, 0, i);
			}
		}
	}

	for (i = 1; i < MAX_MODELS; i++)
	{
		s = sv.model_precache[i];

		if (!s || s[0] == 0)
			break;

		if (svs.maxclients <= 1)
		{
			SV_AddResource(t_model, s, 0, RES_FATALIFMISSING, i);
		}
		else
		{
			nSize = COM_FindFile(s, NULL, &pf);
			if (pf)
				fclose(pf);
			if (nSize == -1)
				nSize = 0;

			SV_AddResource(t_model, s, nSize, RES_FATALIFMISSING, i);
		}
	}

	for (i = 0; i < sv_decalnamecount; i++)
	{
		SV_AddResource(t_decal, sv_decalnames[i].name , Draw_DecalSize(i), 0, i);
	}
}

/*
==================
SV_Customization

Sends resource to all other players, optionally skipping originating player.
==================
*/
void SV_Customization( client_t* pPlayer, resource_t* pResource, qboolean bSkipPlayer )
{
	int			i;
	int			nPlayerNumber;
	client_t*	pHost;

	// Get originating player id
	for (nPlayerNumber = -1, i = 0, pHost = svs.clients; i < svs.maxclients; i++, pHost++)
	{
		if (pHost == pPlayer)
		{
			nPlayerNumber = i;
			break;
		}
	}
	if (i == svs.maxclients || nPlayerNumber == -1)
	{
		Sys_Error("Couldn't find player index for customization.");
	}

	// Send resource to all other active players
	for (i = 0, pHost = svs.clients; i < svs.maxclients; i++, pHost++)
	{
		if (pHost->fakeclient)
			continue;

		if (!pHost->active && !pHost->spawned)
			continue;

		if (pHost == pPlayer && bSkipPlayer)
			continue;

		MSG_WriteByte(&pHost->netchan.message, svc_customization);
		MSG_WriteByte(&pHost->netchan.message, nPlayerNumber);
		MSG_WriteByte(&pHost->netchan.message, pResource->type);
		MSG_WriteString(&pHost->netchan.message, pResource->szFileName);
		MSG_WriteShort(&pHost->netchan.message, pResource->nIndex);
		MSG_WriteLong(&pHost->netchan.message, pResource->nDownloadSize);
		MSG_WriteByte(&pHost->netchan.message, pResource->ucFlags);
		if (pResource->ucFlags & RES_CUSTOM)
		{
			SZ_Write(&pHost->netchan.message, pResource->rgucMD5_hash, sizeof(pResource->rgucMD5_hash));
		}
	}
}

/*
==================
SV_PropagateCustomizations

Sends customizations from all active players to the current player.
==================
*/
void SV_PropagateCustomizations( void )
{
	// TODO: Implement
}