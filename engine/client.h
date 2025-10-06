// client.h

#include "wrect.h"
#include "cdll_int.h"
#include "cshift.h"

typedef struct
{
	int		length;
	char	map[MAX_STYLESTRING];
} lightstyle_t;

typedef struct screenfade_s
{
	float		fadeSpeed;		// How fast to fade (tics / second) (+ fade in, - fade out)
	float		fadeEnd;		// When the fading hits maximum
	float		fadeReset;		// When to reset to not fading (for fadeout and hold)
	byte		fader, fadeg, fadeb, fadealpha;	// Fade color
	int			fadeFlags;		// Fading flags
} screenfade_t;

#define MAX_DL_STATS	8
typedef struct
{
	qboolean bUsed;
	float fTime;
	int nBytesRemaining;
} downloadtime_t;

#define	MAX_SCOREBOARDNAME		32
typedef struct player_info_s
{
	// User id on server
	int		userid;

	// Name
	char	name[MAX_SCOREBOARDNAME];
	int		ping;

	// skin information
	int		color;

	int		spectator;
	byte	translations[VID_GRADES * 256];

	float	maxspeed;

	customization_t customdata;
} player_info_t;

//
// client_state_t should hold all pieces of the client state
//

#define	SIGNONS		3			// signon messages to receive before connected

#define	MAX_DLIGHTS		32
#define	MAX_ELIGHTS		64		// entity only point lights
typedef struct dlight_s
{
	vec3_t	origin;
	float	radius;
	color24	color;
	float	die;				// stop lighting after this time
	float	decay;				// drop this each second
	float	minlight;			// don't add when contributing less
	int		key;
	qboolean	dark;			// subtracts light instead of adding
} dlight_t;

#define	MAX_EFRAGS		640

#define	MAX_MAPSTRING	2048
#define	MAX_DEMOS		32
#define	MAX_DEMONAME	16

typedef enum
{
	ca_dedicated = 0,	// This is a dedicated server, client code is inactive
	ca_disconnected,	// full screen console with no connection
	ca_connecting,		// netchan_t established, waiting for svc_serverdata
	ca_connected,		// processing data lists, donwloading, etc
	ca_uninitialized,	// valid netcon, autodownloading
	ca_active			// everything is in, so frames can be rendered
} cactive_t;

// Structure used for fading in and out client sound volume.
typedef struct soundfade_s
{
	int		nStartPercent;

	// How far to adjust client's volume down by.
	int		nClientSoundFadePercent;

	// realtime when we started adjusting volume
	double	soundFadeStartTime;

	// # of seconds to get to faded out state
	int		soundFadeOutTime;
	// # of seconds to hold
	int		soundFadeHoldTime;
	// # of seconds to restore
	int		soundFadeInTime;
} soundfade_t;

//
// the client_static_t structure is persistant through an arbitrary number
// of server connections
//
typedef struct
{
// connection information
	cactive_t	state;

// network stuff
	netchan_t	netchan;

// Connection to server.
	double		connect_time;		// If gap of connect_time to realtime > 3000, then resend connect packet

	int			connect_retry;      // After CL_CONNECTION_RETRIES, give up...

	int			challenge;			// from the server to use for connecting

	qboolean	spectator;			// TRUE if connected as spectator

	char		trueaddress[32];

	float		slist_time;

// connection information
	int			signon;			// 0 to SIGNONS

	char		servername[MAX_OSPATH];	// name of server from original connect
	char		mapstring[MAX_QPATH];

	char		spawnparms[MAX_MAPSTRING];

// demo loop control
	int			demonum;							// -1 = don't play demos
	char		demos[MAX_DEMOS][MAX_DEMONAME];	// when not playing

	// demo recording info must be here, because record is started before
	// entering a map (and clearing client_state_t)
	qboolean	demorecording;
	qboolean	demoplayback;
	qboolean	timedemo;

	float		demostarttime;
	int			demostartframe;

	int			forcetrack;			// -1 = use normal cd track

	FILE*		demofile;			// For recording demos.
	FILE*		demoheader;			// For saving startup data to start playing a demo midstream.

	// I.e., demo is waiting for first nondeltacompressed message to arrive.
	//  We don't actually start to record until a non-delta message is received
	qboolean	demowaiting;
	qboolean	demoappending;

	qboolean	demomaxdelayexceeded;	// Set when demo playback lags(> 0.1s behind)
	
	// Name of demo file we are appending onto.
	char		demofilename[MAX_OSPATH];

	qboolean	demoupdateentities;	// TRUE if entities need to be updated while playing the demo

	int			demoframecount;		// # of demo frames in the segment.

	float		demototaltimediff;		// Accumulated time difference for demo sync correction
	int			demonorewinds;			// Count of dem_norewind commands in current demo segment
	int			demoskippedmessages;	// Messages deferred due to timing constraints
	float		democurrenttimediff;	// Current delta between playback and demo time

	int			td_lastframe;		// to meter out one message a frame
	int			td_startframe;		// host_framecount at start
	float		td_starttime;		// realtime at second frame of timedemo

	FILE* download;						// file transfer from server
	resource_t* downloadresource;		// the resource we're trying to retrieve from server
	qboolean	doneregistering;
	char		downloadtempname[MAX_QPATH];
	CRC32_t		downloadfinalCRC;
	char		downloadname[MAX_QPATH];
	int			nFilesDownloaded;
	int			downloadpercent;
	qboolean	downloadinprogress;		// TRUE if downloading is in progress

	int			nTotalSize;
	int			nTotalToTransfer;
	int			nRemainingToTransfer;

	float		fLastStatusUpdate;		// The time of the last download status
	float		fLastDownloadTime;		// The last time the file was downloaded

	downloadtime_t rgDownloads[MAX_DL_STATS];
	int			downloadnumber;

	CRC32_t		downloadcurrentCRC;

	qboolean	custom;					// TRUE is downloading a custom resource

	FILE*		upload;					// Handle of the file being uploaded
	int			uploadsize;
	int			uploadpos;
	qboolean	uploading;
	CRC32_t		uploadCRC;

	float		latency;		// rolling average

	soundfade_t soundfade;			// Client sound fading object
} client_static_t;

extern client_static_t	cls;

// player_state_t is the information needed by a player entity
// to do move prediction and to generate a drawable entity
typedef struct
{
	int			messagenum;		// all player's won't be updated each frame

	double		state_time;		// not the same as the packet time,
								// because player commands come asyncronously
	float		received_time;	// timestamp when the state was received

	usercmd_t	command;		// last command for prediction

	byte		number;

	vec3_t		prevorigin;		// last predicted origin
	vec3_t		predorigin;		// delta

	vec3_t		origin;
	vec3_t		viewangles;		// only for demos, not from server
	vec3_t		velocity;
	int			weaponframe;

	int			movetype;

	int			modelindex;
	int			frame;
	int			skinnum;
	int			effects;		// MUZZLE FLASH, e.g.

	int			flags;			// dead, gib, etc
	int			physflags;		// FL_ONGROUND, FL_DUCKING, etc.

	float		waterjumptime;	// Amount of time left in jumping out of water cycle.
	int			onground;		// -1 = in air, else pmove entity number
	int			oldbuttons;		// Buttons last usercmd

	// Render information
	int			rendermode;
	int			renderamt;
	color24		rendercolor;
	int			renderfx;

	int			sequence;
	float		animtime;

	float		framerate;
	int			body;
	byte		controller[4];
	byte		blending[4];

	int			weaponmodel;

	byte pad4[8];

	// If standing on conveyor, e.g.
	vec3_t		basevelocity;

	// Friction, for prediction.
	float		friction;
} player_state_t;

typedef struct
{
	// generated on client side
	usercmd_t	cmd;		// cmd that generated the frame
	double		senttime;	// time cmd was sent off
	int			delta_sequence;		// sequence number to delta from, -1 = full update

	// received from server
	double		receivedtime;	// time message was received, or -1
	qboolean	invalid;		// true if the packet_entities delta was invalid

	player_state_t	playerstate[MAX_CLIENTS];	// message received that reflects performing the usercmd
	packet_entities_t	packet_entities;
} frame_t;

//
// the client_state_t structure is wiped completely at every
// server signon
//
typedef struct
{
	int			max_edicts;

	resource_t	resourcesonhand;
	resource_t	resourcesneeded;
	resource_t	resourcelist[MAX_RESOURCES];	// Resource download list

	int			num_resources;

	int			servercount;	// server identification for prespawns, must match the svs.spawncount which
								// is incremented on server spawning.  This supercedes svs.spawn_issued, in that
								// we can now spend a fair amount of time sitting connected to the server
								// but downloading models, sounds, etc.  So much time that it is possible that the
								// server might change levels again and, if so, we need to know that.

	int			validsequence;	// this is the sequence number of the last good
								// world snapshot/update we got.  If this is 0, we can't
								// render a frame yet

	int			parsecount;		// server message counter

	usercmd_t	cmd;			// last command sent to the server

// information for local display
	int			stats[MAX_CL_STATS];	// health, etc
	int			weapons;

	float		frame_lerp;

	// the client maintains its own idea of view angles, which are
	// sent to the server each frame.  The server sets punchangle when
	// the view is temporarliy offset, and an angle reset commands at the start
	// of each level and after teleporting.
	vec3_t		viewangles;

	vec3_t		mvelocity[3];	// update by server, used for lean+bob
								// (0 is newest)

	vec3_t		punchangle;		// temporary offset
	vec3_t		crosshairangle;

	float		maxspeed;

	vec3_t		simorg;
	vec3_t		simvel;
	vec3_t		simangles;

// pitch drifting vars
	float		idealpitch;
	float		pitchvel;
	qboolean	nodrift;
	float		driftmove;
	double		laststop;

	float		viewheight;

	screenfade_t sf;

	qboolean	paused;			// send over by server
	int			onground;
	int			inwater;
	int			waterlevel;

// light level at player's position including dlights
// this is sent back to the server each frame
// architectually ugly but it works
	int			light_level;

	int			intermission;	// don't change view angle, full screen, etc
	int			completed_time;	// latched ffrom time at intermission start

	double		mtime[2];		// the timestamp of last two messages

	// Client clock
	double		time;

	// Old Client clock
	double		oldtime;

	frame_t		frames[UPDATE_BACKUP];

	//
	// information that is static for the entire time connected to a server
	//
	int			playernum;	 // player entity.  skips world. Add 1 to get cl_entitites index;

	qboolean	spectator;   // we're spectating

//
// information that is static for the entire time connected to a server
//
	struct model_s*	model_precache[MAX_MODELS];
	sfx_t* sound_precache[MAX_SOUNDS];

	char		levelname[40];	// for display on solo scoreboard
	int			maxclients;

	int			gametype;

	// refresh related state
	int			viewentity;		    // cl_entitites[cl.viewentity] == player point of view

	struct model_s*	worldmodel;	// cl_entitites[0].model
	efrag_t*	free_efrags;
	int			num_entities;	// held in cl_entities array
	int			num_statics;	// held in cl_staticentities array

	cl_entity_t viewent;			// the gun model

	int			cdtrack, looptrack;	// cd audio

	CRC32_t		serverCRC;		// To determine if client is playing hacked .map. (entities lump is skipped)
	CRC32_t		clientdllCRC;

	float		weaponstarttime;
	int			weaponsequence;

	int			fPrecaching;

	struct dlight_s* pLight;

// all player information
	player_info_t	players[MAX_CLIENTS];
} client_state_t;


//
// cvars
//
extern	cvar_t	cl_name;
extern	cvar_t	cl_color;

extern	cvar_t	cl_timeout;
extern	cvar_t	cl_shownet;
extern	cvar_t	cl_showsizes;
extern	cvar_t	cl_nolerp;
extern	cvar_t	cl_stats;
extern	cvar_t	cl_spectator_password;

extern	cvar_t	lookspring;
extern	cvar_t	lookstrafe;
extern	cvar_t	sensitivity;

extern	cvar_t	cl_skyname;
extern	cvar_t	cl_skycolor_r;
extern	cvar_t	cl_skycolor_g;
extern	cvar_t	cl_skycolor_b;
extern	cvar_t	cl_skyvec_x;
extern	cvar_t	cl_skyvec_y;
extern	cvar_t	cl_skyvec_z;

extern	cvar_t	cl_predict_players;
extern	cvar_t	cl_solid_players;
extern	cvar_t	cl_nodelta;
extern	cvar_t	cl_printplayers;

extern	cvar_t	m_pitch;
extern	cvar_t	m_yaw;
extern	cvar_t	m_forward;
extern	cvar_t	m_side;

extern	cvar_t	cl_upspeed;
extern	cvar_t	cl_forwardspeed;
extern	cvar_t	cl_backspeed;
extern	cvar_t	cl_sidespeed;

extern	cvar_t	cl_movespeedkey;

extern	cvar_t	cl_yawspeed;
extern	cvar_t	cl_pitchspeed;

extern	cvar_t	cl_anglespeedkey;

extern	cvar_t	cl_pitchup;
extern	cvar_t	cl_pitchdown;

extern	cvar_t	cl_nopred;
extern	cvar_t	cl_pushlatency;
extern	cvar_t	cl_dumpents;
extern	cvar_t	cl_showpred;

extern	cvar_t	cl_downloadinterval;

extern	cvar_t	cl_allowdownload;
extern	cvar_t	cl_download_ingame;
extern	cvar_t	cl_download_max;

extern	qboolean cl_inmovie;

extern	qboolean g_bSkipDownload;
extern	qboolean g_bSkipUpload;

extern	int	bitcounts[32 + 8];
extern	int	playerbitcounts[MAX_CLIENTS];
extern	int	custombitcounts[32];

#define MAX_TEMP_ENTITIES	350			// lightning bolts, etc
#define	MAX_STATIC_ENTITIES	32			// torches, etc

extern	client_static_t	cls;
extern	client_state_t	cl;

// FIXME, allocate dynamically
extern	efrag_t			cl_efrags[MAX_EFRAGS];
extern	cl_entity_t*	cl_entities;
extern  cl_entity_t		cl_static_entities[MAX_STATIC_ENTITIES];
extern	lightstyle_t	cl_lightstyle[MAX_LIGHTSTYLES];
extern	dlight_t		cl_dlights[MAX_DLIGHTS];
extern	dlight_t		cl_elights[MAX_ELIGHTS];

#define MAX_DATA_HISTORY	64
extern int	msg_buckets[MAX_DATA_HISTORY];
extern int	total_data[MAX_DATA_HISTORY];

//=============================================================================

// CL_MAIN.C
//
dlight_t* CL_AllocDlight( int key );
dlight_t* CL_AllocElight( int key );
void	CL_DecayLights( void );
void	CL_TouchLight( dlight_t* dl );

void CL_Init( void );
void CL_Disconnect( void );
void CL_SignonReply( void );
void CL_UpdateSoundFade( void );

// Resource
void CL_CreateResourceList( void );
void CL_ClearResourceList( resource_t* pList );
void CL_AddToResourceList( resource_t* pResource, resource_t* pList );
void CL_RemoveFromResourceList( resource_t* pResource );
void CL_MoveToOnHandList( resource_t* pResource );
void CL_SendResourceListBlock( void );
qboolean CL_RequestMissingResources( void );
void CL_ClearResourceLists( void );

#define			MAX_VISEDICTS	512
extern	int		cl_numvisedicts, cl_oldnumvisedicts, cl_numbeamentities;
extern	cl_entity_t* cl_visedicts, * cl_oldvisedicts;
extern	cl_entity_t	cl_visedicts_list[2][MAX_VISEDICTS];
extern	cl_entity_t* cl_beamentities[MAX_BEAMENTS];

extern	particle_t* free_particles;
extern	particle_t* active_particles;

//
// cl_input
//
typedef struct
{
	int		down[2];		// key nums holding it down
	int		state;			// low bit is down state
} kbutton_t;

extern	kbutton_t	in_mlook, in_klook;
extern 	kbutton_t 	in_strafe;
extern 	kbutton_t 	in_speed;

extern	int			in_impulse;

void CL_InitInput( void );
float CL_LerpPoint( void );
void CL_SendCmd( void );
void CL_BaseMove( usercmd_t* cmd );

int CL_ButtonBits( int bResetState );
void CL_ResetButtonBits( int bits );

void CAM_Init( void );
void CAM_Think( void );
void CAM_ClearStates( void );
void CAM_StartMouseMove( void );
void CAM_EndMouseMove( void );

void CL_CreateCustomizationList( void );
void CL_ClearState( qboolean bQuiet );
void CL_ClearClientState( void );
void CL_ReadPackets( void );        // Read packets from server and other sources (ping requests, etc.)
void CL_SendConnectPacket( void );  // Send the actual connection packet, after server supplies us a challenge value.
void CL_CheckForResend( void );     // If we are in ca_connecting state and it has been cl_resend.value seconds, 
									// request a challenge value again if we aren't yet connected. 
void CL_PingServers_f( void );
void CL_AddToServerCache( netadr_t adr, char* name, char* map, char* desc, int active, int maxplayers );
void CL_Connect_f( void );
void CL_Spectate_f( void );
void CL_Disconnect_f( void );
void CL_NextDemo( void );
void CL_ParseNextUpload( void );

//
// cl_ents.c
//
void CL_SetSolidPlayers( int playernum );
void CL_SetUpPlayerPrediction( qboolean dopred );
void CL_EmitEntities( void );
void CL_ParsePacketEntities( qboolean delta );
void CL_SetSolidEntities( void );
void CL_ParsePlayerinfo( void );
void CL_Particle( vec_t* origin, int color, float life, int zpos, int zvel );
void CL_PrintEntity( cl_entity_t* ent );

//
// cl_pred.c
//
void CL_InitPrediction( void );
void CL_PredictMove( void );
void CL_PredictUsercmd( player_state_t* from, player_state_t* to, usercmd_t* u, qboolean spectator );

//
// cl_cam.c
//
#define CAM_NONE		0
#define CAM_TRACK		1
#define CAM_FIRSTPERSON	2
#define CAM_TOPDOWN		3
#define CAM_NUMMODES	4

extern	vec3_t	cam_ofs;

extern	int		autocam;
extern	int		spec_track; // player# of who we are tracking
extern	int		cam_thirdperson;

void Cam_GetPredictedTopDownOrigin( vec_t* vec );
void Cam_GetTopDownOrigin( vec_t* source, vec_t* dest );
void Cam_GetPredictedFirstPersonOrigin( vec_t* vec );
void Cam_Track( usercmd_t* cmd );
void Cam_FinishMove( usercmd_t* cmd );
void Cam_Reset( void );
void CL_InitCam( void );

//
// cl_parse.c
//
cl_entity_t* CL_EntityNum( int num );
void CL_UserMsgs_f( void );
void CL_PrintResource( int index, resource_t* pResource );
void CL_PrintResourceLists_f( void );
void CL_DumpMessageLoad_f( void );
void CL_BitCounts_f( void );
void CL_ShowSizes( void );
void CL_ParseServerMessage( void );
void CL_DeallocateDynamicData( void );
qboolean CL_CheckFile( char* filename );

//
// view
//
void V_StartPitchDrift( void );
void V_StopPitchDrift( void );

//
// CL_TENT.C
//
void CL_InitTEnts( void );
void CL_ParseTEnt( void );
int ModelFrameCount( struct model_s* model );
void CL_TempEntUpdate( void );
int CL_FxBlend( cl_entity_t* ent );
void CL_FxTransform( cl_entity_t* ent, float* transform );
struct mspriteframe_s* R_GetSpriteFrame( struct msprite_s* pSprite, int frame );
void R_GetSpriteAxes( cl_entity_t* pEntity, int type, vec_t* forward, vec_t* right, vec_t* up );
void R_SpriteColor( colorVec* pColor, cl_entity_t* pEntity, int alpha );
float* R_GetAttachmentPoint( int entity, int attachment );