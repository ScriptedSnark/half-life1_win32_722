// r_trans.h
#ifndef R_TRANS_H
#define R_TRANS_H
#ifdef _WIN32
#pragma once
#endif

void	R_AllocObjects( int nMax );
void	R_DestroyObjects( void );

float	GlowBlend( cl_entity_t* pEntity );
void	RotatedBBox( vec_t* mins, vec_t* maxs, vec_t* angles, vec_t* tmins, vec_t* tmaxs );

void	AddTEntity( cl_entity_t* pEnt );
void	AppendTEntity( cl_entity_t* pEnt );
void	R_DrawTEntitiesOnList( void );

#endif // R_TRANS_H