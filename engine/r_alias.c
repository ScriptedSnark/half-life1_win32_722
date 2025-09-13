// r_alias.c: routines for setting up to draw alias models

#include "quakedef.h"
#include "d_local.h"	// FIXME: shouldn't be needed (is needed for patch
						// right now, but that should move)

mtriangle_t* ptriangles;
affinetridesc_t	r_affinetridesc;
#if	id386
void* acolormap;	// FIXME: should go away
#endif
trivertx_t* r_apverts;

// TODO: these probably will go away with optimized rasterization
mdl_t* pmdl;
vec3_t				r_plightvec;
int					r_ambientlight;
float				r_shadelight;
aliashdr_t* paliashdr;
finalvert_t* pfinalverts;
auxvert_t* pauxverts;
float		ziscale;
float		zishift;

//static model_t* pmodel;

//static vec3_t		alias_forward, alias_right, alias_up;

//static maliasskindesc_t* pskindesc;

int				r_amodels_drawn;
int				a_skinwidth;
int				r_anumverts;

float	aliastransform[3][4];

#define NUMVERTEXNORMALS	162

float r_avertexnormals[NUMVERTEXNORMALS][3] = {
#include "anorms.h"
};


/*
================
R_AliasCheckBBox
================
*/
qboolean R_AliasCheckBBox( void )
{
	// TODO: Implement
	return FALSE;
}

/*
================
R_AliasProjectFinalVert
================
*/
void R_AliasProjectFinalVert( finalvert_t* fv, auxvert_t* av )
{
	// TODO: Implement
}

/*
================
R_AliasDrawModel
================
*/
void R_AliasDrawModel( alight_t* plighting )
{
	// TODO: Implement
}