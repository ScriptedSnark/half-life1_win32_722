// hashpak.c - HPAK system, handles resources in a compressed form

#include "quakedef.h"
#include "custom.h"
#include "cl_demo.h"
#include "hashpak.h"

#define MAX_HASHPAK_FILE_ENTRIES	1024 * MAX_CLIENTS // 1KB for each client

typedef struct hash_pack_entry_s
{
	resource_t resource;        // Lump data
	int nOffset;				// File offset of track data
	int nFileLength;			// Length of track
} hash_pack_entry_t;

typedef struct hash_pack_directory_s
{
	int nEntries;                   // Number of tracks
	hash_pack_entry_t* p_rgEntries; // Track entry info
} hash_pack_directory_t;

typedef struct hash_pack_header_s
{
	char szFileStamp[4];		// Should be HPAK
	int version;                // Should be HASHPAK_VERSION
	int nDirectoryOffset;		// Offset of Entry Directory.
} hash_pack_header_t;

hash_pack_directory_t hash_pack_dir = { 0, NULL };
hash_pack_header_t hash_pack_header;

/*
=================
HPAK_GetDataPointer

=================
*/
qboolean HPAK_GetDataPointer( char* pakname, resource_t* pResource, FILE** fpOut )
{
	qboolean retval = FALSE;

	hash_pack_header_t header;
	hash_pack_directory_t directory;
	hash_pack_entry_t* entry;
	char	name[MAX_OSPATH];
	int     i;
	qboolean bfound = FALSE;

	//
	// open the hpak file
	//
	sprintf(name, "%s/%s", com_gamedir, pakname);
	COM_DefaultExtension(name, HASHPAK_EXTENSION);
	*fpOut = fopen(name, "rb");
	if (!*fpOut)
	{
		Con_Printf("ERROR: couldn't open %s.\n", name);
		return FALSE;
	}

	// Read in the hpakheader
	fread(&header, sizeof(hash_pack_header_t), 1, *fpOut);

	if (strncmp(header.szFileStamp, "HPAK", 4) != 0)
	{
		Con_Printf("%s is not an HPAK file\n", name);
		fclose(*fpOut);
		*fpOut = NULL;
		return FALSE;
	}

	if (header.version != HASHPAK_VERSION)
	{
		Con_Printf("HPAK_List:  version mismatch\n");
		fclose(*fpOut);
		*fpOut = NULL;
		return FALSE;
	}

	// Now read in the directory structure.
	fseek(*fpOut, header.nDirectoryOffset, SEEK_SET);
	fread(&directory.nEntries, sizeof(int), 1, *fpOut);

	if (directory.nEntries < 1 ||
		directory.nEntries > MAX_HASHPAK_FILE_ENTRIES)
	{
		Con_Printf("ERROR: HPAK had bogus # of directory entries:  %i\n", directory.nEntries);
		fclose(*fpOut);
		*fpOut = NULL;
		return FALSE;
	}

	directory.p_rgEntries = (hash_pack_entry_t*)malloc(sizeof(hash_pack_entry_t) * directory.nEntries);
	fread(directory.p_rgEntries, sizeof(hash_pack_entry_t), directory.nEntries, *fpOut);

	//
	// Scan all directory entries, look for the needed resource
	//
	for (i = 0; i < directory.nEntries; i++)
	{
		entry = &directory.p_rgEntries[i];

		// See if the MD5 hash does match
		bfound = memcmp(entry->resource.rgucMD5_hash, pResource->rgucMD5_hash, sizeof(pResource->rgucMD5_hash)) == 0;
		if (!bfound)
			continue;

		fseek(*fpOut, entry->nOffset, SEEK_SET);
		break;
	}

	free(directory.p_rgEntries);

	if (directory.nEntries == i)
	{
		fclose(*fpOut);
		*fpOut = NULL;
		retval = FALSE;
	}
	else
	{
		retval = TRUE;
	}

	return retval;
}

/*
=================
HPAK_FindResource

=================
*/
qboolean HPAK_FindResource( hash_pack_directory_t* dir, byte* hash, resource_t* pResourceEntry )
{
	int     iLow, iHigh, iMid;
	int     cmp;
	hash_pack_entry_t* entry;

	iLow = 0;
	iHigh = dir->nEntries - 1;

	entry = dir->p_rgEntries;

	// Perform binary search through sorted entries
	while (iHigh - iLow > 1)
	{
		iMid = (iLow + iHigh) / 2;

		cmp = memcmp(entry[iMid].resource.rgucMD5_hash, hash, sizeof(entry[iMid].resource.rgucMD5_hash));
		if (cmp == 0)
		{
			// Exact match found
			if (pResourceEntry)
				memcpy(pResourceEntry, &entry[iMid], sizeof(resource_t));

			return TRUE;
		}

		if (cmp > 0)
			iLow = iMid;
		else
			iHigh = iMid;
	}

	// Check remaining candidates
	if (!memcmp(entry[iLow].resource.rgucMD5_hash, hash, sizeof(entry[iLow].resource.rgucMD5_hash)))
	{
		if (pResourceEntry)
			memcpy(pResourceEntry, &entry[iLow], sizeof(resource_t));

		return TRUE;
	}

	if (!memcmp(entry[iHigh].resource.rgucMD5_hash, hash, sizeof(entry[iHigh].resource.rgucMD5_hash)))
	{
		if (pResourceEntry)
			memcpy(pResourceEntry, &entry[iHigh], sizeof(resource_t));

		return TRUE;
	}

	return FALSE;
}

/*
=================
HPAK_AddLump

Add a resource to an HPAK file or an HPAK queue
Compute MD5 hash of the resource data
=================
*/
void HPAK_AddLump( char* pakname, resource_t* pResource, void* pData, FILE* fpSource )
{
	int     i, j;
	FILE* iRead, * iWrite;
	int		cmp;
	char    name[MAX_OSPATH];
	int		copysize;
	char	szTempName[MAX_OSPATH];
	char	szOriginalName[MAX_OSPATH];
	hash_pack_directory_t olddirectory, newdirectory;
	qboolean bExact = FALSE;
	hash_pack_entry_t* pCurrentEntry, * pNewEntry;

	if (!pakname || !pResource || (!pData && !fpSource))
	{
		Con_Printf("HPAK_AddLump called with invalid arguments\n");
		return;
	}

	//
	// open the hpak file
	//
	sprintf(name, "%s/%s", com_gamedir, pakname);
	COM_DefaultExtension(name, HASHPAK_EXTENSION);
	strcpy(szOriginalName, name);
	iRead = fopen(name, "rb");
	if (!iRead)
	{
		HPAK_CreatePak(pakname, pResource, pData, fpSource);
		return;
	}

	//
	// open temp file
	//
	COM_StripExtension(name, szTempName);
	COM_DefaultExtension(szTempName, ".hp2");
	iWrite = fopen(szTempName, "w+b");
	if (!iWrite)
	{
		Con_Printf("ERROR: couldn't open %s.\n", name);
		return;
	}

	// Read in the hpakheader
	fread(&hash_pack_header, sizeof(hash_pack_header_t), 1, iRead);

	if (hash_pack_header.version != HASHPAK_VERSION)
	{
		fclose(iRead);
		fclose(iWrite);
		_unlink(szTempName);
		Con_Printf("Invalid .hpk version in HPAK_AddLump\n");
		return;
	}

	// Ready to start reading
	// Now copy the stuff.
	fseek(iRead, 0, SEEK_END);
	copysize = ftell(iRead);
	fseek(iRead, 0, SEEK_SET);

	COM_CopyFileChunk(iWrite, iRead, copysize);

	// Now read in the directory structure.
	fseek(iRead, hash_pack_header.nDirectoryOffset, SEEK_SET);

	fread(&olddirectory.nEntries, sizeof(int), 1, iRead);

	if (olddirectory.nEntries > MAX_HASHPAK_FILE_ENTRIES)
	{
		Con_Printf("ERROR: .hpk had bogus # of directory entries:  %i\n", olddirectory.nEntries);
		fclose(iRead);
		fclose(iWrite);
		_unlink(szTempName);
		return;
	}

	olddirectory.p_rgEntries = (hash_pack_entry_t*)malloc(sizeof(hash_pack_entry_t) * (olddirectory.nEntries + 1));
	fread(olddirectory.p_rgEntries, sizeof(hash_pack_entry_t), olddirectory.nEntries, iRead);

	// The reading is done, close the file
	fclose(iRead);

	// It's already there.
	bExact = HPAK_FindResource(&olddirectory, pResource->rgucMD5_hash, NULL) != FALSE;
	if (bExact)
	{
		fclose(iWrite);
		_unlink(szTempName);
		free(olddirectory.p_rgEntries);
		return;
	}

	newdirectory.nEntries = olddirectory.nEntries + 1;
	newdirectory.p_rgEntries = (hash_pack_entry_t*)malloc(sizeof(hash_pack_entry_t) * newdirectory.nEntries);

	memset(newdirectory.p_rgEntries, 0, sizeof(hash_pack_entry_t) * newdirectory.nEntries);
	memcpy(newdirectory.p_rgEntries, olddirectory.p_rgEntries, sizeof(hash_pack_entry_t) * olddirectory.nEntries);

	// Find the correct position to insert the new entry
	pNewEntry = NULL;
	for (i = 0; i < olddirectory.nEntries; i++)
	{
		pCurrentEntry = &olddirectory.p_rgEntries[i];

		cmp = memcmp(pResource->rgucMD5_hash, pCurrentEntry->resource.rgucMD5_hash, sizeof(pResource->rgucMD5_hash));
		if (cmp >= 0)
			continue;

		pNewEntry = &newdirectory.p_rgEntries[i];

		for (j = i; j < olddirectory.nEntries; j++)
		{
			memcpy(&newdirectory.p_rgEntries[j + 1], &olddirectory.p_rgEntries[j], sizeof(hash_pack_entry_t));
		}
		break;
	}

	if (!pNewEntry)
	{
		pNewEntry = &newdirectory.p_rgEntries[newdirectory.nEntries - 1];
	}

	memset(pNewEntry, 0, sizeof(hash_pack_entry_t));

	// Now write in the directory structure.
	fseek(iWrite, hash_pack_header.nDirectoryOffset, SEEK_SET);

	memcpy(&pNewEntry->resource, pResource, sizeof(resource_t));

	pNewEntry->nOffset = ftell(iWrite);  // Position for this chunk.
	pNewEntry->nFileLength = pResource->nDownloadSize;

	if (pData)
	{
		fwrite(pData, pNewEntry->nFileLength, 1, iWrite);
	}
	else
	{
		COM_CopyFileChunk(iWrite, fpSource, pNewEntry->nFileLength);
	}

	hash_pack_header.nDirectoryOffset = ftell(iWrite);

	fwrite(&newdirectory.nEntries, sizeof(int), 1, iWrite);

	for (i = 0; i < newdirectory.nEntries; i++)
	{
		fwrite(&newdirectory.p_rgEntries[i], sizeof(hash_pack_entry_t), 1, iWrite);
	}

	// Release entry info
	if (newdirectory.p_rgEntries)
		free(newdirectory.p_rgEntries);
	if (olddirectory.p_rgEntries)
		free(olddirectory.p_rgEntries);

	fseek(iWrite, 0, SEEK_SET);

	// Write in the hpakheader
	fwrite(&hash_pack_header, sizeof(hash_pack_header_t), 1, iWrite);

	// Close the file
	fclose(iWrite);

	// Replace original file
	_unlink(szOriginalName);
	rename(szTempName, szOriginalName);
}

/*
=================
HPAK_RemoveLump

Remove a specific resource from a HPAK file
=================
*/
void HPAK_RemoveLump( char* pakname, resource_t* pResource )
{
	char    name[MAX_OSPATH];
	FILE* fp, * tmp;
	char    szTempName[MAX_OSPATH];
	char    szOriginalName[MAX_OSPATH];
	hash_pack_directory_t hash_pack_dir;
	hash_pack_directory_t newdir;
	hash_pack_entry_t* oldentry, * newentry;
	int     n, i;
	qboolean bFound = FALSE;

	if (!pakname || !pakname[0] || !pResource)
	{
		Con_Printf("HPAK_RemoveLump:  Invalid arguments\n");
		return;
	}

	//
	// open the hpak file
	//
	sprintf(name, "%s/%s", com_gamedir, Cmd_Argv(1));
	COM_DefaultExtension(name, HASHPAK_EXTENSION);
	strcpy(szOriginalName, name);
	fp = fopen(szOriginalName, "rb");
	if (!fp)
	{
		Con_Printf("Error:  couldn't open HPAK file %s for removal.\n", name);
		return;
	}

	COM_StripExtension(name, szTempName);
	COM_DefaultExtension(szTempName, ".hp2");
	tmp = fopen(szTempName, "w+b");
	if (!tmp)
	{
		fclose(fp);
		Con_Printf("ERROR: couldn't create %s.\n", szTempName);
		return;
	}

	fseek(fp, 0, SEEK_SET);
	fseek(tmp, 0, SEEK_SET);

	// Read in the hpakheader
	fread(&hash_pack_header, sizeof(hash_pack_header_t), 1, fp);

	// Write in the hpakheader
	fwrite(&hash_pack_header, sizeof(hash_pack_header_t), 1, tmp);

	if (strncmp(hash_pack_header.szFileStamp, "HPAK", 4) != 0)
	{
		Con_Printf("%s is not an HPAK file\n", name);
		fclose(tmp);
		fclose(fp);
		_unlink(szTempName);
		return;
	}

	if (hash_pack_header.version != HASHPAK_VERSION)
	{
		Con_Printf("ERROR: HPAK version outdated\n");
		fclose(tmp);
		fclose(fp);
		_unlink(szTempName);
		return;
	}

	// Now read in the directory structure.
	fseek(fp, hash_pack_header.nDirectoryOffset, SEEK_SET);

	fread(&hash_pack_dir.nEntries, sizeof(int), 1, fp);

	if (hash_pack_dir.nEntries < 1 ||
		hash_pack_dir.nEntries > MAX_HASHPAK_FILE_ENTRIES)
	{
		Con_Printf("ERROR: HPAK had bogus # of directory entries:  %i\n", hash_pack_dir.nEntries);
		fclose(tmp);
		fclose(fp);
		_unlink(szTempName);
		return;
	}

	if (hash_pack_dir.nEntries == 1)
	{
		Con_Printf("Removing final lump from HPAK, deleting HPAK:\n  %s\n", szOriginalName);
		fclose(tmp);
		fclose(fp);
		_unlink(szTempName);
		_unlink(szOriginalName);
		return;
	}

	hash_pack_dir.p_rgEntries = (hash_pack_entry_t*)malloc(sizeof(hash_pack_entry_t) * hash_pack_dir.nEntries);
	fread(hash_pack_dir.p_rgEntries, sizeof(hash_pack_entry_t), hash_pack_dir.nEntries, fp);

	newdir.nEntries = hash_pack_dir.nEntries - 1;
	newdir.p_rgEntries = (hash_pack_entry_t*)malloc(sizeof(hash_pack_entry_t) * newdir.nEntries);

	// Check if it exists.
	bFound = HPAK_FindResource(&hash_pack_dir, pResource->rgucMD5_hash, NULL);
	if (!bFound)
	{
		Con_Printf("ERROR: HPAK doesn't contain specified lump:  %s\n", pResource->szFileName);
		fclose(tmp);
		fclose(fp);
		_unlink(szTempName);
		return;
	}

	Con_Printf("Removing %s from HPAK %s.\n", pResource->szFileName, name);

	for (i = 0, n = 0; i < hash_pack_dir.nEntries; i++)
	{
		oldentry = &hash_pack_dir.p_rgEntries[i];

		if (memcmp(oldentry->resource.rgucMD5_hash, pResource->rgucMD5_hash, sizeof(pResource->rgucMD5_hash)) == 0)
			continue;

		newentry = &newdir.p_rgEntries[n++];
		memcpy(newentry, oldentry, sizeof(hash_pack_entry_t));
		newentry->nOffset = ftell(tmp);  // Position for this chunk.

		fseek(fp, oldentry->nOffset, SEEK_SET);

		COM_CopyFileChunk(tmp, fp, newentry->nFileLength);
	}

	hash_pack_header.nDirectoryOffset = ftell(tmp);

	fwrite(&newdir.nEntries, sizeof(int), 1, tmp);

	for (i = 0; i < newdir.nEntries; i++)
	{
		fwrite(&newdir.p_rgEntries[i], sizeof(hash_pack_entry_t), 1, tmp);
	}

	fseek(tmp, 0, SEEK_SET);

	// Write in the hpakheader
	fwrite(&hash_pack_header, sizeof(hash_pack_header_t), 1, tmp);

	// Close the file
	fclose(tmp);
	fclose(fp);

	// Replace original file
	_unlink(szOriginalName);
	rename(szTempName, szOriginalName);

	// Release entry info
	free(newdir.p_rgEntries);
	free(hash_pack_dir.p_rgEntries);
}

/*
=================
HPAK_ResourceForIndex

Look up for a resource in an HPAK file by its index
=================
*/
qboolean HPAK_ResourceForIndex( char* pakname, int nIndex, resource_t* pResource )
{
	hash_pack_header_t header;
	hash_pack_directory_t directory;
	hash_pack_entry_t* entry;
	char	name[MAX_OSPATH];
	FILE* fp;

	if (cmd_source != src_command)
		return FALSE;

	//
	// open the hpak file
	//
	sprintf(name, "%s/%s", com_gamedir, pakname);
	COM_DefaultExtension(name, HASHPAK_EXTENSION);
	fp = fopen(name, "rb");
	if (!fp)
	{
		Con_Printf("ERROR: couldn't open %s.\n", name);
		return FALSE;
	}

	// Read in the hpakheader
	fread(&header, sizeof(hash_pack_header_t), 1, fp);

	if (strncmp(header.szFileStamp, "HPAK", 4) != 0)
	{
		Con_Printf("%s is not an HPAK file\n", name);
		fclose(fp);
		return FALSE;
	}

	if (header.version != HASHPAK_VERSION)
	{
		Con_Printf("HPAK_List:  version mismatch\n");
		fclose(fp);
		return FALSE;
	}

	// Now read in the directory structure.
	fseek(fp, header.nDirectoryOffset, SEEK_SET);

	fread(&directory.nEntries, sizeof(int), 1, fp);

	if (directory.nEntries < 1 ||
		directory.nEntries > MAX_HASHPAK_FILE_ENTRIES)
	{
		Con_Printf("ERROR: HPAK had bogus # of directory entries:  %i\n", directory.nEntries);
		fclose(fp);
		return FALSE;
	}

	directory.p_rgEntries = (hash_pack_entry_t*)malloc(sizeof(hash_pack_entry_t) * directory.nEntries);

	if (nIndex < 1 || nIndex > directory.nEntries)
	{
		Con_Printf("ERROR: HPAK bogus directory entry request:  %i\n", nIndex);
		fclose(fp);
		return FALSE;
	}

	// Read entry info
	fread(directory.p_rgEntries, sizeof(hash_pack_entry_t), directory.nEntries, fp);
	entry = &directory.p_rgEntries[nIndex - 1];

	// Copy resource data
	memcpy(pResource, &entry->resource, sizeof(resource_t));

	// Close the file
	fclose(fp);

	// Release entry info
	free(directory.p_rgEntries);

	return TRUE;
}

/*
=================
HPAK_ResourceForHash

=================
*/
qboolean HPAK_ResourceForHash( char* pakname, byte* hash, resource_t* pResourceEntry )
{
	qboolean bFound = FALSE;

	hash_pack_header_t header;
	hash_pack_directory_t directory;
	char	name[MAX_OSPATH];
	FILE* fp;

	//
	// open the hpak file
	//
	sprintf(name, "%s/%s", com_gamedir, pakname);
	COM_DefaultExtension(name, HASHPAK_EXTENSION);
	fp = fopen(name, "rb");
	if (!fp)
	{
		Con_Printf("ERROR: couldn't open %s.\n", name);
		return FALSE;
	}

	// Read in the hpakheader
	fread(&header, sizeof(hash_pack_header_t), 1, fp);

	if (strncmp(header.szFileStamp, "HPAK", 4) != 0)
	{
		Con_Printf("%s is not an HPAK file\n", name);
		fclose(fp);
		return FALSE;
	}

	if (header.version != HASHPAK_VERSION)
	{
		Con_Printf("HPAK_List:  version mismatch\n");
		fclose(fp);
		return FALSE;
	}

	// Now read in the directory structure.
	fseek(fp, header.nDirectoryOffset, SEEK_SET);

	fread(&directory.nEntries, sizeof(int), 1, fp);

	if (directory.nEntries < 1 ||
		directory.nEntries > MAX_HASHPAK_FILE_ENTRIES)
	{
		Con_Printf("ERROR: HPAK had bogus # of directory entries:  %i\n", directory.nEntries);
		fclose(fp);
		return FALSE;
	}

	directory.p_rgEntries = (hash_pack_entry_t*)malloc(sizeof(hash_pack_entry_t) * directory.nEntries);
	fread(directory.p_rgEntries, sizeof(hash_pack_entry_t), directory.nEntries, fp);

	bFound = HPAK_FindResource(&directory, hash, pResourceEntry);

	// Close the file
	fclose(fp);

	// Release entry info
	free(directory.p_rgEntries);

	return bFound;
}

/*
=================
HPAK_List_f

=================
*/
void HPAK_List_f( void )
{
	int		nCurrent;
	hash_pack_header_t header;
	hash_pack_directory_t directory;
	hash_pack_entry_t* entry;
	char	name[MAX_OSPATH];
	char	szFileName[MAX_OSPATH];
	char	type[32];
	FILE* fp;

	if (cmd_source != src_command)
		return;

	//
	// open the hpak file
	//
	sprintf(name, "%s/%s", com_gamedir, Cmd_Argv(1));
	COM_DefaultExtension(name, HASHPAK_EXTENSION);
	Con_Printf("Contents for %s.\n", name);
	fp = fopen(name, "rb");
	if (!fp)
	{
		Con_Printf("ERROR: couldn't open %s.\n", name);
		return;
	}

	// Read in the hpakheader
	fread(&header, sizeof(hash_pack_header_t), 1, fp);

	if (strncmp(header.szFileStamp, "HPAK", 4) != 0)
	{
		Con_Printf("%s is not an HPAK file\n", name);
		fclose(fp);
		return;
	}

	if (header.version != HASHPAK_VERSION)
	{
		Con_Printf("HPAK_List:  version mismatch\n");
		fclose(fp);
		return;
	}

	// Now read in the directory structure.
	fseek(fp, header.nDirectoryOffset, SEEK_SET);

	fread(&directory, sizeof(int), 1, fp);

	if (directory.nEntries < 1 ||
		directory.nEntries > MAX_HASHPAK_FILE_ENTRIES)
	{
		Con_Printf("ERROR: HPAK had bogus # of directory entries:  %i\n", directory.nEntries);
		fclose(fp);
		return;
	}

	Con_Printf("# of Entries:  %i\n", directory.nEntries);
	Con_Printf("# Type Size FileName : MD5 Hash\n");

	directory.p_rgEntries = (hash_pack_entry_t*)malloc(sizeof(hash_pack_entry_t) * directory.nEntries);
	fread(directory.p_rgEntries, sizeof(hash_pack_entry_t), directory.nEntries, fp);

	// Print all HPAK resources
	for (nCurrent = 0; nCurrent < directory.nEntries; nCurrent++)
	{
		entry = &directory.p_rgEntries[nCurrent];
		COM_FileBase(entry->resource.szFileName, szFileName);

		switch (entry->resource.type)
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
			sprintf(type, "?");
			break;
		}

		Con_Printf("%i: %10s %.2fK %s\n  :  %s\n", nCurrent, type, entry->resource.nDownloadSize / 1024.0, szFileName, MD5_Print(entry->resource.rgucMD5_hash));
	}

	// Close the file
	fclose(fp);

	// Release entry info
	free(directory.p_rgEntries);
}

/*
=================
HPAK_CreatePak

Create a new HPAK file and add one resource to it
=================
*/
void HPAK_CreatePak( char* pakname, resource_t* pResource, void* pData, FILE* fpSource )
{
	int     i;
	char	name[MAX_OSPATH];
	long    curpos;
	FILE* fp;
	hash_pack_entry_t* pCurrentEntry;

	if ((fpSource && pData) || (!fpSource && !pData))
	{
		Con_Print("HPAK_CreatePak, must specify one of pData or fpSource\n");
		return;
	}

	//
	// open the hpak file
	//
	sprintf(name, "%s/%s", com_gamedir, pakname);
	COM_DefaultExtension(name, HASHPAK_EXTENSION);
	Con_Printf("Creating HPAK %s.\n", name);
	fp = fopen(name, "wb");
	if (!fp)
	{
		Con_Printf("ERROR: couldn't open new .hpk, check access rights to %s.\n", name);
		return;
	}

	memset(&hash_pack_header, 0, sizeof(hash_pack_header_t));

	strncpy(hash_pack_header.szFileStamp, "HPAK", 4);
	hash_pack_header.version = HASHPAK_VERSION;
	hash_pack_header.nDirectoryOffset = 0;  // Temporary.  We rewrite the structure at the end.

	// Write in the hpakheader
	fwrite(&hash_pack_header, sizeof(hash_pack_header_t), 1, fp);

	memset(&hash_pack_dir, 0, sizeof(hash_pack_directory_t));

	hash_pack_dir.nEntries = 1;
	hash_pack_dir.p_rgEntries = (hash_pack_entry_t*)malloc(sizeof(hash_pack_entry_t));
	memset(hash_pack_dir.p_rgEntries, 0, sizeof(hash_pack_entry_t) * hash_pack_dir.nEntries);

	// DIRECTORY ENTRY # 0
	pCurrentEntry = hash_pack_dir.p_rgEntries; // Only one here.
	memcpy(&pCurrentEntry->resource, pResource, sizeof(resource_t));

	pCurrentEntry->nOffset = ftell(fp);  // Position for this chunk.
	pCurrentEntry->nFileLength = pResource->nDownloadSize;

	if (pData)
	{
		fwrite(pData, pCurrentEntry->nFileLength, 1, fp);
	}
	else
	{
		COM_CopyFileChunk(fp, fpSource, pCurrentEntry->nFileLength);
	}

	curpos = ftell(fp);

	fwrite(&hash_pack_dir.nEntries, sizeof(int), 1, fp);

	for (i = 0; i < hash_pack_dir.nEntries; i++)
	{
		fwrite(&hash_pack_dir.p_rgEntries[i], sizeof(hash_pack_entry_t), 1, fp);
	}

	// Release entry info
	if (hash_pack_dir.p_rgEntries)
		free(hash_pack_dir.p_rgEntries);

	hash_pack_dir.p_rgEntries = NULL;
	hash_pack_dir.nEntries = 0;

	hash_pack_header.nDirectoryOffset = curpos;

	fseek(fp, 0, SEEK_SET);

	// Write in the hpakheader
	fwrite(&hash_pack_header, sizeof(hash_pack_header_t), 1, fp);

	// Close the file
	fclose(fp);
}

/*
=================
HPAK_Remove_f

=================
*/
void HPAK_Remove_f( void )
{
	int	nIndex;
	resource_t resource;
	char* pakname;

	if (cmd_source != src_command)
		return;

	if (Cmd_Argc() != 3)
	{
		Con_Printf("Usage:  hpkremove <hpk> <index>\n");
		return;
	}

	nIndex = atoi(Cmd_Argv(2));
	pakname = Cmd_Argv(1);
	if (!HPAK_ResourceForIndex(pakname, nIndex, &resource))
	{
		Con_Printf("Could not locate resource %i in %s\n", atoi(Cmd_Argv(2)), Cmd_Argv(1));
		return;
	}

	HPAK_RemoveLump(Cmd_Argv(1), &resource);
}