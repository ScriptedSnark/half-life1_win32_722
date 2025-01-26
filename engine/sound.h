// sound.h -- client sound i/o functions
#ifndef SOUND_H
#define SOUND_H
#ifdef _WIN32
#pragma once
#endif

// sound engine rate defines
#define SOUND_DMA_SPEED		22050		// hardware playback rate


// !!! if this is changed, it much be changed in asm_i386.h too !!!
typedef struct
{
	int left;
	int right;
} portable_samplepair_t;

typedef struct
{
	char 	name[MAX_QPATH];
	cache_user_t	cache;
	int		servercount;
} sfx_t;

// !!! if this is changed, it much be changed in asm_i386.h too !!!
typedef struct
{
	int 	length;
	int 	loopstart;
	int 	speed;
	int 	width;
	int 	stereo;
	byte	data[1];		// variable sized
} sfxcache_t;

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

// !!! if this is changed, it much be changed in asm_i386.h too !!!
typedef struct channel_s
{
	sfx_t*	sfx;			// sfx number
	int		leftvol;		// 0-255 volume
	int		rightvol;		// 0-255 volume
	int		end;			// end time in global paintsamples
	int		pos;			// sample position in sfx
	int		looping;		// where to loop, -1 = no looping
	int		entnum;			// to allow overriding a specific sound
	int		entchannel;		// 
	vec3_t	origin;			// origin of sound effect
	vec_t	dist_mult;		// distance multiplier (attenuation/clipK)
	int		master_vol;		// 0-255 master volume
	int		isentence;		// true if playing linked sentence
	int		iword;
	int		pitch;
} channel_t;



void S_Init( void );
void S_Startup( void );
void S_Shutdown( void );
void S_StopAllSounds( qboolean clear );
DLL_EXPORT void S_ClearBuffer( void );
void S_Update( vec_t* origin, vec_t* forward, vec_t* right, vec_t* up );
void S_ExtraUpdate( void );




// initializes cycling through a DMA buffer and returns information on it
qboolean SNDDMA_Init( void );

// gets the current DMA position
int SNDDMA_GetDMAPos( void );

// shutdown the DMA xfer.
DLL_EXPORT void SNDDMA_Shutdown( void );

void SNDDMA_BeginPainting( void );

// ====================================================================
// User-setable variables
// ====================================================================

#define	MAX_CHANNELS			128
#define	MAX_DYNAMIC_CHANNELS	8

extern channel_t    channels[MAX_CHANNELS];
// 0 to MAX_DYNAMIC_CHANNELS-1	= normal entity sounds
// MAX_DYNAMIC_CHANNELS to MAX_DYNAMIC_CHANNELS + NUM_AMBIENTS -1 = water, etc
// MAX_DYNAMIC_CHANNELS + NUM_AMBIENTS to total_channels = static sounds

extern int			total_channels;

//
// Fake dma is a synchronous faking of the DMA progress used for
// isolating performance in the renderer.  The fakedma_updates is
// number of times S_Update() is called per second.
//

extern qboolean 		fakedma;



extern volatile dma_t* shm;
extern volatile dma_t sn;



extern cvar_t snd_show;
extern cvar_t loadas8bit;
extern cvar_t bgmvolume;
extern cvar_t volume;
extern cvar_t suitvolume;
extern cvar_t hisound;

extern qboolean snd_initialized;

extern int snd_blocked;

extern int sound_started;




sfx_t* S_FindName( char* name, int* pfInCache );

void SND_InitScaletable( void );
void SNDDMA_Submit( void );





// DSP Routines

void SX_Init( void );



// WAVE Stream

qboolean Wavstream_Init( void );



DLL_EXPORT void Snd_ReleaseBuffer( void );
DLL_EXPORT void Snd_AcquireBuffer( void );

extern void				VOX_Init( void );




void S_LocalSound( char* s );

#ifdef __USEA3D
extern qboolean snd_isa3d;

extern cvar_t a3d;

extern cvar_t s_buffersize;
extern cvar_t s_rolloff;
extern cvar_t s_doppler;
extern cvar_t s_distance;
extern cvar_t s_automin_distance;
extern cvar_t s_automax_distance;
extern cvar_t s_min_distance;
extern cvar_t s_max_distance;
extern cvar_t s_2dvolume;

// 2.0 Vars

extern cvar_t s_geometry;
extern cvar_t s_leafnum;
extern cvar_t s_blipdir;
extern cvar_t s_refgain;
extern cvar_t s_refdelay;
extern cvar_t s_polykeep;
extern cvar_t s_polysize;
extern cvar_t s_showtossed;
extern cvar_t s_numpolys;
extern cvar_t s_usepvs;
extern cvar_t s_verbwet;
extern cvar_t s_bloat;
extern cvar_t s_reverb;
extern cvar_t s_occlude;
extern cvar_t s_reflect;
extern cvar_t s_materials;
extern cvar_t s_occfactor;
extern cvar_t s_occ_epsilon;
#endif

#endif // SOUND_H