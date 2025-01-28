// snd_mem.c: sound caching

#include "quakedef.h"

wavinfo_t GetWavinfo( char* name, byte* wav, int wavlength );

int			cache_full_cycle;

byte* S_Alloc( int size );

/*
================
ResampleSfx
================
*/
void ResampleSfx( sfx_t* sfx, int inrate, int inwidth, byte* data )
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
	char	namebuffer[256];
	byte* data;
	wavinfo_t	info;
	int		len;
	float	stepscale;
	sfxcache_t* sc;
	byte	stackbuf[1 * 1024];		// avoid dirtying the cache heap

	if (s->name[0] == CHAR_STREAM)
		return S_LoadStreamSound(s, ch);

// see if still in memory
	sc = (sfxcache_t*)Cache_Check(&s->cache);
	if (sc)
	{
		if (hisound.value || sc->speed <= shm->speed)
			return sc;
		Cache_Free(&s->cache);
	}

//	Con_Printf("S_LoadSound: %x\n", (int)stackbuf);
// load it in
	Q_strcpy(namebuffer, "sound");

	if (s->name[0] != '/')
		strcat(namebuffer, "/");
	Q_strcat(namebuffer, s->name);

//	Con_Printf("loading %s\n",namebuffer);

	data = COM_LoadStackFile(namebuffer, stackbuf, sizeof(stackbuf));
	if (!data)
	{
		Con_DPrintf("Couldn't load %s\n", namebuffer);
		return NULL;
	}

	info = GetWavinfo(s->name, data, com_filesize);
	if (info.channels != 1)
	{
		Con_DPrintf("%s is a stereo sample\n", s->name);
		return NULL;
	}

	if (info.width != 1)
	{
		Con_DPrintf("%s is a 16 bit sample\n", s->name);
		return NULL;
	}

	if (info.rate == shm->speed || (info.rate == 2 * shm->speed && hisound.value > 0.0))
	{
		stepscale = 1;
	}
	else
	{
		stepscale = (float)info.rate / (float)shm->speed;
	}

	len = info.samples / stepscale;

	len = len * info.width * info.channels;

	sc = (sfxcache_t*)Cache_Alloc(&s->cache, len + sizeof(sfxcache_t), s->name);
	if (!sc)
		return NULL;

	sc->length = info.samples;
	sc->loopstart = info.loopstart;
	sc->speed = info.rate;
	sc->width = info.width;
	sc->stereo = info.channels;

	ResampleSfx(s, sc->speed, sc->width, data + info.dataofs);

	return sc;
}