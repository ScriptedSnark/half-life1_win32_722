// draw.c -- this is the only file outside the refresh that touches the
// vid buffer

#include "quakedef.h"
#include "winquake.h"
#include "decal.h"

// Scissor clipping
int scissor_x = 0, scissor_y = 0, scissor_x2 = 0, scissor_y2 = 0;

qboolean giScissorTest = FALSE;

qfont_t* draw_creditsfont;
qfont_t* draw_chars;
qpic_t* draw_disc;

cachewad_t	decal_wad;
cachewad_t	custom_wad;
cachewad_t	menu_wad;

char		decal_names[MAX_BASE_DECALS][16];

short hlRGB( word* p, int i )
{
	if (is15bit)
		return RGBPAL555(p, i);
	else
		return RGBPAL565(p, i);
}

short PackedRGB( byte* p, int i )
{
	if (is15bit)
		return PACKEDRGB555(p[i * 3 + 0], p[i * 3 + 1], p[i * 3 + 2]);
	else
		return PACKEDRGB565(p[i * 3 + 0], p[i * 3 + 1], p[i * 3 + 2]);
}

short PutRGB( colorVec* pcv )
{
	if (is15bit)
		return PACKEDRGB555(pcv->r, pcv->g, pcv->b);
	else
		return PACKEDRGB565(pcv->r, pcv->g, pcv->b);
}

void GetRGB( short s, colorVec* pcv )
{
	pcv->b = (s & 0x001F) << 3;

	if (is15bit)
	{
		pcv->g = (s & 0x03E0) >> 2;
		pcv->r = (s & 0x7C00) >> 7;
	}
	else
	{
		pcv->g = (s & 0x07E0) >> 3;
		pcv->r = (s & 0xF800) >> 8;
	}
}

/*
===============
Draw_StringLength
===============
*/
int Draw_StringLength( char* psz )
{
	int totalWidth = 0;

	while (*psz)
	{
		totalWidth += draw_chars->fontinfo[*psz].charwidth;
		psz++;
	}
	return totalWidth;
}

/*
===============
Draw_MiptexTexture

===============
*/
void Draw_MiptexTexture( cachewad_t* wad, byte* data )
{
	// TODO: Implement
}

void Draw_CacheWadHandler( cachewad_t* wad, PFNCACHE fn, int extraDataSize )
{
	wad->cacheExtra = extraDataSize;
	wad->pfnCacheBuild = fn;
}

//=============================================================================
/* Support Routines */

qpic_t* Draw_PicFromWad( char* pszName )
{
	if (pszName)
		return (qpic_t *)W_GetLumpName(pszName);

	return NULL;
}

qpic_t* Draw_CachePic( char* path )
{
	qpic_t* ret;
	int idx;

	idx = Draw_CacheIndex(&menu_wad, path);
	ret = (qpic_t*)Draw_CacheGet(&menu_wad, idx);
	return ret;
}

void Draw_DecalSetName( int decal, char* name )
{
	if (decal >= MAX_BASE_DECALS)
		return;

	strncpy(decal_names[decal], name, sizeof(decal_names[0]) - 1);
	decal_names[decal][sizeof(decal_names[0]) - 1] = 0;
}

int Draw_DecalIndex( int id )
{
	char* pName;

	pName = decal_names[id];
	if (!pName[0])
		Sys_Error("Used decal #%d without a name\n", id);

	return Draw_CacheIndex(&decal_wad, decal_names[id]);
}

int Draw_CacheIndex( cachewad_t* wad, char* path )
{
	cachepic_t* pic;
	int i;

	for (i = 0, pic = wad->cache; i < wad->cacheCount; ++i, pic++)
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

int Draw_DecalCount( void )
{
	return decal_wad.lumpCount;
}

int Draw_DecalSize( int number )
{
	if (number >= decal_wad.lumpCount)
		return 0;

	return decal_wad.lumps[number].size;
}

char* Draw_DecalName( int number )
{
	if (number >= decal_wad.lumpCount)
		return NULL;

	return decal_wad.lumps[number].name;	
}

texture_t* Draw_DecalTexture( int index )
{
	// TODO: Implement
	return NULL;
}

// called from cl_parse.c
// find the server side decal id given it's name.
// used for save/restore
int Draw_DecalIndexFromName( char* name )
{
	// TODO: Implement
	return 0;
}

qboolean Draw_CacheReload( cachewad_t* wad, lumpinfo_t* pLump, cachepic_t* pic, char* clean, char* path )
{
	byte* buf;
	int		h[3];

	COM_OpenFile(wad->name, h);
	if (h[2] == -1)
		return FALSE;

	buf = (byte*)Cache_Alloc(&pic->cache, pLump->size + wad->cacheExtra + 1, clean);
	if (!buf)
		Sys_Error("Draw_CacheGet: not enough space for %s in %s", path, wad->name);

	buf[wad->cacheExtra + pLump->size] = 0;

	Draw_BeginDisc();

	COM_FileSeek(h[0], h[1], h[2], pLump->filepos);
	Sys_FileRead(h[2], &buf[wad->cacheExtra], pLump->size);
	COM_CloseFile(h[0], h[1], h[2]);

	if (wad->pfnCacheBuild)
		wad->pfnCacheBuild(wad, buf);

	Draw_EndDisc();

	return TRUE;
}

qboolean Draw_CacheLoadFromCustom( char* clean, cachewad_t* wad, void *raw, cachepic_t* pic )
{
	// TODO: Implement
	return FALSE;
}

void* Draw_CacheGet( cachewad_t* wad, int index )
{
	cachepic_t* pic;
	int i;
	void* dat = NULL;

	if (index >= wad->cacheCount)
		Sys_Error("Cache wad indexed before load %s: %d", wad->name, index);

	pic = &wad->cache[index];
	dat = Cache_Check(&pic->cache);
	if (!dat)
	{
		char name[16];
		char clean[16];
		lumpinfo_t* pLump;

		COM_FileBase(pic->name, name);
		W_CleanupName(name, clean);

		for (i = 0, pLump = wad->lumps; i < wad->lumpCount; i++, pLump++)
		{
			if (!strcmp(clean, pLump->name))
				break;
		}

		if (!Draw_CacheReload(wad, pLump, pic, clean, pic->name))
			return NULL;

		dat = pic->cache.data;
		if (!dat)
			Sys_Error("Draw_CacheGet: failed to load %s", pic->name);		
	}

	return dat;
}

void* Draw_CustomCacheGet( cachewad_t* wad, void* raw, int index )
{
	// TODO: Implement
	return NULL;
}

void Decal_Init( void )
{
	// TODO: Implement
}

void Draw_CacheWadInit( char* name, int cacheMax, cachewad_t* wad )
{
	int		h[3];
	int		nFileSize;
	lumpinfo_t* lump_p;
	wadinfo_t header;
	int		i;

	nFileSize = COM_OpenFile(name, h);
	if (h[2] == -1)
		Sys_Error("Draw_LoadWad: Couldn't open %s\n", name);

	Sys_FileRead(h[2], &header, sizeof(header));

	if (header.identification[0] != 'W'
	  || header.identification[1] != 'A'
	  || header.identification[2] != 'D'
	  || header.identification[3] != '3')
	{
		Sys_Error("Wad file %s doesn't have WAD3 id\n", name);
	}

	wad->lumps = (lumpinfo_t*)malloc(nFileSize - header.infotableofs);

	COM_FileSeek(h[0], h[1], h[2], header.infotableofs);
	Sys_FileRead(h[2], wad->lumps, nFileSize - header.infotableofs);
	COM_CloseFile(h[0], h[1], h[2]);

	for (i = 0, lump_p = wad->lumps; i < header.numlumps; i++, lump_p++)
	{
		W_CleanupName(lump_p->name, lump_p->name);
	}

	wad->name = name;
	wad->lumpCount = header.numlumps;
	wad->cacheCount = 0;
	wad->cacheMax = cacheMax;
	wad->cache = (cachepic_t*)malloc(sizeof(cachepic_t) * cacheMax);
	memset(wad->cache, 0, sizeof(cachepic_t) * cacheMax);
	wad->cacheExtra = 0;
	wad->pfnCacheBuild = NULL;
}

void CustomDecal_Init( cachewad_t* wad, void* raw, int nFileSize )
{
	// TODO: Implement
}

void Draw_CustomCacheWadInit( int cacheMax, cachewad_t* wad, void* raw, int nFileSize )
{
	// TODO: Implement
}

int Draw_CacheByIndex( cachewad_t* wad, int nIndex )
{
	// TODO: Implement
	return 0;
}

/*
===============
Draw_Init
===============
*/
void Draw_Init( void )
{
	Draw_CacheWadInit("cached.wad", 16, &menu_wad);

	Draw_CacheWadHandler(&decal_wad, Draw_MiptexTexture, 24);
	Draw_CacheWadHandler(&custom_wad, Draw_MiptexTexture, 24);

	draw_chars = (qfont_t*)W_GetLumpName("conchars");
	draw_creditsfont = (qfont_t*)W_GetLumpName("creditsfont");

	memset(decal_names, 0, sizeof(decal_names));

	draw_disc = (qpic_t*)W_GetLumpName("lambda");
}

int Draw_MessageFontInfo( short* pWidth )
{
	int i;

	if (!draw_creditsfont)
		return 0;

	if (pWidth)
	{
		for (i = 0; i < 256; i++)
			*pWidth++ = draw_creditsfont->fontinfo[i].charwidth;
	}

	return draw_creditsfont->rowheight;
}

/*
================
Draw_Character

Draws a single character
================
*/
int Draw_Character( int x, int y, int num )
{
	word* dest;
	byte* source;
	byte* palette;
	int				row, col;
	int				rowheight, charwidth;

	num &= 255;

	rowheight = draw_chars->rowheight;
	if (y <= -rowheight)
		return 0;			// totally off screen

	source = &draw_chars->data[draw_chars->fontinfo[num].startoffset];

	charwidth = draw_chars->fontinfo[num].charwidth;
	if (y < 0)
		return charwidth;

	dest = (word*)(vid.conbuffer + y * vid.conrowbytes + x * 2);
	palette = &draw_chars->data[draw_chars->height * 256 + 2];

	for (row = 0; row < rowheight; row++)
	{
		for (col = 0; col < charwidth; col++)
		{
			if (source[col] != TRANSPARENT_COLOR)
				dest[col] = PackedRGB(palette, source[col]);
		}

		source += 256;
		dest = (word*)((byte*)dest + vid.conrowbytes);
	}

	return charwidth;
}

int Draw_MessageCharacterAdd( int x, int y, int num, int rr, int gg, int bb )
{
	// TODO: Implement
	return 0;
}

/*
================
Draw_String
================
*/
int Draw_String( int x, int y, char* str )
{
	while (*str)
	{
		x += Draw_Character(x, y, *str);
		str++;
	}
	return x;
}

/*
===============
Draw_StringLen
===============
*/
int Draw_StringLen( char* psz )
{
	int totalWidth = 0;

	while (psz && *psz)
	{
		totalWidth += draw_chars->fontinfo[*psz].charwidth;
		psz++;
	}
	return totalWidth;
}

/*
================
Draw_DebugChar

Draws a single character directly to the upper right corner of the screen.
This is for debugging lockups by drawing different chars in different parts
of the code.
================
*/
void Draw_DebugChar( char num )
{
	// TODO: Implement
}

/*
=============
Draw_Pic
=============
*/
void Draw_Pic( int x, int y, qpic_t* pic )
{
	// TODO: Implement
}


/*
=============
Draw_AlphaPic
=============
*/
void Draw_AlphaPic( int x, int y, qpic_t* pPic, colorVec* pc, int iAlpha )
{
	// TODO: Implement
}


/*
=============
Draw_AlphaSubPic
=============
*/
void Draw_AlphaSubPic( int xDest, int yDest, int xSrc, int ySrc, int iWidth, int iHeight, qpic_t* pPic, colorVec* pc, int iAlpha )
{
	// TODO: Implement
}


/*
=============
Draw_AlphaAddPic
=============
*/
void Draw_AlphaAddPic( int x, int y, qpic_t* pic, colorVec* pc, int iAlpha )
{
	// TODO: Implement
}

/*
===============
Draw_FillRGBA

Fills the given rectangle with a given color
===============
*/
void Draw_FillRGBA( int x, int y, int w, int h, int r, int g, int b, int a )
{
	// TODO: Implement
}

/*
=============
Draw_TransPic
=============
*/
void Draw_TransPic( int x, int y, qpic_t* pic )
{
	// TODO: Implement
}

/*
===============
EnableScissorTest

Set the scissor
 the coordinate system for gl is upsidedown (inverted-y) as compared to software, so the
 specified clipping rect must be flipped
===============
*/
void EnableScissorTest( int x, int y, int width, int height )
{
	if (x > (int)vid.width)
		scissor_x = vid.width;
	else
		scissor_x = max(0, x);	

	if (y > (int)vid.height)
		scissor_y = vid.height;
	else
		scissor_y = max(0, y);

	if (x + width > (int)vid.width)
		scissor_x2 = vid.width;
	else
		scissor_x2 = max(0, x + width);

	if (y + height > (int)vid.height)
		scissor_y2 = vid.height;
	else
		scissor_y2 = max(0, y + height);

	giScissorTest = TRUE;
}

/*
===============
DisableScissorTest
===============
*/
void DisableScissorTest( void )
{
	giScissorTest = FALSE;
	scissor_x = scissor_y = scissor_x2 = scissor_y2 = 0;
}

/*
===============
ValidateWRect

Verify that this is a valid, properly ordered rectangle.
===============
*/
int ValidateWRect( const wrect_t* prc )
{
	if (!prc)
		return FALSE;

	if ((prc->left >= prc->right) || (prc->top >= prc->bottom))
	{
		//!!!UNDONE Dev only warning msg
		return FALSE;
	}

	return TRUE;
}

/*
===============
IntersectWRect

classic interview question
===============
*/
int IntersectWRect( const wrect_t* prc1, const wrect_t* prc2, wrect_t* prc )
{
	wrect_t rc;

	if (!prc)
		prc = &rc;

	prc->left = max(prc1->left, prc2->left);
	prc->right = min(prc1->right, prc2->right);

	if (prc->left < prc->right)
	{
		prc->top = max(prc1->top, prc2->top);
		prc->bottom = min(prc1->bottom, prc2->bottom);

		if (prc->top < prc->bottom)
			return 1;
	}

	return 0;
}

/*
===============
AdjustSubRect
===============
*/
int AdjustSubRect( mspriteframe_t* pFrame, int* pw, int* ph, const wrect_t* prcSubRect )
{
	wrect_t rc;
	int iOffset;

	if (!ValidateWRect(prcSubRect))
		return 0;

	// clip sub rect to sprite

	rc.top = rc.left = 0;
	rc.right = *pw;
	rc.bottom = *ph;

	if (!IntersectWRect(prcSubRect, &rc, &rc))
		return 0;

	*pw = rc.right - rc.left;
	*ph = rc.bottom - rc.top;

	iOffset = rc.left + rc.top * pFrame->width;
	return iOffset;
}

/*
===============
SpriteFrameClip

Clips a sprite frame to either a subrect or the scissor rectangle.
Returns the offset in pixels from the top-left corner of the frame.
===============
*/
int SpriteFrameClip( mspriteframe_t* pFrame, int* x, int* y, int* w, int* h, const wrect_t* prcSubRect )
{
	int offset = 0;
	int	dx, dy;

	if (!pFrame)
		return 0;

	*w = pFrame->width;
	*h = pFrame->height;

	// Clip to the specified subrect
	if (prcSubRect)
	{
		offset = AdjustSubRect(pFrame, w, h, prcSubRect);
	}
	else if (giScissorTest) // Clip to scissor rect
	{
		if (*x < scissor_x)
		{
			dx = scissor_x - *x;
			dx = min(dx, pFrame->width);
			offset = scissor_x - *x;
			*w = pFrame->width - dx;
			*x = scissor_x;
		}

		if (*y < scissor_y)
		{
			dy = scissor_y - *y;
			dy = min(dy, pFrame->height);
			offset += (pFrame->width * (scissor_y - *y));
			*h = pFrame->height - dy;
			*y = scissor_y;
		}

		if (*w + *x > scissor_x2)
			*w = (scissor_x2 - *x) & ((scissor_x2 - *x < 0) - 1);
		if (*h + *y > scissor_y2)
			*h = (scissor_y2 - *y) & ((scissor_y2 - *y < 0) - 1);
	}

	if (*x < 0)
	{
		*w += *x;
		offset = (*x * (-1));
		*x = 0;
	}

	if (*y < 0)
	{
		*h += *y;
		offset -= (*y * pFrame->width);
		*y = 0;
	}

	if (*w + *x > (int)vid.width)
		*w = vid.width - *x;
	if (*h + *y > (int)vid.height)
		*h = vid.height - *y;

	if (*h < 0) // can't be negative
		*h = 0;

	return offset;
}

/*
===============
Draw_SpriteFrame

Draw normal sprite frame onto the screen at a specified location (x, y)
===============
*/
void Draw_SpriteFrame( mspriteframe_t* pFrame, word* pPalette, int x, int y, const wrect_t* prcSubRect )
{
	byte* pSource;
	word* pScreen;
	int	i, j;
	int	width, height;
	int offset;
	int delta;

	offset = SpriteFrameClip(pFrame, &x, &y, &width, &height, prcSubRect);
	pSource = &pFrame->pixels[offset];
	delta = vid.rowbytes >> 1;
	pScreen = (word*)(vid.buffer + y * delta + x * 2);

	for (j = 0; j < height; j++)
	{
		for (i = 0; i < width; i++)
		{
			pScreen[i] = pPalette[pSource[i]];
		}

		pScreen += delta;
		pSource += pFrame->width;
	}
}

/*
===============
Draw_SpriteFrameHoles

Draw normal sprite frame onto the screen at a specified location (x, y)
skip transparent pixels.
===============
*/
void Draw_SpriteFrameHoles( mspriteframe_t* pFrame, word* pPalette, int x, int y, const wrect_t* prcSubRect )
{
	byte* pSource;
	word* pScreen;
	int	i, j;
	int	width, height;
	int offset;
	int delta;

	offset = SpriteFrameClip(pFrame, &x, &y, &width, &height, prcSubRect);
	pSource = &pFrame->pixels[offset];
	delta = vid.rowbytes >> 1;
	pScreen = (word*)(vid.buffer + y * delta + x * 2);

	for (j = 0; j < height; j++)
	{
		for (i = 0; i < width; i++)
		{
			if (pSource[i] != TRANSPARENT_COLOR)
			{
				pScreen[i] = pPalette[pSource[i]];
			}
		}

		pScreen += delta;
		pSource += pFrame->width;
	}
}

/*
===============
Draw_SpriteFrameAdditive

Draw additive sprite frame onto the screen at a specified location (x, y)
using a specified palette.
===============
*/
void Draw_SpriteFrameAdditive( mspriteframe_t* pFrame, word* pPalette, int x, int y, const wrect_t* prcSubRect )
{
	byte* pSource;
	word* pScreen;
	int	width, height;
	int offset;
	int delta;

	offset = SpriteFrameClip(pFrame, &x, &y, &width, &height, prcSubRect);
	pSource = &pFrame->pixels[offset];
	delta = vid.rowbytes >> 1;
	pScreen = (word*)(vid.buffer + y * delta + x * 2);

	if (is15bit)
	{
		Draw_SpriteFrameAdd15(pSource, pPalette, pScreen, width, height, delta, pFrame->width);
	}
	else
	{
		Draw_SpriteFrameAdd16(pSource, pPalette, pScreen, width, height, delta, pFrame->width);
	}
}

/*
===============
Draw_SpriteFrameAdd15

Draw an additive sprite frame using a 15-bit color palette
===============
*/
void Draw_SpriteFrameAdd15( byte* pSource, word* pPalette, word* pScreen, int width, int height, int delta, int sourceWidth )
{
	int			i, j;
	unsigned int oldcolor, newcolor;

	for (j = 0; j < height; j++)
	{
		for (i = 0; i < width; i++)
		{
			oldcolor = pScreen[i];
			newcolor = pPalette[pSource[i]];

			// 15bit additive blending
			if (newcolor != 0)
			{
				unsigned int carrybits;

				const unsigned int highbits = (0x80000000 | 0x00200000 | 0x00000800);
				const unsigned int lowbits = (0x80000000 | 0x00200000 | 0x00000400);
				const unsigned int redblue = (0x7C00 | 0x001F);
				const unsigned int green = (0x03E0);

				// calculate RGB components
				oldcolor = ((oldcolor & redblue) << 16) | (oldcolor & green);
				oldcolor = ((newcolor & redblue) << 16) | (newcolor & green) + oldcolor;

				carrybits = (oldcolor & lowbits);
				if (carrybits)
				{
					// adjust the color
					oldcolor |= lowbits - (carrybits >> 5);
				}

				pScreen[i] = ((oldcolor >> 16) & redblue) | (oldcolor & green);
			}
		}

		pScreen += delta;
		pSource += sourceWidth;
	}
}

/*
===============
Draw_SpriteFrameAdd16

Draw an additive sprite frame using a 16-bit color palette
===============
*/
void Draw_SpriteFrameAdd16( byte* pSource, word* pPalette, word* pScreen, int width, int height, int delta, int sourceWidth )
{
	int			i, j;
	unsigned int oldcolor, newcolor;

	for (j = 0; j < height; j++)
	{
		for (i = 0; i < width; i++)
		{
			oldcolor = pScreen[i];
			newcolor = pPalette[pSource[i]];

			// 16bit additive blending
			if (newcolor != 0)
			{
				unsigned int prevcolor;
				unsigned int carrybits;

				const unsigned int highbits = (0x200000 | 0x000800);
				const unsigned int lowbits = (0x100000 | 0x000400);
				const unsigned int redblue = (0xF800 | 0x001F);
				const unsigned int green = (0x07E0);

				// calculate RGB components
				prevcolor = ((oldcolor & redblue) << 16) | (oldcolor & green);
				oldcolor = (((newcolor & redblue) << 16) | (newcolor & green)) + prevcolor;

				carrybits = (oldcolor & highbits);
				if (prevcolor > oldcolor)
					carrybits |= 1;

				if (carrybits)
				{
					// adjust the color
					carrybits = (carrybits >> 1) | (carrybits << 31);
					oldcolor |= ((carrybits | lowbits) - (carrybits >> 5)) << 1;
				}

				pScreen[i] = ((oldcolor >> 16) & redblue) | (oldcolor & green);
			}
		}

		pScreen += delta;
		pSource += sourceWidth;
	}
}

/*
================
Draw_ConsoleBackground

================
*/
void Draw_ConsoleBackground( int lines )
{
	int				i, x, y, v;
	byte* src;
	byte* palette;
	unsigned short* pusdest;
	int				f, fstep;
	qpic_t* pConBack;
	char			ver[100];
	short			colors[256];

	if (con_loading)
		pConBack = Draw_CachePic("gfx/loading.lmp");
	else
		pConBack = Draw_CachePic("gfx/conback.lmp");

	palette = &pConBack->data[pConBack->width * pConBack->height + 2];

	for (i = 0; i < 256; i++)
	{
		colors[i] = PackedRGB(palette, i);
	}

	// draw the pic
	pusdest = (unsigned short*)vid.conbuffer;

	for (y = 0; y < lines; y++)
	{
		v = (vid.conheight + y - lines) * pConBack->height;
		src = &pConBack->data[pConBack->width * (v / vid.conheight)];

		f = 0;
		fstep = (pConBack->width * 0x10000) / vid.conwidth;
		for (x = 0; x < (int)vid.conwidth; x += 4)
		{
			pusdest[x] = colors[src[f >> 16]];
			f += fstep;
			pusdest[x + 1] = colors[src[f >> 16]];
			f += fstep;
			pusdest[x + 2] = colors[src[f >> 16]];
			f += fstep;
			pusdest[x + 3] = colors[src[f >> 16]];
			f += fstep;
		}

		pusdest = (unsigned short*)((byte*)pusdest + vid.conrowbytes);
	}

	sprintf(ver, "Half-Life 1.0 (build %d)", build_number());

	x = vid.conwidth - Draw_StringLen(ver);
	if (!con_loading && !(giSubState & 4))
	{
		Draw_String(x, 0, ver);
	}
}


/*
==============
R_DrawRect16
==============
*/
void R_DrawRect16( vrect_t* prect, int rowbytes, byte* psrc, byte* palette,
	int transparent )
{
	byte			t;
	int				i, j, srcdelta, destdelta;
	unsigned short* pdest;

	pdest = (unsigned short*)(vid.buffer + (prect->y * (vid.rowbytes >> 1)) + prect->x * 2);

	srcdelta = rowbytes - prect->width;
	destdelta = (vid.rowbytes >> 1) - prect->width;

	if (transparent)
	{
		for (i = 0; i < prect->height; i++)
		{
			for (j = 0; j < prect->width; j++)
			{
				t = *psrc;
				if (t != TRANSPARENT_COLOR)
				{
					*pdest = PackedRGB(palette, t);
				}

				psrc++;
				pdest++;
			}

			psrc += srcdelta;
			pdest += destdelta;
		}
	}
	else
	{
		for (i = 0; i < prect->height; i++)
		{
			for (j = 0; j < prect->width; j++)
			{
				*pdest = PackedRGB(palette, *psrc);
				psrc++;
				pdest++;
			}

			psrc += srcdelta;
			pdest += destdelta;
		}
	}
}


/*
=============
Draw_TileClear

This repeats a 64*64 tile graphic to fill the screen around a sized down
refresh window.
=============
*/
void Draw_TileClear( int x, int y, int w, int h )
{
}


/*
=============
Draw_Fill

Fills a box of pixels with a single color
=============
*/
void Draw_Fill( int x, int y, int w, int h, int c )
{
	unsigned short* pusdest;
	unsigned short	uc;
	int				u, v;

	uc = (unsigned short)c;

	pusdest = (unsigned short*)(vid.buffer + y * (vid.rowbytes >> 1) + x * 2);
	for (v = 0; v < h; v++, pusdest = (unsigned short*)((byte*)pusdest + (vid.rowbytes >> 1)))
	{
		for (u = 0; u < w; u++)
			pusdest[u] = uc;
	}
}
//=============================================================================

/*
================
Draw_FadeScreen

================
*/
void Draw_FadeScreen( void )
{
	int			x, y, w;

	VID_UnlockBuffer();
	S_ExtraUpdate();
	VID_LockBuffer();

	w = vid.width;

	for (y = 0; y < (int)vid.height; y++)
	{
		unsigned short* sbuf = (unsigned short*)(vid.buffer + vid.rowbytes * y);

		for (x = 0; x < w; x++)
		{
			unsigned short color = is15bit ? (*sbuf & 0xEADE) >> 1 : (*sbuf & 0xF7DE) >> 1;
			sbuf[x] = color;
		}
	}

	VID_UnlockBuffer();
	S_ExtraUpdate();
	VID_LockBuffer();
}

//=============================================================================

/*
================
Draw_BeginDisc

Draws the little blue disc in the corner of the screen.
Call before beginning any disc IO.
================
*/
void Draw_BeginDisc( void )
{
	D_BeginDirectRect(vid.width - 24, 0, draw_disc->data, 24, 24);
}


/*
================
Draw_EndDisc

Erases the disc icon.
Call after completing any disc IO
================
*/
void Draw_EndDisc( void )
{
	D_EndDirectRect(vid.width - 24, 0, 24, 24);
}