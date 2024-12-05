// decals.c
// functionality common to wad and decal code in gl_draw.c and draw.c
#include "quakedef.h"
#include "draw.h"
#include "decal.h"
#include "screen.h"
#include "r_local.h"



cachewad_t menu_wad;


void Draw_CacheWadInit( char* name, int cacheMax, cachewad_t* wad )
{
	int		h[3];
	int		len;
	lumpinfo_t* lump_p;
	wadinfo_t header;
	int		i;

	len = COM_OpenFile(name, h);
	if (h[2] == -1)
		Sys_Error("Draw_LoadWad: Couldn't open %s\n", name);

	Sys_FileRead(h[2], &header, sizeof(header));

	if (header.identification[0] != 'W'
	  || header.identification[1] != 'A'
	  || header.identification[2] != 'D'
	  || header.identification[3] != '3')
	{
		Sys_Error( "Wad file %s doesn't have WAD3 id\n", name );
	}

	wad->lumps = (lumpinfo_t*)malloc(len - header.infotableofs);

	COM_FileSeek(h[0], h[1], h[2], header.infotableofs);
	Sys_FileRead(h[2], wad->lumps, len - header.infotableofs);
	COM_CloseFile(h[0], h[1], h[2]);

	for (i = 0, lump_p = wad->lumps; i < header.numlumps; i++, lump_p++)
	{
		W_CleanupName(lump_p->name, lump_p->name);
	}

	wad->lumpCount = header.numlumps;
	wad->cacheCount = 0;
	wad->cacheMax = cacheMax;
	wad->name = name;

#if defined ( GLQUAKE )
	wad->cache = (cacheentry_t*)malloc(sizeof(cacheentry_t) * cacheMax);
	memset(wad->cache, 0, sizeof(cacheentry_t) * cacheMax);
#else
	wad->cache = (cachepic_t*)malloc(sizeof(cachepic_t) * cacheMax);
	memset(wad->cache, 0, sizeof(cachepic_t) * cacheMax);
#endif
	wad->cacheExtra = 0;
	wad->pfnCacheBuild = NULL;

#if defined ( GLQUAKE )
	wad->tempWad = FALSE;
#endif
}

void Draw_CacheWadHandler( cachewad_t* wad, PFNCACHE fn, int extraDataSize )
{
	wad->cacheExtra = extraDataSize;
	wad->pfnCacheBuild = fn;
}

// TODO: Implement

int Draw_CacheIndex( cachewad_t* wad, char* path )
{
#if defined ( GLQUAKE )
	cacheentry_t* pic = NULL;
#else
	cachepic_t* pic = NULL;
#endif
	int i;

	for (i = 0, pic = wad->cache; i < wad->cacheCount; i++, pic++)
	{
		if (!strcmp(path, pic->name))
			break;
	}

	if (i == wad->cacheCount)
	{
		if (wad->cacheCount == wad->cacheMax)
			Sys_Error("Cache wad (%s) out of %d entries", wad->name, wad->cacheMax);
		
		wad->cacheCount++;
		strcpy(pic->name, path);
	}

	return i;
}

// TODO: Implement


#if defined ( GLQUAKE )
qboolean Draw_CacheReload( cachewad_t* wad, lumpinfo_t* pLump, cacheentry_t* pic, char* clean, char* path )
#else
qboolean Draw_CacheReload( cachewad_t* wad, lumpinfo_t* pLump, cachepic_t* pic, char* clean, char* path )
#endif
{
	byte* buf;
	int		h[3];

	COM_OpenFile(wad->name, h);
	if (h[2] == -1)
		return FALSE;

#if defined ( GLQUAKE )
	if (wad->tempWad)
	{
		buf = (byte*)Hunk_TempAlloc(pLump->size + wad->cacheExtra + 1);
		pic->cache.data = buf;
	}
	else
#endif
	{
		buf = (byte*)Cache_Alloc(&pic->cache, pLump->size + wad->cacheExtra + 1, clean);
	}

	if (!buf)
		Sys_Error("Draw_CacheGet: not enough space for %s in %s", path, wad->name);

	buf[pLump->size + wad->cacheExtra] = 0;

	COM_FileSeek(h[0], h[1], h[2], pLump->filepos);
	Sys_FileRead(h[2], &buf[wad->cacheExtra], pLump->size);
	COM_CloseFile(h[0], h[1], h[2]);

	if (wad->pfnCacheBuild)
		wad->pfnCacheBuild(wad, buf);

	return TRUE;
}


// TODO: Implement

void* Draw_CacheGet( cachewad_t* wad, int index )
{
#if defined ( GLQUAKE )
	cacheentry_t* pic;
#else
	cachepic_t* pic;
#endif
	int i;
	void* dat = NULL;

	if (wad->cacheCount <= index)
		Sys_Error("Cache wad indexed before load %s: %d", wad->name, index);

	pic = &wad->cache[index];
	
#if !defined ( GLQUAKE )
	dat = Cache_Check(&pic->cache);

	if (dat == NULL)
#else
	if (wad->tempWad || (dat = Cache_Check(&pic->cache)) == NULL)
#endif
	{
		char name[16];
		char clean[16];
		COM_FileBase(pic->name, name);
		W_CleanupName(name, clean);

		lumpinfo_t* pLump;
		for (i = 0, pLump = wad->lumps; i < wad->lumpCount; i++, pLump++)
		{
			if (!strcmp(clean, pLump->name))
				break;
		}

		if (i >= wad->lumpCount)
			return NULL;

		if (Draw_CacheReload(wad, pLump, pic, clean, pic->name))
		{
			if (pic->cache.data == NULL)
				Sys_Error("Draw_CacheGet: failed to load %s", pic->name);

			dat = pic->cache.data;
		}
		else
		{
			dat = NULL;
		}
	}

	return dat;
}



// TODO: Implement