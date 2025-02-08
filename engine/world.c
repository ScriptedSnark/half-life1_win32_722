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
qboolean SV_RecursiveHullCheck( hull_t* hull, int num, float p1f, float p2f, vec_t* p1, vec_t* p2, trace_t* trace )
{
	// TODO: Implement

	return FALSE;
}

/*
===============
SV_ClipMoveToEntity

Handles selection or creation of a clipping hull, and offseting (and
eventually rotation) of the end points
===============
*/
trace_t SV_ClipMoveToEntity( edict_t* ent, const vec_t* start, const vec_t* mins, const vec_t* maxs, const vec_t* end )
{
	// TODO: Implement
	trace_t t = { 0 };

	return t;
}