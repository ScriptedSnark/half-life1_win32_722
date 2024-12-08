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


	// TODO: Implement


	qboolean	demorecording;
	qboolean	demoplayback;
	qboolean	timedemo;
	float		demostarttime;
	int			demostartframe;
	int			forcetrack; //FF: totally unsure about this field!
	FILE*		demofile;
	void*		demoheader;


	// TODO: Implement


// connection information
	int			signon;			// 0 to SIGNONS


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

	double		time;


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

//=============================================================================

// CL_MAIN.C
//
void	CL_DecayLights( void );

void CL_Init( void );


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
// view
//
void V_StartPitchDrift( void );
void V_StopPitchDrift( void );

// TODO: Implement