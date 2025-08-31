// d_scan.c
//
// Portable C scan-level rasterization code, all pixel depths.

#include "quakedef.h"

// In normal mode blending is always applied, otherwise
// if there's a modulation enabled (a flag for env_fade) we don't blend
void D_SetScreenFade( int r, int g, int b, int alpha, int type )
{
	// TODO: Implement
}

/*
=============
D_WarpScreen

// this performs a slight compression of the screen at the same time as
// the sine warp, to keep the edges from wrapping
=============
*/
void D_WarpScreen( void )
{
	// TODO: Implement
}