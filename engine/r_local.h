// r_local.h -- private refresh defs

#ifndef GLQUAKE

#include "r_shared.h"
#include "color.h"

#define BMODEL_FULLY_CLIPPED	0x10 // value returned by R_BmodelCheckBBox ()
									 //  if bbox is trivially rejected

//===========================================================================
// clipped bmodel edges

typedef struct bedge_s
{
	mvertex_t* v[2];
	struct bedge_s* pnext;
} bedge_t;

//===========================================================================

typedef struct auxvert_s {
	float	fv[3];		// viewspace x, y
} auxvert_t;

typedef struct alight_s {
	int			ambientlight;	// clip at 128
	int			shadelight;		// clip at 192 - ambientlight
	vec3_t		color;
	float*		plightvec;
} alight_t;

extern	cvar_t	r_cachestudio;
extern	cvar_t	r_draworder;



extern	cvar_t	r_fullbright;
extern	cvar_t	r_decals;



extern	cvar_t	r_ambient_r;
extern	cvar_t	r_ambient_g;
extern	cvar_t	r_ambient_b;



extern	cvar_t	r_mmx;
extern	cvar_t	r_traceglow;
extern	cvar_t	r_wadtextures;



// TODO: Implement


extern	vec3_t	r_entorigin;


// TODO: Implement


extern	int		r_visframecount;

//=============================================================================

//
// current entity info
//
extern	qboolean		insubmodel;
extern	vec3_t			r_worldmodelorg;

// TODO: Implement

//=========================================================
// Alias models
//=========================================================

#define MAXALIASVERTS		2000

// TODO: Implement


extern int		r_amodels_drawn;


// TODO: Implement


extern	float		aliastransform[3][4];
extern	float		aliasxscale, aliasyscale, aliasxcenter, aliasycenter;
extern	int			r_ambientlight;
extern	float		r_shadelight;


extern	vec3_t		r_plightvec;


// TODO: Implement

extern int		r_dlightchanged;	// which ones changed
extern int		r_dlightactive;		// which ones are active



// TODO: Implement


extern mleaf_t* r_viewleaf, * r_oldviewleaf;


// TODO: Implement




colorVec R_LightPoint( vec_t* p );
colorVec R_LightVec( vec_t* start, vec_t* end );




// TODO: Implement

#endif