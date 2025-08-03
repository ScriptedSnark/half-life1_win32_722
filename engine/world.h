// world.h
#ifndef WORLD_H
#define WORLD_H
#ifdef _WIN32
#pragma once
#endif

#define MOVE_NORMAL         0 // normal trace
#define MOVE_NOMONSTERS     1 // ignore monsters (edicts with flags (FL_MONSTER|FL_FAKECLIENT|FL_CLIENT) set)
#define MOVE_MISSILE        2 // extra size for monsters

typedef struct areanode_s
{
	int		axis;		// -1 = leaf node
	float	dist;
	struct areanode_s* children[2];
	link_t	trigger_edicts;
	link_t	solid_edicts;
} areanode_t;

#define	AREA_DEPTH	4
#define	AREA_NODES	32

extern	areanode_t	sv_areanodes[AREA_NODES];

void SV_ClearWorld( void );
// called after the world model has been loaded, before linking any entities

void SV_UnlinkEdict( edict_t* ent );
// call before removing an entity, and before trying to move one,
// so it doesn't clip against itself
// flags ent->v.modified

void SV_LinkEdict( edict_t* ent, qboolean touch_triggers );
// Needs to be called any time an entity changes origin, mins, maxs, or solid
// flags ent->v.modified
// sets ent->v.absmin and ent->v.absmax
// if touchtriggers, calls prog functions for the intersected triggers

int SV_PointContents( const vec_t* p );
// returns the CONTENTS_* value from the world at the given point.
// does not check any entities at all

edict_t* SV_TestEntityPosition( edict_t* ent );

hull_t* SV_HullForEntity( edict_t* ent, const vec_t* mins, const vec_t* maxs, vec_t* offset );
qboolean SV_RecursiveHullCheck( hull_t* hull, int num, float p1f, float p2f, vec_t* p1, vec_t* p2, trace_t* trace );

void SV_MoveBounds( const vec_t* start, const vec_t* mins, const vec_t* maxs, const vec_t* end, vec_t* boxmins, vec_t* boxmaxs );

trace_t SV_MoveNoEnts( const vec_t* start, vec_t* mins, vec_t* maxs, const vec_t* end, int type, edict_t* passedict );
trace_t SV_Move( const vec_t* start, const vec_t* mins, const vec_t* maxs, const vec_t* end, int type, edict_t* passedict, qboolean monsterClipBrush );
// mins and maxs are reletive

// if the entire move stays in a solid volume, trace.allsolid will be set

// if the starting point is in a solid, it will be allowed to move out
// to an open area

// nomonsters is used for line of sight or edge testing, where mosnters
// shouldn't be considered solid objects

// passedict is explicitly excluded from clipping checks (normally NULL)

#endif // WORLD_H