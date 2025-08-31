// d_scan.c
//
// Portable C scan-level rasterization code, all pixel depths.

#include "quakedef.h"

int     d_depth, d_fader, d_fadeg, d_fadeb, d_fadelevel;
float   d_fadestart, d_fadetime;

/*
================
D_BuildFogTable
================
*/
void D_BuildFogTable( qboolean blend )
{
	// TODO: Implement
}

// In normal mode blending is always applied, otherwise
// if there's a modulation enabled (a flag for env_fade) we don't blend
void D_SetScreenFade( int r, int g, int b, int alpha, int type )
{
	// TODO: Implement
}

// Initialize fade globals, build fog table
void D_InitFade( qboolean blend )
{
	d_fadelevel = 128;
	d_fadetime = 0.0;
	d_fadestart = 0.0;
	d_fader = 32;
	d_fadeg = 32;
	d_fadeb = 128;
	D_BuildFogTable(blend);
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