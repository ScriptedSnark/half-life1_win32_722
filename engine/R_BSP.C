// r_bsp.c

#include "quakedef.h"

//
// current entity info
//
qboolean		insubmodel;
cl_entity_t* currententity;
vec3_t			modelorg, base_modelorg;
								// modelorg is the viewpoint reletive to
								// the currently rendering entity
vec3_t			r_entorigin;	// the currently rendering entity in world
								// coordinates

float			entity_rotation[3][3];

vec3_t			r_worldmodelorg;

int				r_currentbkey;



/*
================
R_RotateBmodel
================
*/
void R_RotateBmodel( void )
{
	// TODO: Implement
}


/*
================
R_DrawSolidClippedSubmodelPolygons
================
*/
void R_DrawSolidClippedSubmodelPolygons( model_t* pmodel )
{
	// TODO: Implement
}


/*
================
R_DrawSubmodelPolygons
================
*/
void R_DrawSubmodelPolygons( model_t* pmodel, int clipflags )
{
	// TODO: Implement
}