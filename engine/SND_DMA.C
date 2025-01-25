// snd_dma.c - Main control for any streaming sound output device.

#include "quakedef.h"


#ifdef __USEA3D
void S_enableA3D( void );
void S_disableA3D( void );
#endif

// =======================================================================
// Internal sound data & structures
// =======================================================================




int				snd_blocked = 0;



// pointer should go away
volatile dma_t* shm = 0;
volatile dma_t sn;





int			sound_started = 0;








#ifdef __USEA3D
qboolean snd_isa3d;

cvar_t a3d = { "a3d", "0" };

#endif



qboolean fakedma = FALSE;




/*
================
S_Startup
================
*/

void S_Startup( void )
{
	// TODO: Implement
}

/*
================
S_Init
================
*/
void S_Init( void )
{
	Con_DPrintf("\nSound Initialization\n");

	// TODO: Implement

	if (COM_CheckParm("-nosound"))
		return;

	if (COM_CheckParm("-simsound"))
		fakedma = TRUE;

	// TODO: Implement

#ifdef __USEA3D

	Cmd_AddCommand("enable_a3d", S_enableA3D);
	Cmd_AddCommand("disable_a3d", S_disableA3D);

	// TODO: Implement



	Cvar_RegisterVariable(&a3d);

#endif

	// TODO: Implement
}

// =======================================================================
// Shutdown sound engine
// =======================================================================

void S_Shutdown( void )
{
	// TODO: Implement
}

void S_StopAllSounds( qboolean clear )
{
	// TODO: Implement
}

void S_ClearBuffer( void )
{
	// TODO: Implement
}

/*
============
S_Update

Called once each time through the main loop
============
*/
void S_Update( vec_t* origin, vec_t* forward, vec_t* right, vec_t* up )
{
	// TODO: Implement
}

void S_ExtraUpdate( void )
{
	// TODO: Implement
}

void S_LocalSound( char* sound )
{
	// TODO: Implement
}