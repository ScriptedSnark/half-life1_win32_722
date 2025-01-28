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

//Con_Printf("S_LoadSound: %x\n", (int)stackbuf);
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

/*
==============
S_LoadStreamSound
==============
*/
sfxcache_t* S_LoadStreamSound( sfx_t* s, channel_t* ch )
{
	char	namebuffer[256];
	char	wavname[MAX_QPATH];
	byte* data;
	wavinfo_t	info;
	sfxcache_t* sc;
	int		cbread = 0;
	int		i;
	int		ffirstload = FALSE;

	if (cl.fPrecaching)
		return NULL;

	Q_strcpy(wavname, s->name + 1);

	// channel index
	i = (ch - channels);

// see if still in memory
	sc = (sfxcache_t*)Cache_Check(&s->cache);
	if (sc)
	{
		if (wavstreams[i].hFile[2] != -1)
			return sc;
	}

//Con_Printf("S_LoadSound: %x\n", (int)stackbuf);
// load it in
	Q_strcpy(namebuffer, "sound");

	if (wavname[0] != '/')
		strcat(namebuffer, "/");
	Q_strcat(namebuffer, wavname);

	if (wavstreams[i].hFile[2] == -1)
		ffirstload = TRUE;

	data = COM_LoadFileLimit(namebuffer, wavstreams[i].lastposloaded, 0x8000, &cbread, wavstreams[i].hFile);
	if (!data)
	{
		Con_DPrintf("Couldn't load %s\n", namebuffer);
		return NULL;
	}

	if (ffirstload)
	{
		info = GetWavinfo(s->name, data, cbread);
		if (info.channels != 1)
		{
			Con_DPrintf("%s is a stereo sample\n", wavname);
			return NULL;
		}

		if (info.width != 1)
		{
			Con_DPrintf("%s is a 16 bit sample\n", wavname);
			return NULL;
		}

		if (shm->speed != info.rate)
		{
			Con_DPrintf("%s ignored, not stored at playback sample rate!\n", wavname);
			return NULL;
		}
	}
	else
	{
		info = wavstreams[i].info;
	}

	wavstreams[i].csamplesinmem = min(info.samples - wavstreams[i].csamplesplayed, wavstreams[i].csamplesinmem);
	wavstreams[i].info = info;

	if (!sc)
	{
		sc = (sfxcache_t*)Cache_Alloc(&s->cache, min(cbread, 0x8000) + sizeof(sfxcache_t), wavname);
		if (!sc)
			return sc;
	}

	sc->length = wavstreams[i].csamplesinmem;
	sc->loopstart = info.loopstart;
	sc->speed = info.rate;
	sc->width = info.width;
	sc->stereo = info.channels;

	if (ffirstload)
		ResampleSfx(s, sc->speed, sc->width, data + info.dataofs);
	else
		ResampleSfx(s, sc->speed, sc->width, data);

	return sc;
}


/*
===============================================================================

WAV loading

===============================================================================
*/


