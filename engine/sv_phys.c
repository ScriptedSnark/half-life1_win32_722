// sv_phys.c

#include "quakedef.h"
#include "pmove.h"
#include "sv_proto.h"

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

// TODO: Recheck these cvars from scratch

cvar_t sv_gravity = { "sv_gravity", "800", FALSE, TRUE };
cvar_t sv_friction = { "sv_friction", "4", FALSE, TRUE };
cvar_t sv_stopspeed = { "sv_stopspeed", "100", FALSE, TRUE };
cvar_t sv_stepsize = { "sv_stepsize", "18", FALSE, TRUE };
cvar_t sv_clipmode = { "sv_clipmode", "0", FALSE, TRUE };
cvar_t sv_bounce = { "sv_bounce", "1", FALSE, TRUE };
cvar_t sv_airmove = { "sv_airmove", "1", FALSE, TRUE };
cvar_t sv_spectatormaxspeed = { "sv_spectatormaxspeed", "500", FALSE, TRUE };
cvar_t sv_airaccelerate = { "sv_airaccelerate", "10", FALSE, TRUE };
cvar_t sv_wateraccelerate = { "sv_wateraccelerate", "10", FALSE, TRUE };
cvar_t sv_waterfriction = { "sv_waterfriction", "1", FALSE, TRUE };

// TODO: Implement

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

	// Don't do impacts between entities that are flagged for deletion
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


// TODO: Implement

edict_t** g_moved_edict;
vec3_t* g_moved_from;

/*
============
SV_PushMove

============
*/

// TODO: Implement

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
		case MOVETYPE_NONE:
			SV_Physics_None(ent);
			break;

		//default:
		//	Sys_Error("SV_Physics: %s bad movetype %d", &pr_strings[ent->v.classname], ent->v.movetype);
		//	break;
		}

		if (ent->v.flags & FL_KILLME)
			ED_Free(ent);
	}

	if (gGlobalVariables.force_retouch != 0)
		gGlobalVariables.force_retouch -= 1;

	sv.time += host_frametime;
}

// TODO: Implement

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

// TODO: Implement

trace_t SV_Trace_Toss( edict_t* ent, edict_t* ignore )
{
	// TODO: Implement
	trace_t trace;
	memset(&trace, 0, sizeof(trace));
	return trace;
}

// TODO: Implement