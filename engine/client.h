// client.h


// TODO: Implement


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
	vec3_t viewangles;
} client_state_t;


// TODO: Implement



extern	cvar_t	lookspring;
// TODO: Implement
extern	cvar_t	sensitivity;


// TODO: Implement

extern	client_state_t	cl;

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

// TODO: Implement

//
// view
//
void V_StartPitchDrift(void);
void V_StopPitchDrift(void);

// TODO: Implement