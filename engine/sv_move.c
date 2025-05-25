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
		if (ent->v.flags & FL_PARTIALGROUND)
		{	// entity had floor mostly pulled out from underneath it
			// and is trying to correct
			if (relink)
				SV_LinkEdict(ent, TRUE);
			return TRUE;
		}
		VectorCopy(oldorg, ent->v.origin);
		return FALSE;
	}

	if (ent->v.flags & FL_PARTIALGROUND)
	{
//		Con_Printf("back on ground\n");
		ent->v.flags &= ~FL_PARTIALGROUND;
	}
	ent->v.groundentity = trace.ent;

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
		if (ent->v.flags & FL_PARTIALGROUND)
		{	// entity had floor mostly pulled out from underneath it
			// and is trying to correct
			if (relink)
				SV_LinkEdict(ent, TRUE);
			return TRUE;
		}
		VectorCopy(oldorg, ent->v.origin);
		return FALSE;
	}

	if (ent->v.flags & FL_PARTIALGROUND)
	{
//		Con_Printf("back on ground\n");
		ent->v.flags &= ~FL_PARTIALGROUND;
	}
	ent->v.groundentity = trace.ent;

// the move is ok
	if (relink)
		SV_LinkEdict(ent, TRUE);
	return TRUE;
}


//============================================================================

/*
======================
SV_StepDirection

Turns to the movement direction, and walks the current distance if
facing it.

======================
*/
qboolean SV_StepDirection( edict_t* ent, float yaw, float dist )
{
	vec3_t		move, oldorigin;

	yaw = yaw * M_PI * 2.0 / 360.0;
	move[0] = cos(yaw) * dist;
	move[1] = sin(yaw) * dist;
	move[2] = 0;

	VectorCopy(ent->v.origin, oldorigin);
	if (SV_movestep(ent, move, FALSE))
	{
		SV_LinkEdict(ent, TRUE);
		return TRUE;
	}
	SV_LinkEdict(ent, TRUE);

	return FALSE;
}

/*
===============
SV_FlyDirection

===============
*/
qboolean SV_FlyDirection( edict_t* ent, vec_t* direction )
{
	vec3_t		oldorigin;

	VectorCopy(ent->v.origin, oldorigin);
	if (SV_movestep(ent, direction, FALSE))
	{
		SV_LinkEdict(ent, TRUE);
		return TRUE;
	}
	SV_LinkEdict(ent, TRUE);

	return FALSE;
}

/*
======================
SV_FixCheckBottom

======================
*/
void SV_FixCheckBottom( edict_t* ent )
{
//	Con_Printf("SV_FixCheckBottom\n");

	ent->v.flags |= FL_PARTIALGROUND;
}



/*
================
SV_NewChaseDir

================
*/
#define	DI_NODIR	-1
void SV_NewChaseDir( edict_t* actor, edict_t* enemy, float dist )
{
	float		deltax, deltay;
	float		d[3];
	float		tdir, olddir, turnaround;

	olddir = anglemod((int)(actor->v.ideal_yaw / 45) * 45);
	turnaround = anglemod(olddir - 180);

	deltax = enemy->v.origin[0] - actor->v.origin[0];
	deltay = enemy->v.origin[1] - actor->v.origin[1];
	if (deltax > 10)
		d[1] = 0;
	else if (deltax < -10)
		d[1] = 180;
	else
		d[1] = DI_NODIR;
	if (deltay < -10)
		d[2] = 270;
	else if (deltay > 10)
		d[2] = 90;
	else
		d[2] = DI_NODIR;

// try direct route
	if (d[1] != DI_NODIR && d[2] != DI_NODIR)
	{
		if (d[1] == 0)
			tdir = d[2] == 90 ? 45 : 315;
		else
			tdir = d[2] == 90 ? 135 : 215;

		if (tdir != turnaround && SV_StepDirection(actor, tdir, dist))
			return;
	}

// try other directions
	if (RandomLong(0, 1) || abs(deltay) > abs(deltax))
	{
		tdir = d[1];
		d[1] = d[2];
		d[2] = tdir;
	}

	if (d[1] != DI_NODIR && d[1] != turnaround
	&& SV_StepDirection(actor, d[1], dist))
		return;

	if (d[2] != DI_NODIR && d[2] != turnaround
	&& SV_StepDirection(actor, d[2], dist))
		return;

/* there is no direct path to the player, so pick another direction */

	if (olddir != DI_NODIR && SV_StepDirection(actor, olddir, dist))
		return;

	if (RandomLong(0, 1)) 	/*randomly determine direction of search*/
	{
		for (tdir = 0; tdir <= 315; tdir += 45)
			if (tdir != turnaround && SV_StepDirection(actor, tdir, dist))
				return;
	}
	else
	{
		for (tdir = 315; tdir >= 0; tdir -= 45)
			if (tdir != turnaround && SV_StepDirection(actor, tdir, dist))
				return;
	}

	if (turnaround != DI_NODIR && SV_StepDirection(actor, turnaround, dist))
		return;

	actor->v.ideal_yaw = olddir;		// can't move

// if a bridge was pulled out from underneath a monster, it may not have
// a valid standing position at all

	if (!SV_CheckBottom(actor))
		SV_FixCheckBottom(actor);

}

/*
======================
SV_CloseEnough

======================
*/
qboolean SV_CloseEnough( edict_t* ent, edict_t* goal, float dist )
{
	int		i;

	for (i = 0; i < 3; i++)
	{
		if (goal->v.absmin[i] > ent->v.absmax[i] + dist)
			return FALSE;
		if (goal->v.absmax[i] < ent->v.absmin[i] - dist)
			return FALSE;
	}
	return TRUE;
}


/*
======================
SV_ReachedGoal

======================
*/
qboolean SV_ReachedGoal( edict_t* ent, vec_t* vecGoal, float flDist )
{
	int		i;

	for (i = 0; i < 3; i++)
	{
		if (vecGoal[i] > ent->v.absmax[i] + flDist)
			return FALSE;
		if (vecGoal[i] < ent->v.absmin[i] - flDist)
			return FALSE;
	}
	return TRUE;
}

/*
===============
SV_NewChaseDir2

===============
*/
void SV_NewChaseDir2( edict_t* actor, vec_t* vecGoal, float dist )
{
	float		deltax, deltay;
	float		d[3];
	float		tdir, olddir, turnaround;

	olddir = anglemod((int)(actor->v.ideal_yaw / 45) * 45);
	turnaround = anglemod(olddir - 180);

	deltax = vecGoal[0] - actor->v.origin[0];
	deltay = vecGoal[1] - actor->v.origin[1];
	if (deltax > 10)
		d[1] = 0;
	else if (deltax < -10)
		d[1] = 180;
	else
		d[1] = DI_NODIR;
	if (deltay < -10)
		d[2] = 270;
	else if (deltay > 10)
		d[2] = 90;
	else
		d[2] = DI_NODIR;

// try direct route
	if (d[1] != DI_NODIR && d[2] != DI_NODIR)
	{
		if (d[1] == 0.0)
			tdir = d[2] == 90 ? 45 : 315;
		else
			tdir = d[2] == 90 ? 135 : 215;

		if (tdir != turnaround && SV_StepDirection(actor, tdir, dist))
			return;
	}

// try other directions
	if (RandomLong(0, 1) || abs(deltay) > abs(deltax))
	{
		tdir = d[1];
		d[1] = d[2];
		d[2] = tdir;
	}

	if (d[1] != DI_NODIR && d[1] != turnaround
	&& SV_StepDirection(actor, d[1], dist))
		return;

	if (d[2] != DI_NODIR && d[2] != turnaround
	&& SV_StepDirection(actor, d[2], dist))
		return;

/* there is no direct path to the player, so pick another direction */

	if (olddir != DI_NODIR && SV_StepDirection(actor, olddir, dist))
		return;

	if (RandomLong(0, 1)) 	/*randomly determine direction of search*/
	{
		for (tdir = 0; tdir <= 315; tdir += 45)
			if (tdir != turnaround && SV_StepDirection(actor, tdir, dist))
				return;
	}
	else
	{
		for (tdir = 315; tdir >= 0; tdir -= 45)
			if (tdir != turnaround && SV_StepDirection(actor, tdir, dist))
				return;
	}

	if (turnaround != DI_NODIR && SV_StepDirection(actor, turnaround, dist))
		return;

	actor->v.ideal_yaw = olddir;		// can't move

// if a bridge was pulled out from underneath a monster, it may not have
// a valid standing position at all

	if (!SV_CheckBottom(actor))
		SV_FixCheckBottom(actor);
}

/*
===============
SV_MoveToOrigin_I

Moves the given entity to the given destination
===============
*/
void SV_MoveToOrigin_I( edict_t* ent, const float* pflGoal, float dist, int iStrafe )
{
	vec3_t	vecGoal;
	vec3_t	vecDir;
	float	flReturn;

	VectorCopy(pflGoal, vecGoal);

	if (!(ent->v.flags & (FL_FLY | FL_SWIM | FL_ONGROUND)))
		return;
	
	if (iStrafe)
	{
		VectorSubtract(vecGoal, ent->v.origin, vecDir);

		if (!(ent->v.flags & (FL_FLY | FL_SWIM)))
			vecDir[2] = 0;

		flReturn = VectorNormalize(vecDir);
		VectorScale(vecDir, dist, vecDir);
		SV_FlyDirection(ent, vecDir);
	}
	else
	{
	// bump around...
		if (!SV_StepDirection(ent, ent->v.ideal_yaw, dist))
		{
			SV_NewChaseDir2(ent, vecGoal, dist);
		}
	}
}