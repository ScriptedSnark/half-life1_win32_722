// host_cmd.h
#if !defined( HOST_CMD_H )
#define HOST_CMD_H
#ifdef _WIN32
#pragma once
#endif

extern int  gHostSpawnCount;
extern int	current_skill;

void		Host_ClearGameState( void );



int			LoadGamestate( char* level, int createPlayers );



void		Host_EndSection( const char* pszSection );

void		Host_ClearSaveDirectory( void );

#endif // HOST_CMD_H