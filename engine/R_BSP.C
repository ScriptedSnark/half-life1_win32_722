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