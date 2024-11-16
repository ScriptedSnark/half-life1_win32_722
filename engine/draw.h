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
	int				numpaths;
	char** basedirs;
	int* lumppathindices;

#if defined( GLQUAKE )
	int				tempWad;
#endif
} cachewad_t;



void	Draw_Init( void );
void	Draw_Shutdown( void );




#endif // DRAW_H