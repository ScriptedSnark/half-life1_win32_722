#if !defined( GAMEINFO_H )
#define GAMEINFO_H
#ifdef _WIN32
#pragma once
#endif

// Game info retrieved by the launcher
typedef struct GameInfo_s
{
	int			build_number;

	// multiplayer
	qboolean	multiplayer;
	int			maxclients; // svs.maxclients

	char		levelname[32];
	qboolean	active; // sv.active

	// remote address
	unsigned char	ip[4];
	unsigned short	port;

	qboolean	channel;

	// client
	char		szStatus[64];
	cactive_t	state; // cls.state
	int			signon; // signon num
} GameInfo_t;

#endif // GAMEINFO_H