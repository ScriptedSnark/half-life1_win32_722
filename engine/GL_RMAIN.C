// r_main.c

#include "quakedef.h"

// TODO: Implement


int			currenttexture = -1;	// to avoid unnecessary texture sets
int			cnttextures[2] = { -1, -1 };     // cached


//
// screen size info
//
refdef_t	r_refdef;

// TODO: Implement

texture_t*	r_notexture_mip;

// TODO: Implement

void R_SetStackBase( void )
{
	// get stack position so we can guess if we are going to overflow
	//r_stack_start = (byte*)&dummy;
}