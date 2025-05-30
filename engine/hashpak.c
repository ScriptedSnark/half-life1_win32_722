// hashpak.c - HPAK system, handles resources in a compressed and hashed form

#include "quakedef.h"
#include "custom.h"
#include "hashpak.h"

#define HASHPAK_EXTENSION       ".hpk"
#define HASHPAK_VERSION         1



/*
=================
HPAK_AddLump

Add a resource to an HPAK file or an HPAK queue
Compute MD5 hash of the resource data
=================
*/
void HPAK_AddLump( char* pakname, resource_t* pResource, void* pData, FILE* fpSource )
{
	// TODO: Implement
}

qboolean HPAK_ResourceForHash( char* pakname, byte* hash, resource_t* pResourceEntry )
{
	// TODO: Implement

	return FALSE;
}

qboolean HPAK_GetDataPointer( char* pakname, resource_t* pResource, FILE** pfOutput )
{
	// TODO: Implement

	return FALSE;
}