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

void	PF_traceline_DLL( const float* v1, const float* v2, int fNoMonsters, edict_t* pentToSkip, TraceResult* ptr );
void	TraceHull( const float* v1, const float* v2, int fNoMonsters, int hullNumber, edict_t* pentToSkip, TraceResult* ptr );
void	TraceSphere( const float* v1, const float* v2, int fNoMonsters, float radius, edict_t* pentToSkip, TraceResult* ptr );
void	TraceModel( const float* v1, const float* v2, edict_t* pent, TraceResult* ptr );

msurface_t* SurfaceAtPoint( model_t* pModel, mnode_t* node, vec_t* start, vec_t* end );
const char* TraceTexture( edict_t* pTextureEntity, const float* v1, const float* v2 );
void	PF_TraceToss_DLL( edict_t* pent, edict_t* pentToIgnore, TraceResult* ptr );
int		TraceMonsterHull( edict_t* pEdict, const float* v1, const float* v2, int fNoMonsters, edict_t* pentToSkip, TraceResult* ptr );

edict_t* PF_checkclient_I( edict_t* pEdict );
struct mnode_s* PVSNode( struct mnode_s* node, vec_t* emins, vec_t* emaxs );
edict_t* PVSFindEntities( edict_t* pplayer );

void	PF_stuffcmd_I( edict_t* pEdict, char* szFmt, ... );
void	PF_localcmd_I( char* str );

edict_t* FindEntityInSphere( edict_t* pEdictStartSearchAfter, const float* org, float rad );
edict_t* PF_Spawn_I( void );
edict_t* CreateNamedEntity( int className );
void	PF_Remove_I( edict_t* ed );
edict_t* FindEntityByString( edict_t* pEdictStartSearchAfter, const char* pszField, const char* pszValue );
int		GetEntityIllum( edict_t* pEnt );

int		PF_precache_sound_I( char* s );
int		PF_precache_model_I( char* s );

int		PF_IsMapValid_I( char* mapname );
int		PF_NumberOfEntities_I( void );

int		PF_walkmove_I( edict_t* ent, float yaw, float dist, int iMode );
int		PF_droptofloor_I( edict_t* ent );
int		PF_DecalIndex( const char* name );
void	PF_lightstyle_I( int style, char* val );












float	RandomFloat( float flLow, float flHigh );
int		RandomLong( long lLow, long lHigh );

#endif // PR_CMDS_H