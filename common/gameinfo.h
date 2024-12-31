#pragma once

// Game info retreived by the launcher
typedef struct GameInfo_s
{
	int			build_number;

	// multiplayer
	qboolean	multiplayer;
	int			maxclients; // svs.maxclients

	char		levelname[32];
	qboolean	active; // sv.active

	// remote address
	unsigned int	ip;
	unsigned short	port;

	qboolean	channel;

	// client
	char		state_description[64];
	cactive_t	state; // cls.state
	int			signon; // signon num
} GameInfo_t;