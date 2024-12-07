// cl_main.c  -- client main loop

#include "quakedef.h"
#include "winquake.h"

cvar_t	lookspring = { "lookspring", "0", TRUE };
cvar_t	lookstrafe = { "lookstrafe", "0", TRUE };
cvar_t	sensitivity = { "sensitivity", "3", TRUE };

// TODO: Implement

cvar_t	m_pitch = { "m_pitch", "0.022", TRUE };
cvar_t	m_yaw = { "m_yaw", "0.022", TRUE };
cvar_t	m_forward = { "m_forward", "1", TRUE };
cvar_t	m_side = { "m_side", "0.8", TRUE };


client_static_t	cls;
client_state_t cl;

qboolean cl_inmovie;

/*
=================
CL_UpdateSoundFade

Modulates sound volume on the client.
=================
*/
void CL_UpdateSoundFade( void )
{
	// TODO: Implement
}


/*
=================
CL_ReadPackets

Updates the local time and reads/handles messages on client net connection.
=================
*/
void CL_ReadPackets( void )
{
	// TODO: Implement
}


/*
=================
CL_CheckForResend

Resend a connect message if the last one has timed out
=================
*/
void CL_CheckForResend( void )
{
	// TODO: Implement
}



/*
===============
CL_DecayLights

===============
*/
void CL_DecayLights( void )
{
	// TODO: Implement
}


/*
=================
CL_SendCmd
=================
*/
void CL_SendCmd( void )
{
	// TODO: Implement
}


/*
=================
CL_Init
=================
*/
void CL_Init( void )
{
	// TODO: Implement
}