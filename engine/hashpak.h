// hashpak.h
#ifndef HASHPAK_H
#define HASHPAK_H
#ifdef _WIN32
#pragma once
#endif

void	HPAK_AddLump( char* pakname, resource_t* pResource, void* pData, FILE* fpSource );

#endif // HASHPAK_H