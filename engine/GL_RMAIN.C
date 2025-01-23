// r_main.c

#include "quakedef.h"

cl_entity_t	r_worldentity;

// TODO: Implement


int			currenttexture = -1;	// to avoid unnecessary texture sets
int			cnttextures[2] = { -1, -1 };     // cached




int			mirrortexturenum;	// quake texturenum, not gltexturenum
qboolean	mirror;
mplane_t*	mirror_plane;





float	r_world_matrix[16];
float	r_base_world_matrix[16];
float	gProjectionMatrix[16];
float	gWorldToScreen[16];
float	gScreenToWorld[16];

//
// screen size info
//
refdef_t	r_refdef;

mleaf_t*	r_viewleaf, * r_oldviewleaf;

texture_t*	r_notexture_mip;

int			d_lightstylevalue[256];	// 8.8 fraction of base light value

// TODO: Implement

void R_SetStackBase( void )
{
	// get stack position so we can guess if we are going to overflow
	//r_stack_start = (byte*)&dummy;
}

/*
================
R_RenderView

r_refdef must be set before the first call
================
*/
void R_RenderView( void )
{
	// TODO: Implement
}