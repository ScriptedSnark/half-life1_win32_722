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
ClientDLL_HudVidInit

Called when the game initializes and whenever the vid_mode is changed
 so the HUD can reinitialize itself.
==============
*/
DLL_EXPORT void ClientDLL_HudVidInit( void )
{
	// TODO: Implement
}