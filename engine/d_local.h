// d_local.h:  private rasterization driver defs
#if !defined( D_LOCAL_H )
#define D_LOCAL_H
#ifdef _WIN32
#pragma once
#endif

#include "r_shared.h"

#define DS_SPAN_LIST_END	-128

#define SURFCACHE_SIZE_AT_320X200		1024 * 1024

typedef struct surfcache_s
{
	struct surfcache_s*		next;
	struct surfcache_s**		owner;		// NULL is an empty chunk of memory
	int					lightadj[MAXLIGHTMAPS]; // checked for strobe flush
	int					dlight;
	int					size;		// including header
	unsigned			width;
	unsigned			height;		// DEBUG only needed for debug
	float				mipscale;
	texture_t*			texture;	// checked for animating textures
	byte				data[4];	// width*height elements
} surfcache_t;

// !!! if this is changed, it must be changed in asm_draw.h too !!!
typedef struct sspan_s
{
	int				u, v, count;
} sspan_t;

extern short* d_pzbuffer;

#endif // D_LOCAL_H