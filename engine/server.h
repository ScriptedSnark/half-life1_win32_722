// server.h

typedef struct server_static_s
{
	// TODO: Implement

	int stub_var;
} server_static_t;

//=============================================================================

// Max # of master servers this server can be associated with
typedef enum server_state_e
{
	ss_dead,	// No server.
	ss_loading,	// Spawning
	ss_active	// Running
} server_state_t;

typedef struct
{
	qboolean	active;				// false if only a net client
	qboolean	paused;				// are we paused?

	edict_t		*edicts;			// can NOT be array indexed, because
									// edict_t is variable sized, but can
									// be used to reference the world ent

	// TODO: Implement
} server_t;




//============================================================================





extern	server_static_t	svs;				// persistant server info
extern	server_t		sv;					// local server



extern	jmp_buf 	host_abortserver;





//===========================================================




void SV_BroadcastPrintf( char* fmt, ... );