// r_studio.h
#ifndef R_STUDIO_H
#define R_STUDIO_H
#ifdef _WIN32
#pragma once
#endif

// Additional studio flags for client-side models
#define STUDIO_DYNAMIC_LIGHT		0x100	// force to use ambient shading
#define STUDIO_TRACE_HITBOX			0x0200	// always use hitbox trace instead of bbox
#define STUDIO_FORCE_SKYLIGHT		0x400	// always grab lightvalues from the sky settings (even if sky is invisible)

int		R_StudioBodyVariations( struct model_s* model );

int		R_StudioDrawModel( int flags );
int		R_StudioDrawPlayer( int flags, player_state_t* pplayer );
void	R_StudioDynamicLight( cl_entity_t* ent, alight_t* plight );
void	R_StudioEntityLight( alight_t* plight );
void	R_StudioClientEvents( void );
void	R_StudioRenderFinal( void );
void	GLR_StudioDrawShadow( void );
void	R_StudioDrawPoints( void );

#endif // R_STUDIO_H