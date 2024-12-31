// sv_main.c -- server main program

#include "quakedef.h"
#include "decal.h"

server_t		sv;
server_static_t	svs;


// TODO: Implement


globalvars_t gGlobalVariables;

decalname_t	sv_decalnames[MAX_BASE_DECALS];


// TODO: Implement

int			sv_decalnamecount;



// TODO: Implement


/*
==================
SV_CheckTimeouts

If a packet has not been received from a client in sv_timeout.value
seconds, drop the conneciton.

When a client is normally dropped, the client_t goes into a zombie state
for a few seconds to make sure any final reliable message gets resent
if necessary
==================
*/
void SV_CheckTimeouts( void )
{
	// TODO: Implement
}

// TODO: Implement

/*
==================
SV_Init

General initialization of the server
==================
*/
void SV_Init( void )
{
	// TODO: Implement
}

void SV_ClearChannel( qboolean forceclear )
{
	// TODO: Implement; not sure if this function belongs here.
}

/*
================
SV_DeallocateDynamicData

================
*/
void SV_DeallocateDynamicData( void )
{
	// TODO: Implement
}

/*
================
SV_DeallocateDynamicData

================
*/
void SV_ReallocateDynamicData( void )
{
	// TODO: Implement
}