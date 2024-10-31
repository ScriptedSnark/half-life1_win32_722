// host.c -- coordinates spawning and killing of local servers

#include "quakedef.h"

jmp_buf 	host_abortserver;

// TODO: Implement

/*
===============
Host_Shutdown

FIXME: this is a callback from Sys_Quit and Sys_Error.  It would be better
to run quit through here before the final handoff to the sys code.
===============
*/
void Host_Shutdown( void )
{
	// TODO: Implement
}

// TODO: Implement