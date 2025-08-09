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






extern	struct cl_entity_s* currententity;








//extern	refdef_t	r_refdef;

//extern	surf_t* surfaces, * surface_p, * surf_max;

extern vec3_t	modelorg, base_modelorg;

extern	float	xcenter, ycenter;
extern	float	xscale, yscale;
extern	float	xscaleinv, yscaleinv;
extern	float	xscaleshrink, yscaleshrink;

extern void TransformVector( vec_t* in, vec_t* out );

// !!! if this is changed, it must be changed in asm_draw.h too !!!
#define	NEAR_CLIP	0.01







qboolean R_CullBox( vec_t* mins, vec_t* maxs );








#endif // GLQUAKE

#endif // R_SHARED_H