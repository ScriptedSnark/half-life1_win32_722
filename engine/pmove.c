#include "quakedef.h"
#include "pmove.h"

movevars_t		movevars;

playermove_t	pmove;

int			onground;
int			waterlevel;
int			watertype;

float		frametime;

vec3_t		forward, right, up;

cvar_t	cl_showclip = { "cl_showclip", "0" };
cvar_t	cl_printclip = { "cl_printclip", "0" };

cvar_t	pm_nostudio = { "pm_nostudio", "0" };
cvar_t	pm_nocomplex = { "pm_nocomplex", "0" };
cvar_t	pm_worldonly = { "pm_worldonly", "0" };
cvar_t	pm_pushfix = { "pm_pushfix", "0" };
cvar_t	pm_nostucktouch = { "pm_nostucktouch", "0" };

vec3_t	player_mins[4] = {{-16, -16, -36}, {-16, -16, -18}, {0, 0, 0}, 0};
vec3_t	player_maxs[4] = {{16, 16, 36}, {16, 16, 18}, {0, 0, 0}, 0};

void PM_InitBoxHull( void );
qboolean PM_AddToTouched( pmtrace_t tr, vec_t* impactvelocity );

void Pmove_Init( void )
{
	// TODO: Implement
}

char* PM_NameForContents( int contents )
{
	char* name;

	switch (contents)
	{
	case CONTENTS_EMPTY:
		name = "EMPTY";
		break;
	case CONTENTS_SOLID:
		name = "SOLID";
		break;
	case CONTENTS_WATER:
		name = "WATER";
		break;
	case CONTENTS_SLIME:
		name = "SLIME";
		break;
	case CONTENTS_LAVA:
		name = "LAVA";
		break;
	case CONTENTS_SKY:
		name = "SKY";
		break;
	case CONTENTS_ORIGIN:
		name = "ORIGIN";
		break;
	case CONTENTS_CLIP:
		name = "CLIP";
		break;
	case CONTENTS_CURRENT_0:
		name = "C0";
		break;
	case CONTENTS_CURRENT_90:
		name = "C90";
		break;
	case CONTENTS_CURRENT_180:
		name = "C180";
		break;
	case CONTENTS_CURRENT_270:
		name = "C270";
		break;
	case CONTENTS_CURRENT_UP:
		name = "CUP";
		break;
	case CONTENTS_CURRENT_DOWN:
		name = "CDOWN";
		break;
	case CONTENTS_TRANSLUCENT:
		name = "TRANSLUCENT";
		break;
	default:
		name = "UNKNOWN";
		break;
	}

	return name;
}

void PM_PrintPhysEnts( void )
{
	int i;
	physent_t* pe;

	Con_Printf("===== PMOVE ======\n");
	Con_Printf("Origin: %4.2f %4.2f %4.2f\n", pmove.origin[0], pmove.origin[1], pmove.origin[2]);
	Con_Printf("------------------\n");

	for (i = 0, pe = pmove.physents; i < pmove.numphysent; i++, pe++)
	{
		Con_Printf("Ent:  %i\n", i);
		Con_Printf("Org:  %4.2f %4.2f %4.2f\n", pe->origin[0], pe->origin[1], pe->origin[2]);
		Con_Printf("Ang:  %4.2f %4.2f %4.2f\n", pe->angles[0], pe->angles[1], pe->angles[2]);
	}

	Con_Printf("\n");
}

/*
===============
PM_ParticleLine( vec_t* start, vec_t* end, int pcolor, float life, float vert )

================
*/
void PM_ParticleLine( vec_t* start, vec_t* end, int pcolor, float life, float vert )
{
	// TODO: Implement
}

/*
================
PM_DrawRectangle( vec_t* tl, vec_t* br )

================
*/
void PM_DrawRectangle( vec_t* tl, vec_t* bl, vec_t* tr, vec_t* br, int pcolor, float life )
{
	PM_ParticleLine(tl, bl, pcolor, life, 0);
	PM_ParticleLine(bl, br, pcolor, life, 0);
	PM_ParticleLine(br, tr, pcolor, life, 0);
	PM_ParticleLine(tr, tl, pcolor, life, 0);
}

/*
================
PM_DrawPhysEntBBox(int num)

================
*/
void PM_DrawPhysEntBBox( int num, int pcolor, float life )
{
	// TODO: Implement
}

/*
================
PM_DrawBBox( vec_t* mins, vec_t* maxs, vec_t* origin, int pcolor, float life )

================
*/
void PM_DrawBBox( vec_t* mins, vec_t* maxs, vec_t* origin, int pcolor, float life )
{
	// TODO: Implement
}

/*
================
PM_ViewEntity

Shows a particle trail from player to entity in crosshair.
Shows particles at that entities bbox

Tries to shoot a ray out by about 128 units.
================
*/
void PM_ViewEntity( void )
{
	// TODO: Implement
}

/*
================
PM_CheckVelocity

See if the player has a bogus velocity value.
================
*/
void PM_CheckVelocity( void )
{
	int		i;

//
// bound velocity
//
	for (i = 0; i < 3; i++)
	{
		// See if it's bogus.
		if (IS_NAN(pmove.velocity[i]))
		{
			Con_Printf("PM  Got a NaN velocity %i\n", i);
			pmove.velocity[i] = 0;
		}
		if (IS_NAN(pmove.origin[i]))
		{
			Con_Printf("PM  Got a NaN origin on %i\n", i);
			pmove.origin[i] = 0;
		}

		// Bound it.
		if (pmove.velocity[i] > movevars.maxvelocity)
		{
			Con_DPrintf("PM  Got a velocity too high on %i\n", i);
			pmove.velocity[i] = movevars.maxvelocity;
		}
		else if (pmove.velocity[i] < -movevars.maxvelocity)
		{
			Con_DPrintf("PM  Got a velocity too low on %i\n", i);
			pmove.velocity[i] = -movevars.maxvelocity;
		}
	}
}

/*
==================
PM_ClipVelocity

Slide off of the impacting object
returns the blocked flags:
0x01 == floor
0x02 == step / wall
==================
*/
int PM_ClipVelocity( vec_t* in, vec_t* normal, vec_t* out, float overbounce )
{
	// TODO: Implement
	return 0;
}

void PM_AddCorrectGravity( void )
{
	// TODO: Implement
}


void PM_FixupGravityVelocity( void )
{
	// TODO: Implement
}

/*
============
PM_FlyMove

The basic solid body movement clip that slides along multiple planes
============
*/
#define	MAX_CLIP_PLANES	5

int PM_FlyMove( void )
{
	// TODO: Implement
	return 0;
}

/*
============
PM_ApplyFriction

============
*/
void PM_ApplyFriction( physent_t* pe )
{
	// TODO: Implement
}

/*
=====================
PM_WalkMove

Only used by players.  Moves along the ground when player is a MOVETYPE_WALK.
======================
*/
void PM_WalkMove( void )
{
	// TODO: Implement
}

/*
==================
PM_Friction

Handles both ground friction and water friction
==================
*/
void PM_Friction( void )
{
	// TODO: Implement
}


void PM_Accelerate( vec_t* wishdir, float wishspeed, float accel )
{
	// TODO: Implement
}

void PM_AirAccelerate( vec_t* wishdir, float wishspeed, float accel )
{
	// TODO: Implement
}



/*
===================
PM_WaterMove

===================
*/
void PM_WaterMove( void )
{
	// TODO: Implement
}


/*
===================
PM_AirMove

===================
*/
void PM_AirMove( void )
{
	// TODO: Implement
}



/*
=============
PM_CatagorizePosition
=============
*/
void PM_CatagorizePosition( void )
{
	// TODO: Implement
}


/*
=============
JumpButton
=============
*/
void JumpButton( void )
{
	// TODO: Implement
}

/*
=============
CheckWaterJump
=============
*/
void CheckWaterJump( void )
{
	// TODO: Implement
}

/*
=============
PM_CheckWater

Sets pmove->waterlevel and pmove->watertype values.
=============
*/
qboolean PM_CheckWater( void )
{
	// TODO: Implement
	return FALSE;
}

void CreateStuckTable( void )
{
	// TODO: Implement
}

/*
=================
GetRandomStuckOffsets

When a player is stuck, it's costly to try and unstick them
Grab a test offset for the player based on a passed in index
=================
*/
int GetRandomStuckOffsets( int nIndex, int server, vec_t* offset )
{
	// TODO: Implement
	return 0;
}

void ResetStuckOffsets( int nIndex, int server )
{
	// TODO: Implement
}

/*
=================
NudgePosition

If pmove->origin is in a solid position,
try nudging slightly on all axis to
allow for the cut precision of the net coordinates
=================
*/
#define PM_CHECKSTUCK_MINTIME 0.05  // Don't check again too quickly.

int PM_CheckStuck( void )
{
	// TODO: Implement
	return 0;
}

/*
===============
SpectatorMove
===============
*/
void SpectatorMove( void )
{
	// TODO: Implement
}

/*
====================
PM_NoClip

====================
*/
void PM_NoClip( void )
{
	// TODO: Implement
}

void PM_WaterJump( void )
{
	if (pmove.waterjumptime > 2)
	{
		pmove.waterjumptime = 2;
	}

	if (!pmove.waterjumptime)
		return;

	pmove.waterjumptime -= frametime;
	if (pmove.waterjumptime <= 0 ||
		!waterlevel)
	{
		pmove.waterjumptime = 0;
		pmove.flags &= ~FL_WATERJUMP;
	}

	pmove.velocity[0] = pmove.movedir[0];
	pmove.velocity[1] = pmove.movedir[1];
}

/*
============
PM_AddGravity

============
*/
void PM_AddGravity( void )
{
	float	ent_gravity;

	if (pmove.gravity)
		ent_gravity = pmove.gravity;
	else
		ent_gravity = 1.0;

	// Add gravity incorrectly
	pmove.velocity[2] -= (ent_gravity * movevars.gravity * frametime);
	pmove.velocity[2] += pmove.basevelocity[2] * frametime;
	pmove.basevelocity[2] = 0;
	PM_CheckVelocity();
}
/*
============
PM_PushEntity

Does not change the entities velocity at all
============
*/
pmtrace_t PM_PushEntity( vec_t* push )
{
	pmtrace_t	trace;
	vec3_t	end;

	VectorAdd(pmove.origin, push, end);

	trace = PM_PlayerMove(pmove.origin, end, PM_NORMAL);

	VectorCopy(trace.endpos, pmove.origin);

	// So we can run impact function afterwards.
	if (trace.fraction < 1.0 &&
		!trace.allsolid)
	{
		PM_AddToTouched(trace, pmove.velocity);
	}

	return trace;
}

/*
============
PM_Physics_Toss( void )

Dead player flying through air., e.g.
============
*/
void PM_Physics_Toss( void )
{
	pmtrace_t trace;
	vec3_t	move;
	float	backoff;

	PM_CheckWater();

	if (pmove.velocity[2] > 0)
		onground = -1;

// if on ground and not moving, return.
	if (onground != -1)
	{
		if (VectorCompare(pmove.basevelocity, vec3_origin) &&
			VectorCompare(pmove.velocity, vec3_origin))
			return;
	}

	PM_CheckVelocity();

// add gravity
	if (pmove.movetype != MOVETYPE_FLY &&
		pmove.movetype != MOVETYPE_BOUNCEMISSILE &&
		pmove.movetype != MOVETYPE_FLYMISSILE)
		PM_AddGravity();

// move origin
	// Base velocity is not properly accounted for since this entity will move again after the bounce without
	// taking it into account
	VectorAdd(pmove.velocity, pmove.basevelocity, pmove.velocity);

	PM_CheckVelocity();
	VectorScale(pmove.velocity, frametime, move);
	VectorSubtract(pmove.velocity, pmove.basevelocity, pmove.velocity);

	trace = PM_PushEntity(move);

	PM_CheckVelocity();

	if (trace.allsolid)
	{
		// entity is trapped in another solid
		onground = trace.ent;
		VectorCopy(vec3_origin, pmove.velocity);
		return;
	}

	if (trace.fraction == 1)
	{
		PM_CheckWater();
		return;
	}


	if (pmove.movetype == MOVETYPE_BOUNCE)
		backoff = 2.0 - pmove.friction;
	else if (pmove.movetype == MOVETYPE_BOUNCEMISSILE)
		backoff = 2.0;
	else
		backoff = 1;

	PM_ClipVelocity(pmove.velocity, trace.plane.normal, pmove.velocity, backoff);

	// stop if on ground
	if (trace.plane.normal[2] > 0.7)
	{
		float vel;
		vec3_t base;

		VectorCopy(vec3_origin, base);
		if (pmove.velocity[2] < movevars.gravity * frametime)
		{
			// we're rolling on the ground, add static friction.
			onground = trace.ent;
			pmove.velocity[2] = 0;
		}

		vel = DotProduct(pmove.velocity, pmove.velocity);

		// Con_DPrintf("%f %f: %.0f %.0f %.0f\n", vel, trace.fraction, ent->velocity[0], ent->velocity[1], ent->velocity[2]);

		if (vel < (30 * 30) || (pmove.movetype != MOVETYPE_BOUNCE && pmove.movetype != MOVETYPE_BOUNCEMISSILE))
		{
			onground = trace.ent;
			VectorCopy(vec3_origin, pmove.velocity);
		}
		else
		{
			VectorScale(pmove.velocity, (1.0 - trace.fraction) * frametime * 0.9, move);
			trace = PM_PushEntity(move);
		}
		VectorSubtract(pmove.velocity, base, pmove.velocity);
	}

// check for in water
	PM_CheckWater();
}

/*
=============
PlayerMove

Returns with origin, angles, and velocity modified in place.

Numtouch and touchindex[] will be set if any of the physents
were contacted during the move.
=============
*/
void PlayerMove( qboolean server )
{
	static vec3_t lasts;
	static vec3_t lastc;

	// Are we running server code?
	pmove.server = server;

	// Assume we don't touch anything
	pmove.numtouch = 0;

	// # of msec to apply movement
	frametime = pmove.cmd.msec * 0.001;

	// Convert view angles to vectors
	AngleVectors(pmove.angles, forward, right, up);

	if (!server && cl_dumpents.value)
	{
		Cvar_SetValue("cl_dumpents", 0.0);
		PM_PrintPhysEnts();
	}

	if (pmove.spectator)
	{
		SpectatorMove();
		return;
	}

	// Always try and unstick us unless we are in NOCLIP mode
	if (pmove.movetype != MOVETYPE_NOCLIP)
	{
		if (PM_CheckStuck())
		{
			return;  // Can't move, we're stuck
		}
	}

	// Now that we are "unstuck", see where we are ( waterlevel and type, pmove->onground ).
	PM_CatagorizePosition();

	if (pmove.server)
	{
		VectorCopy(pmove.origin, lasts);
	}
	else
	{
		VectorCopy(pmove.origin, lastc);
	}

	// Handle movement
	switch (pmove.movetype)
	{
	default:
		Con_DPrintf("Bogus pmove player movetype %i on (%i) 0=cl 1=sv\n", pmove.movetype, pmove.server);
		break;

	case MOVETYPE_NONE:
		break;

	case MOVETYPE_NOCLIP:
		PM_NoClip();
		break;

	case MOVETYPE_TOSS:
	case MOVETYPE_BOUNCE:
		PM_Physics_Toss();
		break;

	case MOVETYPE_FLY:
		PM_CheckWater();

		// Was jump button pressed?
		// If so, set velocity to 270 away from ladder.  This is currently wrong.
		// Also, set MOVE_TYPE to walk, too.
		if (pmove.cmd.buttons & IN_JUMP)
		{
			JumpButton();
		}
		else
		{
			pmove.oldbuttons &= ~IN_JUMP;
		}

		// Perform the move accounting for any base velocity.
		VectorAdd(pmove.velocity, pmove.basevelocity, pmove.velocity);
		PM_FlyMove();
		VectorSubtract(pmove.velocity, pmove.basevelocity, pmove.velocity);
		break;

	case MOVETYPE_WALK:
		if (!PM_CheckWater() && !pmove.waterjumptime)
		{
			PM_AddCorrectGravity();
		}

		PM_CheckVelocity();

		PM_CatagorizePosition();

		// If we are leaping out of the water, just update the counters.
		if (pmove.waterjumptime)
		{
			PM_WaterJump();
			PM_FlyMove();
			return;
		}

		// If we are swimming in the water, see if we are nudging against a place we can jump up out
		//  of, and, if so, start out jump.  Otherwise, if we are not moving up, then reset jump timer to 0
		if (waterlevel >= 2)
		{
			if (waterlevel == 2 && !pmove.waterjumptime)
			{
				CheckWaterJump();
			}

			// If we are falling again, then we must not trying to jump out of water any more.
			if (pmove.velocity[2] < 0 && pmove.waterjumptime)
			{
				pmove.waterjumptime = 0;
			}

			// Was jump button pressed?
			if (pmove.cmd.buttons & IN_JUMP)
			{
				JumpButton();
			}
			else
			{
				pmove.oldbuttons &= ~IN_JUMP;
			}

			// Perform regular water movement
			PM_WaterMove();

			VectorSubtract(pmove.velocity, pmove.basevelocity, pmove.velocity);
		}
		else
		// Not underwater
		{
			// Fricion is handled before we add in any base velocity. That way, if we are on a conveyor, 
			//  we don't slow when standing still, relative to the conveyor.
			if (onground != -1)
			{
				pmove.velocity[2] = 0.0;
				PM_Friction();
			}

			// Was jump button pressed?
			if (pmove.cmd.buttons & IN_JUMP)
			{
				JumpButton();
			}
			else
			{
				pmove.oldbuttons &= ~IN_JUMP;
			}

			// Make sure velocity is valid.
			PM_CheckVelocity();

			// Set final flags.
			PM_CatagorizePosition();

			// Are we on ground now
			if (onground != -1)
			{
				PM_WalkMove();
			}
			else
			{
				PM_AirMove();  // Take into account movement when in air.
			}

			// Now pull the base velocity back out.
			// Base velocity is set if you are on a moving object, like
			//  a conveyor (or maybe another monster?)
			VectorSubtract(pmove.velocity, pmove.basevelocity, pmove.velocity);

			// Make sure velocity is valid.
			PM_CheckVelocity();

			// Add any remaining gravitational component.
			if (!PM_CheckWater() && !pmove.waterjumptime)
			{
				PM_FixupGravityVelocity();
			}

			PM_CatagorizePosition();

			// If we are on ground, no downward velocity.
			if (onground != -1)
			{
				pmove.velocity[2] = 0;
			}
		}
		break;
	}
}

/*
================
PM_AddToTouched

Add's the trace result to touch list, if contact is not already in list.
================
*/
qboolean PM_AddToTouched( pmtrace_t tr, vec_t* impactvelocity )
{
	int i;

	for (i = 0; i < pmove.numtouch; i++)
	{
		if (pmove.touchindex[i].ent == tr.ent)
			break;
	}
	if (i != pmove.numtouch)  // Already in list.
		return FALSE;

	if (pm_pushfix.value)
	{
		if (pmove.server)
		{
			vec3_t vel;
			trace_t trace;

			// Convert pmtrace_t to trace_t
			trace.allsolid = tr.allsolid;
			trace.startsolid = tr.startsolid;
			trace.inopen = tr.inopen;
			trace.inwater = tr.inwater;
			trace.fraction = tr.fraction;
			VectorCopy(tr.endpos, trace.endpos);
			VectorCopy(tr.plane.normal, trace.plane.normal);
			trace.plane.dist = tr.plane.dist;
			trace.ent = EDICT_NUM(pmove.physents[pmove.touchindex[i].ent].info);
			trace.hitgroup = tr.hitgroup;

			// Save velocity
			VectorCopy(pmove.velocity, vel);

			VectorCopy(impactvelocity, sv_player->v.velocity);

			if (!(trace.ent->v.flags & FL_SPECTATOR) && !pmove.spectator)
			{
				// Run the impact function
				SV_Impact(trace.ent, sv_player, &trace);
			}

			VectorSubtract(sv_player->v.velocity, vel, tr.deltavelocity);

			// Restore it back
			VectorCopy(vel, pmove.velocity);
		}
		else
		{
			VectorCopy(vec3_origin, tr.deltavelocity);
		}
	}
	else
	{
		VectorCopy(impactvelocity, tr.deltavelocity);
	}

	if (pmove.numtouch >= MAX_PHYSENTS)
		Con_DPrintf("Too many entities were touched!\n");

	pmove.touchindex[pmove.numtouch++] = tr;
	return TRUE;
}