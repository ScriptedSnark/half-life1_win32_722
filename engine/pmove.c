#include "quakedef.h"
#include "pmove.h"
#include "pr_cmds.h"
#include "sv_proto.h"

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

vec3_t	player_mins[3] = {{-16, -16, -36}, {-16, -16, -18}, {0, 0, 0}};
vec3_t	player_maxs[3] = {{16, 16, 36}, {16, 16, 18}, {0, 0, 0}};

// Expand debugging BBOX particle hulls by this many units.
#define BOX_GAP 0.0f               

static int PM_boxpnt[6][4] =
{
	{ 0, 4, 6, 2 }, // +X
	{ 0, 1, 5, 4 }, // +Y
	{ 0, 2, 3, 1 }, // +Z
	{ 7, 5, 1, 3 }, // -X
	{ 7, 3, 2, 6 }, // -Y
	{ 7, 6, 4, 5 }, // -Z
};

void PM_InitBoxHull( void );

void CreateStuckTable( void );

void Pmove_Init( void )
{
	PM_InitBoxHull();

	CreateStuckTable();
}

/*
===============
char* PM_NameForContents( int contents )

================
*/
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

/*
===============
void PM_PrintPhysEnts( void )

================
*/
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
	float linestep = 2.0f;
	float curdist;
	float len;
	vec3_t curpos;
	vec3_t diff;
	int i;
	// Determine distance;

	VectorSubtract(end, start, diff);

	len = VectorNormalize(diff);

	curdist = 0;
	while (curdist <= len)
	{
		for (i = 0; i < 3; i++)
			curpos[i] = start[i] + curdist * diff[i];

		CL_Particle(curpos, pcolor, life, 0, vert);
		curdist += linestep;
	}
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
PM_DrawPhysEntBBox( int num )

================
*/
void PM_DrawPhysEntBBox( int num, int pcolor, float life )
{
	physent_t* pe;
	vec3_t org;
	int j;
	vec3_t tmp;
	vec3_t		p[8];
	float gap = BOX_GAP;

	if (num >= pmove.numphysent || num <= 0)
		return;

	pe = &pmove.physents[num];

	if (pe->model)
	{
		VectorCopy(pe->origin, org);

		for (j = 0; j < 8; j++)
		{
			tmp[0] = (j & 1) ? pe->model->mins[0] - gap : pe->model->maxs[0] + gap;
			tmp[1] = (j & 2) ? pe->model->mins[1] - gap : pe->model->maxs[1] + gap;
			tmp[2] = (j & 4) ? pe->model->mins[2] - gap : pe->model->maxs[2] + gap;

			VectorCopy(tmp, p[j]);
		}

		// If the bbox should be rotated, do that
		if (pe->angles[0] || pe->angles[1] || pe->angles[2])
		{
			vec3_t	forward, right, up;

			AngleVectorsTranspose(pe->angles, forward, right, up);
			for (j = 0; j < 8; j++)
			{
				VectorCopy(p[j], tmp);
				p[j][0] = DotProduct(tmp, forward);
				p[j][1] = DotProduct(tmp, right);
				p[j][2] = DotProduct(tmp, up);
			}
		}

		// Offset by entity origin, if any.
		for (j = 0; j < 8; j++)
			VectorAdd(p[j], org, p[j]);

		for (j = 0; j < 6; j++)
		{
			PM_DrawRectangle(
				p[PM_boxpnt[j][1]],
				p[PM_boxpnt[j][0]],
				p[PM_boxpnt[j][2]],
				p[PM_boxpnt[j][3]],
				pcolor, life);
		}
	}
	else
	{
		for (j = 0; j < 8; j++)
		{
			tmp[0] = (j & 1) ? pe->mins[0] : pe->maxs[0];
			tmp[1] = (j & 2) ? pe->mins[1] : pe->maxs[1];
			tmp[2] = (j & 4) ? pe->mins[2] : pe->maxs[2];

			VectorAdd(tmp, pe->origin, tmp);
			VectorCopy(tmp, p[j]);
		}

		for (j = 0; j < 6; j++)
		{
			PM_DrawRectangle(
				p[PM_boxpnt[j][1]],
				p[PM_boxpnt[j][0]],
				p[PM_boxpnt[j][2]],
				p[PM_boxpnt[j][3]],
				pcolor, life);
		}
	}
}

/*
================
PM_DrawBBox( vec_t* mins, vec_t* maxs, vec_t* origin, int pcolor, float life )

================
*/
void PM_DrawBBox( vec_t* mins, vec_t* maxs, vec_t* origin, int pcolor, float life )
{
	int j;

	vec3_t tmp;
	vec3_t		p[8];
	float gap = BOX_GAP;

	for (j = 0; j < 8; j++)
	{
		tmp[0] = (j & 1) ? mins[0] - gap : maxs[0] + gap;
		tmp[1] = (j & 2) ? mins[1] - gap : maxs[1] + gap;
		tmp[2] = (j & 4) ? mins[2] - gap : maxs[2] + gap;

		VectorAdd(tmp, origin, tmp);
		VectorCopy(tmp, p[j]);
	}

	for (j = 0; j < 6; j++)
	{
		PM_DrawRectangle(
			p[PM_boxpnt[j][1]],
			p[PM_boxpnt[j][0]],
			p[PM_boxpnt[j][2]],
			p[PM_boxpnt[j][3]],
			pcolor, life);
	}
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
	vec3_t forward, right, up;
	float raydist = 256.0f;
	vec3_t origin;
	vec3_t end;
	int i;
	pmtrace_t trace;
	int pcolor = 77;
	float fup;

	if (!cl_showclip.value)
		return;

	AngleVectors(pmove.angles, forward, right, up);  // Determine movement angles

	VectorCopy(pmove.origin, origin);

	fup = 0.5 * (player_mins[pmove.usehull][2] + player_maxs[pmove.usehull][2]);
	fup += pmove.view_ofs[2];
	fup -= 4;

	for (i = 0; i < 3; i++)
	{
		end[i] = origin[i] + raydist * forward[i];
	}

	trace = PM_PlayerMove(origin, end, PM_STUDIO_BOX);

	if (trace.ent > 0)  // Not the world
		pcolor = 111;

	// Draw the hull or bbox.
	if (trace.ent > 0)
	{
		PM_DrawPhysEntBBox(trace.ent, pcolor, 0.3f);
		if (cl_printclip.value)
			CL_PrintEntity(&cl_entities[pmove.physents[trace.ent].info]);
	}
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
#define	STOP_EPSILON	0.1

int PM_ClipVelocity( vec_t* in, vec_t* normal, vec_t* out, float overbounce )
{
	float	backoff;
	float	change;
	float angle;
	int		i, blocked;

	angle = normal[2];

	blocked = 0;
	if (angle > 0)
		blocked |= 1;		// floor
	if (!angle)
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

void PM_AddCorrectGravity( void )
{
	float	ent_gravity;

	if (pmove.gravity)
		ent_gravity = pmove.gravity;
	else
		ent_gravity = 1.0;

	pmove.velocity[2] -= (ent_gravity * movevars.gravity * 0.5 * frametime);
	pmove.velocity[2] += pmove.basevelocity[2] * frametime;
	pmove.basevelocity[2] = 0;

	PM_CheckVelocity();
}


void PM_FixupGravityVelocity( void )
{
	float	ent_gravity;

	if (pmove.gravity)
		ent_gravity = pmove.gravity;
	else
		ent_gravity = 1.0;

	// Get the correct velocity for the end of the dt 
	pmove.velocity[2] -= (ent_gravity * movevars.gravity * frametime * 0.5);

	PM_CheckVelocity();
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
	int			bumpcount, numbumps;
	vec3_t		dir;
	float		d;
	int			numplanes;
	vec3_t		planes[MAX_CLIP_PLANES];
	vec3_t		primal_velocity, original_velocity;
	vec3_t      new_velocity;
	int			i, j;
	pmtrace_t	trace;
	vec3_t		end;
	float		time_left, allFraction;
	int			blocked;

	numbumps = 4;

	blocked = 0;
	numplanes = 0;
	VectorCopy(pmove.velocity, original_velocity);
	VectorCopy(pmove.velocity, primal_velocity);

	allFraction = 0;
	time_left = frametime;

	for (bumpcount = 0; bumpcount < numbumps; bumpcount++)
	{
		if (!pmove.velocity[0] && !pmove.velocity[1] && !pmove.velocity[2])
			break;

		for (i = 0; i < 3; i++)
			end[i] = pmove.origin[i] + time_left * pmove.velocity[i];

		trace = PM_PlayerMove(pmove.origin, end, PM_NORMAL);

		allFraction += trace.fraction;
		if (trace.allsolid)
		{	// entity is trapped in another solid
			VectorCopy(vec3_origin, pmove.velocity);
			//Con_DPrintf("Trapped 4\n");
			return 4;
		}

		if (trace.fraction > 0)
		{	// actually covered some distance
			VectorCopy(trace.endpos, pmove.origin);
			VectorCopy(pmove.velocity, original_velocity);
			numplanes = 0;
		}

		if (trace.fraction == 1)
			break;		// moved the entire distance

		//if (!trace.ent)
		//	Sys_Error ("PM_PlayerTrace: !trace.ent");

		// save entity for contact
		PM_AddToTouched(trace, pmove.velocity);

		if (trace.plane.normal[2] > 0.7)
		{
			blocked |= 1;		// floor
		}
		if (!trace.plane.normal[2])
		{
			blocked |= 2;		// step
			//Con_DPrintf("Blocked by %i\n", trace.ent);
		}

		time_left -= time_left * trace.fraction;

	// cliped to another plane
		if (numplanes >= MAX_CLIP_PLANES)
		{	// this shouldn't really happen
			VectorCopy(vec3_origin, pmove.velocity);
			//Con_DPrintf("Too many planes 4\n");

			break;
		}

		VectorCopy(trace.plane.normal, planes[numplanes]);
		numplanes++;

//
// modify original_velocity so it parallels all of the clip planes
//
		if (pmove.movetype == MOVETYPE_WALK &&
			((onground == -1) || (pmove.friction != 1)))	// relfect player velocity
		{
			for (i = 0; i < numplanes; i++)
			{
				if (planes[i][2] > 0.7)
				{// floor or slope
					PM_ClipVelocity(original_velocity, planes[i], new_velocity, 1);
					VectorCopy(new_velocity, original_velocity);
				}
				else
					PM_ClipVelocity(original_velocity, planes[i], new_velocity, 1.0 + movevars.bounce * (1 - pmove.friction));
			}

			VectorCopy(new_velocity, pmove.velocity);
			VectorCopy(new_velocity, original_velocity);
		}
		else
		{
			for (i = 0; i < numplanes; i++)
			{
				PM_ClipVelocity(
					original_velocity,
					planes[i],
					pmove.velocity,
					1);
				for (j = 0; j < numplanes; j++)
					if (j != i)
					{
						if (DotProduct(pmove.velocity, planes[j]) < 0)
							break;	// not ok
					}
				if (j == numplanes)
					break;
			}

			// Did we go all the way through plane set
			if (i != numplanes)
			{	// go along this plane
			}
			else
			{	// go along the crease
				if (numplanes != 2)
				{
					//Con_Printf ("clip velocity, numplanes == %i\n",numplanes);
					VectorCopy(vec3_origin, pmove.velocity);
					//Con_DPrintf("Trapped 4\n");

					break;
				}
				CrossProduct(planes[0], planes[1], dir);
				d = DotProduct(dir, pmove.velocity);
				VectorScale(dir, d, pmove.velocity);
			}

//
// if original velocity is against the original velocity, stop dead
// to avoid tiny occilations in sloping corners
//
			if (DotProduct(pmove.velocity, primal_velocity) <= 0)
			{
				//Con_DPrintf("Back\n");
				VectorCopy(vec3_origin, pmove.velocity);
				break;
			}
		}
	}

	if (allFraction == 0)
	{
		VectorCopy(vec3_origin, pmove.velocity);
		Con_DPrintf("Don't stick\n");
	}

	return blocked;
}

/*
============
PM_ApplyFriction

============
*/
void PM_ApplyFriction( physent_t* pe )
{
	float	d, d2;
	vec3_t  forward, right, up;
	vec3_t	dest;

	AngleVectors(pmove.angles, forward, right, up);   // Determine movement angles

	d = DotProduct(forward, pe->maxs) + 0.5;
	if (d >= 0.0)
		return;

	d2 = DotProduct(pmove.velocity, pe->maxs) * pmove.friction;
	VectorScale(pe->maxs, d2, dest);

	pmove.velocity[0] = (pmove.velocity[0] - dest[0]) * (d + 1.0);
	pmove.velocity[1] = (pmove.velocity[1] - dest[1]) * (d + 1.0);

	PM_CheckVelocity();
}

/*
=====================
PM_WalkMove

Only used by players.  Moves along the ground when player is a MOVETYPE_WALK.
======================
*/
void PM_WalkMove( void )
{
	int			clip;
	int			oldonground;
	int i;

	vec3_t		wishvel;
	float       spd;
	float		fmove, smove;
	vec3_t		wishdir;
	float		wishspeed;

	vec3_t dest, start;
	vec3_t original, originalvel;
	vec3_t down, downvel;
	float downdist, updist;

	pmtrace_t trace;

	fmove = pmove.cmd.forwardmove;
	smove = pmove.cmd.sidemove;

	forward[2] = 0;
	right[2] = 0;

	VectorNormalize(forward);
	VectorNormalize(right);

	for (i = 0; i < 2; i++)
		wishvel[i] = forward[i] * fmove + right[i] * smove;
	wishvel[2] = 0;

	VectorCopy(wishvel, wishdir);
	wishspeed = VectorNormalize(wishdir);

//
// clamp to server defined max speed
//
	if (wishspeed > pmove.maxspeed)
	{
		VectorScale(wishvel, pmove.maxspeed / wishspeed, wishvel);
		wishspeed = pmove.maxspeed;
	}

	pmove.velocity[2] = 0;
	PM_Accelerate(wishdir, wishspeed, movevars.accelerate);
	pmove.velocity[2] = 0;

	VectorAdd(pmove.velocity, pmove.basevelocity, pmove.velocity);

	spd = Length(pmove.velocity);

	if (spd < 1.0f)
	{
		VectorCopy(vec3_origin, pmove.velocity);
		return;
	}

	//if (!pmove.velocity[0] && !pmove.velocity[1] && !pmove.velocity[2])
	//	return;

	oldonground = onground;

// first try just moving to the destination	
	dest[0] = pmove.origin[0] + pmove.velocity[0] * frametime;
	dest[1] = pmove.origin[1] + pmove.velocity[1] * frametime;
	dest[2] = pmove.origin[2];

// first try moving directly to the next spot
	VectorCopy(dest, start);
	trace = PM_PlayerMove(pmove.origin, dest, PM_NORMAL);
	if (trace.fraction == 1.0)
	{
		VectorCopy(trace.endpos, pmove.origin);
		return;
	}

	if (oldonground == -1 &&
		waterlevel == 0)
		return;

	if (pmove.waterjumptime)
		return;

// try sliding forward both on ground and up 16 pixels
//  take the move that goes farthest
	VectorCopy(pmove.origin, original);
	VectorCopy(pmove.velocity, originalvel);

// slide move
	clip = PM_FlyMove();

	VectorCopy(pmove.origin, down);
	VectorCopy(pmove.velocity, downvel);

	VectorCopy(original, pmove.origin);

	VectorCopy(originalvel, pmove.velocity);

	VectorCopy(pmove.origin, dest);
	dest[2] += movevars.stepsize;

	trace = PM_PlayerMove(pmove.origin, dest, PM_NORMAL);
	if (!trace.startsolid && !trace.allsolid)
		VectorCopy(trace.endpos, pmove.origin);

// slide move the rest of the way.
	clip = PM_FlyMove();

// now try going back down from the end point
//  press down the stepheight
	VectorCopy(pmove.origin, dest);
	dest[2] -= movevars.stepsize;

	trace = PM_PlayerMove(pmove.origin, dest, PM_NORMAL);
	if (trace.plane.normal[2] < 0.7)
		goto usedown;

	if (!trace.startsolid && !trace.allsolid)
		VectorCopy(trace.endpos, pmove.origin);

	VectorCopy(pmove.origin, up);

	// decide which one went farther
	downdist = (down[0] - original[0]) * (down[0] - original[0])
		+ (down[1] - original[1]) * (down[1] - original[1]);
	updist = (up[0] - original[0]) * (up[0] - original[0])
		+ (up[1] - original[1]) * (up[1] - original[1]);

	if (downdist > updist)
	{
usedown:
		VectorCopy(down, pmove.origin);
		VectorCopy(downvel, pmove.velocity);
	}
	else // copy z value from slide move
		pmove.velocity[2] = downvel[2];
}

/*
==================
PM_Friction

Handles both ground friction and water friction
==================
*/
void PM_Friction( void )
{
	float* vel;
	float	speed, newspeed, control;
	float	friction;
	float	drop;
	vec3_t	newvel;

	if (pmove.waterjumptime)
		return;

	vel = pmove.velocity;

	speed = sqrt(vel[0] * vel[0] + vel[1] * vel[1] + vel[2] * vel[2]);
	if (speed < 0.1)
	{
		vel[0] = 0;
		vel[1] = 0;
		return;
	}

	drop = 0;

// apply ground friction
	if (onground != -1)
	{
		vec3_t start, stop;
		pmtrace_t trace;

		start[0] = stop[0] = pmove.origin[0] + vel[0] / speed * 16;
		start[1] = stop[1] = pmove.origin[1] + vel[1] / speed * 16;
		start[2] = pmove.origin[2] + player_mins[pmove.usehull][2];
		stop[2] = start[2] - 34;

		trace = PM_PlayerMove(start, stop, PM_NORMAL);

		if (trace.fraction == 1.0)
			friction = movevars.friction * movevars.edgefriction;
		else
			friction = movevars.friction;

		friction *= pmove.friction;  // player friction?

		// Bleed off some speed, but if we have less than the bleed
		//  threshhold, bleed the theshold amount.
		control = (speed <= movevars.stopspeed) ?
			movevars.stopspeed : speed;
		drop += control * friction * frametime;
	}

// apply water friction
//	if (waterlevel)
//		drop += speed * movevars.waterfriction * waterlevel * frametime;

// scale the velocity
	newspeed = speed - drop;
	if (newspeed < 0)
		newspeed = 0;
	newspeed /= speed;

	newvel[0] = vel[0] * newspeed;
	newvel[1] = vel[1] * newspeed;
	newvel[2] = vel[2] * newspeed;

	VectorCopy(newvel, pmove.velocity);
}


void PM_Accelerate( vec_t* wishdir, float wishspeed, float accel )
{
	int			i;
	float		addspeed, accelspeed, currentspeed;

	if (pmove.dead)
		return;
	if (pmove.waterjumptime)
		return;

	currentspeed = DotProduct(pmove.velocity, wishdir);
	addspeed = wishspeed - currentspeed;
	if (addspeed <= 0)
		return;
	accelspeed = accel * frametime * wishspeed * pmove.friction;
	if (accelspeed > addspeed)
		accelspeed = addspeed;

	for (i = 0; i < 3; i++)
		pmove.velocity[i] += accelspeed * wishdir[i];
}

void PM_AirAccelerate( vec_t* wishdir, float wishspeed, float accel )
{
	int			i;
	float		addspeed, accelspeed, currentspeed, wishspd = wishspeed;

	if (pmove.dead)
		return;
	if (pmove.waterjumptime)
		return;

	if (wishspeed > 30)
		wishspd = 30;
	currentspeed = DotProduct(pmove.velocity, wishdir);
	addspeed = wishspd - currentspeed;
	if (addspeed <= 0)
		return;
	accelspeed = accel * wishspeed * frametime * pmove.friction;
	if (accelspeed > addspeed)
		accelspeed = addspeed;

	for (i = 0; i < 3; i++)
		pmove.velocity[i] += accelspeed * wishdir[i];
}



/*
===================
PM_WaterMove

===================
*/
void PM_WaterMove( void )
{
	int		i;
	vec3_t	wishvel;
	float	wishspeed;
	vec3_t	wishdir;
	vec3_t	start, dest;
	vec3_t  temp;
	pmtrace_t	trace;

	float speed, newspeed, addspeed, accelspeed;

//
// user intentions
//
	for (i = 0; i < 3; i++)
		wishvel[i] = forward[i] * pmove.cmd.forwardmove + right[i] * pmove.cmd.sidemove;

	if (!pmove.cmd.forwardmove && !pmove.cmd.sidemove && !pmove.cmd.upmove)
		wishvel[2] -= 60;		// drift towards bottom
	else 
		wishvel[2] += pmove.cmd.upmove;

	VectorCopy(wishvel, wishdir);
	wishspeed = VectorNormalize(wishdir);
	if (wishspeed > pmove.maxspeed)
	{
		VectorScale(wishvel, pmove.maxspeed / wishspeed, wishvel);
		wishspeed = pmove.maxspeed;
	}
	wishspeed *= 0.7;

	VectorAdd(pmove.velocity, pmove.basevelocity, pmove.velocity);
// Water friction
	VectorCopy(pmove.velocity, temp);
	speed = VectorNormalize(temp);
	if (speed)
	{
		newspeed = (1.0 - frametime * movevars.friction * pmove.friction) * speed;

		if (newspeed < 0)
			newspeed = 0;
		VectorScale(pmove.velocity, newspeed / speed, pmove.velocity);
	}
	else
		newspeed = 0;

//
// water acceleration
//
//	if (pmove.waterjumptime)
//		Con_Printf("wm->%f, %f, %f\n", pmove.velocity[0], pmove.velocity[1], pmove.velocity[2]);

	if (wishspeed < 0.1f)
	{
		return;
	}

	addspeed = wishspeed - newspeed;
	if (addspeed > 0)
	{

		VectorNormalize(wishvel);
		accelspeed = movevars.accelerate * wishspeed * frametime * pmove.friction;
		if (accelspeed > addspeed)
			accelspeed = addspeed;

		for (i = 0; i < 3; i++)
			pmove.velocity[i] += accelspeed * wishvel[i];
	}

// assume it is a stair or a slope, so press down from stepheight above
	VectorMA(pmove.origin, frametime, pmove.velocity, dest);
	VectorCopy(dest, start);
	start[2] += movevars.stepsize + 1;
	trace = PM_PlayerMove(start, dest, PM_NORMAL);
	if (!trace.startsolid && !trace.allsolid)	// FIXME: check steep slope?
	{	// walked up the step, so just keep result and exit
		VectorCopy(trace.endpos, pmove.origin);
		return;
	}

	PM_FlyMove();
//	if (pmove.waterjumptime)
//		Con_Printf("<-wm%f, %f, %f\n", pmove.velocity[0], pmove.velocity[1], pmove.velocity[2]);
}


/*
===================
PM_AirMove

===================
*/
void PM_AirMove( void )
{
	int			i;
	vec3_t		wishvel;
	float		fmove, smove;
	vec3_t		wishdir;
	float		wishspeed;

	fmove = pmove.cmd.forwardmove;
	smove = pmove.cmd.sidemove;

	forward[2] = 0;
	right[2] = 0;
	VectorNormalize(forward);
	VectorNormalize(right);

	for (i = 0; i < 2; i++)
		wishvel[i] = forward[i] * fmove + right[i] * smove;
	wishvel[2] = 0;

	VectorCopy(wishvel, wishdir);
	wishspeed = VectorNormalize(wishdir);

//
// clamp to server defined max speed
//
	if (wishspeed > pmove.maxspeed)
	{
		VectorScale(wishvel, pmove.maxspeed / smove, wishvel);
		wishspeed = pmove.maxspeed;
	}

	PM_AirAccelerate(wishdir, wishspeed, movevars.airaccelerate);

	VectorAdd(pmove.velocity, pmove.basevelocity, pmove.velocity);

	PM_FlyMove();
}



/*
=============
PM_CatagorizePosition
=============
*/
void PM_CatagorizePosition( void )
{
	vec3_t		point;
	pmtrace_t		tr;

// if the player hull point one unit down is solid, the player
// is on ground

// see if standing on something solid	
	point[0] = pmove.origin[0];
	point[1] = pmove.origin[1];
	point[2] = pmove.origin[2] - 2;
	if (pmove.velocity[2] > 180)
	{
		onground = -1;
	}
	else
	{
		tr = PM_PlayerMove(pmove.origin, point, PM_NORMAL);	
		if (tr.plane.normal[2] < 0.7)
			onground = -1;	// too steep
		else
			onground = tr.ent;
		if (onground != -1)
		{
			pmove.waterjumptime = 0;
			if (!tr.startsolid && !tr.allsolid)
				VectorCopy(tr.endpos, pmove.origin);
		}

		// standing on an entity other than the world
		if (tr.ent > 0)
		{
			PM_AddToTouched(tr, pmove.velocity);
		}
	}

	PM_CheckWater();
}


/*
=============
JumpButton
=============
*/
#define PLAYER_LONGJUMP_SPEED 350 // how fast we longjump
void JumpButton( void )
{
	int i;

	if (pmove.server)
		return;

	if (pmove.dead)
	{
		pmove.oldbuttons |= IN_JUMP;	// don't jump again until released
		return;
	}

	if (pmove.waterjumptime)
	{
		pmove.waterjumptime -= frametime;
		if (pmove.waterjumptime < 0)
			pmove.waterjumptime = 0;
		return;
	}

	if (waterlevel >= 2)
	{	// swimming, not jumping
		onground = -1;
		if (watertype == CONTENTS_WATER)
			pmove.velocity[2] = 100;
		else if (watertype == CONTENTS_SLIME)
			pmove.velocity[2] = 80;
		else
			pmove.velocity[2] = 50;
		return;
	}

	if (onground == -1)
		return;		// in air, so no effect

	if (pmove.oldbuttons & IN_JUMP)
		return;		// don't pogo stick

	onground = -1;

	// Adjust for super long jump module
	// UNDONE -- note this should be based on forward angles, not current velocity.
	if (pmove.usehull == 1)
	{
		for (i = 0; i < 2; i++)
		{
			pmove.velocity[i] = pmove.velocity[i] * frametime * PLAYER_LONGJUMP_SPEED * 1.6;
		}

		pmove.velocity[2] += sqrt(2 * 800 * 56.0);
	}
	else
	{

		pmove.velocity[2] += sqrt(2 * 800 * 45.0);
	}

	pmove.oldbuttons |= IN_JUMP;	// don't jump again until released
}

/*
=============
CheckWaterJump
=============
*/
#define WJ_HEIGHT 8
void CheckWaterJump( void )
{
	vec3_t	spot;
	int		cont;
	vec3_t	flatforward;

	if (pmove.waterjumptime)
		return;

	// ZOID, don't hop out if we just jumped in
	if (pmove.velocity[2] < -180)
		return; // only hop out if we are moving up

	// see if near an edge
	flatforward[0] = forward[0];
	flatforward[1] = forward[1];
	flatforward[2] = 0.0;
	VectorNormalize(flatforward);

	VectorMA(pmove.origin, 24, flatforward, spot);
	spot[2] += WJ_HEIGHT;
	cont = PM_PointContents(spot);
	if (cont != CONTENTS_SOLID)
		return;
	spot[2] += 24;
	cont = PM_PointContents(spot);
	if (cont != CONTENTS_EMPTY)
		return;
	// jump out of water
	VectorScale(forward, 200, pmove.velocity);
	pmove.velocity[2] = 225;
	pmove.waterjumptime = 2;	// safety net
	pmove.oldbuttons |= IN_JUMP;	// don't jump again until released
}

/*
=============
PM_CheckWater

=============
*/
qboolean PM_CheckWater( void )
{
	vec3_t	point;
	int		cont;
	int		truecont;
	float     height;
	float		heightover2;

	// Pick a spot just above the players feet.
	point[0] = pmove.origin[0] + (player_mins[pmove.usehull][0] + player_maxs[pmove.usehull][0]) * 0.5;
	point[1] = pmove.origin[1] + (player_mins[pmove.usehull][1] + player_maxs[pmove.usehull][1]) * 0.5;
	point[2] = pmove.origin[2] + player_mins[pmove.usehull][2] + 1;

//
// get waterlevel
//
	waterlevel = 0;
	watertype = CONTENTS_EMPTY;

	// Grab point contents.
	cont = PM_PointContents(point);
	// Are we under water? (not solid and not empty?)
	if (cont <= CONTENTS_WATER && cont > CONTENTS_TRANSLUCENT)
	{
		truecont = PM_TruePointContents(point);

		// Set water type
		watertype = cont;

		// We are at least at level one
		waterlevel = 1;

		height = (player_mins[pmove.usehull][2] + player_maxs[pmove.usehull][2]);
		heightover2 = height * 0.5;

		// Now check a point that is at the player hull midpoint.
		point[2] = pmove.origin[2] + heightover2;
		cont = PM_PointContents(point);
		// If that point is also under water...
		if (cont <= CONTENTS_WATER && cont > CONTENTS_TRANSLUCENT)
		{
			// Set a higher water level.
			waterlevel = 2;

			// Now check the eye position.  (view_ofs is relative to the origin)
			point[2] = pmove.origin[2] + pmove.view_ofs[2];

			cont = PM_PointContents(point);
			if (cont <= CONTENTS_WATER && cont > CONTENTS_TRANSLUCENT)
				waterlevel = 3;  // In over our eyes
		}

		// Adjust velocity based on water current, if any.
		if ((truecont <= CONTENTS_CURRENT_0) &&
			(truecont >= CONTENTS_CURRENT_DOWN))
		{
			// The deeper we are, the stronger the current.
			static vec3_t current_table[] =
			{
				{1, 0, 0}, {0, 1, 0}, {-1, 0, 0},
				{0, -1, 0}, {0, 0, 1}, {0, 0, -1}
			};

			VectorMA(pmove.basevelocity, 50.0 * waterlevel, current_table[CONTENTS_CURRENT_0 - truecont], pmove.basevelocity);
		}
	}

	return waterlevel > 1;
}

static vec3_t rgv3tStuckTable[54];
static int rgStuckLast[MAX_CLIENTS][2];

void CreateStuckTable( void )
{
	float x, y, z;
	int idx;
	int i;
	float zi[3];

	memset(rgv3tStuckTable, 0, 54 * sizeof(vec3_t));

	idx = 0;
	// Little Moves.
	x = y = 0;
	// Z moves
	for (z = -0.125; z <= 0.125; z += 0.125)
	{
		rgv3tStuckTable[idx][0] = x;
		rgv3tStuckTable[idx][1] = y;
		rgv3tStuckTable[idx][2] = z;
		idx++;
	}
	x = z = 0;
	// Y moves
	for (y = -0.125; y <= 0.125; y += 0.125)
	{
		rgv3tStuckTable[idx][0] = x;
		rgv3tStuckTable[idx][1] = y;
		rgv3tStuckTable[idx][2] = z;
		idx++;
	}
	y = z = 0;
	// X moves
	for (x = -0.125; x <= 0.125; x += 0.125)
	{
		rgv3tStuckTable[idx][0] = x;
		rgv3tStuckTable[idx][1] = y;
		rgv3tStuckTable[idx][2] = z;
		idx++;
	}

	// Remaining multi axis nudges.
	for (x = -0.125; x <= 0.125; x += 0.250)
	{
		for (y = -0.125; y <= 0.125; y += 0.250)
		{
			for (z = -0.125; z <= 0.125; z += 0.250)
			{
				rgv3tStuckTable[idx][0] = x;
				rgv3tStuckTable[idx][1] = y;
				rgv3tStuckTable[idx][2] = z;
				idx++;
			}
		}
	}

	// Big Moves.
	x = y = 0;
	zi[0] = 0.0f;
	zi[1] = 1.0f;
	zi[2] = 6.0f;

	for (i = 0; i < 3; i++)
	{
		// Z moves
		z = zi[i];
		rgv3tStuckTable[idx][0] = x;
		rgv3tStuckTable[idx][1] = y;
		rgv3tStuckTable[idx][2] = z;
		idx++;
	}

	x = z = 0;

	// Y moves
	for (y = -2.0f; y <= 2.0f; y += 2.0)
	{
		rgv3tStuckTable[idx][0] = x;
		rgv3tStuckTable[idx][1] = y;
		rgv3tStuckTable[idx][2] = z;
		idx++;
	}
	y = z = 0;
	// X moves
	for (x = -2.0f; x <= 2.0f; x += 2.0f)
	{
		rgv3tStuckTable[idx][0] = x;
		rgv3tStuckTable[idx][1] = y;
		rgv3tStuckTable[idx][2] = z;
		idx++;
	}

	// Remaining multi axis nudges.
	for (i = 0; i < 3; i++)
	{
		z = zi[i];

		for (x = -2.0f; x <= 2.0f; x += 2.0f)
		{
			for (y = -2.0f; y <= 2.0f; y += 2.0)
			{
				rgv3tStuckTable[idx][0] = x;
				rgv3tStuckTable[idx][1] = y;
				rgv3tStuckTable[idx][2] = z;
				idx++;
			}
		}
	}
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
 // Last time we did a full
	int idx;
	idx = rgStuckLast[nIndex][server]++;

	VectorCopy(rgv3tStuckTable[idx % 54], offset);

	return (idx % 54);
}

void ResetStuckOffsets( int nIndex, int server )
{
	rgStuckLast[nIndex][server] = 0;
}

#define PM_CHECKSTUCK_MINTIME 0.05  // Don't check again too quickly.

extern pmtrace_t g_Trace;
int PM_CheckStuck( void )
{
	vec3_t	base;
	vec3_t  offset;
	vec3_t  test;
	int     hitent;
	int		idx;
	float	fTime;
	int i;

	static float rgStuckCheckTime[MAX_CLIENTS][2]; // Last time we did a full
	static float ft[3];

	ft[0] = Sys_FloatTime();
	hitent = PM_TestPlayerPosition(pmove.origin);
	ft[1] = Sys_FloatTime();
	if (hitent == -1)
	{
		ResetStuckOffsets(pmove.player_index, pmove.server);
		return 0;
	}

	VectorCopy(pmove.origin, base);

	// 
	// Deal with precision error in network.
	// 
	if (!pmove.server)
	{
		// World or BSP model
		if ((hitent == 0) ||
			(pmove.physents[hitent].model != NULL))
		{
			int nReps = 0;
			ResetStuckOffsets(pmove.player_index, pmove.server);
			do
			{
				i = GetRandomStuckOffsets(pmove.player_index, pmove.server, offset);

				VectorAdd(base, offset, test);
				if (PM_TestPlayerPosition(test) == -1)
				{
					ResetStuckOffsets(pmove.player_index, pmove.server);

					VectorCopy(test, pmove.origin);
					return 0;
				}
				nReps++;
			} while (nReps < 54);
		}
	}

	// Only an issue on the client.

	if (pmove.server)
		idx = 0;
	else
		idx = 1;

	fTime = Sys_FloatTime();
	// Too soon?
	if (rgStuckCheckTime[pmove.player_index][idx] >=
		(fTime - PM_CHECKSTUCK_MINTIME))
	{
		return 1;
	}
	rgStuckCheckTime[pmove.player_index][idx] = fTime;

	if (pmove.server)
	{
		if (!pmove.spectator && !pm_nostucktouch.value)
		{
			int info;
			edict_t* ent;

			info = pmove.physents[hitent].info;
			ent = EDICT_NUM(info);
			if (!(ent->v.flags & FL_SPECTATOR) && !(g_playertouch[info >> 3] & (1 << (info & 7))))
			{
				vec3_t vel;
				trace_t trace;

				trace.allsolid = g_Trace.allsolid;
				trace.startsolid = g_Trace.startsolid;
				trace.inopen = g_Trace.inopen;
				trace.inwater = g_Trace.inwater;
				trace.fraction = g_Trace.fraction;
				VectorCopy(g_Trace.endpos, trace.endpos);
				VectorCopy(g_Trace.plane.normal, trace.plane.normal);
				trace.plane.dist = g_Trace.plane.dist;
				trace.ent = ent;
				trace.hitgroup = g_Trace.hitgroup;

				// Save velocity
				VectorCopy(sv_player->v.velocity, vel);

				VectorCopy(pmove.velocity, sv_player->v.velocity);

				SV_SetGlobalTrace(&trace);

				gEntityInterface.pfnTouch(ent, sv_player);

				// Restore it back
				VectorCopy(vel, sv_player->v.velocity);
			}
		}
	}

	i = GetRandomStuckOffsets(pmove.player_index, pmove.server, offset);

	VectorAdd(base, offset, test);
	if ((hitent = PM_TestPlayerPosition(test)) == -1)
	{
		//Con_DPrintf("Nudged\n");

		ResetStuckOffsets(pmove.player_index, pmove.server);

		if (i >= 27)
			VectorCopy(test, pmove.origin);

		return 0;
	}

	ft[2] = Sys_FloatTime();

	//VectorCopy(base, pmove->origin);

	return 1;
}

/*
===============
SpectatorMove
===============
*/
void SpectatorMove( void )
{
	float	speed, drop, friction, control, newspeed;
	//float   accel;
	float	currentspeed, addspeed, accelspeed;
	int			i;
	vec3_t		wishvel;
	float		fmove, smove;
	vec3_t		wishdir;
	float		wishspeed;

	speed = Length(pmove.velocity);
	if (speed < 1)
	{
		VectorCopy(vec3_origin, pmove.velocity);
	}
	else
	{
		drop = 0;

		friction = movevars.friction * 1.5;	// extra friction
		control = speed <= movevars.stopspeed ? movevars.stopspeed : speed;
		drop += control * friction * frametime;

		// scale the velocity
		newspeed = speed - drop;
		if (newspeed < 0)
			newspeed = 0;

		newspeed /= speed;
		VectorScale(pmove.velocity, newspeed, pmove.velocity);
	}

	// accelerate
	fmove = pmove.cmd.forwardmove;
	smove = pmove.cmd.sidemove;

	VectorNormalize(forward);
	VectorNormalize(right);

	for (i = 0; i < 3; i++)
	{
		wishvel[i] = forward[i] * fmove + right[i] * smove;
	}
	wishvel[2] += pmove.cmd.upmove;

	VectorCopy(wishvel, wishdir);
	wishspeed = VectorNormalize(wishdir);

	//
	// clamp to server defined max speed
	//
	if (wishspeed > movevars.spectatormaxspeed)
	{
		VectorScale(wishvel, movevars.spectatormaxspeed / wishspeed, wishvel);
		wishspeed = movevars.spectatormaxspeed;
	}

	currentspeed = DotProduct(pmove.velocity, wishdir);
	addspeed = wishspeed - currentspeed;
	if (addspeed <= 0)
		return;

	accelspeed = movevars.accelerate * frametime * wishspeed;
	if (accelspeed > addspeed)
		accelspeed = addspeed;

	for (i = 0; i < 3; i++)
		pmove.velocity[i] += accelspeed * wishdir[i];

	// move
	VectorMA(pmove.origin, frametime, pmove.velocity, pmove.origin);
}

/*
====================
PM_NoClip

====================
*/
void PM_NoClip( void )
{
	int			i;
	vec3_t		wishvel;
	float		fmove, smove;
//	float		currentspeed, addspeed, accelspeed;

	// Copy movement amounts
	fmove = pmove.cmd.forwardmove;
	smove = pmove.cmd.sidemove;

	VectorNormalize(forward);
	VectorNormalize(right);

	for (i = 0; i < 3; i++)       // Determine x and y parts of velocity
	{
		wishvel[i] = forward[i] * fmove + right[i] * smove;
	}
	wishvel[2] += pmove.cmd.upmove;

	VectorMA(pmove.origin, frametime, wishvel, pmove.origin);

	// Zero out the velocity so that we don't accumulate a huge downward velocity from
	//  gravity, etc.
	VectorCopy(vec3_origin, pmove.velocity);
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