// r_studio.h
#ifndef R_STUDIO_H
#define R_STUDIO_H
#ifdef _WIN32
#pragma once
#endif

int		R_StudioBodyVariations( struct model_s* model );

int		R_StudioDrawModel( int flags );
int		R_StudioDrawPlayer( int flags, struct player_state_s* pplayer );

#endif // R_STUDIO_H