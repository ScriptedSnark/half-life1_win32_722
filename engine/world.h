// world.h
#ifndef WORLD_H
#define WORLD_H
#ifdef _WIN32
#pragma once
#endif

typedef struct areanode_s
{
	int		axis;		// -1 = leaf node
	float	dist;
	struct areanode_s	*children[2];
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

qboolean SV_RecursiveHullCheck( hull_t* hull, int num, float p1f, float p2f, vec_t* p1, vec_t* p2, trace_t* trace );

#endif // WORLD_H