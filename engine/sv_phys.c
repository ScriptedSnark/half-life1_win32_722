// sv_phys.c

#include "quakedef.h"
#include "pmove.h"
#include "sv_proto.h"
#include "pr_cmds.h"

/*


pushmove objects do not obey gravity, and do not interact with each other or trigger fields, but block normal movement and push normal objects when they move.

onground is set for toss objects when they come to a complete rest.  it is set for steping or walking objects

doors, plats, etc are SOLID_BSP, and MOVETYPE_PUSH
bonus items are SOLID_TRIGGER touch, and MOVETYPE_TOSS
corpses are SOLID_NOT and MOVETYPE_TOSS
crates are SOLID_BBOX and MOVETYPE_TOSS
walking monsters are SOLID_SLIDEBOX and MOVETYPE_STEP
flying/floating monsters are SOLID_SLIDEBOX and MOVETYPE_FLY

solid_edge items only clip against bsp models.

*/

cvar_t	sv_friction = { "sv_friction", "4", FALSE, TRUE };
cvar_t	sv_stopspeed = { "sv_stopspeed", "100", FALSE, TRUE };
cvar_t	sv_gravity = { "sv_gravity", "800", FALSE, TRUE };
cvar_t	sv_maxvelocity = { "sv_maxvelocity", "2000" };
cvar_t	sv_stepsize = { "sv_stepsize", "18", FALSE, TRUE };
cvar_t	sv_clipmode = { "sv_clipmode", "0", FALSE, TRUE };
cvar_t	sv_bounce = { "sv_bounce", "1", FALSE, TRUE };
cvar_t	sv_airmove = { "sv_airmove", "1", FALSE, TRUE };
cvar_t	sv_spectatormaxspeed = { "sv_spectatormaxspeed", "500", FALSE, TRUE };
cvar_t	sv_airaccelerate = { "sv_airaccelerate", "10", FALSE, TRUE };
cvar_t	sv_wateraccelerate = { "sv_wateraccelerate", "10", FALSE, TRUE };
cvar_t	sv_waterfriction = { "sv_waterfriction", "1", FALSE, TRUE };
cvar_t	sv_zmax = { "sv_zmax", "4096" };
cvar_t	sv_wateramp = { "sv_wateramp", "0" };

cvar_t	sv_skyname = { "sv_skyname", "desert" };

vec3_t	vec_origin = { 0, 0, 0 };


#define	MOVE_EPSILON	0.01

void SV_Physics_Toss( edict_t* ent );

/*
================
SV_CheckAllEnts
================
*/
void SV_CheckAllEnts( void )
{
	int			e;
	edict_t* check;

// see if any solid entities are inside the final position
	check = sv.edicts;
	for (e = 1; e < sv.num_edicts; e++, check++)
	{
		if (check->free)
			continue;
		if (check->v.movetype == MOVETYPE_PUSH
		|| check->v.movetype == MOVETYPE_NONE
		|| check->v.movetype == MOVETYPE_FOLLOW
		|| check->v.movetype == MOVETYPE_NOCLIP)
			continue;

		if (SV_TestEntityPosition(check))
			Con_Printf("entity in invalid position\n");
	}
}

/*
================
SV_CheckVelocity
================
*/
void SV_CheckVelocity( edict_t* ent )
{
	int		i;

//
// bound velocity
//
	for (i = 0; i < 3; i++)
	{
		if (IS_NAN(ent->v.velocity[i]))
		{
			Con_Printf("Got a NaN velocity on %s\n", &pr_strings[ent->v.classname]);
			ent->v.velocity[i] = 0;
		}
		if (IS_NAN(ent->v.origin[i]))
		{
			Con_Printf("Got a NaN origin on %s\n", &pr_strings[ent->v.classname]);
			ent->v.origin[i] = 0;
		}
		if (ent->v.velocity[i] > sv_maxvelocity.value)
		{
			Con_DPrintf("Got a velocity too high on %s\n", &pr_strings[ent->v.classname]);
			ent->v.velocity[i] = sv_maxvelocity.value;
		}
		else if (ent->v.velocity[i] < -sv_maxvelocity.value)
		{
			Con_DPrintf("Got a velocity too low on %s\n", &pr_strings[ent->v.classname]);
			ent->v.velocity[i] = -sv_maxvelocity.value;
		}
	}
}

/*
=============
SV_RunThink

Runs thinking code if time.  There is some play in the exact time the think
function will be called, because it is called before any movement is done
in a frame.  Not used for pushmove objects, because they must be exact.
Returns false if the entity removed itself.
=============
*/
qboolean SV_RunThink( edict_t* ent )
{
	float	thinktime;

	if (!(ent->v.flags & FL_KILLME))
	{
		thinktime = ent->v.nextthink;
		if (thinktime <= 0)
			return TRUE;
		if (thinktime > sv.time + host_frametime)
			return TRUE;

		if (thinktime < sv.time)
			thinktime = sv.time;	// don't let things stay in the past.
									// it is possible to start that way
									// by a trigger with a local time.
		ent->v.nextthink = 0;
		gGlobalVariables.time = thinktime;

		gEntityInterface.pfnThink(ent);
	}

	if (ent->v.flags & FL_KILLME)
		ED_Free(ent);

	return ent->free == FALSE;
}

/*
==================
SV_Impact

Two entities have touched, so run their touch functions
==================
*/
void SV_Impact( edict_t* e1, edict_t* e2, trace_t* ptrace )
{
	gGlobalVariables.time = sv.time;

	if ((e1->v.flags & FL_KILLME) || (e2->v.flags & FL_KILLME))
		return;

	if (e1->v.solid != SOLID_NOT)
	{
		SV_SetGlobalTrace(ptrace);
		gEntityInterface.pfnTouch(e1, e2);
	}

	if (e2->v.solid != SOLID_NOT)
	{
		SV_SetGlobalTrace(ptrace);
		gEntityInterface.pfnTouch(e2, e1);
	}
}


/*
==================
ClipVelocity

Slide off of the impacting object
returns the blocked flags (1 = floor, 2 = step / wall)
==================
*/
#define	STOP_EPSILON	0.1

int ClipVelocity( vec_t* in, vec_t* normal, vec_t* out, float overbounce )
{
	float	backoff;
	float	change;
	float	angle = 0.0;
	int		i, blocked;

	blocked = 0;
	if (normal[2] > 0)
		blocked |= 1;		// floor
	if (!normal[2])
		blocked |= 2;		// step

	backoff = DotProduct(in, normal) * overbounce;

	for (i = 0; i < 3; i++)
	{
		change = normal[i] * backoff;
		out[i] = in[i] - change;
		if (out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON)
			out[i] = 0;
	}

	return blocked;
}


/*
============
SV_FlyMove

The basic solid body movement clip that slides along multiple planes
Returns the clipflags if the velocity was modified (hit something solid)
1 = floor
2 = wall / step
4 = dead stop
If steptrace is not NULL, the trace of any vertical wall hit will be stored
============
*/
#define	MAX_CLIP_PLANES	5
int SV_FlyMove( edict_t* ent, float time, trace_t* steptrace )
{
	// TODO: Implement
	return 0;
}


/*
============
SV_AddGravity

============
*/
void SV_AddGravity( edict_t* ent )
{
	float ent_gravity;

	if (ent->v.gravity)
		ent_gravity = ent->v.gravity;
	else		
		ent_gravity = 1.0;

	ent->v.velocity[2] -= (ent_gravity * sv_gravity.value * host_frametime);
	ent->v.velocity[2] += (ent->v.basevelocity[2] * host_frametime);
	ent->v.basevelocity[2] = 0;

	SV_CheckVelocity(ent);
}


void SV_AddCorrectGravity( edict_t* ent )
{
	float	ent_gravity;

	if (ent->v.gravity)
		ent_gravity = ent->v.gravity;
	else	
		ent_gravity = 1.0;

	// Add gravity so they'll be in the correct position during movement
	// yes, this 0.5 looks wrong, but it's not.  
	ent->v.velocity[2] -= (ent_gravity * sv_gravity.value * host_frametime * 0.5);
	ent->v.velocity[2] += (ent->v.basevelocity[2] * host_frametime);
	ent->v.basevelocity[2] = 0;

	SV_CheckVelocity(ent);
}


void SV_FixupGravityVelocity( edict_t* ent )
{
	float	ent_gravity;

	if (ent->v.gravity)
		ent_gravity = ent->v.gravity;
	else
		ent_gravity = 1.0;

	// Get the correct velocity for the end of the dt 
	ent->v.velocity[2] -= (ent_gravity * sv_gravity.value * host_frametime * 0.5);

	SV_CheckVelocity(ent);
}

/*
===============================================================================

PUSHMOVE

===============================================================================
*/

/*
============
SV_PushEntity

Does not change the entities velocity at all
============
*/
trace_t SV_PushEntity( edict_t* ent, vec_t* push )
{
	// TODO: Implement
	trace_t trace;
	memset(&trace, 0, sizeof(trace));
	return trace;
}

edict_t** g_moved_edict;
vec3_t* g_moved_from;

/*
============
SV_PushMove

============
*/
void SV_PushMove( edict_t* pusher, float movetime )
{
	// TODO: Implement
}

/*
============
SV_PushRotate

Returns FALSE if the pusher can't push
============
*/
int SV_PushRotate( edict_t* pusher, float movetime )
{
	// TODO: Implement
	return FALSE;
}

/*
================
SV_Physics_Pusher

================
*/
void SV_Physics_Pusher( edict_t* ent )
{
	// TODO: Implement
}

/*
================
SV_CheckWater

Computes the water level + type also checks if entity is in the water
and applies any current to velocity and sets appropriate water flags
================
*/
qboolean SV_CheckWater( edict_t* ent )
{
	// TODO: Implement
	return FALSE;
}

// Recursively determine the water level at a given position
float SV_RecursiveWaterLevel( vec_t* center, float out, float in, int count )
{
	// TODO: Implement
	return 0.0f;
}

// Determine the depth that an entity is submerged in water
float SV_Submerged( edict_t* ent )
{
	// TODO: Implement
	return 0.0f;
}

/*
=============
SV_Physics_None

Non moving objects can only think
=============
*/
void SV_Physics_None( edict_t* ent )
{
// regular thinking
	SV_RunThink(ent);
}

/*
=============
SV_Physics_Follow

Copy the angles and origin of the parent
=============
*/
void SV_Physics_Follow( edict_t* ent )
{
// regular thinking
	if (!SV_RunThink(ent))
		return;
	
	// no entity to follow
	if (!ent->v.aiment)
	{
		Con_DPrintf("%s movetype FOLLOW with NULL aiment\n", &pr_strings[ent->v.classname]);
		ent->v.movetype = MOVETYPE_NONE;
		return;
	}

	VectorAdd(ent->v.aiment->v.origin, ent->v.v_angle, ent->v.origin);
	VectorCopy(ent->v.aiment->v.angles, ent->v.angles);

	SV_LinkEdict(ent, TRUE);
}

/*
=============
SV_Physics_Noclip

A moving object that doesn't obey physics
=============
*/
void SV_Physics_Noclip( edict_t* ent )
{
// regular thinking
	if (!SV_RunThink(ent))
		return;

	VectorMA(ent->v.angles, host_frametime, ent->v.avelocity, ent->v.angles);
	VectorMA(ent->v.origin, host_frametime, ent->v.velocity, ent->v.origin);

	SV_LinkEdict(ent, FALSE);
}

/*
==============================================================================

TOSS / BOUNCE

==============================================================================
*/

/*
=============
SV_CheckWaterTransition

=============
*/
void SV_CheckWaterTransition( edict_t* ent )
{
	int		cont;

	vec3_t	point;

	point[0] = (ent->v.absmin[0] + ent->v.absmax[0]) * 0.5;
	point[1] = (ent->v.absmin[1] + ent->v.absmax[1]) * 0.5;
	point[2] = (ent->v.absmin[2] + 1.0);

	cont = SV_PointContents(point);
	if (!ent->v.watertype)
	{	// just spawned here
		ent->v.watertype = cont;
		ent->v.waterlevel = 1;
		return;
	}

	if (cont <= CONTENTS_WATER && cont > CONTENTS_TRANSLUCENT)
	{
		if (watertype == -1)
		{	// just crossed into water
			SV_StartSound(ent, CHAN_AUTO, "player/pl_wade1.wav", 255, 1.0, 0, PITCH_NORM);
			ent->v.velocity[2] *= 0.5;
		}
		ent->v.watertype = cont;
		ent->v.waterlevel = 1;

		if (ent->v.absmin[2] == ent->v.absmax[2])
		{
			// a point entity
			ent->v.waterlevel = 3;
			return;
		}

		point[2] = (ent->v.absmin[2] + ent->v.absmax[2]) * 0.5;

		cont = SV_PointContents(point);
		if (cont <= CONTENTS_WATER && cont > CONTENTS_TRANSLUCENT)
		{
			ent->v.waterlevel = 2;

			point[2] += ent->v.view_ofs[2];

			cont = SV_PointContents(point);
			if (cont <= CONTENTS_WATER && cont > CONTENTS_TRANSLUCENT)
			{
				ent->v.waterlevel = 3;
			}
		}
	}
	else
	{
		if (ent->v.watertype != CONTENTS_EMPTY)
		{	// just crossed into water
			SV_StartSound(ent, CHAN_AUTO, "player/pl_wade2.wav", 255, 1.0, 0, PITCH_NORM);
		}
		ent->v.watertype = CONTENTS_EMPTY;
		ent->v.waterlevel = 0;
	}
}

/*
=============
SV_Physics_Toss

Toss, bounce, and fly movement.  When onground, do nothing.
=============
*/
void SV_Physics_Toss( edict_t* ent )
{
	trace_t trace;
	vec3_t	move;
	float	backoff;

	SV_CheckWater(ent);

// regular thinking
	if (!SV_RunThink(ent))
		return;

	if (ent->v.velocity[2] > 0.0 || !ent->v.groundentity || (ent->v.groundentity->v.flags & (FL_MONSTER | FL_CLIENT)))
	{
		ent->v.flags &= ~FL_ONGROUND;
	}

// if on ground and not moving, return.
	if ((ent->v.flags & FL_ONGROUND) && VectorCompare(ent->v.velocity, vec_origin))
	{
		VectorCopy(vec3_origin, ent->v.avelocity);

		if (VectorCompare(ent->v.basevelocity, vec_origin))
		{
			return; // at rest
		}
	}

	SV_CheckVelocity(ent);

// add gravity
	switch (ent->v.movetype)
	{
	case MOVETYPE_FLY:
	case MOVETYPE_FLYMISSILE:
	case MOVETYPE_BOUNCEMISSILE:
		break;
	default:
		SV_AddGravity(ent);
		break;
	}

// move angles
	// Compute new angles based on the angular velocity
	VectorMA(ent->v.angles, host_frametime, ent->v.avelocity, ent->v.angles);

// move origin
	VectorAdd(ent->v.velocity, ent->v.basevelocity, ent->v.velocity);
	SV_CheckVelocity(ent);

	VectorScale(ent->v.velocity, host_frametime, move);
	VectorSubtract(ent->v.velocity, ent->v.basevelocity, ent->v.velocity);

	trace = SV_PushEntity(ent, move);
	SV_CheckVelocity(ent);

	// If we started in a solid object, or we were in solid space the whole way, zero out our velocity.
	if (trace.allsolid)
	{
		// entity is trapped in another solid
		VectorCopy(vec3_origin, ent->v.velocity);
		VectorCopy(vec3_origin, ent->v.avelocity);
		return;
	}

	if (trace.fraction == 1)
	{
		// moved the entire distance
		SV_CheckWaterTransition(ent);
		return;
	}

	if (ent->free)
		return;

	if (ent->v.movetype == MOVETYPE_BOUNCE)
		backoff = 2.0 - ent->v.friction;
	else if (ent->v.movetype == MOVETYPE_BOUNCEMISSILE)
		backoff = 2.0; // A backoff of 2.0 is a reflection
	else
		backoff = 1.0;

	ClipVelocity(ent->v.velocity, trace.plane.normal, ent->v.velocity, backoff);

// stop if on ground
	if (trace.plane.normal[2] > 0.7)
	{
		// Get the total velocity (player + conveyors, etc.)
		VectorAdd(ent->v.velocity, ent->v.basevelocity, move);
		float vel = DotProduct(move, move);

		// Are we on the ground?
		if (move[2] < (sv_gravity.value * host_frametime))
		{
			// we're rolling on the ground, add static friction
			ent->v.flags |= FL_ONGROUND;
			ent->v.groundentity = trace.ent;
			ent->v.velocity[2] = 0.0;
		}

		//Con_DPrintf("%f %f: %.0f %.0f %.0f\n", vel, trace.fraction, ent->velocity[0], ent->velocity[1], ent->velocity[2]);

		if (vel < (30 * 30) || (ent->v.movetype != MOVETYPE_BOUNCE && ent->v.movetype != MOVETYPE_BOUNCEMISSILE))
		{
			ent->v.flags |= FL_ONGROUND;
			ent->v.groundentity = trace.ent;
			VectorCopy(vec3_origin, ent->v.velocity);
			VectorCopy(vec3_origin, ent->v.avelocity);
		}
		else
		{
			VectorScale(ent->v.velocity, (1.0 - trace.fraction) * host_frametime * 0.9, move);
			VectorMA(move, (1.0 - trace.fraction) * host_frametime * 0.9, ent->v.basevelocity, move);
			trace = SV_PushEntity(ent, move);
		}
	}

// check for in water
	SV_CheckWaterTransition(ent);
}

/*
===============================================================================

STEPPING MOVEMENT

===============================================================================
*/

void PF_WaterMove( edict_t* pSelf )
{
	int		flags;
	int		waterlevel;
	int		watertype;
	float	drownlevel;

	if (pSelf->v.movetype == MOVETYPE_NOCLIP)
	{
		pSelf->v.air_finished = sv.time + 12;
		return;
	}

	if (pSelf->v.health < 0.0)
		return;

	drownlevel = (pSelf->v.deadflag == DEAD_NO) ? 3 : 1;
	flags = pSelf->v.flags;
	waterlevel = pSelf->v.waterlevel;
	watertype = pSelf->v.watertype;

	if (!(flags & (FL_IMMUNE_WATER | FL_GODMODE)))
	{
		if ((flags & FL_SWIM) && (waterlevel < drownlevel) || (waterlevel >= drownlevel))
		{
			if (pSelf->v.air_finished < sv.time && pSelf->v.pain_finished < sv.time)
			{
				pSelf->v.dmg += 2;
				if (pSelf->v.dmg > 15)
					pSelf->v.dmg = 10;

				pSelf->v.pain_finished = sv.time + 1;
			}
		}
		else
		{
			pSelf->v.dmg = 2;
			pSelf->v.air_finished = sv.time + 12;
		}
	}

	if (waterlevel == 0)
	{
		// play leave water sound
		if (flags & FL_INWATER)
		{
			switch (RandomLong(0, 3))
			{
			case 0:
				SV_StartSound(pSelf, CHAN_BODY, "player/pl_wade1.wav", 255, ATTN_NORM, 0, PITCH_NORM);
				break;
			case 1:
				SV_StartSound(pSelf, CHAN_BODY, "player/pl_wade2.wav", 255, ATTN_NORM, 0, PITCH_NORM);
				break;
			case 2:
				SV_StartSound(pSelf, CHAN_BODY, "player/pl_wade3.wav", 255, ATTN_NORM, 0, PITCH_NORM);
				break;
			case 3:
				SV_StartSound(pSelf, CHAN_BODY, "player/pl_wade4.wav", 255, ATTN_NORM, 0, PITCH_NORM);
				break;
			}

			pSelf->v.flags &= ~FL_INWATER;
		}

		pSelf->v.air_finished = sv.time + 12;
		return;
	}

	if (watertype == CONTENTS_LAVA)
	{
		if (!(flags & (FL_IMMUNE_LAVA | FL_GODMODE)) && pSelf->v.dmgtime < sv.time)
		{
			if (pSelf->v.radsuit_finished < sv.time)
				pSelf->v.dmgtime = sv.time + 0.2;
			else
				pSelf->v.dmgtime = sv.time + 1.0;
		}
	}
	else if (watertype == CONTENTS_SLIME)
	{
		if (!(flags & (FL_IMMUNE_SLIME | FL_GODMODE)) && pSelf->v.dmgtime < sv.time)
		{
			if (pSelf->v.radsuit_finished < sv.time)
				pSelf->v.dmgtime = sv.time + 1.0;
		}
	}

	if (!(flags & FL_INWATER))
	{
		// player enter water sound
		if (watertype == CONTENTS_WATER)
		{
			switch (RandomLong(0, 3))
			{
			case 0:
				SV_StartSound(pSelf, CHAN_BODY, "player/pl_wade1.wav", 255, ATTN_NORM, 0, PITCH_NORM);
				break;
			case 1:
				SV_StartSound(pSelf, CHAN_BODY, "player/pl_wade2.wav", 255, ATTN_NORM, 0, PITCH_NORM);
				break;
			case 2:
				SV_StartSound(pSelf, CHAN_BODY, "player/pl_wade3.wav", 255, ATTN_NORM, 0, PITCH_NORM);
				break;
			case 3:
				SV_StartSound(pSelf, CHAN_BODY, "player/pl_wade4.wav", 255, ATTN_NORM, 0, PITCH_NORM);
				break;
			}
		}

		pSelf->v.dmgtime = 0;
		pSelf->v.flags |= FL_INWATER;
	}

	if (!(flags & FL_WATERJUMP))
	{
		VectorMA(pSelf->v.velocity, (-0.8 * pSelf->v.waterlevel * host_frametime), pSelf->v.velocity, pSelf->v.velocity);
	}
}

/*
=============
SV_Physics_Step

Monsters freefall when they don't have a ground entity, otherwise
all movement is done with discrete steps.

This is also used for objects that have become still on the ground, but
will fall if the floor is pulled out from under them.
FIXME: is this true?
=============
*/
void SV_Physics_Step( edict_t* ent )
{
	qboolean	wasonground;
	qboolean	inwater;
	qboolean	hitsound = FALSE;
	float* vel = NULL;
	float		speed, newspeed, control;
	float		friction;

	PF_WaterMove(ent);

	SV_CheckVelocity(ent);

	wasonground = (ent->v.flags & FL_ONGROUND) ? TRUE : FALSE;
	inwater = SV_CheckWater(ent);

	if ((ent->v.flags & FL_FLOAT) && ent->v.waterlevel > 0)
	{
		float buoyancy = SV_Submerged(ent) * ent->v.skin * host_frametime;

		SV_AddGravity(ent);
		ent->v.velocity[2] += buoyancy;
	}

	// add gravity except:
	//  flying monsters
	//  swimming monsters who are in the water
	if (!wasonground && !(ent->v.flags & FL_FLY) && (!(ent->v.flags & FL_SWIM) || ent->v.waterlevel <= 0))
	{
		if (!inwater)
		{
			SV_AddGravity(ent);
		}
	}

	if (!VectorCompare(ent->v.velocity, vec_origin) || !VectorCompare(ent->v.basevelocity, vec_origin))
	{
		ent->v.flags &= ~FL_ONGROUND;

		// apply friction
		// let dead monsters who aren't completely onground slide
		if (wasonground && (ent->v.health > 0.0 || SV_CheckBottom(ent)))
		{
			speed = sqrt(ent->v.velocity[0] * ent->v.velocity[0] + ent->v.velocity[1] * ent->v.velocity[1]);
			if (speed)
			{
				friction = ent->v.friction * sv_friction.value;
				ent->v.friction = 1.0;

				control = (sv_stopspeed.value < speed) ? speed : sv_stopspeed.value;
				newspeed = speed - (control * friction * host_frametime);
				if (newspeed < 0.0)
					newspeed = 0.0;

				newspeed = newspeed / speed;

				ent->v.velocity[0] *= newspeed;
				ent->v.velocity[1] *= newspeed;
			}
		}

		VectorAdd(ent->v.velocity, ent->v.basevelocity, ent->v.velocity);
		SV_CheckVelocity(ent);

		SV_FlyMove(ent, host_frametime, NULL);
		SV_CheckVelocity(ent);

		VectorSubtract(ent->v.velocity, ent->v.basevelocity, ent->v.velocity);
		SV_CheckVelocity(ent);

		// determine if it's on solid ground at all
		vec3_t mins, maxs, point;
		int x, y;

		VectorAdd(ent->v.origin, ent->v.mins, mins);
		VectorAdd(ent->v.origin, ent->v.maxs, maxs);

		point[2] = mins[2] - 1.0;

		for (x = 0; x <= 1; x++)
		{
			for (y = 0; y <= 1; y++)
			{
				point[0] = x ? maxs[0] : mins[0];
				point[1] = y ? maxs[1] : mins[1];

				if (SV_PointContents(point) == CONTENTS_SOLID)
				{
					ent->v.flags |= FL_ONGROUND;
					break;
				}
			}
		}

		SV_LinkEdict(ent, TRUE);
	}
	else
	{
		if (gGlobalVariables.force_retouch != 0)
		{
			trace_t trace = SV_Move(ent->v.origin, ent->v.mins, ent->v.maxs, ent->v.origin, MOVE_NORMAL, ent, (ent->v.flags & FL_MONSTERCLIP) ? TRUE : FALSE);

			// tentacle impact code
			if (trace.fraction < 1.0 || trace.startsolid)
			{
				if (trace.ent)
					SV_Impact(ent, trace.ent, &trace);
			}
		}
	}

// regular thinking
	SV_RunThink(ent);

	SV_CheckWaterTransition(ent);
}

/*
=============
SV_Physics

Runs the main physics simulation loop against all entities
except the players
=============
*/
void SV_Physics( void )
{
	int		i;
	edict_t* ent;
	edict_t* groundentity;

	// let the progs know that a new frame has started
	gGlobalVariables.time = sv.time;

	gEntityInterface.pfnStartFrame();

	// iterate through all entities and have them think or simulate
	for (i = 0; i < sv.num_edicts; i++)
	{
		ent = &sv.edicts[i];
		if (ent->free)
			continue;

		if (gGlobalVariables.force_retouch != 0)
		{
			// force retouch even for stationary
			SV_LinkEdict(&sv.edicts[i], TRUE);
		}

		if (i > 0 && i <= svs.maxclients)
			continue;

		// Checks if an entity is standing on a moving entity to adjust the velocity
		if (ent->v.flags & FL_ONGROUND)
		{
			groundentity = ent->v.groundentity;
			if (groundentity)
			{
				if (groundentity->v.flags & FL_CONVEYOR)
				{
					if (ent->v.flags & FL_BASEVELOCITY)
					{
						VectorMA(ent->v.basevelocity, groundentity->v.speed, groundentity->v.movedir, ent->v.basevelocity);
					}
					else
					{
						VectorScale(groundentity->v.movedir, groundentity->v.speed, ent->v.basevelocity);
					}
					ent->v.flags |= FL_BASEVELOCITY;
				}
			}
		}

		if (!(ent->v.flags & FL_BASEVELOCITY))
		{
			// Apply momentum (add in half of the previous frame of velocity first)
			VectorMA(ent->v.velocity, 1.0 + (host_frametime * 0.5), ent->v.basevelocity, ent->v.velocity);
			VectorCopy(vec3_origin, ent->v.basevelocity);
		}

		ent->v.flags &= ~FL_BASEVELOCITY;

		switch (ent->v.movetype)
		{
		case MOVETYPE_PUSH:
			SV_Physics_Pusher(ent);
			break;
		case MOVETYPE_NONE:
			SV_Physics_None(ent);
			break;
		case MOVETYPE_NOCLIP:
			SV_Physics_Noclip(ent);
			break;
		case MOVETYPE_STEP:
		case MOVETYPE_PUSHSTEP:
			SV_Physics_Step(ent);
			break;
		case MOVETYPE_FOLLOW:
			SV_Physics_Follow(ent);
			break;
		case MOVETYPE_TOSS:
		case MOVETYPE_BOUNCE:
		case MOVETYPE_BOUNCEMISSILE:
		case MOVETYPE_FLY:
		case MOVETYPE_FLYMISSILE:
			SV_Physics_Toss(ent);
			break;
		default:
			Sys_Error("SV_Physics: %s bad movetype %d", &pr_strings[ent->v.classname], ent->v.movetype);
		}

		if (ent->v.flags & FL_KILLME)
			ED_Free(ent);
	}

	if (gGlobalVariables.force_retouch != 0)
		gGlobalVariables.force_retouch -= 1;

	sv.time += host_frametime;
}

trace_t SV_Trace_Toss( edict_t* ent, edict_t* ignore )
{
	edict_t tempent, * tent;
	trace_t trace;
	vec3_t	move;
	vec3_t	end;
	double	save_frametime;

	save_frametime = host_frametime;
	host_frametime = 0.05;

	memcpy(&tempent, ent, sizeof(tempent));
	tent = &tempent;

	do
	{
		SV_CheckVelocity(tent);
		SV_AddGravity(tent);
		VectorMA(tent->v.angles, host_frametime, tent->v.avelocity, tent->v.angles);
		VectorScale(tent->v.velocity, host_frametime, move);
		VectorAdd(tent->v.origin, move, end);
		trace = SV_Move(tent->v.origin, tent->v.mins, tent->v.maxs, end, MOVE_NORMAL, tent, FALSE);
		VectorCopy(trace.endpos, tent->v.origin);
	} while (!trace.ent || trace.ent == ignore);

	host_frametime = save_frametime;
	return trace;
}

void SV_SetMoveVars( void )
{
	movevars.gravity = sv_gravity.value;
	movevars.stopspeed = sv_stopspeed.value;
	movevars.maxspeed = sv_maxspeed.value;
	movevars.spectatormaxspeed = sv_spectatormaxspeed.value;
	movevars.accelerate = sv_accelerate.value;
	movevars.airaccelerate = sv_airaccelerate.value;
	movevars.wateraccelerate = sv_wateraccelerate.value;
	movevars.friction = sv_friction.value;
	movevars.edgefriction = sv_edgefriction.value;
	movevars.waterfriction = sv_waterfriction.value;
	movevars.bounce = sv_bounce.value;
	movevars.stepsize = sv_stepsize.value;
	movevars.maxvelocity = sv_maxvelocity.value;
	movevars.zmax = sv_zmax.value;
	movevars.entgravity = 1.0;
	movevars.waveHeight = sv_wateramp.value;
	strcpy(movevars.skyName, sv_skyname.string);
}