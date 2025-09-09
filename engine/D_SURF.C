// d_surf.c: rasterization driver surface heap manager

#include "quakedef.h"
#include "d_local.h"

qboolean        r_cache_thrash;         // set if surface cache is thrashing
int                                     sc_size;
surfcache_t* sc_rover, * sc_base;

#define GUARDSIZE       4

/*
================
D_InitCaches

================
*/
qboolean is15bit;

void D_InitCaches( void )
{
	// TODO: Implement
}


/*
==================
D_FlushCaches
==================
*/
void D_FlushCaches( void )
{
	// TODO: Implement
}

//=============================================================================

/*
================
D_CacheSurface
================
*/
surfcache_t* D_CacheSurface( msurface_t* surface, int miplevel )
{
	surfcache_t* cache;

	// TODO: Implement

//
// see if the cache holds apropriate data
//
	cache = surface->cachespots[miplevel];

	// TODO: Implement

	return surface->cachespots[miplevel];
}