// save.h - everything related to server data saving/restoring
#if !defined( SAVE_H )
#define SAVE_H
#ifdef _WIN32
#pragma once
#endif

// Makes a 4-byte "packed ID" int out of 4 characters
#define MAKEID(d,c,b,a)					( ((int)(a) << 24) | ((int)(b) << 16) | ((int)(c) << 8) | ((int)(d)) )

#define SAVEFILE_HEADER		MAKEID('V','A','L','V')		// little-endian "VALV"
#define SAVEGAME_HEADER		MAKEID('J','S','A','V')		// little-endian "JSAV"
#define SAVEGAME_VERSION	0x0071						// Version 0.71

//=============================================================================
// Those are just a save data structs, which are responsible for saving/restoring
// game data when the "save" button has been pressed
//=============================================================================
typedef struct
{
	int		saveId;
	int		version;
	int		skillLevel;
	int		entityCount;
	int		connectionCount;
	int		lightStyleCount;
	float	time;
	char	mapName[32];
	char	skyName[32];
	int		skyColor_r;
	int		skyColor_g;
	int		skyColor_b;
	float	skyVec_x;
	float	skyVec_y;
	float	skyVec_z;
} SAVE_HEADER;

typedef struct
{
	char mapName[32];
	char comment[80];

	int mapCount;
} GAME_HEADER;

typedef struct
{
	int index;
	char style[MAX_LIGHTSTYLES];
} SAVELIGHTSTYLE;

void    CL_Save( char* name );

#endif // SAVE_H