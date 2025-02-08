// server.h
#if !defined( SERVER_H )
#define SERVER_H
#ifdef _WIN32
#pragma once
#endif

#define	MAX_SIGNON_BUFFERS	16

typedef struct server_static_s
{
	// TODO: Implement

	struct client_s* clients;			// array of up to [maxclients] client slots

	int			serverflags;			// episode completion information

	int			maxclients;				// Current max #
	int			maxclientslimit;		// Max allowed on server.
	int			spawncount;				// Number of servers spawned since start,
										// used to check late spawns (e.g., when d/l'ing lots of
										// data)

	
} server_static_t;

//=============================================================================

// Max # of master servers this server can be associated with
typedef enum server_state_e
{
	ss_dead,	// No server.
	ss_loading,	// Spawning
	ss_active	// Running
} server_state_t;

// sizeof(server_t) = 0x34F90u
typedef struct
{
	qboolean	active;				// false if only a net client
	qboolean	paused;				// are we paused?

	// TODO: Implement

	double		time;

	// TODO: Implement

	char name[MAX_QPATH]; // map name
	char startspot[MAX_QPATH];
	char modelname[MAX_QPATH];

	struct model_s*	worldmodel;			// cl_entitites[0].model

	CRC32_t			worldmapCRC;		// For detecting that client has a hacked local copy of map, the client will be dropped if this occurs.
	CRC32_t			clientSideDllCRC;	// The dll that this server is expecting clients to be using.
										// To prevent cheating with hacked client dlls


	// TODO: Implement

	char*	model_precache[MAX_MODELS];
	
	struct model_s*	models[MAX_MODELS];

	char*	sound_precache[MAX_SOUNDS];

	// TODO: Implement


	int			num_edicts;
	int			max_edicts;
	edict_t*	edicts;			// can NOT be array indexed, because
									// edict_t is variable sized, but can
									// be used to reference the world ent

	server_state_t state;

	// added to every client's unreliable buffer each frame, then cleared
	sizebuf_t		datagram;
	byte			datagram_buf[MAX_DATAGRAM];

	// added to every client's reliable buffer each frame, then cleared
	sizebuf_t		reliable_datagram;
	byte			reliable_datagram_buf[MAX_DATAGRAM];

	// the master buffer is used for building log packets
	sizebuf_t		master;
	byte			master_buf[MAX_DATAGRAM];

	// the multicast buffer is used to send a message to a set of clients
	sizebuf_t		multicast;
	byte			multicast_buf[MAX_MULTICAST];	// Longest multicast message

	// the signon buffer will be sent to each client as they connect
	// includes the entity baselines, the static entities, etc
	// large levels will have >MAX_DATAGRAM sized signons, so 
	// multiple signon messages are kept
	sizebuf_t		signon;
	int				num_signon_buffers;
	int				signon_buffer_size[MAX_SIGNON_BUFFERS];
	byte			signon_buffers[MAX_SIGNON_BUFFERS][MAX_DATAGRAM];

	// TODO: Implement

} server_t;

// client_t
typedef struct client_s
{
	qboolean active;					// false = client is free
	qboolean spawned;					// false = don't send datagrams


	// TODO: Implement


	//===== NETWORK ============
	netchan_t netchan;					// The client's net connection.


	// TODO: Implement


	double rate;						// seconds / byte

	// TODO: Implement

	resource_t resourcesonhand;			// Head of resources accounted for list
	resource_t resourcesneeded;			// Head of resources to download list

	// TODO: Implement

	customization_t customdata;			// Head of custom client data list


	// TODO: Implement


} client_t;


//============================================================================



extern cvar_t sv_cheats;




extern	cvar_t	skill;
extern	cvar_t	deathmatch;
extern	cvar_t	coop;


extern float		g_LastScreenUpdateTime;
extern float		scr_centertime_off;


extern	server_static_t	svs;				// persistant server info
extern	server_t		sv;					// local server



extern	jmp_buf 	host_abortserver;








//===========================================================

// sv_main.c
void SV_Init( void );
void SV_CheckTimeouts( void );

void SV_DeallocateDynamicData( void );

void SV_BroadcastPrintf( char* fmt, ... );

void SV_ClearChannel( qboolean forceclear );

void SV_ClearResourceLists( client_t* cl );

int SV_SpawnServer( qboolean bIsDemo, char* server, char* startspot );

//
// sv_send.c
//
typedef enum
{
	RD_NONE = 0,
	RD_CLIENT,
	RD_PACKET
} redirect_t;
extern redirect_t	sv_redirected;
void SV_FlushRedirect( void );

//
// sv_phys.c
//

void SV_SetMoveVars( void );

#endif // SERVER_H