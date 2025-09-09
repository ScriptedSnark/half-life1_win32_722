// host_cmd.h
#if !defined( HOST_CMD_H )
#define HOST_CMD_H
#ifdef _WIN32
#pragma once
#endif

extern int  gHostSpawnCount;

void		Host_LoadProfile( void );
void		Host_UnloadProfile( char* name );

int			EntryInTable( SAVERESTOREDATA* pSaveData, const char* pMapName, int index );
int			EntityInSolid( edict_t* pent );
void		LandmarkOrigin( SAVERESTOREDATA* pSaveData, vec_t* output, const char* pLandmarkName );

void		Host_ClearGameState( void );
int			Host_Load( const char* pName );

SAVERESTOREDATA* SaveInit( int size );
SAVERESTOREDATA* SaveGamestate( void );
void		SaveExit( SAVERESTOREDATA* save );
int			LoadGamestate( char* level, int createPlayers );

void		Host_EndSection( const char* pszSection );

char*		Host_SaveGameDirectory( void );
void		Host_ClearSaveDirectory( void );

void		EntityInit( edict_t* pEdict, int className );
void		EntityPatchWrite( SAVERESTOREDATA* pSaveData, const char* level );
void		EntityPatchRead( SAVERESTOREDATA* pSaveData, const char* level );

void		DirectoryCopy( const char* pPath, FILE* pFile );
void		DirectoryExtract( FILE* pFile, int fileCount );
int			DirectoryCount( const char* pPath );
void		DirectoryClear( const char* pPath );

void COM_HexConvert( const char* pszInput, int nInputLength, unsigned char* pOutput );

#endif // HOST_CMD_H