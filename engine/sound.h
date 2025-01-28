// sound.h -- client sound i/o functions
#ifndef SOUND_H
#define SOUND_H
#ifdef _WIN32
#pragma once
#endif

#define	PAINTBUFFER_SIZE	512

// sound engine rate defines
#define SOUND_DMA_SPEED		22050		// hardware playback rate

// sentence groups
#define CBSENTENCENAME_MAX		16
#define CVOXFILESENTENCEMAX		1536		// max number of sentences in game. NOTE: this must match
											// CVOXFILESENTENCEMAX in dlls\util.h!!!

#define CHAR_STREAM			'*'		// as one of 1st 2 chars in name, indicates streaming wav data
#define CHAR_USERVOX		'?'		// as one of 1st 2 chars in name, indicates user realtime voice data
#define CHAR_SENTENCE		'!'		// as one of 1st 2 chars in name, indicates sentence wav
#define CHAR_DRYMIX			'#'		// as one of 1st 2 chars in name, indicates wav bypasses dsp fx

//=====================================================================
// FX presets
//=====================================================================

#define SXROOM_OFF			0		

#define SXROOM_GENERIC		1		// general, low reflective, diffuse room

#define SXROOM_METALIC_S	2		// highly reflective, parallel surfaces
#define SXROOM_METALIC_M	3
#define SXROOM_METALIC_L	4

#define SXROOM_TUNNEL_S		5		// resonant reflective, long surfaces
#define SXROOM_TUNNEL_M		6
#define SXROOM_TUNNEL_L		7

#define SXROOM_CHAMBER_S	8		// diffuse, moderately reflective surfaces
#define SXROOM_CHAMBER_M	9
#define SXROOM_CHAMBER_L	10

#define SXROOM_BRITE_S		11		// diffuse, highly reflective
#define SXROOM_BRITE_M		12
#define SXROOM_BRITE_L		13

#define SXROOM_WATER1		14		// underwater fx
#define SXROOM_WATER2		15
#define SXROOM_WATER3		16

#define SXROOM_CONCRETE_S	17		// bare, reflective, parallel surfaces
#define SXROOM_CONCRETE_M	18
#define SXROOM_CONCRETE_L	19

#define SXROOM_OUTSIDE1		20		// echoing, moderately reflective
#define SXROOM_OUTSIDE2		21		// echoing, dull
#define SXROOM_OUTSIDE3		22		// echoing, very dull

#define SXROOM_CAVERN_S		23		// large, echoing area
#define SXROOM_CAVERN_M		24
#define SXROOM_CAVERN_L		25

#define SXROOM_WEIRDO1		26		
#define SXROOM_WEIRDO2		27
#define SXROOM_WEIRDO3		28

#define CSXROOM				29

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

typedef struct
{
	int		rate;
	int		width;
	int		channels;
	int		loopstart;
	int		samples;
	int		dataofs;		// chunk starts this many bytes from file start
} wavinfo_t;

typedef struct
{
	int		csamplesplayed;
	int		csamplesinmem;
	int		hFile[3];
	wavinfo_t info;
	int		lastposloaded;
} wavstream_t;

typedef struct
{
	int		volume;					// increase percent, ie: 125 = 125% increase
	int		pitch;					// pitch shift up percent
	int		start;					// offset start of wave percent
	int		end;					// offset end of wave percent
	int		cbtrim;					// end of wave after being trimmed to 'end'
	int		fKeepCached;			// 1 if this word was already in cache before sentence referenced it
	int		samplefrac;				// if pitch shifting, this is position into wav * 256
	int		timecompress;			// % of wave to skip during playback (causes no pitch shift)
	sfx_t*	sfx;					// name and cache pointer
} voxword_t;

#define CVOXWORDMAX		32

extern char* rgpparseword[CVOXWORDMAX];
extern voxword_t rgrgvoxword[CBSENTENCENAME_MAX][CVOXWORDMAX];

void S_Init( void );
void S_Startup( void );
void S_Shutdown( void );
void S_StartDynamicSound( int entnum, int entchannel, sfx_t* sfx, vec_t* origin, float fvol, float attenuation, int flags, int pitch );
void S_StartStaticSound( int entnum, int entchannel, sfx_t* sfxin, vec_t* origin, float fvol, float attenuation, int flags, int pitch );
void S_StopSound( int entnum, int entchannel );
void S_StopAllSounds( qboolean clear );
DLL_EXPORT void S_ClearBuffer( void );
void S_Update( vec_t* origin, vec_t* forward, vec_t* right, vec_t* up );
void S_ExtraUpdate( void );

sfx_t* S_PrecacheSound( char* name );
void S_BeginPrecaching( void );
void S_EndPrecaching( void );
void S_PaintChannels( int endtime );

// spatializes a channel
void SND_Spatialize( channel_t* ch );

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
extern int		paintedtime;
extern vec3_t listener_origin;
extern vec3_t listener_forward;
extern vec3_t listener_right;
extern vec3_t listener_up;
extern volatile dma_t* shm;
extern volatile dma_t sn;
extern vec_t sound_nominal_clip_dist;

extern cvar_t snd_show;
extern cvar_t loadas8bit;
extern cvar_t bgmvolume;
extern cvar_t volume;
extern cvar_t suitvolume;
extern cvar_t hisound;

extern qboolean snd_initialized;

extern int snd_blocked;

extern int sound_started;

extern char* rgpszrawsentence[CVOXFILESENTENCEMAX];
extern int cszrawsentences;

void S_LocalSound( char* sound );
sfxcache_t* S_LoadSound( sfx_t* s, channel_t* channel );
sfxcache_t* S_LoadStreamSound( sfx_t* s, channel_t* ch );
sfx_t* S_FindName( char* name, int* pfInCache );

void SND_InitScaletable( void );
void SNDDMA_Submit( void );

void S_AmbientOff( void );
void S_AmbientOn( void );
void S_FreeChannel( channel_t* ch );



extern void SND_CloseMouth( channel_t* ch );
extern void SND_InitMouth( int entnum, int entchannel );



// DSP Routines

void SX_Init( void );


void SX_RoomFX( int count, int fFilter, int fTimefx );

// WAVE Stream

extern wavstream_t wavstreams[MAX_CHANNELS];

qboolean Wavstream_Init( void );
void Wavstream_Close( int i );
void Wavstream_GetNextChunk( channel_t* ch, sfx_t* s );


DLL_EXPORT void Snd_ReleaseBuffer( void );
DLL_EXPORT void Snd_AcquireBuffer( void );

extern void				VOX_Init( void );


extern void				VOX_SetChanVol( channel_t* ch );



extern sfxcache_t*		VOX_LoadSound( channel_t* pchan, char* pszin );

extern void				VOX_MakeSingleWordSentence( channel_t* ch, int pitch );
extern void				VOX_TrimStartEndTimes( channel_t* ch, sfxcache_t* sc );





extern portable_samplepair_t paintbuffer[];
extern portable_samplepair_t drybuffer[];

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