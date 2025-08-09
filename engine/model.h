//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

// model.h
#if !defined( MODEL_H )
#define MODEL_H
#if defined( _WIN32 )
#pragma once
#endif

#include "modelgen.h"
#include "spritegn.h"

/*

d*_t structures are on-disk representations
m*_t structures are in-memory

*/

#include "studio.h"

#define STUDIO_RENDER 1
#define STUDIO_EVENTS 2

#define MAX_CLIENTS			32
#define	MAX_EDICTS			900

#define MAX_MODEL_NAME		64

#define ALIAS_BASE_SIZE_RATIO		(1.0 / 11.0)

// must match definition in modelgen.h
#ifndef SYNCTYPE_T
#define SYNCTYPE_T

enum synctype_t
{
	ST_SYNC = 0,
	ST_RAND
};

#endif

/*
==============================================================================

BRUSH MODELS

==============================================================================
*/

//
// in memory representation
//
// !!! if this is changed, it must be changed in asm_draw.h too !!!
typedef struct mvertex_s
{
	vec3_t		position;
} mvertex_t;

#define	SIDE_FRONT	0
#define	SIDE_BACK	1
#define	SIDE_ON		2
#define	SIDE_CROSS	-2


// plane_t structure
// !!! if this is changed, it must be changed in asm_i386.h too !!!
typedef struct mplane_s
{
	vec3_t	normal;			// surface normal
	float	dist;			// closest appoach to origin
	byte	type;			// for texture axis selection and fast side tests
	byte	signbits;		// signx + signy<<1 + signz<<1
	byte	pad[2];
} mplane_t;

typedef struct texture_s
{
	char		name[16];
	unsigned	width, height;
	int			anim_total;				// total tenths in sequence ( 0 = no)
	int			anim_min, anim_max;		// time for this frame min <=time< max
	struct texture_s*	anim_next;		// in the animation sequence
	struct texture_s*	alternate_anims;	// bmodels in frame 1 use these
	unsigned	offsets[MIPLEVELS];		// four mip maps stored
	unsigned	paloffset;
} texture_t;

#define SURF_PLANEBACK			2
#define SURF_DRAWSKY			4
#define SURF_DRAWSPRITE			8
#define SURF_DRAWTURB			0x10
#define SURF_DRAWTILED			0x20
#define SURF_DRAWBACKGROUND		0x40
#define SURF_UNDERWATER			0x80
#define SURF_DONTWARP			0x100

// !!! if this is changed, it must be changed in asm_draw.h too !!!
typedef struct medge_s
{
	unsigned short	v[2];
	unsigned int	cachededgeoffset;
} medge_t;

typedef struct mtexinfo_s
{
	float		vecs[2][4];		// [s/t] unit vectors in world space. 
								// [i][3] is the s/t offset relative to the origin.
								// s or t = dot(3Dpoint,vecs[i])+vecs[i][3]
	float		mipadjust;		// ?? mipmap limits for very small surfaces
	texture_t*	texture;
	int			flags;			// sky or slime, no lightmap or 256 subdivision
} mtexinfo_t;

typedef struct msurface_s msurface_t;
typedef struct decal_s decal_t;

// JAY: Compress this as much as possible
struct decal_s
{
	struct decal_s*	pnext;			// linked list for each surface
	msurface_t* psurface;		// Surface id for persistence / unlinking
	short		dx;				// Offsets into surface texture (in texture coordinates, so we don't need floats)
	short		dy;
	short		texture;		// Decal texture
	byte		scale;			// scale
	byte		flags;			// Decal flags

	short		entityIndex;	// Entity this is attached to
};

struct msurface_s
{
	int			visframe;		// should be drawn when node is crossed

	int			dlightframe;	// last frame the surface was checked by an animated light
	int			dlightbits;		// dynamically generated. Indicates if the surface illumination 
								// is modified by an animated light.

	mplane_t*	plane;			// pointer to shared plane			
	int			flags;			// see SURF_ #defines

	int			firstedge;	// look up in model->surfedges[], negative numbers
	int			numedges;	// are backwards edges

// surface generation data
	struct surfcache_s* cachespots[MIPLEVELS];

	short		texturemins[2]; // smallest s/t position on the surface.
	short		extents[2];		// ?? s/t texture size, 1..256 for all non-sky surfaces

	mtexinfo_t* texinfo;

// lighting info
	byte		styles[MAXLIGHTMAPS]; // index into d_lightstylevalue[] for animated lights 
									  // no one surface can be effected by more than 4 
									  // animated lights.
	color24*	samples;		// [numstyles*surfsize]
	decal_t*	pdecals;
};

typedef struct mnode_s
{
// common with leaf
	int			contents;		// 0, to differentiate from leafs
	int			visframe;		// node needs to be traversed if current

	short		minmaxs[6];		// for bounding box culling


	struct mnode_s*	parent;

// node specific
	mplane_t*	plane;
	struct mnode_s*	children[2];

	unsigned short		firstsurface;
	unsigned short		numsurfaces;
} mnode_t;

typedef struct mleaf_s
{
// common with node
	int			contents;		// wil be a negative contents number
	int			visframe;		// node needs to be traversed if current

	short		minmaxs[6];		// for bounding box culling

	struct mnode_s*	parent;

// leaf specific
	byte*		compressed_vis;
	struct efrag_s*	efrags;

	msurface_t** firstmarksurface;
	int			nummarksurfaces;
	int			key;			// BSP sequence number for leaf's contents
	byte		ambient_sound_level[NUM_AMBIENTS];
} mleaf_t;

// !!! if this is changed, it must be changed in asm_i386.h too !!!
typedef struct hull_s
{
	dclipnode_t*	clipnodes;
	mplane_t*		planes;
	int				firstclipnode;
	int				lastclipnode;
	vec3_t			clip_mins;
	vec3_t			clip_maxs;
} hull_t;

/*
==============================================================================

SPRITE MODELS

==============================================================================
*/

// FIXME: shorten these?
typedef struct mspriteframe_s
{
	int		width;
	int		height;
	void*	pcachespot;			// remove?
	float	up, down, left, right;
	byte	pixels[4];
} mspriteframe_t;

typedef struct mspritegroup_s
{
	int				numframes;
	float*			intervals;
	struct mspriteframe_s* frames[1];
} mspritegroup_t;

typedef struct mspriteframedesc_s
{
	spriteframetype_t	type;
	struct mspriteframe_s*		frameptr;
} mspriteframedesc_t;

typedef struct msprite_s
{
	short				type;
	short				texFormat;
	int					maxwidth;
	int					maxheight;
	int					numframes;
	int					paloffset;
	float				beamlength;		// remove?
	void*				cachespot;		// remove?
	mspriteframedesc_t	frames[1];
} msprite_t;

/*
==============================================================================

ALIAS MODELS

Alias models are position independent, so the cache manager can move them.
==============================================================================
*/

typedef struct maliasframedesc_s
{
	aliasframetype_t	type;
	trivertx_t			bboxmin;
	trivertx_t			bboxmax;
	int					frame;
	char				name[16];
} maliasframedesc_t;

typedef struct maliasskindesc_s
{
	aliasskintype_t		type;
	void*				pcachespot;
	int					skin;
} maliasskindesc_t;

typedef struct maliasgroupframedesc_s
{
	trivertx_t			bboxmin;
	trivertx_t			bboxmax;
	int					frame;
} maliasgroupframedesc_t;

typedef struct maliasgroup_s
{
	int						numframes;
	int						intervals;
	maliasgroupframedesc_t	frames[1];
} maliasgroup_t;

typedef struct maliasskingroup_s
{
	int					numskins;
	int					intervals;
	maliasskindesc_t	skindescs[1];
} maliasskingroup_t;

// !!! if this is changed, it must be changed in asm_draw.h too !!!
typedef struct mtriangle_s
{
	int					facesfront;
	int					vertindex[3];
} mtriangle_t;

typedef struct aliashdr_s
{
	int					model;
	int					stverts;
	int					skindesc;
	int					triangles;
	int					palette;
	maliasframedesc_t	frames[1];
} aliashdr_t;

//===================================================================

//
// Whole model
//

typedef enum {
	mod_brush,
	mod_sprite,
	mod_alias,
	mod_studio
} modtype_t;

#define	EF_ROCKET	1			// leave a trail
#define	EF_GRENADE	2			// leave a trail
#define	EF_GIB		4			// leave a trail
#define	EF_ROTATE	8			// rotate (bonus items)
#define	EF_TRACER	16			// green split trail
#define	EF_ZOMGIB	32			// small blood trail
#define	EF_TRACER2	64			// orange split trail + rotate
#define	EF_TRACER3	128			// purple trail

// values for model_t's needload
#define NL_PRESENT		0
#define NL_NEEDS_LOADED	1
#define NL_UNREFERENCED	2

#if !defined( CACHE_USER ) && !defined( QUAKEDEF_H )
#define CACHE_USER
typedef struct cache_user_s
{
	void* data;
} cache_user_t;
#endif

typedef struct model_s
{
	char		name[MAX_MODEL_NAME];
	qboolean	needload;		// bmodels and sprites don't cache normally

	modtype_t	type;
	int			numframes;
	synctype_t	synctype;

	int			flags;

//
// volume occupied by the model
//		
	vec3_t		mins, maxs;
	float		radius;

//
// brush model
//
	int			firstmodelsurface, nummodelsurfaces;

	int			numsubmodels;
	dmodel_t* submodels;

	int			numplanes;
	mplane_t* planes;

	int			numleafs;		// number of visible leafs, not counting 0
	struct mleaf_s* leafs;

	int			numvertexes;
	mvertex_t* vertexes;

	int			numedges;
	medge_t* edges;

	int			numnodes;
	mnode_t* nodes;

	int			numtexinfo;
	mtexinfo_t* texinfo;

	int			numsurfaces;
	msurface_t* surfaces;

	int			numsurfedges;
	int* surfedges;

	int			numclipnodes;
	dclipnode_t* clipnodes;

	int			nummarksurfaces;
	msurface_t** marksurfaces;

	hull_t		hulls[MAX_MAP_HULLS];

	int			numtextures;
	texture_t** textures;

	byte*		visdata;
	color24*	lightdata;
	char*		entities;

//
// additional model data
//
	cache_user_t	cache;		// only access through Mod_Extradata
} model_t;

//============================================================================

typedef struct
{
	char*		name;
	short		entityIndex;
	byte		depth;
	byte		flags;
	vec3_t		position;
} DECALLIST;

//============================================================================

void SW_Mod_Init( void );

void Mod_ClearAll( void );
model_t* Mod_ForName( char* name, qboolean crash );
model_t* Mod_FindName( char* name );
void* Mod_Extradata( model_t* mod );	// handles caching
void Mod_TouchModel( char* name );
void Mod_MarkClient( model_t* pModel );

mleaf_t* Mod_PointInLeaf( vec_t* p, model_t* model );

model_t* Mod_LoadModel( model_t* mod, qboolean crash );

void Mod_Print( void );

#endif // MODEL_H