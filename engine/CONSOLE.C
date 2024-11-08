// console.c

#include <fcntl.h>
#include "quakedef.h"


// TODO: Implement


qboolean 	con_forcedup;		// because no entities to refresh

int			con_totallines;		// total lines in console scrollback
int			con_backscroll;		// lines up from bottom to display
int			con_rows = 0;


// TODO: Implement


/*
================
Con_ToggleConsole_f
================
*/
void Con_ToggleConsole_f( void )
{
	// TODO: Implement
}


// TODO: Implement

/*
================
Con_Printf

Handles cursor positioning, line wrapping, etc
================
*/
#define	MAXPRINTMSG	4096
// FIXME: make a buffer size safe vsprintf?
void Con_Printf( char* fmt, ... )
{
	// TODO: Implement
}

/*
================
Con_DPrintf

A Con_Printf that only shows up if the "developer" cvar is set
================
*/
void Con_DPrintf( char* fmt, ... )
{
	// TODO: Implement
}

// TODO: Implement