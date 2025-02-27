// pr_cmds.h
#ifndef PR_CMDS_H
#define PR_CMDS_H
#ifdef _WIN32
#pragma once
#endif

void	PF_makevectors_I( const float* rgflVector );
float	PF_Time( void );
void	PF_setorigin_I( edict_t* e, const float* org );
void	PF_setsize_I( edict_t* e, float* rgflMin, float* rgflMax );
void	PF_setmodel_I( edict_t* e, const char* m );
int		PF_modelindex( const char* pstr );
int		ModelFrames( int modelIndex );

void	PF_bprint( char* s );
void	PF_sprint( char* s, int entnum );
void	ClientPrintf( edict_t* pEdict, PRINT_TYPE ptype, const char* szMsg );

float	PF_vectoyaw_I( const float* rgflVector );
void	PF_vectoangles_I( const float* rgflVectorIn, float* rgflVectorOut );
void	PF_particle_I( const float* org, const float* dir, float color, float count );
void	PF_ambientsound_I( edict_t* entity, float* pos, const char* samp, float vol, float attenuation, int fFlags, int pitch );
void	PF_sound_I( edict_t* entity, int channel, const char* sample, float volume, float attenuation, int fFlags, int pitch );




msurface_t* SurfaceAtPoint( model_t* pModel, mnode_t* node, vec_t* start, vec_t* end );

void SV_SetGlobalTrace( trace_t* ptrace );

struct mnode_s* PVSNode( struct mnode_s* node, vec_t* emins, vec_t* emaxs );

float	RandomFloat( float flLow, float flHigh );
int		RandomLong( long lLow, long lHigh );

int PF_IsMapValid_I( char* mapname );

#endif // PR_CMDS_H