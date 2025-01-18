// client.h

#include "wrect.h"
#include "cdll_int.h"

// TODO: Implement


typedef struct
{
	int		length;
	char	map[MAX_STYLESTRING];
} lightstyle_t;





#define MAX_DL_STATS	8
typedef struct
{
	qboolean bUsed;
	float fTime;
	int nBytesRemaining;
} downloadtime_t;

typedef struct
{
	FILE*		download;				// Handle of file being downloaded
	resource_t* resource;
	qboolean	doneregistering;

	char		extension[MAX_QPATH];	// The extension of a file that we are downloading from the server
										// ".cst" if it's a custom resource
	CRC32_t		crcFile;				// For detecting that client's resource is different
	char		filename[MAX_QPATH];	// The filename

	// TODO: Implement

	int			percent;

	qboolean	isdownloading;			// TRUE if downloading is in progress

	int			nTotalSize;
	int			nTotalToTransfer;
	int			nRemainingToTransfer;

	float		fLastStatusUpdate;		// The time of the last update
	float		fLastDownloadTime;		// The last time the file was downloaded

	downloadtime_t rgStats[MAX_DL_STATS];
	int			nCurStat;

	// TODO: Implement

	qboolean	custom;					// TRUE is downloading a custom resource
	FILE*		upload;					// Handle of file being uploaded

	// TODO: Implement

} incomingtransfer_t;



#define	MAX_SCOREBOARDNAME		32
typedef struct player_info_s
{
	// TODO: Implement


	// Name
	char	name[MAX_SCOREBOARDNAME];


	// TODO: Implement


	customization_t customdata;


	// TODO: Implement


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

//
// the client_static_t structure is persistant through an arbitrary number
// of server connections
//
typedef struct
{
	cactive_t	state;


	// TODO: Implement


// Our sequenced channel to the remote server.
	netchan_t   netchan;

// Connection to server.
	double		connect_time;		// If gap of connect_time to realtime > 3000, then resend connect packet

	int			connect_retry;      // After CL_CONNECTION_RETRIES, give up...

	int			challenge;			// from the server to use for connecting

	qboolean	spectator;			// TRUE if connected as spectator

	char		trueaddress[32];

	float		slist_time;

// connection information
	int			signon;			// 0 to SIGNONS

	char		servername[128];	// name of server from original connect
	char		mapstring[MAX_QPATH];

	char		spawnparams[MAX_MAPSTRING];

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

	// TODO: Implement

	int			td_lastframe;		// to meter out one message a frame
	int			td_startframe;		// host_framecount at start
	float		td_starttime;		// realtime at second frame of timedemo

	incomingtransfer_t dl;

	// TODO: Implement

} client_static_t;

extern client_static_t	cls;

// player_state_t is the information needed by a player entity
// to do move prediction and to generate a drawable entity
typedef struct
{
	int			messagenum;		// all player's won't be updated each frame

	double		state_time;		// not the same as the packet time,
								// because player commands come asyncronously
	usercmd_t	command;		// last command for prediction

	vec3_t		origin;
	vec3_t		viewangles;		// only for demos, not from server
	vec3_t		velocity;
	int			weaponframe;

	int			modelindex;
	int			frame;
	int			skinnum;
	int			effects;

	int			flags;			// dead, gib, etc

	float		waterjumptime;
	int			onground;		// -1 = in air, else pmove entity number
	int			oldbuttons;
} player_state_t;

// entity_state_t is the information conveyed from the server
// in an update message
typedef struct
{
	int		number;			// edict index
	int		flags;			// nolerp, etc
	vec3_t	origin;
	vec3_t	angles;
	int		modelindex;
	int		frame;
	int		colormap;
	int		skinnum;
	int		effects;
} cl_entity_state_t;

#define	MAX_PACKET_ENTITIES	64	// doesn't count nails
typedef struct
{
	int		num_entities;
	cl_entity_state_t	entities[MAX_PACKET_ENTITIES];
} cl_packet_entities_t;

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
	cl_packet_entities_t	packet_entities;
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


	// TODO: Implement


	// information for local display
	int			stats[MAX_CL_STATS];	// health, etc

	int			weapons;


	// TODO: Implement


	// the client maintains its own idea of view angles, which are
	// sent to the server each frame.  The server sets punchangle when
	// the view is temporarliy offset, and an angle reset commands at the start
	// of each level and after teleporting.
	vec3_t		viewangles;


	// TODO: Implement


	qboolean	paused;			// send over by server

	int			onground;



	// TODO: Implement


	int			intermission;	// don't change view angle, full screen, etc

	// TODO: Implement

	double		mtime[2];		// the timestamp of last two messages

	// Client clock
	double		time;

	// Old Client clock
	double		oldtime;


	// TODO: Implement

	frame_t		frames[UPDATE_BACKUP];

	// TODO: Implement

	//
	// information that is static for the entire time connected to a server
	//
	int			playernum;	 // player entity.  skips world. Add 1 to get cl_entitites index;

	qboolean	spectator;   // we're spectating

	// TODO: Implement

	char		levelname[40];	// for display on solo scoreboard
	int			maxclients;

	int			gametype;

	// refresh related state
	int			viewentity;		    // cl_entitites[cl.viewentity] == player point of view

	struct model_s*	worldmodel;	// cl_entitites[0].model
	efrag_t*	free_efrags;
	int			num_entities;	// held in cl_entities array
	int			num_statics;	// held in cl_staticentities array



	// TODO: Implement

	int			cdtrack, looptrack;	// cd audio

	CRC32_t		serverCRC;		// To determine if client is playing hacked .map. (entities lump is skipped)
	CRC32_t		mapCRC;			// client's map CRC value, CL_CheckCRCs() checks this with cl.serverCRC to make sure
								// that the client and server play the same map

	float		weaponstarttime;
	int			weaponsequence;

	int			fPrecaching;

	// TODO: Implement

// all player information
	player_info_t	players[MAX_CLIENTS];


	// TODO: Implement


} client_state_t;


//
// cvars
//
extern	cvar_t	cl_name;
extern	cvar_t	cl_color;

extern	cvar_t	cl_timeout;
extern	cvar_t	cl_shownet;

extern	cvar_t	cl_upspeed;
extern	cvar_t	cl_forwardspeed;
extern	cvar_t	cl_backspeed;
extern	cvar_t	cl_sidespeed;

extern	cvar_t	cl_movespeedkey;

extern	cvar_t	cl_yawspeed;
extern	cvar_t	cl_pitchspeed;


extern	cvar_t	lookspring;
extern	cvar_t	lookstrafe;
extern	cvar_t	sensitivity;

extern	cvar_t	cl_skyname;

extern	cvar_t	m_pitch;
extern	cvar_t	m_yaw;
extern	cvar_t	m_forward;
extern	cvar_t	m_side;

extern	cvar_t	cl_pitchup;
extern	cvar_t	cl_pitchdown;

// TODO: Implement

extern qboolean cl_inmovie;

// TODO: Implement

extern	client_state_t	cl;

extern int	msg_buckets[64];
extern int	total_data[64];

//=============================================================================

// CL_MAIN.C
//
void	CL_DecayLights( void );

void CL_Init( void );


void CL_Disconnect( void );


void CL_UpdateSoundFade( void );


// Resource
void CL_CreateResourceList( void );
void CL_ClearResourceList( resource_t* pList );
void CL_AddToResourceList( resource_t* pResource, resource_t* pList );
void CL_RemoveFromResourceList( resource_t* pResource );
void CL_SendResourceListBlock( void );
qboolean CL_RequestMissingResources( void );
void CL_ClearResourceLists( void );


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

void CL_InitInput( void );
void CL_SendCmd( void );

void CAM_Think( void );

// TODO: Implement

void CL_CreateCustomizationList( void );
void CL_ClearState( qboolean bQuiet );
void CL_ClearClientState( void );
void CL_ReadPackets( void );        // Read packets from server and other sources (ping requests, etc.)
void CL_SendConnectPacket( void );  // Send the actual connection packet, after server supplies us a challenge value.
void CL_CheckForResend( void );     // If we are in ca_connecting state and it has been cl_resend.value seconds, 
									// request a challenge value again if we aren't yet connected. 
void CL_Connect_f( void );
void CL_Disconnect_f( void );

//
// cl_ents.c
//
void CL_SetSolidPlayers( int playernum );
void CL_SetUpPlayerPrediction( qboolean dopred );
void CL_EmitEntities( void );


//
// cl_pred.c
//
void CL_PredictMove( void );


//
// cl_parse.c
//
void CL_ParseServerMessage( void );



void CL_DeallocateDynamicData( void );




//
// view
//
void V_StartPitchDrift( void );
void V_StopPitchDrift( void );

// TODO: Implement