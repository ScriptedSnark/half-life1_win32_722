// textures.h
#if !defined( TEXTURES_H )
#define TEXTURES_H
#if defined( _WIN32 )
#pragma once
#endif

qboolean	TEX_InitFromWad( char* path );
void		TEX_CleanupWadInfo( void );
int			TEX_LoadLump( char* name, byte* dest );
void		TEX_AddAnimatingTextures( void );

#endif // TEXTURES_H