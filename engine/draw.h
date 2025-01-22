// draw.h -- these are the only functions outside the refresh allowed
// to touch the vid buffer

#ifndef DRAW_H
#define DRAW_H
#ifdef _WIN32
#pragma once
#endif

#if !defined( GLQUAKE )
#include "r_shared.h"
#endif

typedef struct
{
	char			name[64];
	cache_user_t	cache;
} cacheentry_t;

typedef struct cachewad_s cachewad_t;

typedef void (*PFNCACHE)(cachewad_t*, byte*);

typedef struct cachewad_s
{
	char* name;

#if defined( GLQUAKE )
	cacheentry_t* cache;
#else
	cachepic_t* cache;
#endif

	int				cacheCount;
	int				cacheMax;
	lumpinfo_t* lumps;
	int				lumpCount;
	int				cacheExtra;
	PFNCACHE		pfnCacheBuild;

#if defined( GLQUAKE )
	int				tempWad;
#endif
} cachewad_t;

#include "qfont.h"

extern qfont_t* draw_creditsfont;
extern qfont_t* draw_chars;
extern qpic_t* draw_disc;	// also used on sbar
extern qpic_t* draw_backtile;

void	Decal_Init( void );

void	Draw_Init( void );
int		Draw_Character( int x, int y, int num );
void	Draw_DebugChar( char num );
void	Draw_Pic( int x, int y, qpic_t *pic );
void	Draw_AlphaSubPic( int xDest, int yDest, int xSrc, int ySrc, int iWidth, int iHeight, qpic_t* pPic, colorVec* pc, int iAlpha );
void	Draw_AlphaPic( int x, int y, qpic_t* pic, colorVec* pc, int iAlpha );
void	Draw_AlphaAddPic( int x, int y, qpic_t* pic, colorVec* pc, int iAlpha );
void	Draw_TransPic( int x, int y, qpic_t* pic );
void	Draw_TransPicTranslate( int x, int y, qpic_t* pic, unsigned char* translation );
void	Draw_SpriteFrame( struct mspriteframe_s* pFrame, unsigned short* pPalette, int x, int y, const struct wrect_s* prcSubRect );
void	Draw_SpriteFrameHoles( struct mspriteframe_s* pFrame, unsigned short* pPalette, int x, int y, const struct wrect_s* prcSubRect );
void	Draw_SpriteFrameAdditive( struct mspriteframe_s* pFrame, unsigned short* pPalette, int x, int y, const struct wrect_s* prcSubRect );
void	Draw_ConsoleBackground( int lines );
void	Draw_FillRGBA( int x, int y, int w, int h, int r, int g, int b, int a );
void	Draw_TileClear( int x, int y, int w, int h );
void	Draw_Fill( int x, int y, int w, int h, int c );
void	Draw_FadeScreen( void );
void	Draw_BeginDisc( void );
void	Draw_EndDisc( void );
int		Draw_StringLen( char* psz );
int		Draw_MessageFontInfo( short* pWidth );
int		Draw_String( int x, int y, char* str );
qpic_t* Draw_PicFromWad( char* name );
qpic_t* Draw_CachePic( char* path );

void	EnableScissorTest( int x, int y, int width, int height );
void	DisableScissorTest( void );

#endif // DRAW_H