// snd_mix.c -- portable code to mix sounds for snd_dma.c

#include "quakedef.h"


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


char* rgpszrawsentence[CVOXFILESENTENCEMAX];
int cszrawsentences;


// TODO: Implement