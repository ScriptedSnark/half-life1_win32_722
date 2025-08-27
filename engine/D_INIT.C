// d_init.c: rasterization driver initialization

#include "quakedef.h"
#include "winquake.h"

/*
===============
D_Init
===============
*/
void D_Init( void )
{
	// TODO: Implement
}


/*
===============
D_EnableBackBufferAccess
===============
*/
void D_EnableBackBufferAccess( void )
{
	VID_LockBuffer();
}


/*
===============
D_TurnZOn
===============
*/
void D_TurnZOn( void )
{
// not needed for software version
}


/*
===============
D_DisableBackBufferAccess
===============
*/
void D_DisableBackBufferAccess( void )
{
	VID_UnlockBuffer();
}


// TODO: Implement


/*
===============
D_UpdateRects
===============
*/
void D_UpdateRects( vrect_t *prect )
{

// the software driver draws these directly to the vid buffer

}