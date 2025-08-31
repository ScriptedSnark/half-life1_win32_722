// r_light.c

#include "quakedef.h"

int	r_dlightframecount;
int	r_dlightchanged;
int r_dlightactive;


/*
==================
R_AnimateLight
==================
*/
void R_AnimateLight( void )
{
	// TODO: Implement
}


/*
=============================================================================

DYNAMIC LIGHTS

=============================================================================
*/

/*
=============
R_MarkLights
=============
*/
void R_MarkLights( dlight_t* light, int bit, mnode_t* node )
{
	// TODO: Implement
}


/*
=============
R_PushDlights
=============
*/
void R_PushDlights( void )
{
	// TODO: Implement
}

colorVec R_LightPoint( vec_t* p )
{
	// TODO: Implement
	colorVec c;
	c.r = c.g = c.b = c.a = 0;
	return c;
}

colorVec R_LightVec( vec_t* start, vec_t* end )
{
	// TODO: Implement
	colorVec c;
	c.r = c.g = c.b = c.a = 0;
	return c;
}