// hashpak.h
#ifndef HASHPAK_H
#define HASHPAK_H
#ifdef _WIN32
#pragma once
#endif

#define	HASHPAK_FILENAME	"custom.hpk"
#define	HASHPAK_EXTENSION	".hpk"

#define HASHPAK_VERSION		1

void		HPAK_AddLump( char* pakname, resource_t* pResource, void* pData, FILE* fpSource );
void		HPAK_RemoveLump( char* pakname, resource_t* pResource );

qboolean	HPAK_ResourceForIndex( char* pakname, int nIndex, resource_t* pResource );
qboolean	HPAK_ResourceForHash( char* pakname, byte* hash, resource_t* pResourceEntry );
qboolean	HPAK_GetDataPointer( char* pakname, resource_t* pResource, FILE** pfOutput );
void		HPAK_CreatePak( char* pakname, resource_t* pResource, void* pData, FILE* fpSource );

void		HPAK_List_f( void );
void		HPAK_Remove_f( void );

#endif // HASHPAK_H