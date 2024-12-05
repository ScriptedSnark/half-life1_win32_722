// view.c -- player eye positioning

#include "quakedef.h"

byte		texgammatable[256];	// palette is sent through this to convert to screen gamma

vec3_t		r_soundOrigin;
vec3_t		r_playerViewportAngles;

void V_StartPitchDrift( void )
{
}

void V_StopPitchDrift( void )
{
}

/*
=============
V_UpdatePalette
=============
*/
#ifdef	GLQUAKE
void V_UpdatePalette( void )
{
	// TODO: Implement
}
#endif


/*
==================
V_RenderView

Render the world
==================
*/
void V_RenderView( void )
{
	// TODO: Implement
}



//============================================================================

/*
=============
V_Init
=============
*/
void V_Init( void )
{
	// TODO: Implement
}