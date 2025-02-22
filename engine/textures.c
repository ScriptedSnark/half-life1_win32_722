// textures.c

#include "quakedef.h"
#include "textures.h"

#define TEX_MAX_WADS			128
#define TEX_MAX_MIPTEX_NAME		64

int nummiptex = 0;
char miptex[MAX_MAP_TEXTURES][TEX_MAX_MIPTEX_NAME];

// Lump data
typedef struct
{
	lumpinfo_t	lump;
	int			iTexFile;	// index of the wad this texture is located in
} texlumpinfo_t;

FILE* texfiles[TEX_MAX_WADS];
int nTexFiles = 0;
texlumpinfo_t* lumpinfo = NULL;
int nTexLumps = 0;

void SafeRead( FILE* f, void* buffer, int count )
{
	if (fread(buffer, 1, count, f) != count)
		Sys_Error("File read failure");
}

void CleanupName( char* in, char* out )
{
	int		i;

	for (i = 0; i < 16; i++)
	{
		if (!in[i])
			break;

		out[i] = toupper(in[i]);
	}

	for (; i < 16; i++)
		out[i] = 0;
}

/*
=================
lump_sorter
=================
*/

int
lump_sorter( const void* lump1, const void* lump2 )
{
	texlumpinfo_t* plump1 = (texlumpinfo_t*)lump1;
	texlumpinfo_t* plump2 = (texlumpinfo_t*)lump2;
	return strcmp(plump1->lump.name, plump2->lump.name);
}

// Convert \\ to /
void ForwardSlashes( char* pname )
{
	while (*pname)
	{
		if (*pname == '\\')
			*pname = '/';
		pname++;
	}
}

/*
=================
TEX_InitFromWad
=================
*/
qboolean TEX_InitFromWad( char* path )
{
	int			i;
	wadinfo_t	wadinfo;
	char* szTmpPath;
	char* pszWadFile;

	szTmpPath = _strdup(path);

	// temporary kludge so we don't have to deal with no occurances of a semicolon
	//  in the path name ..
	if (strchr(szTmpPath, ';') == NULL)
		strcat(szTmpPath, ";");

	pszWadFile = strtok(szTmpPath, ";");

	while (pszWadFile)
	{
		FILE* texfile;	// temporary used in this loop
		char wadPath[MAX_PATH];
		char wadName[MAX_PATH];

		ForwardSlashes(pszWadFile);

		COM_FileBase(pszWadFile, wadName);
		sprintf(wadPath, "%s/%s", com_gamedir, wadName);
		COM_DefaultExtension(wadPath, ".wad");

		texfile = fopen(wadPath, "rb");
		texfiles[nTexFiles] = texfile;
		if (!texfiles[nTexFiles])
		{
			COM_FileBase(pszWadFile, wadName);
			sprintf(wadPath, "valve/%s", wadName);
			COM_DefaultExtension(wadPath, ".wad");

			texfile = fopen(wadPath, "rb");
			texfiles[nTexFiles] = texfile;
			if (!texfiles[nTexFiles])
			{
				Sys_Error("WARNING: couldn't open %s\n", wadPath);
				return FALSE;
			}
		}

		texfile = texfiles[nTexFiles];
		nTexFiles++;

		Con_SafePrintf("Using WAD File: %s\n", wadPath);

		SafeRead(texfile, &wadinfo, sizeof(wadinfo));
		if (strncmp(wadinfo.identification, "WAD2", 4) &&
			strncmp(wadinfo.identification, "WAD3", 4))
			Sys_Error("TEX_InitFromWad: %s isn't a wadfile", wadPath);

		wadinfo.numlumps = LittleLong(wadinfo.numlumps);
		wadinfo.infotableofs = LittleLong(wadinfo.infotableofs);
		fseek(texfile, wadinfo.infotableofs, SEEK_SET);
		lumpinfo = (texlumpinfo_t*)realloc(lumpinfo, sizeof(texlumpinfo_t) * (nTexLumps + wadinfo.numlumps));

		for (i = 0; i < wadinfo.numlumps; i++)
		{
			SafeRead(texfile, &lumpinfo[nTexLumps], sizeof(lumpinfo_t));
			CleanupName(lumpinfo[nTexLumps].lump.name, lumpinfo[nTexLumps].lump.name);

			lumpinfo[nTexLumps].lump.filepos = LittleLong(lumpinfo[nTexLumps].lump.filepos);
			lumpinfo[nTexLumps].lump.disksize = LittleLong(lumpinfo[nTexLumps].lump.disksize);
			lumpinfo[nTexLumps].iTexFile = nTexFiles - 1;

			nTexLumps++;
		}

		// next wad file
		pszWadFile = strtok(NULL, ";");
	}

	qsort(lumpinfo, nTexLumps, sizeof(texlumpinfo_t), lump_sorter);

	free(szTmpPath);

	return TRUE;
}

/*
=================
TEX_CleanupWadInfo
=================
*/
void TEX_CleanupWadInfo( void )
{
	int i;

	if (lumpinfo)
	{
		free(lumpinfo);
		lumpinfo = NULL;
	}

	for (i = 0; i < nTexFiles; i++)
	{
		fclose(texfiles[i]);
		texfiles[i] = NULL;
	}

	nTexLumps = 0;
	nTexFiles = 0;
}

/*
=================
TEX_LoadLump
=================
*/
int TEX_LoadLump( char* name, byte* dest )
{
	texlumpinfo_t key;
	texlumpinfo_t* found;

	CleanupName(name, key.lump.name);

	// Find the lump
	found = (texlumpinfo_t*)bsearch(&key, lumpinfo, nTexLumps, sizeof(key), lump_sorter);
	if (found)
	{
		fseek(texfiles[found->iTexFile], found->lump.filepos, SEEK_SET);
		SafeRead(texfiles[found->iTexFile], dest, found->lump.disksize);
		return found->lump.disksize;
	}

	Con_SafePrintf("WARNING: texture lump \"%s\" not found\n", name);
	return 0;
}

int FindMiptex( char* name )
{
	int		i;

	for (i = 0; i < nummiptex; i++)
	{
		if (!Q_strcasecmp(name, miptex[i]))
			return i;
	}

	if (nummiptex == MAX_MAP_TEXTURES)
		Sys_Error("Exceeded MAX_MAP_TEXTURES");
	strcpy(miptex[i], name);
	nummiptex++;
	return i;
}

/*
==================
TEX_AddAnimatingTextures
==================
*/
void TEX_AddAnimatingTextures( void )
{
	int		base;
	int		i, j, k;
	char	name[32];

	base = nummiptex;

	for (i = 0; i < base; i++)
	{
		if (miptex[i][0] != '+' && miptex[i][0] != '-')
			continue;
		strcpy(name, miptex[i]);

		for (j = 0; j < 20; j++)
		{
			if (j < 10)
				name[1] = '0' + j;
			else
				name[1] = 'A' + j - 10;		// alternate animation


			// see if this name exists in the wadfile
			for (k = 0; k < nTexLumps; k++)
				if (!strcmp(name, lumpinfo[k].lump.name))
				{
					FindMiptex(name);	// add to the miptex list
					break;
				}
		}
	}

	if (nummiptex != base)
		Con_SafePrintf("added %i texture frames\n", nummiptex - base);
}