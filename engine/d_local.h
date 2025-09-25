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

extern cvar_t	d_subdiv16;

extern float	scale_for_mip;

extern qboolean		d_roverwrapped;
extern surfcache_t* sc_rover;
extern surfcache_t* d_initial_rover;

extern int		d_zidist;

extern float	d_sdivzstepu, d_tdivzstepu, d_zistepu;
extern float	d_sdivzstepv, d_tdivzstepv, d_zistepv;
extern float	d_sdivzorigin, d_tdivzorigin, d_ziorigin;

extern fixed16_t	sadjust, tadjust;
extern fixed16_t	bbextents, bbextentt;

extern void D_DrawSpans8( espan_t* pspans );
extern void D_DrawSpans16( espan_t* pspans );
extern void D_DrawZSpans( espan_t* pspans );
extern void D_DrawTiled8( espan_t* pspan );
extern void D_DrawTiled8Trans( espan_t* pspan );
extern void D_SpriteDrawSpans( sspan_t* pspan );
extern void D_SpriteDrawSpansTrans( sspan_t* pspan );
extern void D_SpriteDrawSpansGlow( sspan_t* pspan );
extern void D_SpriteDrawSpansAlpha( sspan_t* pspan );
extern void D_SpriteDrawSpansAdd( sspan_t* pspan );

void TilingSetup( int sMask, int tMask, int tShift );

surfcache_t* D_CacheSurface( msurface_t* surface, int miplevel );

extern int D_MipLevelForScale( float scale );

#if id386
extern void D_PolysetAff8Start( void );
extern void D_PolysetAff8End( void );
#endif

extern short* d_pzbuffer;
extern unsigned int d_zrowbytes, d_zwidth;

extern int* d_pscantable;
extern int	d_scantable[MAXHEIGHT];

extern int	d_vrectx, d_vrecty, d_vrectright_particle, d_vrectbottom_particle;

extern int	d_vox_min, d_vox_max, d_vrectright_vox, d_vrectbottom_vox;

extern int	d_y_aspect_shift, d_pix_min, d_pix_max, d_pix_shift;

extern pixel_t* d_viewbuffer;

extern short* zspantable[MAXHEIGHT];

extern int		d_minmip;
extern float	d_scalemip[3];

// !!! if this is changed, it must be changed in asm_draw.h/d_polyse.c too !!!
typedef struct {
	void* pdest;
	short* pz;
	int				count;
	byte* ptex;
	int				sfrac, tfrac, light, zi;
} spanpackage_t;

typedef struct {
	int		isflattop;
	int		numleftedges;
	int* pleftedgevert0;
	int* pleftedgevert1;
	int* pleftedgevert2;
	int		numrightedges;
	int* prightedgevert0;
	int* prightedgevert1;
	int* prightedgevert2;
} edgetable;

extern void (*d_drawspans)			(espan_t* pspan);
extern void (*spritedraw)			(sspan_t* pspan);
extern void (*polysetdraw)			(spanpackage_t* pspanpackage);

void D_PolysetDrawSpans8( spanpackage_t* pspanpackage );
void D_PolysetDrawSpansTrans( spanpackage_t* pspanpackage );
void D_PolysetDrawSpansTransAdd( spanpackage_t* pspanpackage );

#endif // D_LOCAL_H