// console.c

#include <fcntl.h>
#include "quakedef.h"


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