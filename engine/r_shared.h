#ifndef GLQUAKE
// r_shared.h: general refresh-related stuff shared between the refresh and the
// driver

// FIXME: clean up and move into d_iface.h

#ifndef R_SHARED_H
#define R_SHARED_H
#pragma once

typedef byte pixel_t;


typedef struct
{
	char			name[64];
	cache_user_t	cache;
} cachepic_t;

// FIXME: clean up and move into d_iface.h

// !!! if this is changed, it must be changed in d_ifacea.h too !!!
#define	MAXHEIGHT		1024
#define	MAXWIDTH		1280
#define MAXDIMENSION	((MAXHEIGHT > MAXWIDTH) ? MAXHEIGHT : MAXWIDTH)

#define SIN_BUFFER_SIZE	(MAXDIMENSION+CYCLE)

//===================================================================





extern int	sintable[];
extern int	intsintable[];

extern int		cachewidth;
extern pixel_t*	cacheblock;
extern int		screenwidth;

extern	float	pixelAspect;

extern int		r_drawnpolycount;

extern	vec3_t	vup, base_vup;
extern	vec3_t	vpn, base_vpn;
extern	vec3_t	vright, base_vright;
extern	struct cl_entity_s* currententity;

#define NUMSTACKEDGES		2400
#define	MINEDGES			NUMSTACKEDGES
#define NUMSTACKSURFACES	800
#define MINSURFACES			NUMSTACKSURFACES

// !!! if this is changed, it must be changed in asm_draw.h too !!!
typedef struct espan_s
{
	int				u, v, count;
	struct espan_s* pnext;
} espan_t;

// FIXME: compress, make a union if that will help
// insubmodel is only 1, flags is fewer than 32, spanstate could be a byte
typedef struct surf_s
{
	struct surf_s* next;			// active surface stack in r_edge.c
	struct surf_s* prev;			// used in r_edge.c for active surf stack
	struct espan_s* spans;			// pointer to linked list of spans to draw
	int			key;				// sorting key (BSP order)
	int			last_u;				// set during tracing
	int			spanstate;			// 0 = not in span
									// 1 = in span
									// -1 = in inverted span (end before
									//  start)
	int			flags;				// currentface flags
	void* data;				// associated data like msurface_t
	struct cl_entity_s* entity;
	float		nearzi;				// nearest 1/z on surface, for mipmapping
	qboolean	insubmodel;
	float		d_ziorigin, d_zistepu, d_zistepv;

	int			pad[2];				// to 64 bytes
} surf_t;

extern	surf_t* surfaces, * surface_p, * surf_max;

extern vec3_t	modelorg, base_modelorg;

extern	float	xcenter, ycenter;
extern	float	xscale, yscale;
extern	float	xscaleinv, yscaleinv;
extern	float	xscaleshrink, yscaleshrink;

extern void TransformVector( vec_t* in, vec_t* out );

// !!! if this is changed, it must be changed in asm_draw.h too !!!
#define	NEAR_CLIP	0.01





extern	int d_lightstylevalue[256]; // 8.8 frac of base light value

extern int* pfrustum_indexes[4];

qboolean R_CullBox( vec_t* mins, vec_t* maxs );




// flags in finalvert_t.flags
#define ALIAS_LEFT_CLIP				0x0001
#define ALIAS_TOP_CLIP				0x0002
#define ALIAS_RIGHT_CLIP			0x0004
#define ALIAS_BOTTOM_CLIP			0x0008
#define ALIAS_Z_CLIP				0x0010
// !!! if this is changed, it must be changed in d_ifacea.h too !!!
#define ALIAS_ONSEAM				0x0020	// also defined in modelgen.h;
											//  must be kept in sync
#define ALIAS_XY_CLIP_MASK			0x000F

// !!! if this is changed, it must be changed in asm_draw.h too !!!
typedef struct edge_s
{
	fixed16_t		u;
	fixed16_t		u_step;
	struct edge_s* prev, * next;
	unsigned short	surfs[2];
	struct edge_s* nextremove;
	float			nearzi;
	struct medge_s* owner;
} edge_t;

#endif // GLQUAKE

#endif // R_SHARED_H