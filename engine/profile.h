#if !defined( PROFILE_H )
#define PROFILE_H
#ifdef _WIN32
#pragma once
#endif

// Stores key bindings and related control settings for a player profile
typedef struct player_keybinding_s
{
	// Key binding name
	char* binding;

	// Control settings
	float	m_pitch;
	float	lookstrafe;
	float	lookspring;
	float	crosshair;
	float	windowed_mouse;
	float	mfilter;
	float	mlook;
	float	joystick;
	float	sensitivity;
} player_keybinding_t;

// Stores all player-specific configuration
typedef struct player_profile_s
{
	// Profile name
	char	name[32];
	// Total memory size used by key binding commands
	int		size;

	// keyboard/control configuration
	player_keybinding_t keybindings[256];

	// video settings
	float	viewsize;
	float	brightness;
	float	gamma;
	// audio settings
	float	bgmvolume;
	float	suitvolume;
	float	hisound;
	float	volume;
	float	a3d;
} player_profile_t;

extern char		g_szProfileName[MAX_QPATH];
extern qboolean	g_bForceReloadOnCA_Active;

void	Profile_LoadKeyBindings( char* pszName, player_profile_t* pProfile );
void	Profile_LoadSettings( char* pszName, player_profile_t* pProfile );
void	Profile_SaveKeyBindings( char* pszName, player_profile_t* pProfile );
void	Profile_SaveSettings( char* pszName, player_profile_t* pProfile );

void	Sys_GetProfileRegKeyValue( char* pszName, char* pszPath, char* pszSetting, char* pszElement, char* pszReturnString, int nReturnLength, char* pszDefaultValue );
void	Sys_SetProfileRegKeyValue( char* pszName, char* pszPath, char* pszSetting, char* pszElement, char* pszDefaultValue );

void	ExecuteProfileSettings( char* pszName );

#endif // PROFILE_H