#include "quakedef.h"


netadr_t	net_local_adr;


netadr_t	net_from;


sizebuf_t	net_message;


qboolean NET_CompareAdr( netadr_t a, netadr_t b )
{
	// TODO: Implement
	return FALSE;
}

char* NET_AdrToString( netadr_t a )
{
	// TODO: Implement
	return NULL;
}


/*
====================
NET_SendPacket
====================
*/
void NET_SendPacket( netsrc_t sock, int length, void* data, netadr_t to )
{
	// TODO: Implement
}



/*
====================
NET_Config

A single player game will only use the loopback code
====================
*/
void NET_Config( qboolean multiplayer )
{
	// TODO: Implement
}



/*
====================
NET_Init

====================
*/
void NET_Init( void )
{
	// TODO: Implement
}


void SCR_InitNetGraph( void )
{
	// TODO: Implement
}