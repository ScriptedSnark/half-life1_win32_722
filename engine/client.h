// client.h

#include "wrect.h"
#include "cdll_int.h"

// TODO: Implement


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



	// TODO: Implement


	float		demostarttime;
	int			demostartframe;
	int			forcetrack; //FF: totally unsure about this field!
	FILE*		demofile;
	void*		demoheader;


	// TODO: Implement


} client_static_t; //FF: add comments for the fields

extern client_static_t	cls;

//
// the client_state_t structure is wiped completely at every
// server signon
//
typedef struct
{
	// TODO: Implement

	// the client maintains its own idea of view angles, which are
	// sent to the server each frame.  The server sets punchangle when
	// the view is temporarliy offset, and an angle reset commands at the start
	// of each level and after teleporting.
	vec3_t		viewangles;


	// TODO: Implement


	qboolean	paused;			// send over by server


	// TODO: Implement


	int			intermission;	// don't change view angle, full screen, etc

	// TODO: Implement

	// Client clock
	double		time;

	// Old Client clock
	double		oldtime;


	// TODO: Implement


	struct model_s*	worldmodel;	// cl_entitites[0].model


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
qboolean CL_RequestMissingResources( void );


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


void CL_ReadPackets( void );        // Read packets from server and other sources (ping requests, etc.)
void CL_SendConnectPacket( void );  // Send the actual connection packet, after server supplies us a challenge value.
void CL_CheckForResend( void );     // If we are in ca_connecting state and it has been cl_resend.value seconds, 
									// request a challenge value again if we aren't yet connected. 
void CL_Connect_f( void );

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



//
// view
//
void V_StartPitchDrift( void );
void V_StopPitchDrift( void );

// TODO: Implement