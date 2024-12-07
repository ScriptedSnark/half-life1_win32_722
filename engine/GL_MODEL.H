// gl_model.h
#if !defined( GL_MODEL_H )
#define GL_MODEL_H
#if defined( _WIN32 )
#pragma once
#endif

#define STUDIO_RENDER 1
#define STUDIO_EVENTS 2

#define MAX_CLIENTS			32
#define	MAX_EDICTS			900

#define MAX_MODEL_NAME		64
#define MAX_MAP_HULLS		4
#define	MIPLEVELS			4
#define	NUM_AMBIENTS		4		// automatic ambient sounds
#define	MAXLIGHTMAPS		4
#define	PLANE_ANYZ			5


typedef struct msurface_s msurface_t;
typedef struct decal_s decal_t;

/*
==============================================================================

BRUSH MODELS

==============================================================================
*/

//
// in memory representation
//
// !!! if this is changed, it must be changed in asm_draw.h too !!!
typedef struct
{
	vec3_t position;
} mvertex_t;

#define	SIDE_FRONT	0
#define	SIDE_BACK	1
#define	SIDE_ON		2
#define	SIDE_CROSS	-2


// plane_t structure
// !!! if this is changed, it must be changed in asm_i386.h too !!!
typedef struct mplane_s
{
	vec3_t normal;
	float dist;
	byte type;			// for texture axis selection and fast side tests
	byte signbits;		// signx + signy<<1 + signz<<1
	byte pad[2];
} mplane_t;

typedef struct texture_s
{
	char		name[16];
	unsigned	width, height;
	int			gl_texturenum;
	struct msurface_s* texturechain;
	int			anim_total;				// total tenths in sequence ( 0 = no)
	int			anim_min, anim_max;		// time for this frame min <=time< max
	struct texture_s*	anim_next;		// in the animation sequence
	struct texture_s*	alternate_anims;	// bmodels in frame 1 use these
	unsigned int offsets[MIPLEVELS];		// four mip maps stored
	byte*		pPal;
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

#define	VERTEXSIZE	7

typedef struct glpoly_s
{
	struct glpoly_s*	next;
	struct glpoly_s*	chain;
	int			numverts;
	int			flags;					// for SURF_UNDERWATER
	float		verts[4][VERTEXSIZE];	// variable sized (xyz s1t1 s2t2)
} glpoly_t;

// JAY: Compress this as much as possible
struct decal_s
{
	struct decal_s*	pnext;			// linked list for each surface
	struct msurface_s* psurface;		// Surface id for persistence / unlinking
	float		dx;				// Offsets into surface texture (in texture coordinates, so we don't need floats)
	float		dy;
	float		scale;			// scale
	short		texture;		// Decal texture
	short		flags;			// Decal flags

	short		entityIndex;	// Entity this is attached to
};

struct msurface_s
{
	int			visframe;		// should be drawn when node is crossed

	mplane_t*	plane;			// pointer to shared plane
	int			flags;			// see SURF_ #defines

	int			firstedge;	// look up in model->surfedges[], negative numbers
	int			numedges;	// are backwards edges

	short		texturemins[2]; // smallest s/t position on the surface.
	short		extents[2];		// ?? s/t texture size, 1..256 for all non-sky surfaces

	int			light_s, light_t;	// gl lightmap coordinates

	glpoly_t*	polys;			// multiple if warped
	msurface_t* texturechain;

	mtexinfo_t* texinfo;

// lighting info
	int			dlightframe;
	int			dlightbits;

	int			lightmaptexturenum;
	byte		styles[MAXLIGHTMAPS]; // index into d_lightstylevalue[] for animated lights 
									  // no one surface can be effected by more than 4 
									  // animated lights.

	int			cached_light[MAXLIGHTMAPS];	// values currently used in lightmap
	qboolean	cached_dlight;				// true if dynamic light in cache
	color24*	samples;		// [numstyles*surfsize]
	decal_t*	pdecals;
};

typedef struct mnode_s
{
// common with leaf
	int			contents;		// 0, to differentiate from leafs
	int			visframe;		// node needs to be traversed if current

	float		minmaxs[6];		// for bounding box culling


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
	int			contents;		// will be a negative contents number
	int			visframe;		// node needs to be traversed if current

	float		minmaxs[6];		// for bounding box culling

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
	float	up, down, left, right;
	int		gl_texturenum;
} mspriteframe_t;



// TODO: Implement



//===================================================================

//
// Whole model
//

#define	EF_ROCKET	1			// leave a trail
#define	EF_GRENADE	2			// leave a trail
#define	EF_GIB		4			// leave a trail
#define	EF_ROTATE	8			// rotate (bonus items)
#define	EF_TRACER	16			// green split trail
#define	EF_ZOMGIB	32			// small blood trail
#define	EF_TRACER2	64			// orange split trail + rotate
#define	EF_TRACER3	128			// purple trail

typedef enum modtype_e
{
	mod_brush,
	mod_sprite,
	mod_alias,
	mod_studio
} modtype_t;

// values for model_t's needload
#define NL_PRESENT		0
#define NL_NEEDS_LOADED	1
#define NL_UNREFERENCED	2

#if !defined( CACHE_USER ) && !defined( QUAKEDEF_H )
#define CACHE_USER
typedef struct cache_user_s
{
	void	*data;
} cache_user_t;
#endif

typedef struct model_s
{
	char		name[MAX_QPATH];
	qboolean	needload;		// bmodels and sprites don't cache normally

	modtype_t	type;
	int			numframes;
	synctype_t	synctype;

	int			flags;

//
// volume occupied by the model graphics
//	
	vec3_t		mins, maxs;
	float		radius;

//
// brush model
//
	int			firstmodelsurface, nummodelsurfaces;

	int			numsubmodels;
	dmodel_t*	submodels;

	int			numplanes;
	mplane_t*	planes;

	int			numleafs;		// number of visible leafs, not counting 0
	mleaf_t*	leafs;

	int			numvertexes;
	mvertex_t*	vertexes;

	int			numedges;
	medge_t*	edges;

	int			numnodes;
	mnode_t*	nodes;

	int			numtexinfo;
	mtexinfo_t* texinfo;

	int			numsurfaces;
	msurface_t* surfaces;

	int			numsurfedges;
	int*		surfedges;

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
	cache_user_t cache;		// only access through Mod_Extradata
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






#endif // GL_MODEL_H