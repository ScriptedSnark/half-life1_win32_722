#if !defined( PROFILE_H )
#define PROFILE_H
#ifdef _WIN32
#pragma once
#endif

typedef struct profile_keybinding_s
{
	char* binding;
	float m_pitch;
	float lookstrafe;
	float lookspring;
	float crosshair;
	float windowed_mouse;
	float mfilter;
	float mlook;
	float joystick;
	float sensitivity;
} profile_keybinding_t;

typedef struct profile_s
{
	char name[32];
	int size;

	profile_keybinding_t keybindings[256];

	float viewsize;
	float brightness;
	float gamma;
	float bgmvolume;
	float suitvolume;
	float hisound;
	float volume;
	float a3d;
} profile_t;

extern char		g_szProfileName[MAX_QPATH];
extern qboolean	g_bForceReloadOnCA_Active;

void	ExecuteProfileSettings( char* pszName );
void	GetProfileRegKeyValue( char* pszName, char* pszPath, char* pszSetting, char* pszElement, char* pszBuffer, int bufLength, char* pszDefaultValue );

void	ClearProfileKeyBindings( profile_t* profile );

int		LoadProfile( char* pszName, profile_t* profile );
void	LoadProfileFile( void );
void	LoadProfileKeyBindings( char* pszName, profile_t* profile );
void	LoadProfileCvars( char* pszName, profile_t* profile );

#endif // PROFILE_H