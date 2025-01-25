// snd_dma.c - Main control for any streaming sound output device.

#include "quakedef.h"



// pointer should go away
volatile dma_t* shm = 0;
volatile dma_t sn;



#ifdef __USEA3D
qboolean snd_isa3d;

#endif

/*
================
S_Init
================
*/
void S_Init( void )
{
	Con_DPrintf("Sound Initialization\n");

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