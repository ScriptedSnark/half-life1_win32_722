#include "quakedef.h"
#include "pmove.h"
#include "pr_cmds.h"

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

void PM_InitBoxHull( void );
qboolean PM_CheckWater( void );
qboolean PM_AddToTouched( pmtrace_t tr, vec_t* impactvelocity );

void CreateStuckTable( void );

void Pmove_Init( void )
{
	PM_InitBoxHull();

	CreateStuckTable();
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