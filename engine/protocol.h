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

// Special protocol for rejected connections.
#define S2C_CONNREJECT			'l' 

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


// Challenge response from master
#define M2A_CHALLENGE			's'
								//'s' + challenge value

#define PORT_CLIENT		"27005"     // Must use atoi to convert to integer
#define PORT_SERVER		"27015"     //  "


//==================
// note that there are some defs.qc that mirror to these numbers
// also related to svc_strings[] in cl_parse
//==================

//
// server to client
//
#define	svc_bad					0
#define	svc_nop					1
#define	svc_disconnect			2
#define svc_updatestat			3		// [byte] [long]
#define	svc_version				4		// [long] server version
#define	svc_setview				5		// [short] entity number
#define svc_sound				6		// <see code>
#define	svc_time				7		// [float] server time
#define	svc_print				8		// [string] null terminated string
#define	svc_stufftext			9		// [string] stuffed into client's console buffer
											// the string should be \n terminated
#define	svc_setangle			10		// [angle3] set the view angle to this absolute value
#define	svc_serverinfo			11		// [long] version
											// [string] signon string
											// [string]..[0]model cache
											// [string]...[0]sounds cache
#define	svc_lightstyle			12		// [byte] [string]
#define svc_updatename			13		// [byte] [string]
#define	svc_updatefrags			14		// [byte] [short]
#define	svc_clientdata			15		// <shortbits + data>
#define svc_stopsound			16		// <see code>
#define svc_updatecolors		17		// [byte] [byte]
#define	svc_particle			18		// [vec3] <variable>
#define	svc_damage				19
#define svc_spawnstatic			20

#define svc_spawnbaseline		22
#define svc_tempentity			23

#define svc_signonnum			25		// [byte]  used for the signon sequence
#define svc_centerprint			26		// [string] to put in center of the screen
#define svc_killedmonster		27
#define svc_foundsecret			28

#define svc_spawnstaticsound	29		// [coord3] [byte] samp [byte] vol [byte] aten
#define svc_intermission		30		// [string] music
#define svc_finale				31		// [string] music [string] text
#define	svc_cdtrack				32		// [byte] track [byte] looptrack

#define svc_cutscene			34
#define svc_weaponanim			35

#define svc_roomtype			37		// [byte] roomtype (dsp effect)
#define svc_addangle			38		// [angle3] set the view angle to this absolute value
#define svc_newusermsg			39
#define svc_download			40
#define	svc_packetentities		41		// [...]  Non-delta compressed entities
#define	svc_deltapacketentities	42		// [...]  Delta compressed entities
#define svc_playerinfo			43
#define	svc_chokecount   		44		// # of packets held back on channel because too much data was flowing.
#define svc_resourcelist		45
#define svc_newmovevars			46

#define svc_resourcerequest		48

#define svc_crosshairangle		50		// [char] pitch * 5 [char] yaw * 5
#define svc_soundfade			51      // char percent, char holdtime, char fadeouttime, char fadeintime
#define svc_clientmaxspeed		52
#define svc_lastmsg				52

//
// client to server
//
#define	clc_bad					0		// immediately drop client when received
#define clc_nop					1		// used to pad messages, ignored by the server entirely
#define	clc_move				3		// [[usercmd_t]
#define	clc_stringcmd			4		// [string] message
#define	clc_delta				5		// [byte] sequence number, requests delta compression of message
#define clc_tmove				6		// teleport request, spectator only
#define clc_upload				7		// teleport request, spectator only
#define clc_resourcelist		8

//==============================================


#define	SU_VIEWHEIGHT	(1<<0)
#define	SU_IDEALPITCH	(1<<1)
#define	SU_PUNCH1		(1<<2)


#define	SU_VELOCITY1	(1<<5)


#define	SU_WEAPONS		(1<<8)
#define	SU_ITEMS		(1<<9)
#define	SU_ONGROUND		(1<<10)		// no data follows, the bit is it
#define	SU_INWATER		(1<<11)		// no data follows, the bit is it


#define	SU_FULLYINWATER	(1<<15)

// defaults for clientinfo messages
#define	DEFAULT_VIEWHEIGHT	22


// game types sent by serverinfo
// these determine which intermission screen plays
#define	GAME_COOP			0
#define	GAME_DEATHMATCH		1

//==============================================

// playerinfo flags from server
// playerinfo allways sends: playernum, flags, origin[] and framenumber

#define	PF_MSEC			(1<<0)
#define	PF_COMMAND		(1<<1)



#define	PF_MODEL		(1<<5)
#define	PF_SKINNUM		(1<<6)
#define	PF_EFFECTS		(1<<7)
#define	PF_WEAPONMODEL	(1<<8)
#define PF_DEAD			(1<<9)


#define	PF_MOVETYPE		(1<<12)
#define	PF_SEQUENCE		(1<<13)
#define	PF_RENDER		(1<<14)
#define	PF_FRAMERATE	(1<<15)
#define	PF_BODY			(1<<16)
#define	PF_CONTROLLER1	(1<<17)
#define	PF_CONTROLLER2	(1<<18)
#define	PF_CONTROLLER3	(1<<19)
#define	PF_CONTROLLER4	(1<<20)
#define	PF_BLENDING1	(1<<21)
#define	PF_BLENDING2	(1<<22)
#define	PF_BASEVELOCITY	(1<<23)
#define	PF_FRICTION		(1<<24)
#define	PF_PING			(1<<25)




#define PN_SPECTATOR	(1<<7)		// extra playernum flag to determine if we joined as spectator

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

//==============================================

// if the high bit of the servercmd is set, the low bits are fast update flags:
#define	U_MOREBITS		(1<<0)
#define	U_ORIGIN1		(1<<1)
#define	U_ORIGIN2		(1<<2)
#define	U_ORIGIN3		(1<<3)
#define	U_REMOVE		(1<<4)		// REMOVE this entity, don't add it
#define U_ANGLE2		(1<<5)
#define	U_MOVETYPE		(1<<6)
#define	U_FRAME			(1<<7)
#define U_EVENMOREBITS	(1<<8)
#define U_ANGLE1		(1<<9)
#define U_ANGLE3		(1<<10)
#define	U_MODELINDEX	(1<<11)
#define	U_SEQUENCE		(1<<12)
#define	U_CUSTOM		(1<<13)		// Custom entity data (beams)
#define U_EFFECTS		(1<<14)
#define U_LONGENTITY	(1<<15)
#define U_FRAMERATE		(1<<16)

#define U_COLORMAP		(1<<18)
#define U_CONTENTS		(1<<19)
#define U_RENDER		(1<<20)
#define U_MONSTERMOVE	(1<<21)
#define U_BODY			(1<<22)
#define U_YETMOREBITS	(1<<23)
#define	U_CONTROLLER1	(1<<24)
#define	U_CONTROLLER2	(1<<25)
#define	U_CONTROLLER3	(1<<26)
#define	U_CONTROLLER4	(1<<27)
#define	U_BLENDING1		(1<<28)
#define	U_BLENDING2		(1<<29)
#define U_SCALE			(1<<30)

#define U_BBOXMINS1		(1<<0)
#define U_BBOXMINS2		(1<<1)
#define U_BBOXMINS3		(1<<2)
#define U_BBOXMAXS1		(1<<3)
#define U_BBOXMAXS2		(1<<4)
#define U_BBOXMAXS3		(1<<5)

//==============================================

// sound flags
#define	SND_VOLUME			(1<<0)		// send volume
#define	SND_ATTENUATION		(1<<1)		// send attenuation
#define SND_LARGE_INDEX		(1<<2)		// send sound number as short instead of byte
#define SND_PITCH			(1<<3)		// send pitch
#define SND_SENTENCE		(1<<4)		// set if sound num is actually a sentence num
#define SND_STOP			(1<<5)		// duplicated in dlls/util.h stop sound
#define SND_CHANGE_VOL		(1<<6)		// duplicated in dlls/util.h change sound vol
#define SND_CHANGE_PITCH	(1<<7)		// duplicated in dlls/util.h change sound pitch
#define SND_SPAWNING		(1<<8)		// duplicated in dlls/util.h we're spawing, used in some cases for ambients 

#define DEFAULT_SOUND_PACKET_VOLUME			1.0
#define DEFAULT_SOUND_PACKET_ATTENUATION	1.0

//==============================================

// usermsg flags
#define MSG_FL_PVS				(1<<0)	// Send to PVS
#define MSG_FL_PAS				(1<<1)	// Send to PAS

/*
==========================================================

ELEMENTS COMMUNICATED ACROSS THE NET

==========================================================
*/

#define	MAX_CLIENTS		32

#define	UPDATE_BACKUP	32	// copies of entity_state_t to keep buffered
							// must be power of two
#define	UPDATE_MASK		(UPDATE_BACKUP - 1)

#define	MAX_PACKET_ENTITIES	256	// doesn't count nails
typedef struct
{
	int		num_entities;
	entity_state_t* entities;
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