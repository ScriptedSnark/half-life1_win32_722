//
//  cdll_int.c
//
// 4-23-98  
// JOHN:  implementation of interface between client-side DLL and game engine.
//  The cdll shouldn't have to know anything about networking or file formats.
//  This file is Win32-dependant
//


#include "quakedef.h"
#include "winquake.h"



/*
==============
ClientDLL_Init

Loads the client .dll
==============
*/
void ClientDLL_Init( void )
{
	// TODO: Implement
}

/*
==============
ClientDLL_HudVidInit

Called when the game initializes and whenever the vid_mode is changed
 so the HUD can reinitialize itself.
==============
*/
void ClientDLL_HudVidInit( void )
{
	// TODO: Implement
}






/*
==============
ClientDLL_HudRedraw

Called to redraw the HUD
==============
*/
void ClientDLL_HudRedraw( int intermission )
{
	// TODO: Implement
}

/*
==============
ClientDLL_UpdateClientData

Called every frame while running a map
==============
*/
void ClientDLL_UpdateClientData( void )
{
	// TODO: Implement
}