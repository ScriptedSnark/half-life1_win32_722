// decal.h
#if !defined( DECAL_H )
#define DECAL_H
#ifdef _WIN32
#pragma once
#endif

#ifndef DRAW_H
#include "draw.h"
#endif

#define MAX_DECALS				4096		// MAX decals in world

#if defined ( GLQUAKE )
#define MIP_EXTRASIZE		32
#else
#define MIP_EXTRASIZE		24
#endif













void		R_DecalRemoveAll( int textureIndex );

extern void					Draw_MiptexTexture( cachewad_t* wad, byte* data );




extern void					Draw_CacheWadInit( char* name, int cacheMax, cachewad_t* wad );
extern void					Draw_CacheWadHandler( cachewad_t* wad, PFNCACHE fn, int extraDataSize );



extern void*				Draw_CacheGet( cachewad_t* wad, int index );

extern int					Draw_CacheIndex( cachewad_t* wad, char* path );

#endif // DECAL_H