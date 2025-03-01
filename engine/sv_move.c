// sv_move.c -- monster movement

#include "quakedef.h"
#include "server.h"
#include "pr_cmds.h"

/*
=============
SV_CheckBottom

Returns false if any part of the bottom of the entity is off an edge that
is not a staircase.

=============
*/
int c_yes, c_no;

qboolean SV_CheckBottom( edict_t* ent )
{
	// TODO: Implement
	return FALSE;
}

// TODO: Implement

/*
=============
SV_movetest
=============
*/
qboolean SV_movetest( edict_t* ent, vec_t* move, qboolean relink )
{
	// TODO: Implement
	return FALSE;
}


/*
===============
SV_movestep

Called by monster program code.
The move will be adjusted for slopes and stairs, but if the move isn't
possible, no move is done, false is returned, and
pr_global_struct->trace_normal is set to the normal of the blocking wall
===============
*/
qboolean SV_movestep( edict_t* ent, vec_t* move, qboolean relink )
{
	// TODO: Implement
	return FALSE;
}

// TODO: Implement

/*
===============
SV_MoveToOrigin_I

Moves the given entity to the given destination
===============
*/
void SV_MoveToOrigin_I( edict_t* ent, const float* pflGoal, float dist, int iStrafe )
{
	// TODO: Implement
}