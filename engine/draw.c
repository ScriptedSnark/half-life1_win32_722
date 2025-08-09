// draw.c -- this is the only file outside the refresh that touches the
// vid buffer

#include "quakedef.h"
#include "decal.h"

qfont_t* draw_creditsfont;
qfont_t* draw_chars;

cachewad_t	decal_wad;
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
	// TODO: Implement
	return 0;
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
	// TODO: Implement
	return FALSE;
}

qboolean Draw_CacheLoadFromCustom( char* clean, cachewad_t* wad, void *raw, cachepic_t* pic )
{
	// TODO: Implement
	return FALSE;
}

void* Draw_CacheGet( cachewad_t* wad, int index )
{
	// TODO: Implement
	return 0;
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
	// TODO: Implement
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
	// TODO: Implement
}

int Draw_MessageFontInfo( short* pWidth )
{
	// TODO: Implement
	return 0;
}

/*
================
Draw_Character

Draws a single character
================
*/
int Draw_Character( int x, int y, int num )
{
	// TODO: Implement
	return 0;
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
	// TODO: Implement
	return 0;
}

/*
===============
Draw_StringLen
===============
*/
int Draw_StringLen( char* psz )
{
	// TODO: Implement
	return 0;
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
	// TODO: Implement
}

/*
===============
DisableScissorTest
===============
*/
void DisableScissorTest( void )
{
	// TODO: Implement
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
	// TODO: Implement
	return 0;
}

/*
===============
AdjustSubRect
===============
*/
int AdjustSubRect( mspriteframe_t* pFrame, int* pw, int* ph, const wrect_t* prcSubRect )
{
	// TODO: Implement
	return 0;
}

/*
===============
SpriteFrameClip
===============
*/
int SpriteFrameClip( mspriteframe_t* pFrame, int* x, int* y, int* w, int* h, const wrect_t* prcSubRect )
{
	// TODO: Implement
	return 0;
}

/*
===============
Draw_SpriteFrame

Draw normal sprite frame onto the screen at a specified location (x, y)
===============
*/
void Draw_SpriteFrame( mspriteframe_t* pFrame, word* pPalette, int x, int y, const wrect_t* prcSubRect )
{
	// TODO: Implement
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
	// TODO: Implement
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
	// TODO: Implement
}

/*
===============
Draw_SpriteFrameAdd15

Draw an additive sprite frame using a 15-bit color palette
===============
*/
void Draw_SpriteFrameAdd15( byte* pSource, word* pPalette, word* pScreen, int width, int height, int delta, int sourceWidth )
{
	// TODO: Implement
}

/*
===============
Draw_SpriteFrameAdd16

Draw an additive sprite frame using a 16-bit color palette
===============
*/
void Draw_SpriteFrameAdd16( byte* pSource, word* pPalette, word* pScreen, int width, int height, int delta, int sourceWidth )
{
	// TODO: Implement
}

/*
================
Draw_ConsoleBackground

================
*/
void Draw_ConsoleBackground( int lines )
{
	// TODO: Implement
}


/*
==============
R_DrawRect16
==============
*/
void R_DrawRect16( vrect_t* prect, int rowbytes, byte* psrc, byte* palette,
	int transparent )
{
	// TODO: Implement
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
	// TODO: Implement
}
//=============================================================================

/*
================
Draw_FadeScreen

================
*/
void Draw_FadeScreen( void )
{
	// TODO: Implement
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
	// TODO: Implement
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
	// TODO: Implement
}