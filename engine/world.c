// world.c -- world query functions

#include "quakedef.h"


/*
===============
SV_ClearWorld

===============
*/
void SV_ClearWorld( void )
{
	// TODO: Implement
}

/*
==================
SV_RecursiveHullCheck

==================
*/
qboolean SV_RecursiveHullCheck(hull_t *hull, int num, float p1f, float p2f, vec3_t p1, vec3_t p2, trace_t *trace)
{
	// TODO: Implement

	return FALSE;
}

/*
==================
SV_ClipMoveToEntity

Handles selection or creation of a clipping hull, and offseting (and
eventually rotation) of the end points
==================
*/
trace_t SV_ClipMoveToEntity( edict_t *ent, vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end )
{
	// TODO: Implement
	trace_t t = { 0 };

	return t;
}