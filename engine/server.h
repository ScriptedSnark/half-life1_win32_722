// server.h
#if !defined( SERVER_H )
#define SERVER_H
#ifdef _WIN32
#pragma once
#endif

#define	MAX_SIGNON_BUFFERS	16

typedef struct
{
	double	active;
	double	idle;
	int		count;
	int		packets;

	double	latched_active;
	double	latched_idle;
	int		latched_packets;
} svstats_t;

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

	// TODO: Implement

	svstats_t stats;
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

	qboolean	loadgame;			// handle connections specially

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

typedef struct
{
	// received from client

	// Time world sample was sent to client.
	double				senttime;
	// Realtime when client ack'd the update.
	float				ping_time;
	 // Internal lag (i.e., local client should = 0 ping)
	float				frame_time;
	// State of entities this frame from the POV of the client.
	packet_entities_t	entities;
} client_frame_t;

// client_t
typedef struct client_s
{
	qboolean active;	// false = client is free
	qboolean spawned;	// false = don't send datagrams

	qboolean connected; // On server, getting data.
	//FF:    ^^^ this can be client_state_t, but I'm unsure.

	qboolean uploading; // true = uploading a file to the server

	int spec_track; // the player we are tracking in spectator mode

	//===== NETWORK ============
	netchan_t netchan;					// The client's net connection.

	int chokecount; // the amount of packets we couldn't transmit to the client

	int delta_sequence;	// -1 = no compression.  This is where the server is creating the
										// compressed info from.

	// TODO: Implement
	//FF: one field (four bytes)

	qboolean privileged; // can execute any host command

	qboolean sendsignon; // only valid before spawned

	qboolean spectator;	 // non-interactive

	qboolean fakeclient; // JAC: This client is a fake player controlled by the game DLL

	// TODO: Implement
	//FF: 12 bytes (vec3_t?)

	usercmd_t lastcmd; // for filling in big drops and partial predictions

	// TODO: Implement
	//FF: one field (four bytes)

	double localtime; // of last message

	// TODO: Implement
	//FF: 52 bytes

	float maxspeed; // localized maxspeed

	// TODO: Implement
	//FF: one field (four bytes), probably "entgravity"

	// the datagram is written to after every frame, but only cleared
	// when it is sent out to the client.  overflow is tolerated.
	sizebuf_t datagram;
	byte datagram_buf[MAX_DATAGRAM];

	double connection_started;	// the time this client has started receiving messages from us

	qboolean send_message;		// set on frames a datagram arived on

	client_frame_t frames[UPDATE_BACKUP]; // updates can be deltad from here

	edict_t *edict; // EDICT_NUM(clientnum+1)

	const edict_t *pViewEntity; // View Entity (camera or the client itself)

	char hashedcdkey[33];

	char name[32];	// for printing to other people

	int colors;

	int saverestoredatasize; // the amount this client's edict is taking in our SAVERESTOREDATA

	FILE *download;			// file being downloaded
	int downloadsize;		// total bytes
	int downloadcount;		// bytes sent
	qboolean downloading;	// true = client is downloading a file
	CRC32_t downloadingCRC; // CRC32 of the file we are downloading

	resource_t resourcesonhand; // Head of resources accounted for list
	resource_t resourcesneeded; // Head of resources to download list

	FILE *upload; // file being uploaded (by the client)
	resource_t *uploadingresource; //the resource we're trying to retrieve from the client (e.g. spray)

	char uploadfn[MAX_QPATH];

	// TODO: Implement

	int uploadcount;
	qboolean bUploading;

	// TODO: Implement

	int uploadsize;

	// TODO: Implement

	float flLastUploadTime;

	// TODO: Implement

	int numuploads;

	qboolean uploaddoneregistering;

	customization_t customdata;
} client_t;



//============================================================================

extern	cvar_t	skill;
extern	cvar_t	deathmatch;
extern	cvar_t	coop;




extern	cvar_t	sv_cheats;



extern	cvar_t	sv_gravity;





extern float		g_LastScreenUpdateTime;
extern float		scr_centertime_off;


extern	server_static_t	svs;				// persistant server info
extern	server_t		sv;					// local server

extern	client_t		*host_client;

extern	edict_t			*sv_player;


extern	jmp_buf 	host_abortserver;








//===========================================================

//
// sv_main.c
//
void SV_Init( void );
void SV_ReadPackets( void );
void SV_CheckTimeouts( void );
void SV_CountPlayers( int* clients, int* spectators );
void SV_DropClient( client_t* drop, qboolean crash );

void SV_DeallocateDynamicData( void );

void SV_BroadcastPrintf( char* fmt, ... );

void SV_ClearChannel( qboolean forceclear );

void SV_QueryMovevarsChanged( void );
int SV_SpawnServer( qboolean bIsDemo, char* server, char* startspot );

void Master_Heartbeat( void );

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
void SV_SendClientMessages( void );
void SV_FlushRedirect( void );
void SV_SendBan( void );
qboolean SV_FilterPacket( void );

//
// sv_user.c
//
void SV_ExecuteClientMessage( client_t* cl );

//
// sv_phys.c
//
// TODO: Implement
void SV_Physics( void );
// TODO: Implement
void SV_SetMoveVars( void );

//
// sv_upld.c
//
void SV_ClearResourceLists( client_t* cl );
void SV_RequestMissingResourcesFromClients( void );
void SV_ParseResourceList( void );

#endif // SERVER_H