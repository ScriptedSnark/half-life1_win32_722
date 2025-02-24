// sv_phys.c

#include "quakedef.h"
#include "pmove.h"

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
cvar_t sv_edgefriction = { "sv_edgefriction", "2", FALSE, TRUE };
cvar_t sv_stopspeed = { "sv_stopspeed", "100", FALSE, TRUE };
cvar_t sv_maxspeed = { "sv_maxspeed", "320", FALSE, TRUE };
cvar_t sv_accelerate = { "sv_accelerate", "10", FALSE, TRUE };
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
==================
SV_Impact

Two entities have touched, so run their touch functions
==================
*/
void SV_Impact( edict_t* e1, edict_t* e2, trace_t* ptrace )
{
	// TODO: Implement
}

void SV_Physics( void )
{
	// TODO: Implement
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