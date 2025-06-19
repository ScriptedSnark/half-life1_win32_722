// net.h -- Half-Life's interface to the networking layer
// For banning IP addresses (or allowing private games)
#ifndef NET_H
#define NET_H
#ifdef _WIN32
#pragma once
#endif

#define	PORT_ANY	-1

#define	MIN_RATE		100
#define DEFAULT_RATE	9999 // Default data rate
#define	MAX_RATE		10000

#define	PACKET_HEADER	8

typedef enum netsrc_s
{
	NS_CLIENT,
	NS_SERVER
} netsrc_t;

typedef enum
{
	NA_UNUSED,
	NA_LOOPBACK,
	NA_BROADCAST,
	NA_IP,
	NA_IPX,
	NA_BROADCAST_IPX,
} netadrtype_t;

typedef struct netadr_s
{
	netadrtype_t	type;
	unsigned char	ip[4];
	unsigned char	ipx[10];
	unsigned short	port;
} netadr_t;

extern	netadr_t	net_local_adr;
extern	netadr_t	net_from;		// address of who sent the packet
extern	sizebuf_t	net_message;

extern	cvar_t	host_name;

extern int		net_activeconnections;

void		NET_InitColors( void );
void		SCR_NetUsage( void );
void		R_NetGraph( void );

// Start up networking
void		NET_Init( void );
// Shut down networking
void		NET_Shutdown( void );
// Retrieve packets from network layer
qboolean	NET_GetPacket( netsrc_t sock );
// Send packet over network layer
void		NET_SendPacket( netsrc_t sock, int length, void* data, netadr_t to );
// Start up/shut down sockets layer
void		NET_Config( qboolean multiplayer );

// Compare addresses
qboolean	NET_CompareAdr( netadr_t a, netadr_t b );
qboolean	NET_CompareClassBAdr( netadr_t a, netadr_t b );
// Address conversion
char*		NET_AdrToString( netadr_t a );
char*		NET_BaseAdrToString( netadr_t a );
qboolean	NET_StringToSockaddr( char* s, struct sockaddr* sadr );
qboolean	NET_StringToAdr( char* s, netadr_t* a );
qboolean	NET_IsLocalAddress( netadr_t adr );

//============================================================================

#define	OLD_AVG		0.99		// total = oldtotal*OLD_AVG + new*(1-OLD_AVG)

#define	MAX_LATENT	32

typedef struct netchan_s
{
	// Address this channel is talking to.
	netadr_t	remote_address;

	// NS_SERVER, NS_CLIENT or NS_MULTICAST, depending on channel.
	netsrc_t	sock;

	qboolean	fatal_error;

	float		last_received;		// for timeouts

	// Time when channel was connected.
	float		connect_time;

// the statistics are cleared at each client begin, because
// the server connecting process gives a bogus picture of the data
	float		frame_latency;		// rolling average
	float		frame_rate;

	int			drop_count;			// dropped packets, cleared each level
	int			good_count;			// cleared each level

	int			qport;

// bandwidth estimator
	double		cleartime;			// if realtime > nc->cleartime, free to go
	double		rate;				// seconds / byte

// sequencing variables
	int			incoming_sequence;
	int			incoming_acknowledged;
	int			incoming_reliable_acknowledged;	// single bit

	int			incoming_reliable_sequence;		// single bit, maintained local

	int			outgoing_sequence;
	int			reliable_sequence;			// single bit
	int			last_reliable_sequence;		// sequence number of last send

// reliable staging and holding areas
	sizebuf_t	message;		// writing buffer to send to server
	byte		message_buf[MAX_MSGLEN];

	int			reliable_length;
	byte		reliable_buf[MAX_MSGLEN];	// unacked reliable message

// time and size data to calculate bandwidth
	int			outgoing_size[MAX_LATENT];
	double		outgoing_time[MAX_LATENT];
} netchan_t;

extern	int	net_drop;		// packets dropped before this one
extern	cvar_t	r_netgraph;

extern	cvar_t	noip;    // Disable IP Support

#ifdef _WIN32
extern	cvar_t	noipx;    // Disable IPX Support
extern	netadr_t net_local_ipx_adr;
#endif //_WIN32

// Initialize subsystem
void	Netchan_Init( void );
void	Netchan_Transmit( netchan_t* chan, int length, byte* data );
void	Netchan_OutOfBand( netsrc_t sock, netadr_t adr, int length, byte* data );
void	Netchan_OutOfBandPrint( netsrc_t sock, netadr_t adr, char* format, ... );
qboolean Netchan_Process( netchan_t* chan );
void	Netchan_Setup( netsrc_t socketnumber, netchan_t* chan, netadr_t adr );

qboolean Netchan_CanPacket( netchan_t* chan );
qboolean Netchan_CanReliable( netchan_t* chan );

#endif // NET_H