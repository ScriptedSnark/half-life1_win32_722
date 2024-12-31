// server.h
#if !defined( SERVER_H )
#define SERVER_H
#ifdef _WIN32
#pragma once
#endif

typedef struct server_static_s
{
	// TODO: Implement

	struct client_s* clients;			// array of up to [maxclients] client slots

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

	char name[MAX_QPATH]; // map name
	char oldname[MAX_QPATH];
	char startspot[MAX_QPATH];

	struct model_s*	worldmodel;			// cl_entitites[0].model


	// TODO: Implement


	int			num_edicts;
	int			max_edicts;
	edict_t*	edicts;			// can NOT be array indexed, because
									// edict_t is variable sized, but can
									// be used to reference the world ent

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

	customization_t customdata;			// Head of custom client data list


	// TODO: Implement


} client_t;


//============================================================================





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

void SV_ClearResourceLists(client_t* cl);


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

#endif // SERVER_H