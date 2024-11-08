// screen.c -- master for refresh, status bar, console, chat, notify, etc

#include "quakedef.h"

qboolean	scr_skipupdate;

/*
==================
SCR_UpdateScreen

This is called every frame, and can also be called explicitly to flush
text to the screen.

WARNING: be very careful calling this from elsewhere, because the refresh
needs almost the entire 256k of stack space!
==================
*/
void SCR_UpdateScreen( void )
{
	// TODO: Implement
}