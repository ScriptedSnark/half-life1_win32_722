// snd_mem.c: sound caching

#include "quakedef.h"

qboolean GetWavinfo( char* name, byte* wav, int wavlength, wavinfo_t* info );

int			cache_full_cycle;

byte* S_Alloc( int size );

/*
================
ResampleSfx
================
*/
void ResampleSfx( sfx_t* sfx, int inrate, int inwidth, byte* data, int datasize )
{
	int		outcount;
	int		srcsample;
	float	stepscale;
	int		i;
	int		sample, samplefrac, fracstep;
	sfxcache_t* sc;

	sc = (sfxcache_t*)Cache_Check(&sfx->cache);
	if (!sc)
		return;

	stepscale = (float)inrate / (float)shm->speed;	// this is usually 0.5, 1, or 2

	if (stepscale == 2 && hisound.value)
	{
		outcount = sc->length;
		stepscale = 1;
	}
	else
	{
		outcount = sc->length / stepscale;
		sc->length = outcount;
		if (sc->loopstart != -1)
			sc->loopstart = sc->loopstart / stepscale;

		sc->speed = shm->speed;
	}

	if (loadas8bit.value)
		sc->width = 1;
	else
		sc->width = inwidth;
	sc->stereo = 0;

// resample / decimate to the current source rate

	if (stepscale == 1 && inwidth == 1 && sc->width == 1)
	{
// fast special case
		for (i = 0; i < outcount; i++)
			((signed char*)sc->data)[i] = (int)((unsigned char)(data[i]) - 128);
	}
	else
	{
// general case
		if (stepscale != 1 && stepscale != 2)
			Con_DPrintf("WARNING! %s is causing runtime sample conversion!\n", sfx->name);

		samplefrac = 0;
		fracstep = stepscale * 256;

		for (i = 0; i < outcount; i++)
		{
			srcsample = samplefrac >> 8;
			samplefrac += fracstep;

			if (inwidth == 2)
				sample = LittleShort(((short*)data)[srcsample]);
			else
				sample = (int)((unsigned char)(data[srcsample]) - 128) << 8;

			if (sc->width == 2)
				((short*)sc->data)[i] = sample;
			else
				((signed char*)sc->data)[i] = sample >> 8;
		}
	}
}

//=============================================================================

/*
==============
S_LoadSound
==============
*/
sfxcache_t* S_LoadSound( sfx_t* s, channel_t* ch )
{
	// TODO: Implement
	return NULL;
}