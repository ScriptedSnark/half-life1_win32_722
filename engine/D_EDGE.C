// d_edge.c

#include "quakedef.h"
#include "d_local.h"

static int	miplevel;

float		scale_for_mip;

int			ubasestep, errorterm, erroradjustup, erroradjustdown;


// FIXME: should go away
extern void			R_RotateBmodel( void );
extern void			R_TransformFrustum( void );

vec3_t		transformed_modelorg;

/*
==============
D_DrawPoly

==============
*/
void D_DrawPoly( void )
{
// this driver takes spans, not polygons
}

/*
==============
D_DrawSurfaces
==============
*/
void D_DrawSurfaces( void )
{
	// TODO: Implement
}