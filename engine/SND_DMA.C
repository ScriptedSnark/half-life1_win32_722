// snd_dma.c - Main control for any streaming sound output device.

#include "quakedef.h"


#if defined (__USEA3D)
#include "a3d.h"
//#include "../a3dwrapper/a3dwrapperDP.h"
#endif

#ifdef __USEA3D
void S_enableA3D( void );
void S_disableA3D( void );
#endif

// =======================================================================
// Internal sound data & structures
// =======================================================================


int			total_channels;

int				snd_blocked = 0;
static qboolean	snd_ambient = TRUE;
qboolean		snd_initialized = FALSE;

// pointer should go away
volatile dma_t* shm = 0;
volatile dma_t sn;





int			sound_started = 0;








#ifdef __USEA3D
qboolean snd_isa3d;

cvar_t a3d = { "a3d", "0" };

#endif



// ====================================================================
// User-setable variables
// ====================================================================


//
// Fake dma is a synchronous faking of the DMA progress used for
// isolating performance in the renderer.  The fakedma_updates is
// number of times S_Update() is called per second.
//

qboolean fakedma = FALSE;
int fakedma_updates = 15;


void S_AmbientOff( void )
{
	snd_ambient = FALSE;
}


void S_AmbientOn( void )
{
	snd_ambient = TRUE;
}


void S_SoundInfo_f( void )
{
	if (!sound_started || !shm)
	{
		Con_Printf("sound system not started\n");
		return;
	}

	Con_Printf("%5d stereo\n", shm->channels - 1);
	Con_Printf("%5d samples\n", shm->samples);
	Con_Printf("%5d samplepos\n", shm->samplepos);
	Con_Printf("%5d samplebits\n", shm->samplebits);
	Con_Printf("%5d submission_chunk\n", shm->submission_chunk);
	Con_Printf("%5d speed\n", shm->speed);
	Con_Printf("0x%x dma buffer\n", shm->buffer);
	Con_Printf("%5d total_channels\n", total_channels);
}


/*
================
S_Startup
================
*/

void S_Startup( void )
{
	int		rc;

	if (!snd_initialized)
		return;

	if (!fakedma)
	{
		rc = SNDDMA_Init();

		if (!rc)
		{
#ifndef	_WIN32
			Con_Printf("S_Startup: SNDDMA_Init failed.\n");
#endif
			sound_started = FALSE;
			return;
		}
	}

	sound_started = TRUE;
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

	Cmd_AddCommand("soundinfo", S_SoundInfo_f);

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