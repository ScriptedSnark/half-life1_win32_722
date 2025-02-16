// sv_main.c -- server main program

#include "quakedef.h"

#define CHAN_AUTO   0
#define CHAN_WEAPON 1
#define CHAN_VOICE  2
#define CHAN_ITEM   3
#define CHAN_BODY   4

/*
=============================================================================

Con_Printf redirection

=============================================================================
*/

char	outputbuf[8000];

redirect_t	sv_redirected;

/*
==================
SV_FlushRedirect
==================
*/
void SV_FlushRedirect( void )
{
	// TODO: Implement
}

/*
=================
SV_SendBan
=================
*/
void SV_SendBan( void )
{
	// TODO: Implement
}

/*
=================
SV_FilterPacket
=================
*/
qboolean SV_FilterPacket( void )
{
	// TODO: Implement

	return FALSE;
}

/*
=======================
SV_SendClientMessages
=======================
*/
void SV_SendClientMessages( void )
{
	// TODO: Implement
}