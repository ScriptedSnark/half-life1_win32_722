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








#endif // GL_MODEL_H