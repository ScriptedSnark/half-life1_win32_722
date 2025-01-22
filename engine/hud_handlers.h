#if !defined( HUD_HANDLERS_H )
#define HUD_HANDLERS_H
#ifdef _WIN32
#pragma once
#endif

int		hudAddCommand( char* cmd_name, void (*function)(void) );

// message handling
int		hudServerCmd( char* pszCmdString );
int		hudClientCmd( char* pszCmdString );

int		hudHookUserMsg( char* szMsgName, pfnUserMsgHook pfn );

void	hudPlaySoundByName( char* szSound, float volume );
void	hudPlaySoundByIndex( int iSound, float volume );

void	hudDrawConsoleStringLen( const char* string, int* length, int* height );
void	hudConsolePrint( const char* string );

// info handling
void	hudGetPlayerInfo( int ent_num, hud_player_info_t* pinfo );

#endif // HUD_HANDLERS_H