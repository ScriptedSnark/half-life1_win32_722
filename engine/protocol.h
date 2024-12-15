// protocol.h -- communications protocols
#ifndef PROTOCOL_H
#define PROTOCOL_H

#ifdef _WIN32
#pragma once
#endif


#define	PROTOCOL_VERSION	31



//=========================================

// out of band message id bytes

// M = master, S = server, C = client, A = any
// the second character will allways be \n if the message isn't a single
// byte long (?? not true anymore?)

// Client connection is initiated by requesting a challenge value
//  the server sends this value back
#define S2C_CHALLENGE			'A'	// + challenge value

// Send a userid, client remote address, is this server secure and engine build number
#define S2C_CONNECTION			'B'




// Response to server info requests
#define S2A_INFO				'C' // + Address, hostname, map, gamedir, gamedescription, active players, maxplayers, protocol





// Generic Ping Request
#define A2A_PING				'i'	// respond with an A2A_ACK

// Generic Ack
#define A2A_ACK					'j'	// general acknowledgement without info

// Print to client console.
#define	A2C_PRINT				'l'	// print a message on client





// MOTD response HLMaster
#define	M2A_MOTD				'h'





#define PORT_CLIENT		"27005"     // Must use atoi to convert to integer
#define PORT_SERVER		"27015"     //  "




//
// client to server
//
#define	clc_move				3		// [[usercmd_t]
#define	clc_stringcmd			4		// [string] message
#define	clc_delta				5		// [byte] sequence number, requests delta compression of message





//==============================================

// if the high bit of the client to server byte is set, the low bits are
// client move cmd bits
// ms and angle2 are allways sent, the others are optional
#define	CM_ANGLE1 	(1<<0)
#define	CM_ANGLE2 	(1<<1)
#define	CM_ANGLE3 	(1<<2)
#define	CM_FORWARD	(1<<3)
#define	CM_SIDE		(1<<4)
#define	CM_UP		(1<<5)
#define	CM_BUTTONS	(1<<6)
#define	CM_IMPULSE	(1<<7)





/*
==========================================================

ELEMENTS COMMUNICATED ACROSS THE NET

==========================================================
*/

#define	MAX_CLIENTS		32

#define	UPDATE_BACKUP	32	// copies of entity_state_t to keep buffered
							// must be power of two
#define	UPDATE_MASK		(UPDATE_BACKUP-1)

// entity_state_t is the information conveyed from the server
// in an update message
typedef struct
{
	int		number;			// edict index

	int		flags;			// nolerp, etc
	vec3_t	origin;
	vec3_t	angles;
	int		modelindex;
	int		frame;
	int		colormap;
	int		skinnum;
	int		effects;
} entity_state_t;

#define	MAX_PACKET_ENTITIES	64	// doesn't count nails
typedef struct
{
	int		num_entities;
	entity_state_t	entities[MAX_PACKET_ENTITIES];
} packet_entities_t;

// Server default maxplayers value
#define DEFAULT_SERVER_CLIENTS	8


typedef struct usercmd_s
{
	byte	msec;           // Duration in ms of command

	vec3_t	angles;			// Command view angles.

// intended velocities
	float	forwardmove;    // Forward velocity.
	float	sidemove;       // Sideways velocity.
	float	upmove;         // Upward velocity.
	byte	lightlevel;     // Light level at spot where we are standing.
	unsigned short  buttons;  // Attack buttons
	byte    impulse;          // Impulse command issued.
} usercmd_t;

#endif // PROTOCOL_H