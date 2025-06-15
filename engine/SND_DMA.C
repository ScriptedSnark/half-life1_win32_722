// snd_dma.c - Main control for any streaming sound output device.

#include "quakedef.h"
#include "winquake.h"
#include "pr_cmds.h"
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
wavstream_t wavstreams[MAX_CHANNELS];

int			total_channels;

int				snd_blocked = 0;
static qboolean	snd_ambient = TRUE;
qboolean		snd_initialized = FALSE;

// pointer should go away
volatile dma_t* shm = 0;
volatile dma_t sn;

vec3_t		listener_origin;
vec3_t		listener_forward;
vec3_t		listener_right;
vec3_t		listener_up;
vec_t		sound_nominal_clip_dist = 1000.0;

int			soundtime;		// sample PAIRS
int   		paintedtime; 	// sample PAIRS

sfx_t*		known_sfx;		// hunk allocated [MAX_SFX]
int			num_sfx;

sfx_t* ambient_sfx[NUM_AMBIENTS];

int 		desired_speed = 11025;
int 		desired_bits = 16;

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
		Sys_GetProfileRegKeyValue(g_szProfileName, szPath, "CVAR", "a3d", szValue, sizeof(szValue), "0.0");
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


/*
==================
S_TouchSound

==================
*/
void S_TouchSound( char* name )
{
	sfx_t* sfx;

	if (!sound_started)
		return;
	
	sfx = S_FindName(name, NULL);
	Cache_Check(&sfx->cache);
}

/*
==================
S_PrecacheSound

Reserve space for the name of the sound in a global array.
Load the data for the sound and assign a valid pointer,
unless the sound is a streaming or sentence type.  These
defer loading of data until just before playback.
==================
*/
sfx_t* S_PrecacheSound( char* name )
{
	sfx_t* sfx;

	if (!sound_started || nosound.value)
		return NULL;

	if (name[0] == CHAR_STREAM || name[0] == CHAR_SENTENCE)
	{
		// This is a streaming sound or a sentence name.
		// don't actually precache data, just store name

		sfx = S_FindName(name, NULL);
		return sfx;
	}

	// Entity sound.
	sfx = S_FindName(name, NULL);

// cache it in
	if (precache.value)
		S_LoadSound(sfx, NULL);

	return sfx;
}

int SND_FStreamIsPlaying( sfx_t* sfx )
{
	int ch_idx;

	ch_idx = NUM_AMBIENTS;

	for (; ch_idx < total_channels; ch_idx++)
	{
		if (channels[ch_idx].sfx == sfx)
			return TRUE;
	}

	return FALSE;
}

/*
=================
SND_PickDynamicChannel
Select a channel from the dynamic channel allocation area.  For the given entity,
override any other sound playing on the same channel (see code comments below for
exceptions).
=================
*/
channel_t* SND_PickDynamicChannel( int entnum, int entchannel,
#if defined (__USEA3D)
	qboolean bUseAutoSettings,
#endif
	sfx_t* sfx )
{
	int ch_idx;
	int first_to_die;
	int life_left;
#if defined (__USEA3D)
	int ch_first_3d;
#endif

	if (entchannel == CHAN_STREAM && SND_FStreamIsPlaying(sfx))
		return NULL;

	// Check for replacement sound, or find the best one to replace
	first_to_die = -1;
	life_left = 0x7fffffff;

#if defined (__USEA3D)
	ch_first_3d = -1;
#endif

	ch_idx = NUM_AMBIENTS;

	for (; ch_idx < NUM_AMBIENTS + MAX_DYNAMIC_CHANNELS; ch_idx++)
	{
		// Never override a streaming sound that is currently playing or
		// voice over IP data that is playing or any sound on CHAN_VOICE( acting )
		if (channels[ch_idx].entchannel == CHAN_STREAM &&
			wavstreams[ch_idx].hFile[2])
		{
			if (entchannel == CHAN_VOICE)
				return NULL;

			continue;
		}

		if (entchannel != CHAN_AUTO		// channel 0 never overrides
			&& channels[ch_idx].entnum == entnum
			&& (channels[ch_idx].entchannel == entchannel || entchannel == -1))
		{
#if defined (__USEA3D)
			// Want to override same entity sounds also.
			// Aggressively move 2D sounds into 3D.
			if (snd_isa3d)
				ch_first_3d = ch_idx;
			else
				ch_first_3d = -1;
#endif
			// always override sound from same entity
			first_to_die = ch_idx;
			break;
		}

#if defined (__USEA3D)
		// don't let monster sounds override player sounds
		if (channels[ch_idx].entnum == cl.playernum + 1 && entnum != cl.playernum + 1 && channels[ch_idx].sfx)
			continue;
#else
		// don't let monster sounds override player sounds
		if (channels[ch_idx].entnum == cl.viewentity && entnum != cl.viewentity && channels[ch_idx].sfx)
			continue;
#endif

		if (channels[ch_idx].end - paintedtime < life_left)
		{
			life_left = channels[ch_idx].end - paintedtime;
			first_to_die = ch_idx;

#if defined (__USEA3D)
			if (channels[ch_idx].sfx == NULL)
				ch_first_3d = ch_idx;
#endif
		}
	}
	
	if (first_to_die == -1)
		return NULL;

#if defined (__USEA3D)
	if (snd_isa3d && ch_first_3d >= 0)
	{
		first_to_die = ch_first_3d;
		// TODO: Implement
	}
#endif

	if (channels[first_to_die].sfx)
	{
		// be sure and release previous channel
		// if sentence.
		//Con_DPrintf("Stealing channel from %s\n", channels[first_to_die].sfx->name);
		S_FreeChannel(&channels[first_to_die]);
		channels[first_to_die].sfx = NULL;
	}

	return &channels[first_to_die];
}



/*
=====================
SND_PickStaticChannel
=====================
Pick an empty channel from the static sound area, or allocate a new
channel.  Only fails if we're at max_channels (128!!!) or if
we're trying to allocate a channel for a stream sound that is
already playing.

*/
channel_t* SND_PickStaticChannel( int entnum, int entchannel,
#if defined (__USEA3D)
	qboolean bUseAutoSettings,
#endif								  
	sfx_t* sfx )
{
	int i;
	channel_t* ch = NULL;

	if (sfx->name[0] == CHAR_STREAM && SND_FStreamIsPlaying(sfx))
		return NULL;

	// Check for replacement sound, or find the best one to replace
	for (i = NUM_AMBIENTS + MAX_DYNAMIC_CHANNELS; i < total_channels; i++)
		if (channels[i].sfx == NULL)
			break;


	if (i < total_channels)
	{
		// reuse an empty static sound channel
		ch = &channels[i];
	}
	else
	{
		// no empty slots, alloc a new static sound channel
		if (total_channels == MAX_CHANNELS)
		{
			Con_DPrintf("total_channels == MAX_CHANNELS\n");
			return NULL;
		}


		// get a channel for the static sound

		ch = &channels[total_channels];
		total_channels++;
	}

#if defined (__USEA3D)
	if (snd_isa3d)
	{
		// TODO: Implement
	}
#endif

	return ch;
}

/*
=================
SND_Spatialize
=================
*/
void SND_Spatialize( channel_t* ch )
{
	vec_t dot;
	vec_t dist;
	vec_t lscale, rscale, scale;
	vec3_t source_vec;
	sfx_t* snd;
	cl_entity_t* pent;

// anything coming from the view entity will always be full volume
	if (ch->entnum == cl.viewentity)
	{
		ch->leftvol = ch->master_vol;
		ch->rightvol = ch->master_vol;

		// set volume for the current word being spoken
		VOX_SetChanVol(ch);
		return;
	}

	// Make sure index is valid
	if (ch->entnum > 0 && ch->entnum < sv.num_edicts)
	{
		pent = &cl_entities[ch->entnum];

		if (pent && pent->model)
		{
			VectorCopy(pent->origin, ch->origin);

			if (pent->model->type == mod_brush)
			{
				ch->origin[0] += (pent->model->mins[0] + pent->model->maxs[0]) * 0.5;
				ch->origin[1] += (pent->model->mins[1] + pent->model->maxs[1]) * 0.5;
				ch->origin[2] += (pent->model->mins[2] + pent->model->maxs[2]) * 0.5;
			}
		}
	}

// calculate stereo seperation and distance attenuation

	snd = ch->sfx;
	VectorSubtract(ch->origin, listener_origin, source_vec);

#if defined (__USEA3D)
	if (snd_isa3d)
		dist = VectorNormalize(source_vec) * ch->dist_mult * 0.5;
	else
#endif
		dist = VectorNormalize(source_vec) * ch->dist_mult;

	dot = DotProduct(source_vec, listener_right);

	if (shm->channels == 1)
	{
		rscale = 1.0;
		lscale = 1.0;
	}
	else
	{
		rscale = 1.0 + dot;
		lscale = 1.0 - dot;
	}

	// add in distance effect
	scale = (1.0 - dist) * rscale;
	ch->rightvol = (int)(ch->master_vol * scale);

	scale = (1.0 - dist) * lscale;
	ch->leftvol = (int)(ch->master_vol * scale);

	// set the volume if playing a word
	VOX_SetChanVol(ch);

	if (ch->rightvol < 0)
		ch->rightvol = 0;
	if (ch->leftvol < 0)
		ch->leftvol = 0;
}


// search through all channels for a channel that matches this
// entnum, entchannel and sfx, and perform alteration on channel
// as indicated by 'flags' parameter. If shut down request and
// sfx contains a sentence name, shut off the sentence.
// returns TRUE if sound was altered,
// returns FALSE if sound was not found (sound is not playing)

int S_AlterChannel( int entnum, int entchannel, sfx_t* sfx, int vol, int pitch, int flags )
{
	int ch_idx;

	if (sfx->name[0] == CHAR_SENTENCE)
	{
		// This is a sentence name.
		// For sentences: assume that the entity is only playing one sentence
		// at a time, so we can just shut off
		// any channel that has ch->isentence >= 0 and matches the
		// entnum.

		for (ch_idx = NUM_AMBIENTS; ch_idx < total_channels; ch_idx++)
		{
			if (channels[ch_idx].entnum == entnum
				&& channels[ch_idx].entchannel == entchannel
				&& channels[ch_idx].sfx != NULL
				&& channels[ch_idx].isentence >= 0)
			{

				if (flags & SND_CHANGE_PITCH)
					channels[ch_idx].pitch = pitch;

				if (flags & SND_CHANGE_VOL)
					channels[ch_idx].master_vol = vol;

				if (flags & SND_STOP)
				{
					S_FreeChannel(&channels[ch_idx]);
				}

				return TRUE;
			}
		}
		// channel not found
		return FALSE;
	}

	// regular sound or streaming sound

	for (ch_idx = NUM_AMBIENTS; ch_idx < total_channels; ch_idx++)
	{
		if (channels[ch_idx].entnum == entnum
			&& channels[ch_idx].entchannel == entchannel
			&& channels[ch_idx].sfx == sfx)
		{
			if (flags & SND_CHANGE_PITCH)
				channels[ch_idx].pitch = pitch;

			if (flags & SND_CHANGE_VOL)
				channels[ch_idx].master_vol = vol;

			if (flags & SND_STOP)
			{
				S_FreeChannel(&channels[ch_idx]);
			}

			return TRUE;
		}
	}

	return FALSE;
}

// =======================================================================
// S_StartDynamicSound
// =======================================================================
// Start a sound effect for the given entity on the given channel (ie; voice, weapon etc).  
// Try to grab a channel out of the 8 dynamic spots available.
// Currently used for looping sounds, streaming sounds, sentences, and regular entity sounds.
// NOTE: volume is 0.0 - 1.0 and attenuation is 0.0 - 1.0 when passed in.
// Pitch changes playback pitch of wave by % above or below 100.  Ignored if pitch == 100

// NOTE: it's not a good idea to play looping sounds through StartDynamicSound, because
// if the looping sound starts out of range, or is bumped from the buffer by another sound
// it will never be restarted.  Use S_StartStaticSound (pass CHAN_STATIC to EMIT_SOUND or
// SV_StartSound.


void S_StartDynamicSound( int entnum, int entchannel, sfx_t* sfx, vec_t* origin, float fvol, float attenuation, int flags, int pitch )
{
	channel_t* target_chan, * check;
	sfxcache_t* sc;
	int		vol;
	int		ch_idx;
	int		skip;
	int		fsentence = 0;
	
	if (!sound_started)
		return;

	if (!sfx)
		return;

	if (nosound.value)
		return;

	//Con_Printf("Start sound %s\n", sfx->name);
	// override the entchannel to CHAN_STREAM if this is a 
	// stream sound.
	if (sfx->name[0] == CHAR_STREAM)
		entchannel = CHAN_STREAM;

	strstr(sfx->name, "tride/");

	if (entchannel == CHAN_STREAM && pitch != PITCH_NORM)
	{
		Con_DPrintf("Warning: pitch shift ignored on stream sound %s\n", sfx->name);
		pitch = PITCH_NORM;
	}

	vol = fvol * 255;

	if (vol > 255)
	{
		Con_DPrintf("S_StartDynamicSound: %s volume > 255", sfx->name);
		vol = 255;
	}

	if (flags & (SND_STOP | SND_CHANGE_VOL | SND_CHANGE_PITCH))
	{
		if (S_AlterChannel(entnum, entchannel, sfx, vol, pitch, flags))
			return;
		if (flags & SND_STOP)
			return;
		// fall through - if we're not trying to stop the sound, 
		// and we didn't find it (it's not playing), go ahead and start it up
	}

	if (pitch == 0)
	{
		Con_DPrintf("Warning: S_StartDynamicSound Ignored, called with pitch 0");
		return;
	}

	target_chan = SND_PickDynamicChannel(entnum, entchannel, FALSE, sfx);

	if (!target_chan)
		return;

	if (sfx->name[0] == CHAR_SENTENCE)
		fsentence = 1;

// spatialize
	memset(target_chan, 0, sizeof(*target_chan));
	VectorCopy(origin, target_chan->origin);

	// reference_dist / (reference_power_level / actual_power_level)
	target_chan->dist_mult = attenuation / sound_nominal_clip_dist;
	target_chan->master_vol = vol;
	target_chan->entnum = entnum;
	target_chan->entchannel = entchannel;
	target_chan->pitch = pitch;
	target_chan->isentence = -1;
	SND_Spatialize(target_chan);

	// If a client can't hear a sound when they FIRST receive the StartSound message,
	// the client will never be able to hear that sound. This is so that out of 
	// range sounds don't fill the playback buffer.  For streaming sounds, we bypass this optimization.

	if (!target_chan->leftvol && !target_chan->rightvol)
	{
		// if this is a streaming sound, play
		// the whole thing.

		if (entchannel != CHAN_STREAM)
		{
			target_chan->sfx = NULL;
			return;		// not audible at all
		}
	}

	if (fsentence)
	{
		// this is a sentence
		// link all words and load the first word

		// NOTE: sentence names stored in the cache lookup are
		// prepended with a '!'.  Sentence names stored in the
		// sentence file do not have a leading '!'. 

		char name[MAX_QPATH];
		Q_strcpy(name, sfx->name + 1); // skip sentence char

		sc = VOX_LoadSound(target_chan, name);
	}
	else
	{
		// regular or streamed sound fx
		sc = S_LoadSound(sfx, target_chan);
		target_chan->sfx = sfx;
	}

	if (!sc)
	{
		target_chan->sfx = NULL;
		return;		// couldn't load the sound's data
	}

	target_chan->pos = 0;
	target_chan->end = sc->length + paintedtime;

	if (!fsentence && target_chan->pitch != PITCH_NORM)
		VOX_MakeSingleWordSentence(target_chan, target_chan->pitch);

	VOX_TrimStartEndTimes(target_chan, sc);

	// Init client entity mouth movement vars
	SND_InitMouth(entnum, entchannel);

// if an identical sound has also been started this frame, offset the pos
// a bit to keep it from just making the first one louder

// UNDONE: this should be implemented using a start delay timer in the 
// mixer, instead of skipping forward in the wave.  Either method also cause
// phasing problems for skip times of < 10 milliseconds. KB


	check = &channels[NUM_AMBIENTS];
	for (ch_idx = NUM_AMBIENTS; ch_idx < NUM_AMBIENTS + MAX_DYNAMIC_CHANNELS; ch_idx++, check++)
	{
		if (check == target_chan)
			continue;
		if (check->sfx == sfx && !check->pos)
		{
			skip = RandomLong(0, (long)(0.1 * shm->speed));		// skip up to 0.1 seconds of audio
			if (skip >= target_chan->end)
				skip = target_chan->end - 1;
			target_chan->pos += skip;
			target_chan->end -= skip;
			break;
		}

	}
}


/*
=================
S_StartStaticSound
=================
Start playback of a sound, loaded into the static portion of the channel array.
Currently, this should be used for looping ambient sounds, looping sounds
that should not be interrupted until complete, non-creature sentences,
and one-shot ambient streaming sounds.  Can also play 'regular' sounds one-shot,
in case designers want to trigger regular game sounds.
Pitch changes playback pitch of wave by % above or below 100.  Ignored if pitch == 100

  NOTE: volume is 0.0 - 1.0 and attenuation is 0.0 - 1.0 when passed in.
*/
void S_StartStaticSound( int entnum, int entchannel, sfx_t* sfxin, vec_t* origin, float fvol, float attenuation, int flags, int pitch )
{
	channel_t* ch;
	sfxcache_t* sc;
	int fvox = 0;
	sfx_t* sfx = sfxin;
	int vol;

	if (!sfx)
		return;

	// override the entchannel to CHAN_STREAM if this is a stream sound
	if (sfx->name[0] == CHAR_STREAM)
		entchannel = CHAN_STREAM;

	strstr(sfx->name, "tride/");

	if (entchannel == CHAN_STREAM && pitch != PITCH_NORM)
	{
		Con_DPrintf("Warning: pitch shift ignored on stream sound %s\n", sfx->name);
		pitch = PITCH_NORM;
	}

//	Con_Printf("Start static sound %s\n", sfx->name);
	vol = fvol * 255;

	if (vol > 255)
	{
		Con_DPrintf("S_StartStaticSound: %s volume > 255", sfx->name);
		vol = 255;
	}

	if ((flags & SND_STOP) || (flags & SND_CHANGE_VOL) || (flags & SND_CHANGE_PITCH))
	{
		if (S_AlterChannel(entnum, entchannel, sfx, vol, pitch, flags) || (flags & SND_STOP))
			return;
	}

	if (pitch == 0)
	{
		Con_DPrintf("Warning: S_StartStaticSound Ignored, called with pitch 0");
		return;
	}

	ch = SND_PickStaticChannel(entnum, entchannel, TRUE, sfx); // Autolooping sounds are always fixed origin(?)

	if (!ch)
		return;

	if (sfx->name[0] == CHAR_SENTENCE)
	{
		// this is a sentence. link words to play in sequence.

		// NOTE: sentence names stored in the cache lookup are
		// prepended with a '!'.  Sentence names stored in the
		// sentence file do not have a leading '!'. 

		char name[MAX_QPATH];
		Q_strcpy(name, sfx->name + 1); // skip sentence char

		// link all words and load the first word
		sc = VOX_LoadSound(ch, name);
		fvox = 1;
	}
	else
	{
		// load regular or stream sound

		sc = S_LoadSound(sfx, ch);
		ch->sfx = sfx;
		ch->isentence = -1;
	}

	if (!sc)
	{
		ch->sfx = NULL;
		return;
	}

// spatialize

	VectorCopy(origin, ch->origin);

	ch->pos = 0;

	ch->master_vol = vol;
	ch->dist_mult = attenuation / sound_nominal_clip_dist;

	ch->end = sc->length + paintedtime;
	ch->entnum = entnum;
	ch->pitch = pitch;
	ch->entchannel = entchannel;

	if (!fvox && ch->pitch != PITCH_NORM)
		VOX_MakeSingleWordSentence(ch, ch->pitch);

	VOX_TrimStartEndTimes(ch, sc);

	SND_Spatialize(ch);
}

// Stop all sounds for entity on a channel.
void S_StopSound( int entnum, int entchannel )
{
	int i;

	for (i = NUM_AMBIENTS; i < total_channels; i++)
	{
		if (channels[i].entnum == entnum && channels[i].entchannel == entchannel)
		{
			S_FreeChannel(&channels[i]);
		}
	}
}

void S_StopAllSounds( qboolean clear )
{
	int		i;

	if (!sound_started)
		return;

	total_channels = MAX_DYNAMIC_CHANNELS + NUM_AMBIENTS;	// no statics

	for (i = 0; i < MAX_CHANNELS; i++)
	{
		if (channels[i].sfx)
			S_FreeChannel(&channels[i]);
	}

	Q_memset(channels, 0, sizeof(channels));
	Wavstream_Init();

	if (clear)
		S_ClearBuffer();

#if defined (__USEA3D)
	// TODO: Implement
#endif
}

void S_StopAllSoundsC( void )
{
	S_StopAllSounds(TRUE);
}

void S_ClearBuffer( void )
{
	int		clear;

	if (!sound_started || !shm)
		return;

#if defined (__USEA3D)
	if (snd_isa3d)
	{
		// Can't lock if we have no buffer.  This occurs while the
		// main window is deactivated.
		// TODO: Implement
	}
	else
#endif

#ifdef _WIN32
	if ((!shm->buffer && !pDSBuf))
#else
	if (!shm->buffer)
#endif
		return;

	if (shm->samplebits == 8)
		clear = 0x80;
	else
		clear = 0;

#if defined (__USEA3D)
	if (snd_isa3d)
	{
		// TODO: Implement
	}
#endif

#ifdef _WIN32
	if (pDSBuf)
	{
		DWORD	dwSize;
		PVOID	pData;
		int		reps;
		HRESULT	hresult;

		reps = 0;

		while ((hresult = pDSBuf->lpVtbl->Lock(pDSBuf, 0, gSndBufSize, &pData, &dwSize, NULL, NULL, 0)) != DS_OK)
		{
			if (hresult != DSERR_BUFFERLOST)
			{
				Con_Printf("S_ClearBuffer: DS::Lock Sound Buffer Failed\n");
				S_Shutdown();
				return;
			}

			if (++reps > 10000)
			{
				Con_Printf("S_ClearBuffer: DS: couldn't restore buffer\n");
				S_Shutdown();
				return;
			}
		}

		Q_memset(pData, clear, shm->samples * shm->samplebits / 8);

		pDSBuf->lpVtbl->Unlock(pDSBuf, pData, dwSize, NULL, 0);

	}
	else
#endif
	{
		Q_memset(shm->buffer, clear, shm->samples * shm->samplebits / 8);
	}
}

//=============================================================================

/*
===================
S_UpdateAmbientSounds
===================
*/
void S_UpdateAmbientSounds( void )
{
	mleaf_t* l;
	float		vol;
	int			ambient_channel;
	channel_t* chan;

	if (!snd_ambient)
		return;

	// calc ambient sound levels
	if (!cl.worldmodel)
		return;

	l = Mod_PointInLeaf(listener_origin, cl.worldmodel);
	if (!l || !ambient_level.value)
	{
		for (ambient_channel = 0; ambient_channel < NUM_AMBIENTS; ambient_channel++)
			channels[ambient_channel].sfx = NULL;
		return;
	}

	for (ambient_channel = 0; ambient_channel < NUM_AMBIENTS; ambient_channel++)
	{
		chan = &channels[ambient_channel];
		chan->sfx = ambient_sfx[ambient_channel];

		vol = ambient_level.value * l->ambient_sound_level[ambient_channel];
		if (vol < 8)
			vol = 0;

	// don't adjust volume too fast
		if (chan->master_vol < vol)
		{
			chan->master_vol += host_frametime * ambient_fade.value;
			if (chan->master_vol > vol)
				chan->master_vol = vol;
		}
		else if (chan->master_vol > vol)
		{
			chan->master_vol -= host_frametime * ambient_fade.value;
			if (chan->master_vol < vol)
				chan->master_vol = vol;
		}

		chan->leftvol = chan->rightvol = chan->master_vol;
	}
}

/*
============
S_Update

Called once each time through the main loop
============
*/
void S_Update( vec_t* origin, vec_t* forward, vec_t* right, vec_t* up )
{
	int			i;
	int			total;
	channel_t* ch;
	channel_t* combine;

	if (!sound_started || (snd_blocked > 0))
		return;

	VectorCopy(origin, listener_origin);
	VectorCopy(forward, listener_forward);
	VectorCopy(right, listener_right);
	VectorCopy(up, listener_up);

// update general area ambient sound sources
#if defined (__USEA3D)
	if (snd_isa3d)
	{
		// TODO: Implement
	}
	
	if (!snd_isa3d)
	{
		S_UpdateAmbientSounds();
	}
#else
	S_UpdateAmbientSounds();
#endif

	combine = NULL;

// update spatialization for static and dynamic sounds	
	ch = channels + NUM_AMBIENTS;
	for (i = NUM_AMBIENTS; i < total_channels; i++, ch++)
	{
		if (!ch->sfx)
			continue;
		SND_Spatialize(ch);         // respatialize channel
	}
	
//
// debugging output
//
	if (snd_show.value)
	{
		total = 0;
		ch = channels;
		for (i = 0; i < total_channels; i++, ch++)
		{
			if (ch->sfx && (ch->leftvol || ch->rightvol))
			{
				Con_Printf("%3i %3i %s\n", ch->leftvol, ch->rightvol, ch->sfx->name);
				total++;
			}
		}

		Con_Printf("----(%i)----\n", total);
	}

// mix some sound
	S_Update_();
}

void GetSoundtime( void )
{
	int		samplepos;
	static	int		buffers;
	static	int		oldsamplepos;
	int		fullsamples;

	fullsamples = shm->samples / shm->channels;
	
// it is possible to miscount buffers if it has wrapped twice between
// calls to S_Update.  Oh well.
	samplepos = SNDDMA_GetDMAPos();


	if (samplepos < oldsamplepos)
	{
		buffers++;					// buffer wrapped

		if (paintedtime > 0x40000000)
		{	// time to chop things off to avoid 32 bit limits
			buffers = 0;
			paintedtime = fullsamples;
			S_StopAllSounds(TRUE);
		}
	}
	oldsamplepos = samplepos;

	soundtime = buffers * fullsamples + samplepos / shm->channels;
}

void S_ExtraUpdate( void )
{
#ifdef _WIN32
	IN_Accumulate();
#endif

	if (snd_noextraupdate.value)
		return;		// don't pollute timings
	S_Update_();
}

void S_Update_( void )
{
	unsigned        endtime;
	int				samps;

	if (!sound_started || (snd_blocked > 0))
		return;

// Get soundtime, which tells how many samples have
// been played out of the dma buffer since sound system startup.

	GetSoundtime();

// paintedtime indicates how many samples we've actually mixed
// and sent to the dma buffer since sound system startup.

	if (paintedtime < soundtime)
	{
		// if soundtime > paintedtime, then the dma buffer
		// has played out more sound than we've actually
		// mixed.  We need to call S_Update_ more often.

		// Con_DPrintf("S_Update_ : overflow\n"); 
		// paintedtime = soundtime;		

		// (kdb) above code doesn't work - should actually
		// zero out the paintbuffer to advance to the new
		// time.
	}

	// mix ahead of current position
	endtime = soundtime + _snd_mixahead.value * shm->dmaspeed;
	samps = shm->samples >> ((char)shm->channels - 1 & 0x1F);

	if ((int)(endtime - soundtime) > samps)
		endtime = soundtime + samps;

#ifdef __USEA3D
	if (snd_isa3d)
	{
		SNDDMA_BeginPainting();
	}
	else
#endif
	{
#ifdef _WIN32
		DWORD	dwStatus;

		// if the buffer was lost or stopped, restore it and/or restart it
		if (pDSBuf)
		{
			if (pDSBuf->lpVtbl->GetStatus(pDSBuf, &dwStatus) != DS_OK)
				Con_Printf("Couldn't get sound buffer status\n");

			if (dwStatus & DSBSTATUS_BUFFERLOST)
				pDSBuf->lpVtbl->Restore(pDSBuf);

			if (!(dwStatus & DSBSTATUS_PLAYING))
				pDSBuf->lpVtbl->Play(pDSBuf, 0, 0, DSBPLAY_LOOPING);
		}
#endif
	}

	S_PaintChannels(endtime >> 1);

	SNDDMA_Submit();
}

/*
===============================================================================

console functions

===============================================================================
*/

void S_Play( void )
{
	static int hash = 345;
	int 	i;
	char name[256];
	sfx_t* sfx;

	i = 1;
	while (i < Cmd_Argc())
	{
		if (Q_strrchr(Cmd_Argv(i), '.'))
		{
			Q_strcpy(name, Cmd_Argv(i));
		}
		else
		{
			Q_strcpy(name, Cmd_Argv(i));
			Q_strcat(name, ".wav");
		}

		sfx = S_PrecacheSound(name);
		S_StartDynamicSound(hash++, CHAN_AUTO, sfx, listener_origin, VOL_NORM, 1.0, 0, PITCH_NORM);
		i++;
	}
}

void S_PlayVol( void )
{
	static int hash = 543;
	int i;
	float vol;
	char name[256];
	sfx_t* sfx;

	i = 1;
	while (i < Cmd_Argc())
	{
		if (Q_strrchr(Cmd_Argv(i), '.'))
		{
			Q_strcpy(name, Cmd_Argv(i));
		}
		else
		{
			Q_strcpy(name, Cmd_Argv(i));
			Q_strcat(name, ".wav");
		}

		sfx = S_PrecacheSound(name);
		vol = Q_atof(Cmd_Argv(i + 1));
		S_StartDynamicSound(hash++, CHAN_AUTO, sfx, listener_origin, vol, 1.0, 0, PITCH_NORM);
		i += 2;
	}
}

void S_SoundList( void )
{
	int		i;
	sfx_t* sfx;
	sfxcache_t* sc;
	int		size, total;

	total = 0;
	for (sfx = known_sfx, i = 0; i < num_sfx; i++, sfx++)
	{
		sc = (sfxcache_t*)Cache_Check(&sfx->cache);
		if (!sc)
			continue;
		size = sc->length * sc->width * (sc->stereo + 1);
		total += size;
		if (sc->loopstart >= 0)
			Con_Printf("L");
		else
			Con_Printf(" ");
		Con_Printf("(%2db) %6i : %s\n", sc->width * 8, size, sfx->name);
	}
	Con_Printf("Total resident: %i\n", total);
}

void S_LocalSound( char* sound )
{
	sfx_t* sfx;

	if (nosound.value)
		return;
	if (!sound_started)
		return;

	sfx = S_PrecacheSound(sound);
	if (!sfx)
	{
		Con_Printf("S_LocalSound: can't cache %s\n", sound);
		return;
	}
	S_StartDynamicSound(cl.viewentity, -1, sfx, vec3_origin, VOL_NORM, 1.0, 0, PITCH_NORM);
}

// speak a sentence from console; works by passing in "!sentencename"
// or "sentence"

void S_Say( void )
{
	sfx_t* sfx;
	char sound[256];

	if (nosound.value)
		return;

	if (!sound_started)
		return;

	Q_strcpy(sound, Cmd_Argv(1));

	// DEBUG - test performance of dsp code
	if (!Q_strcmp(sound, "dsp"))
	{
		unsigned time;
		int i;
		int count = 10000;

		for (i = 0; i < PAINTBUFFER_SIZE; i++)
		{
			paintbuffer[i].left = RandomLong(0, 2999);
			paintbuffer[i].right = RandomLong(0, 2999);
		}

		Con_Printf("Start profiling 10,000 calls to DSP\n");

		// get system time

		time = timeGetTime();

		for (i = 0; i < count; i++)
		{
			SX_RoomFX(PAINTBUFFER_SIZE, TRUE, TRUE);
		}
		// display system time delta 
		Con_Printf("%d milliseconds \n", timeGetTime() - time);
		return;
	}
	else if (!Q_strcmp(sound, "paint"))
	{
		unsigned time;
		int count = 10000;
		static int hash = 543;
		sfx_t* sfx;
		int psav = paintedtime;

		Con_Printf("Start profiling S_PaintChannels\n");

		sfx = S_PrecacheSound("ambience/labdrone1.wav");
		S_StartDynamicSound(hash++, CHAN_AUTO, sfx, listener_origin, VOL_NORM, 1.0, 0, PITCH_NORM);

		// get system time
		time = timeGetTime();

		// paint a boatload of sound

		S_PaintChannels(paintedtime + 512 * count);

		// display system time delta 
		Con_Printf("%d milliseconds \n", timeGetTime() - time);
		paintedtime = psav;
		return;
	}
	// DEBUG

	if (sound[0] != CHAR_SENTENCE)
	{
		// build a fake sentence name, then play the sentence text

		Q_strcpy(sound, "xxtestxx ");
		Q_strcat(sound, Cmd_Argv(1));

		// insert null terminator after sentence name
		sound[8] = 0;

		rgpszrawsentence[cszrawsentences] = sound;
		cszrawsentences++;

		sfx = S_PrecacheSound("!xxtestxx");
		if (!sfx)
		{
			Con_Printf("S_Say: can't cache %s\n", sound);
			return;
		}

		S_StartStaticSound(-2, CHAN_STATIC, sfx, vec3_origin, VOL_NORM, 1.0, 0, PITCH_NORM);

		// remove last
		rgpszrawsentence[--cszrawsentences] = NULL;
	}
	else
	{
		sfx = S_FindName(sound, NULL);
		if (!sfx)
		{
			Con_Printf("S_Say: can't find sentence name %s\n", sound);
			return;
		}

		S_StartStaticSound(-2, CHAN_STATIC, sfx, vec3_origin, VOL_NORM, 1.0, 0, PITCH_NORM);
	}
}

void S_ClearPrecache( void )
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