// r_draw.c

#include "quakedef.h"
#include "d_local.h"	// FIXME: shouldn't need to include this

int			c_faceclip;					// number of faces clipped

clipplane_t	view_clipplanes[4];

int		sintable[SIN_BUFFER_SIZE];
int		intsintable[SIN_BUFFER_SIZE];

/*
================
R_ZDrawSubmodelPolys
================
*/
void R_ZDrawSubmodelPolys( model_t* pmodel )
{
	// TODO: Implement
}