// draw.c -- this is the only file outside the refresh that touches the
// vid buffer

#include "quakedef.h"



qfont_t* draw_chars;
qfont_t* draw_creditsfont;


// This is called to reset all loaded decals
// called from cl_parse.c and host.c
void Decal_Init( void )
{
	// TODO: Implement
}

/*
===============
Draw_Init
===============
*/
void Draw_Init( void )
{
	// TODO: Implement
}



/*
================
Draw_Character

Draws a single character
================
*/
int Draw_Character( int x, int y, int num )
{
	// TODO: Implement
	return 0;
}

/*
================
Draw_String
================
*/
int Draw_String( int x, int y, char* str )
{
	// TODO: Implement
	return 0;
}










void Draw_ConsoleBackground( int lines )
{
	// TODO: Implement
}