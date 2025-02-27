// pr_edict.h - expose things from pr_edict.c
#ifndef PR_EDICT_H
#define PR_EDICT_H

void SuckOutClassname( char* szInputStream, edict_t* pEdict );

void ReleaseEntityDLLFields( edict_t* pEdict );
void InitEntityDLLFields( edict_t* pEdict );
void* PvAllocEntPrivateData( edict_t* pEdict, long cb );
void* PvEntPrivateData( edict_t* pEdict );
void FreeEntPrivateData( edict_t* pEdict );

edict_t* PEntityOfEntOffset( int iEntOffset );
int EntOffsetOfPEntity( const edict_t* pEdict );
int IndexOfEdict( const edict_t* pEdict );
edict_t* PEntityOfEntIndex( int iEntIndex );
char* SzFromIndex( int iString );
entvars_t* GetVarsOfEnt( edict_t* pEdict );
edict_t* FindEntityByVars( entvars_t* pvars );

float CVarGetFloat( const char* szVarName );
const char* CVarGetString( const char* szVarName );
void CVarSetFloat( const char* szVarName, float flValue );
void CVarSetString( const char* szVarName, const char* szValue );

int AllocEngineString( const char* szValue );
void SaveSpawnParms( edict_t* pEdict );
void* GetModelPtr( edict_t* pEdict );

#endif