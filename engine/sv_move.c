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
	vec3_t	mins, maxs, start, stop;
	trace_t	trace;
	int		x, y;
	float	mid, bottom;
	qboolean monsterClip;

	// Check for FL_MONSTERCLIP flag
	monsterClip = (ent->v.flags & FL_MONSTERCLIP) ? TRUE : FALSE;

	VectorAdd(ent->v.origin, ent->v.mins, mins);
	VectorAdd(ent->v.origin, ent->v.maxs, maxs);

// if all of the points under the corners are solid world, don't bother
// with the tougher checks
// the corners must be within 16 of the midpoint
	start[2] = mins[2] - 1.0;
	for (x = 0; x <= 1; x++)
	{
		for (y = 0; y <= 1; y++)
		{
			start[0] = x ? maxs[0] : mins[0];
			start[1] = y ? maxs[1] : mins[1];
			if (SV_PointContents(start) != CONTENTS_SOLID)
				goto realcheck;
		}
	}

	c_yes++;
	return TRUE;		// we got out easy

realcheck:
	c_no++;
//
// check it for real...
//
	start[2] = mins[2] + sv_stepsize.value;

// the midpoint must be within 16 of the bottom
	start[0] = stop[0] = (mins[0] + maxs[0]) * 0.5;
	start[1] = stop[1] = (mins[1] + maxs[1]) * 0.5;
	stop[2] = start[2] - 2.0 * sv_stepsize.value;
	trace = SV_Move(start, vec3_origin, vec3_origin, stop, MOVE_NOMONSTERS, ent, monsterClip);

	if (trace.fraction == 1.0)
		return FALSE;
	mid = bottom = trace.endpos[2];

// the corners must be within 16 of the midpoint
	for (x = 0; x <= 1; x++)
	{
		for (y = 0; y <= 1; y++)
		{
			start[0] = stop[0] = x ? maxs[0] : mins[0];
			start[1] = stop[1] = y ? maxs[1] : mins[1];

			trace = SV_Move(start, vec3_origin, vec3_origin, stop, MOVE_NOMONSTERS, ent, monsterClip);

			if (trace.fraction != 1.0 && trace.endpos[2] > bottom)
				bottom = trace.endpos[2];

			if (trace.fraction == 1.0 || (mid - trace.endpos[2]) > sv_stepsize.value)
				return FALSE;
		}
	}

	c_yes++;
	return TRUE;
}

/*
=============
SV_movetest
=============
*/
qboolean SV_movetest( edict_t* ent, vec_t* move, qboolean relink )
{
	vec3_t	oldorg, neworg, end;
	trace_t	trace;

// try the move
	VectorCopy(ent->v.origin, oldorg);
	VectorAdd(ent->v.origin, move, neworg);

// push down from a step height above the wished position
	neworg[2] += sv_stepsize.value;
	VectorCopy(neworg, end);
	end[2] -= sv_stepsize.value * 2.0;

	trace = SV_MoveNoEnts(neworg, ent->v.mins, ent->v.maxs, end, MOVE_NORMAL, ent);

	if (trace.allsolid)
		return FALSE;

	if (trace.startsolid)
	{
		neworg[2] -= sv_stepsize.value;
		trace = SV_MoveNoEnts(neworg, ent->v.mins, ent->v.maxs, end, MOVE_NORMAL, ent);
		if (trace.allsolid || trace.startsolid)
			return FALSE;
	}

	if (trace.fraction == 1.0)
	{
		// if monster had the ground pulled out, go ahead and fall
		if (ent->v.flags & FL_PARTIALGROUND)
		{
			VectorAdd(ent->v.origin, move, ent->v.origin);
			if (relink)
				SV_LinkEdict(ent, TRUE);
			ent->v.flags &= ~FL_ONGROUND;
//			Con_Printf("fall down\n"); 
			return TRUE;
		}

		return FALSE;		// walked off an edge
	}

// check point traces down for dangling corners
	VectorCopy(trace.endpos, ent->v.origin);

	if (!SV_CheckBottom(ent))
	{
		if (!(ent->v.flags & FL_PARTIALGROUND))
		{
			VectorCopy(oldorg, ent->v.origin);
			return FALSE;
		}

		// entity had floor mostly pulled out from underneath it
		// and is trying to correct
	}
	else
	{
		if (ent->v.flags & FL_PARTIALGROUND)
		{
			// back on ground
			ent->v.flags &= ~FL_PARTIALGROUND;
		}

		ent->v.groundentity = trace.ent;
	}

	// the move is ok
	if (relink)
		SV_LinkEdict(ent, TRUE);
	return TRUE;
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
	float		dz;
	vec3_t		oldorg, neworg, end;
	trace_t		trace;
	int			i;
	edict_t*	enemy;

	qboolean	monsterClip;

	// Check for FL_MONSTERCLIP flag
	monsterClip = (ent->v.flags & FL_MONSTERCLIP) ? TRUE : FALSE;

// try the move
	VectorCopy(ent->v.origin, oldorg);
	VectorAdd(ent->v.origin, move, neworg);

// flying monsters don't step up
	if (ent->v.flags & (FL_FLY | FL_SWIM))
	{
	// try one move with vertical motion, then one without
		for (i = 0; i < 2; i++)
		{
			VectorAdd(ent->v.origin, move, neworg);
			enemy = ent->v.enemy;
			if (i == 0 && enemy != NULL)
			{
				dz = ent->v.origin[2] - enemy->v.origin[2];
				if (dz > 40)
					neworg[2] -= 8;
				else if (dz < 30)
					neworg[2] += 8;
			}
			trace = SV_Move(ent->v.origin, ent->v.mins, ent->v.maxs, neworg, MOVE_NORMAL, ent, monsterClip);

			if (trace.fraction == 1)
			{
				if ((ent->v.flags & FL_SWIM) && SV_PointContents(trace.endpos) == CONTENTS_EMPTY)
					return FALSE;	// swim monster left water

				VectorCopy(trace.endpos, ent->v.origin);
				if (relink)
					SV_LinkEdict(ent, TRUE);
				return TRUE;
			}

			if (enemy == NULL)
				break;
		}

		return FALSE;
	}

// push down from a step height above the wished position
	neworg[2] += sv_stepsize.value;
	VectorCopy(neworg, end);
	end[2] -= sv_stepsize.value * 2.0;

	trace = SV_Move(neworg, ent->v.mins, ent->v.maxs, end, MOVE_NORMAL, ent, monsterClip);

	if (trace.allsolid)
		return FALSE;

	if (trace.startsolid)
	{
		neworg[2] -= sv_stepsize.value;
		trace = SV_Move(neworg, ent->v.mins, ent->v.maxs, end, MOVE_NORMAL, ent, monsterClip);
		if (trace.allsolid || trace.startsolid)
			return FALSE;
	}
	if (trace.fraction == 1)
	{
		// if monster had the ground pulled out, go ahead and fall
		if (ent->v.flags & FL_PARTIALGROUND)
		{
			VectorAdd(ent->v.origin, move, ent->v.origin);
			if (relink)
				SV_LinkEdict(ent, TRUE);
			ent->v.flags &= ~FL_ONGROUND;
//			Con_Printf("fall down\n");
			return TRUE;
		}

		return FALSE;		// walked off an edge
	}

// check point traces down for dangling corners
	VectorCopy(trace.endpos, ent->v.origin);

	if (!SV_CheckBottom(ent))
	{
		if (!(ent->v.flags & FL_PARTIALGROUND))
		{
			VectorCopy(oldorg, ent->v.origin);
			return FALSE;
		}

		// entity had floor mostly pulled out from underneath it
		// and is trying to correct
	}
	else
	{
		if (ent->v.flags & FL_PARTIALGROUND)
		{
			// back on ground
			ent->v.flags &= ~FL_PARTIALGROUND;
		}

		ent->v.groundentity = trace.ent;
	}

	// the move is ok
	if (relink)
		SV_LinkEdict(ent, TRUE);
	return TRUE;
}


//============================================================================








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