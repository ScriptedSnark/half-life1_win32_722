// sound.h -- client sound i/o functions
#ifndef SOUND_H
#define SOUND_H
#ifdef _WIN32
#pragma once
#endif

// sound engine rate defines
#define SOUND_DMA_SPEED		22050		// hardware playback rate

typedef struct
{
	qboolean		gamealive;
	qboolean		soundalive;
	qboolean		splitbuffer;
	int				channels;
	int				samples;				// mono samples in buffer
	int				submission_chunk;		// don't mix less than this #
	int				samplepos;				// in mono samples
	int				samplebits;
	int				speed;
	int				dmaspeed;
	unsigned char*	buffer;
} dma_t;

void S_Init( void );
void S_Startup( void );
void S_Shutdown( void );
void S_StopAllSounds( qboolean clear );
DLL_EXPORT void S_ClearBuffer( void );
void S_Update( vec_t* origin, vec_t* forward, vec_t* right, vec_t* up );
void S_ExtraUpdate( void );


// shutdown the DMA xfer.
DLL_EXPORT void SNDDMA_Shutdown( void );



// ====================================================================
// User-setable variables
// ====================================================================


extern int			total_channels;

//
// Fake dma is a synchronous faking of the DMA progress used for
// isolating performance in the renderer.  The fakedma_updates is
// number of times S_Update() is called per second.
//



extern volatile dma_t* shm;
extern volatile dma_t sn;




extern int snd_blocked;

extern int sound_started;






DLL_EXPORT void Snd_ReleaseBuffer( void );
DLL_EXPORT void Snd_AcquireBuffer( void );



void S_LocalSound( char* s );

#ifdef __USEA3D
extern qboolean snd_isa3d;

extern	cvar_t	a3d;

#endif

#endif // SOUND_H