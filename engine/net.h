// net.h -- Half-Life's interface to the networking layer
// For banning IP addresses (or allowing private games)
#ifndef NET_H
#define NET_H
#ifdef _WIN32
#pragma once
#endif

#define	PORT_ANY	-1

typedef enum netsrc_s
{
	NS_CLIENT,
	NS_SERVER,
	NS_MULTICAST	// xxxMO
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


// Max length of a reliable message
#define MAX_MSGLEN				7500 // 10 reserved for fragheader?


extern	sizebuf_t	net_message;

//============================================================================


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


// Start up networking
void		NET_Init( void );



// Start up/shut down sockets layer
void		NET_Config( qboolean multiplayer );






// Initialize subsystem
void	Netchan_Init( void );



#endif // NET_H