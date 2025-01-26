// snd_dma.c - Main control for any streaming sound output device.

#include "quakedef.h"
#include "profile.h"


#if defined (__USEA3D)
#include "a3d.h"
//#include "../a3dwrapper/a3dwrapperDP.h"
#endif

void S_Play( void );
void S_PlayVol( void );
void S_SoundList( void );
void S_Say( void );
void S_Update_( void );
void S_StopAllSounds( qboolean clear );
void S_StopAllSoundsC( void );

#ifdef __USEA3D
void S_enableA3D( void );
void S_disableA3D( void );
#endif

// =======================================================================
// Internal sound data & structures
// =======================================================================

channel_t   channels[MAX_CHANNELS];


int			total_channels;

int				snd_blocked = 0;
static qboolean	snd_ambient = TRUE;
qboolean		snd_initialized = FALSE;

// pointer should go away
volatile dma_t* shm = 0;
volatile dma_t sn;




sfx_t*		known_sfx;		// hunk allocated [MAX_SFX]
int			num_sfx;

sfx_t* ambient_sfx[NUM_AMBIENTS];




int			sound_started = 0;

cvar_t bgmvolume = { "bgmvolume", "1", TRUE };
cvar_t volume = { "volume", "0.7", TRUE };
cvar_t suitvolume = { "suitvolume", "0.25", TRUE };
cvar_t hisound = { "hisound", "1.0", TRUE };

cvar_t nosound = { "nosound", "0" };
cvar_t precache = { "precache", "1" };
cvar_t loadas8bit = { "loadas8bit", "0" };
cvar_t bgmbuffer = { "bgmbuffer", "4096" };
cvar_t ambient_level = { "ambient_level", "0.3" };
cvar_t ambient_fade = { "ambient_fade", "100" };
cvar_t snd_noextraupdate = { "snd_noextraupdate", "0" };
cvar_t snd_show = { "snd_show", "0" };
cvar_t _snd_mixahead = { "_snd_mixahead", "0.1", TRUE };

#ifdef __USEA3D
qboolean snd_isa3d;

cvar_t a3d = { "a3d", "0" };

cvar_t s_buffersize = { "s_buffersize", "65536" };
cvar_t s_rolloff = { "s_rolloff", "1.0", TRUE, FALSE, TRUE };
cvar_t s_doppler = { "s_doppler", "0.0", TRUE };
cvar_t s_distance = { "s_distance", "60", TRUE };
cvar_t s_automin_distance = { "s_automin_distance", "2.0", TRUE };
cvar_t s_automax_distance = { "s_automax_distance", "30.0", TRUE };
cvar_t s_min_distance = { "s_min_distance", "5.0", TRUE };
cvar_t s_max_distance = { "s_max_distance", "1000.0", TRUE };
cvar_t s_2dvolume = { "s_2dvolume", "1.0" };

// A3D 2.0 specific
#ifndef __A3D_GEOM
cvar_t s_geometry = { "s_geometry", "0" };
#else
cvar_t s_geometry = { "s_geometry", "1" };
#endif // __A3D_GEOM
cvar_t s_leafnum = { "s_leafnum", "0", TRUE };
cvar_t s_blipdir = { "s_blipdir", "off" };
cvar_t s_refgain = { "s_refgain", "0.4", TRUE };
cvar_t s_refdelay = { "s_refdelay", "2.5", TRUE };
cvar_t s_polykeep = { "s_polykeep", "1000000000", TRUE };
cvar_t s_polysize = { "s_polysize", "10000000", TRUE };
cvar_t s_showtossed = { "s_showtossed", "0" };
cvar_t s_numpolys = { "s_numpolys", "200", TRUE };
cvar_t s_usepvs = { "s_usepvs", "1" };
cvar_t s_verbwet = { "s_verbwet", "0.15", TRUE };
cvar_t s_bloat = { "s_bloat", "2.0", TRUE };
cvar_t s_reverb = { "s_reverb", "1" };
cvar_t s_occlude = { "s_occlude", "1" };
cvar_t s_reflect = { "s_reflect", "1" };
cvar_t s_materials = { "s_materials", "0" };
cvar_t s_occfactor = { "s_occfactor", "0.3" };
cvar_t s_occ_epsilon = { "s_occ_epsilon", "1.0" };
#endif



// initializes cycling through a DMA buffer and returns information on it
qboolean SNDDMA_Init( void );

// gets the current DMA position
int SNDDMA_GetDMAPos( void );

// shutdown the DMA xfer.
void SNDDMA_Shutdown( void );

void SNDDMA_BeginPainting( void );

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

	VOX_Init();

	if (COM_CheckParm("-nosound"))
		return;

	if (COM_CheckParm("-simsound"))
		fakedma = TRUE;

	Cmd_AddCommand("play", S_Play);
	Cmd_AddCommand("playvol", S_PlayVol);
	Cmd_AddCommand("speak", S_Say);
	Cmd_AddCommand("stopsound", S_StopAllSoundsC);
	Cmd_AddCommand("soundlist", S_SoundList);
	Cmd_AddCommand("soundinfo", S_SoundInfo_f);

#ifdef __USEA3D
	Cmd_AddCommand("enable_a3d", S_enableA3D);
	Cmd_AddCommand("disable_a3d", S_disableA3D);

	Cvar_RegisterVariable(&s_buffersize);
	Cvar_RegisterVariable(&s_rolloff);
	Cvar_RegisterVariable(&s_doppler);
	Cvar_RegisterVariable(&s_distance);
	Cvar_RegisterVariable(&s_automin_distance);
	Cvar_RegisterVariable(&s_automax_distance);
	Cvar_RegisterVariable(&s_min_distance);
	Cvar_RegisterVariable(&s_max_distance);
	Cvar_RegisterVariable(&s_2dvolume);
	Cvar_RegisterVariable(&s_geometry);
	Cvar_RegisterVariable(&s_leafnum);
	Cvar_RegisterVariable(&s_blipdir);
	Cvar_RegisterVariable(&s_refgain);
	Cvar_RegisterVariable(&s_refdelay);
	Cvar_RegisterVariable(&s_polykeep);
	Cvar_RegisterVariable(&s_polysize);
	Cvar_RegisterVariable(&s_numpolys);
	Cvar_RegisterVariable(&s_showtossed);
	Cvar_RegisterVariable(&s_usepvs);
	Cvar_RegisterVariable(&s_bloat);
	Cvar_RegisterVariable(&s_verbwet);
	Cvar_RegisterVariable(&s_reverb);
	Cvar_RegisterVariable(&s_occlude);
	Cvar_RegisterVariable(&s_reflect);
	Cvar_RegisterVariable(&s_materials);
	Cvar_RegisterVariable(&s_occfactor);
	Cvar_RegisterVariable(&s_occ_epsilon);

	Cvar_RegisterVariable(&a3d);
#endif

	Cvar_RegisterVariable(&nosound);
	Cvar_RegisterVariable(&volume);
	Cvar_RegisterVariable(&suitvolume);
	Cvar_RegisterVariable(&hisound);
	Cvar_RegisterVariable(&precache);
	Cvar_RegisterVariable(&loadas8bit);
	Cvar_RegisterVariable(&bgmvolume);
	Cvar_RegisterVariable(&bgmbuffer);
	Cvar_RegisterVariable(&ambient_level);
	Cvar_RegisterVariable(&ambient_fade);
	Cvar_RegisterVariable(&snd_noextraupdate);
	Cvar_RegisterVariable(&snd_show);
	Cvar_RegisterVariable(&_snd_mixahead);

	if (host_parms.memsize < 0x800000)
	{
		Cvar_Set("loadas8bit", "1");
		Con_DPrintf("loading all sounds as 8bit\n");
	}

	if (g_szProfileName)
	{
		char szPath[256];
		char szValue[256];
		sprintf(szPath, "Software\\Valve\\Half-Life\\Player Profiles");
		GetProfileRegKeyValue(g_szProfileName, szPath, "CVAR", "a3d", szValue, sizeof(szValue), "0.0");
		Cvar_Set("a3d", szValue);
	}
	else
	{
		Cvar_Set("a3d", "0");
	}

	snd_initialized = TRUE;

	S_Startup();

	SND_InitScaletable();

	known_sfx = (sfx_t*)Hunk_AllocName(MAX_SFX * sizeof(sfx_t), "sfx_t");
	num_sfx = 0;

// create a piece of DMA memory

	if (fakedma)
	{
		shm = (volatile dma_t*)Hunk_AllocName(sizeof(dma_t), "shm");
		shm->splitbuffer = FALSE;
		shm->samplebits = 16;
		shm->speed = SOUND_DMA_SPEED;
		shm->channels = 2;
		shm->samples = 32768;
		shm->samplepos = 0;
		shm->soundalive = TRUE;
		shm->gamealive = TRUE;
		shm->submission_chunk = 1;
		shm->buffer = (unsigned char*)Hunk_AllocName(1 << 16, "shmbuf");
	}

	Con_DPrintf("Sound sampling rate: %i\n", shm->speed);

	S_StopAllSounds(TRUE);

	SX_Init();

	Wavstream_Init();
}

// =======================================================================
// Shutdown sound engine
// =======================================================================

void S_Shutdown( void )
{
	if (!sound_started)
		return;

	if (shm)
		shm->gamealive = FALSE;

	shm = 0;
	sound_started = FALSE;

	if (!fakedma)
	{
		SNDDMA_Shutdown();
	}
}


// =======================================================================
// Load a sound
// =======================================================================

/*
==================
S_FindName

==================
*/
// Return sfx and set pfInCache to 1 if 
// name is in name cache. Otherwise, alloc
// a new spot in name cache and return 0 
// in pfInCache.
sfx_t* S_FindName( char* name, int* pfInCache )
{
	int		i;
	sfx_t* sfx = NULL;

	if (!name)
		Sys_Error("S_FindName: NULL\n");

	if (Q_strlen(name) >= MAX_QPATH)
		Sys_Error("Sound name too long: %s", name);

// see if already loaded
	for (i = 0; i < num_sfx; i++)
	{
		if (!Q_strcmp(known_sfx[i].name, name))
		{
			sfx = &known_sfx[i];
			if (pfInCache)
			{
				// indicate whether or not sound is currently in the cache.
				*pfInCache = Cache_Check(&sfx->cache) ? 1 : 0;
			}

			if (sfx->servercount > 0)
				sfx->servercount = cl.servercount;

			return sfx;
		}

		if (!sfx)
		{
			if (known_sfx[i].servercount > 0)
			{
				if (known_sfx[i].servercount != cl.servercount)
					sfx = &known_sfx[i];
			}
		}
	}

	if (!sfx)
	{
		if (num_sfx == MAX_SFX)
			Sys_Error("S_FindName: out of sfx_t");

		sfx = &known_sfx[i];
		num_sfx++;
	}
	else
	{
		if (Cache_Check(&sfx->cache))
			Cache_Free(&sfx->cache);
	}

	strcpy(sfx->name, name);

	if (pfInCache)
	{
		*pfInCache = 0;
	}

	sfx->servercount = cl.servercount;
	return sfx;
}

// Used when switching from/to hires sound mode.
// discard sound's data from cache so that
// data will be reloaded (and resampled)
// 'rate' is sound_11k,22k or 44k

void S_FlushSoundData( int fDumpLores, int fDumpHires )
{
	int i, j;
	sfxcache_t* sc;
	channel_t* ch = channels;
	int fNoDiscard = FALSE;

	// scan precache, looking for sounds in memory
	// if a sound is in memory, make sure it's not
	// a block from a streaming sound
	// if not a streaming sound, then discard
	// according to speed

	for (i = 0; i < num_sfx; i++)
	{
		sc = (sfxcache_t*)Cache_Check(&known_sfx[i].cache);
		if (!sc)
			continue;

		// make sure this data is not part of streaming sound
		for (j = 0; j < total_channels; j++, ch++)
		{
			// skip innactive channels
			if (!ch->sfx)
				continue;

			if (ch->entchannel != CHAN_STREAM)
				continue;

			if (ch->sfx == &known_sfx[i])
			{
				fNoDiscard = TRUE;
				break;
			}
		}

		if (fNoDiscard)
		{
			fNoDiscard = FALSE;
			continue;
		}
		// discard all sound data at this 'rate' from cache
		if (fDumpLores)
		{
			if (sc->speed <= shm->speed)
				Cache_Free(&known_sfx[i].cache);
		}

		if (fDumpHires)
		{
			if (sc->speed > shm->speed)
				Cache_Free(&known_sfx[i].cache);
		}
	}
}





// TODO: Implement

void S_StopAllSounds( qboolean clear )
{
	// TODO: Implement
}

void S_StopAllSoundsC( void )
{
	S_StopAllSounds(TRUE);
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




// TODO: Implement




/*
===============================================================================

console functions

===============================================================================
*/

void S_Play( void )
{
	// TODO: Implement
}

void S_PlayVol( void )
{
	// TODO: Implement
}

void S_SoundList( void )
{
	// TODO: Implement
}

// TODO: Implement

// speak a sentence from console; works by passing in "!sentencename"
// or "sentence"

void S_Say( void )
{
	// TODO: Implement
}

void S_ClearPrecache(void)
{
}

void S_BeginPrecaching( void )
{
	cl.fPrecaching = 1;
}

void S_EndPrecaching( void )
{
	cl.fPrecaching = 0;
}