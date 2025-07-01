// server.h
#if !defined( SERVER_H )
#define SERVER_H
#ifdef _WIN32
#pragma once
#endif

#define	MAX_SIGNON_BUFFERS	16

#define MAX_SVCHANNELS 10

typedef struct svchannel_s
{
	char		        szServerChannel[16];
	qboolean            bIsDefault;  // Can't be deleted if true (unless bLeaveDefault is false to clear channel func.)
	struct svchannel_s* pNext;
} svchannel_t;

extern svchannel_t* svchannels;

qboolean SV_CheckChannel( char* pszChannel );
void SV_ClearChannels( qboolean bLeaveDefault );

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

// server_static_t
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

	svstats_t	stats;
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
	double		oldtime;

	int			lastcheck;
	double		lastchecktime;

	char		name[MAX_QPATH];			// map name
	char		startspot[MAX_QPATH];
	char		modelname[MAX_QPATH];

	struct model_s*	worldmodel;			// cl_entitites[0].model
	CRC32_t		worldmapCRC;		// For detecting that client has a hacked local copy of map, the client will be dropped if this occurs.
	CRC32_t		clientSideDllCRC;	// The dll that this server is expecting clients to be using.

	resource_t	resources[MAX_RESOURCES];	// The resource list
	int			num_resources;

	// TODO: Implement

	char*		model_precache[MAX_MODELS];	
	struct model_s*	models[MAX_MODELS];
	char*		sound_precache[MAX_SOUNDS];
	char*		lightstyles[MAX_LIGHTSTYLES];

	int			num_edicts;
	int			max_edicts;
	edict_t*	edicts;			// Can array index now, edict_t is fixed

	server_state_t	state;

	// added to every client's unreliable buffer each frame, then cleared
	sizebuf_t	datagram;
	byte		datagram_buf[MAX_DATAGRAM];

	// added to every client's reliable buffer each frame, then cleared
	sizebuf_t	reliable_datagram;
	byte		reliable_datagram_buf[MAX_DATAGRAM];

	// the master buffer is used for building log packets
	sizebuf_t	master;
	byte		master_buf[MAX_DATAGRAM];

	// the multicast buffer is used to send a message to a set of clients
	sizebuf_t	multicast;
	byte		multicast_buf[MAX_MULTICAST];	// Longest multicast message

	// the signon buffer will be sent to each client as they connect
	// includes the entity baselines, the static entities, etc
	// large levels will have >MAX_DATAGRAM sized signons, so 
	// multiple signon messages are kept
	sizebuf_t	signon;
	int			num_signon_buffers;
	int			signon_buffer_size[MAX_SIGNON_BUFFERS];
	byte		signon_buffers[MAX_SIGNON_BUFFERS][MAX_DATAGRAM];

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
	qboolean active;					// false = client is free
	qboolean spawned;					// false = don't send datagrams
	qboolean connected;					// Has been assigned to a client_t, but not in game yet
	qboolean uploading;					// true = client uploading custom resources

	int spec_track;						// the player we are tracking in spectator mode

	//===== NETWORK ============
	netchan_t netchan;					// The client's net connection.
	int chokecount;						// amount of choke since last client message

	int delta_sequence;					// -1 = no compression.  This is where the server is creating the
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

	int oldbuttons;

	// TODO: Implement
	//FF: 48 bytes

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

	edict_t* edict; // EDICT_NUM(clientnum+1)

	const edict_t* pViewEntity; // View Entity (camera or the client itself)

	char hashedcdkey[SIGNED_GUID_LEN + 1];

	char name[32];	// for printing to other people

	int colors;

	int saverestoredatasize; // the amount this client's edict is taking in our SAVERESTOREDATA

	// FF: If changing ANYTHING below, also insert the changes into sv_upld.c and other files!
	// FF: Some fields may have wrong names e.g. isuploading. These names were just straight guessing

	byte* download;						// the download data
	int downloadsize;					// total bytes
	int downloadcount;					// bytes sent
	qboolean downloading;				// true = client is downloading a file
	CRC32_t downloadCRC;				// CRC32 of the file we are downloading

	resource_t resourcesonhand;			// Head of resources accounted for list
	resource_t resourcesneeded;			// Head of resources to download list

	FILE* upload;						// Handle of file being uploaded
	resource_t* uploadresource;			// The resource we're trying to retrieve from the client (e.g. spray)
	char		uploadfntmp[MAX_QPATH];
	CRC32_t		uploadfinalCRC;
	char		uploadfn[MAX_QPATH];
	int			uploadcount;

	qboolean	uploadinprogress;

	int			nTotalSize;
	int			nTotalToTransfer;
	int			nRemainingToTransfer;

	float		fLastStatusUpdate;		// The time of the last update
	float		fLastUploadTime;		// The last time the file was downloaded

	downloadtime_t rgUploads[MAX_DL_STATS];
	int			nCurUpload;

	qboolean	uploaddoneregistering;

	CRC32_t		uploadcurrentCRC;

	customization_t customdata;
} client_t;

// server flags
#define	SFL_EPISODE_1		1
#define	SFL_EPISODE_2		2
#define	SFL_EPISODE_3		4
#define	SFL_EPISODE_4		8
#define	SFL_NEW_UNIT		16
#define	SFL_NEW_EPISODE		32
#define	SFL_CROSS_TRIGGERS	65280

//============================================================================

extern	cvar_t	teamplay;
extern	cvar_t	skill;
extern	cvar_t	deathmatch;
extern	cvar_t	coop;
extern	cvar_t	pausable;




extern	cvar_t	sv_newunit;

extern	cvar_t	sv_clienttrace;

extern	cvar_t	sv_cheats;



extern	cvar_t	sv_friction;
extern	cvar_t	sv_stopspeed;
extern	cvar_t	sv_gravity;
extern	cvar_t	sv_maxvelocity;
extern	cvar_t	sv_stepsize;
extern	cvar_t	sv_clipmode;
extern	cvar_t	sv_bounce;
extern	cvar_t	sv_airmove;
extern	cvar_t	sv_spectatormaxspeed;
extern	cvar_t	sv_airaccelerate;
extern	cvar_t	sv_wateraccelerate;
extern	cvar_t	sv_waterfriction;
extern	cvar_t	sv_zmax;
extern	cvar_t	sv_wateramp;
extern	cvar_t	sv_skyname;

extern	cvar_t	sv_idealpitchscale;
extern	cvar_t	sv_edgefriction;
extern	cvar_t	sv_maxspeed;
extern	cvar_t	sv_accelerate;



extern	cvar_t	sv_aim;

extern	cvar_t	sv_allow_download;
extern	cvar_t	sv_allow_upload;
extern	cvar_t	sv_upload_maxsize;
extern	cvar_t	sv_showcmd;

extern	cvar_t	filterban;

extern cvar_t	sv_uploadinterval;

extern float		g_LastScreenUpdateTime;
extern float		scr_centertime_off;

extern qboolean		g_bShouldUpdatePing;

extern UserMsg* sv_gpNewUserMsgs;
extern UserMsg* sv_gpUserMsgs;

extern int		nReliableBytesSent;
extern int		nDatagramBytesSent;
extern int		nReliables;
extern int		nDatagrams;
extern qboolean bUnreliableOverflow;

extern	qboolean	allow_cheats;

extern	server_static_t	svs;				// persistant server info
extern	server_t		sv;					// local server

extern	client_t*		host_client;

extern	edict_t*		sv_player;


extern	jmp_buf 	host_abortserver;




extern edict_t** g_moved_edict;
extern vec3_t* g_moved_from;
extern byte* g_playertouch;


//===========================================================

//
// sv_main.c
//
void SV_Init( void );
void SV_ReadPackets( void );
void SV_CheckTimeouts( void );
void SV_FullClientUpdate( client_t* cl, sizebuf_t* sb );
void SV_CountPlayers( int* clients, int* spectators );
void SV_DropClient( client_t* drop, qboolean crash );

int SV_PointLeafnum( vec_t* p );

int SV_CalcPing( client_t* cl );


void SV_StartParticle( const vec_t* org, const vec_t* dir, int color, int count );
void SV_StartSound( edict_t* entity, int channel, const char* sample, int volume, float attenuation, int fFlags, int pitch );


void SV_Multicast( vec_t* origin, int to, qboolean reliable );




void SV_ClientPrintf( char* fmt, ... );
void SV_BroadcastPrintf( char* fmt, ... );

void SV_QueryMovevarsChanged( void );
void SV_New_f( void );
void SV_PTrack_f( void );
void SV_SpawnSpectator( void );
void SV_ActivateServer( qboolean runPhysics );
int SV_SpawnServer( qboolean bIsDemo, char* server, char* startspot );
void SV_LoadEntities( void );
void SV_ClearEntities( void );
void SV_InactivateClients( void );
void SV_MemPrediction_f( void );
void SV_AddIP_f( void );
void SV_RemoveIP_f( void );
void SV_ListIP_f( void );
void SV_WriteIP_f( void );
void SV_Keys_f( void );

int RegUserMsg( const char* pszName, int iSize );



void SV_ClearClientStates( void );

typedef enum
{
	RD_NONE = 0,
	RD_CLIENT,
	RD_PACKET
} redirect_t;
extern redirect_t	sv_redirected;
void Host_FlushRedirect( void );
void Host_BeginRedirect( redirect_t rd, netadr_t* addr );
void Host_EndRedirect( void );

void Host_RemoteCommand( netadr_t* net_from );

//
// sv_send.c
//


void SV_SendUserReg( sizebuf_t* sb );
void SV_SendBan( void );
qboolean SV_FilterPacket( void );
void SV_SendClientMessages( void );
void SV_WriteClientdataToMessage( client_t* client, sizebuf_t* msg );

//
// sv_user.c
//
void SV_SetIdealPitch( void );
void SV_ExecuteClientMessage( client_t* cl );
void SV_PreRunCmd( void );
void SV_RunCmd( usercmd_t* ucmd );
void SV_PostRunCmd(	void );
void SV_Drop_f( void );

//
// sv_phys.c
//
// TODO: Implement
void SV_Physics( void );
trace_t SV_Trace_Toss( edict_t* ent, edict_t* ignore );
qboolean SV_RunThink( edict_t* ent );
void SV_Impact( edict_t* e1, edict_t* e2, trace_t* ptrace );
// TODO: Implement
void SV_SetMoveVars( void );

//
// sv_move.c
//
qboolean SV_CheckBottom( edict_t* ent );
qboolean SV_movetest( edict_t* ent, vec_t* move, qboolean relink );
qboolean SV_movestep( edict_t* ent, vec_t* move, qboolean relink );
void SV_MoveToOrigin_I( edict_t* ent, const float* pflGoal, float dist, int iStrafe );

//
// sv_upld.c
//
// Wrong functions, wrong function order. fixme
// TODO: Implement
int SV_ModelIndex( char* name );
void SV_FlushSignon( void );
void SV_SendResourceListBlock_f( void );
void SV_RequestResourceList_f( void );
void SV_PrintCusomizations_f( void );
void SV_AddResource( resourcetype_t type, const char* name, int size, byte flags, int index );
void SV_CreateResourceList( void );
void SV_ClearResourceLists( client_t* cl );
void SV_RemoveFromResourceList( resource_t* pResource );
qboolean SV_RequestMissingResources( void );
void SV_RequestMissingResourcesFromClients( void );
void SV_ParseResourceList( void );
void SV_ParseUpload( void );
void SV_PropagateCustomizations( void );
void SV_MoveToOnHandList( resource_t* pResource );
void SV_Customization( client_t* pPlayer, resource_t* pResource, qboolean bSkipPlayer );
void SV_ClearResourceList( resource_t* pList );
void SV_AddToResourceList( resource_t* pResource, resource_t* pList );

#endif // SERVER_H