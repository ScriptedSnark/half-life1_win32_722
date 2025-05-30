// hashpak.h
#ifndef HASHPAK_H
#define HASHPAK_H
#ifdef _WIN32
#pragma once
#endif

void	 HPAK_AddLump( char* pakname, resource_t* pResource, void* pData, FILE* fpSource );
qboolean HPAK_ResourceForHash( char* pakname, byte* hash, resource_t* pResourceEntry );
qboolean HPAK_GetDataPointer( char* pakname, resource_t* pResource, FILE** pfOutput );

#endif // HASHPAK_H