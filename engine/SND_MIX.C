// snd_mix.c -- portable code to mix sounds for snd_dma.c

#include "quakedef.h"
#include "winquake.h"
#include "pr_cmds.h"

extern LPDIRECTSOUND pDS;
extern LPDIRECTSOUNDBUFFER pDSBuf, pDSPBuf;

extern DWORD gSndBufSize;


portable_samplepair_t paintbuffer[(PAINTBUFFER_SIZE + 1) * 2];

#if defined (__USEA3D)
#include "a3d.h"
//#include "../a3dwrapper/a3dwrapperDP.h"
#endif

#if defined (__USEA3D) || defined (_ADD_EAX_)
portable_samplepair_t drybuffer[(PAINTBUFFER_SIZE + 1) * 2];
#endif

int			snd_scaletable[32][256];


// TODO: Implement


/*
===============================================================================

CHANNEL MIXING

===============================================================================
*/

// free channel so that it may be allocated by the
// next request to play a sound.  If sound is a 
// word in a sentence, release the sentence.
// Works for static, dynamic, sentence and stream sounds

void S_FreeChannel( channel_t* ch )
{
	// TODO: Implement
}


// TODO: Implement


void S_PaintChannels( int endtime )
{
	// TODO: Implement
}


// TODO: Implement


void SND_InitScaletable( void )
{
	int i, j;

	for (i = 0; i < 32; i++)
		for (j = 0; j < 256; j++)
			snd_scaletable[i][j] = ((signed char)j) * i * 8;
}


// TODO: Implement


//=====================================================================
// Init/release all structures for sound effects
//=====================================================================

void SX_Init( void )
{
	// TODO: Implement
}


// TODO: Implement


// main routine for processing room sound fx
// if fFilter is TRUE, then run in-line filter (for underwater fx)
// if fTimefx is TRUE, then run reverb and delay fx
// NOTE: only processes preset room_types from 0-29 (CSXROOM)

void SX_RoomFX( int count, int fFilter, int fTimefx )
{
	// TODO: Implement
}


// TODO: Implement


//===============================================================================
//
// WAVE Streams
// 
// Routines for CHAN_STREAM sound channel (from static/dynamic areas)
//===============================================================================

// Initialize wavestreams
qboolean Wavstream_Init( void )
{
	// TODO: Implement
	return TRUE;
}


// TODO: Implement


void SND_InitMouth( int entnum, int entchannel )
{
	// TODO: Implement
}


// TODO: Implement


void VOX_Init( void )
{
	// TODO: Implement
}


// TODO: Implement


// set channel volume based on volume of current word

void VOX_SetChanVol( channel_t* ch )
{
	// TODO: Implement
}


// TODO: Implement


void VOX_MakeSingleWordSentence( channel_t* ch, int pitch )
{
	// TODO: Implement
}

// link all sounds in sentence, start playing first word.

sfxcache_t* VOX_LoadSound( channel_t* pchan, char* pszin )
{
	// TODO: Implement
	return NULL;
}


// TODO: Implement


char* rgpszrawsentence[CVOXFILESENTENCEMAX];
int cszrawsentences;


// TODO: Implement


// trim the start and end times of a voice channel's audio data
// based on specified start and end points

void VOX_TrimStartEndTimes( channel_t* ch, sfxcache_t* sc )
{
	// TODO: Implement
}


// TODO: Implement