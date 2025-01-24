// view.c -- player eye positioning

#include "quakedef.h"
#include "r_local.h"
#include "pmove.h"
#include "shake.h"

/*

The view is allowed to move slightly from it's true position for bobbing,
but if it exceeds 8 pixels linear distance (spherical, not box), the list of
entities sent from the server may not include everything in the pvs, especially
when crossing a water boudnary.

*/

cvar_t		lcd_x = { "lcd_x", "0" };
cvar_t		lcd_yaw = { "lcd_yaw", "0" };

cvar_t	scr_ofsx = { "scr_ofsx", "0" };
cvar_t	scr_ofsy = { "scr_ofsy", "0" };
cvar_t	scr_ofsz = { "scr_ofsz", "0" };

cvar_t	cl_rollspeed = { "cl_rollspeed", "200" };
cvar_t	cl_rollangle = { "cl_rollangle", "2.0" };

cvar_t	cl_bobcycle = { "cl_bobcycle", "0.8" };
cvar_t	cl_bob = { "cl_bob", "0.01" };
cvar_t	cl_bobup = { "cl_bobup", "0.5" };



byte		texgammatable[256];	// palette is sent through this to convert to screen gamma

#ifdef	GLQUAKE
byte		ramps[3][256];
float		v_blend[4];		// rgba 0.0 - 1.0
#endif	// GLQUAKE

vec3_t		r_soundOrigin;
vec3_t		r_playerViewportAngles;


// TODO: Implement

cl_entity_t view;

/*
===============
V_CalcRoll

Used by view and sv_user
===============
*/
vec3_t	forward, right, up;



// TODO: Implement




/*
===============
V_CalcBob

===============
*/
float V_CalcBob( void )
{
	static	double	bobtime;
	static float	bob;
	float	cycle;

	if (cl.spectator)
		return 0;

	if (onground == -1 ||
		cl.time == cl.oldtime)
	{
		// just use old value
		return bob;
	}

	bobtime += host_frametime;
	cycle = bobtime - (int)(bobtime / cl_bobcycle.value) * cl_bobcycle.value;
	cycle /= cl_bobcycle.value;
	if (cycle < cl_bobup.value)
		cycle = M_PI * cycle / cl_bobup.value;
	else
		cycle = M_PI + M_PI * (cycle - cl_bobup.value) / (1.0 - cl_bobup.value);

// bob is proportional to simulated velocity in the xy plane
// (don't count Z, or jumping messes it up)

	bob = sqrt(cl.simvel[0] * cl.simvel[0] + cl.simvel[1] * cl.simvel[1]) * cl_bob.value;
	bob = bob * 0.3 + bob * 0.7 * sin(cycle);
	if (bob > 4)
		bob = 4;
	else if (bob < -7)
		bob = -7;
	return bob;
}


//=============================================================================


cvar_t	v_centermove = { "v_centermove", "0.15" };
cvar_t	v_centerspeed = { "v_centerspeed", "500" };



void V_StartPitchDrift( void )
{
	if (cl.laststop == cl.time)
	{
		return;		// something else is keeping it from drifting
	}

	if (cl.nodrift || !cl.pitchvel)
	{
		cl.pitchvel = v_centerspeed.value;
		cl.nodrift = FALSE;
		cl.driftmove = 0;
	}
}

void V_StopPitchDrift( void )
{
	cl.nodrift = TRUE;
	cl.pitchvel = 0.0;
	cl.laststop = cl.time;
}

/*
===============
V_DriftPitch

Moves the client pitch angle towards idealpitch sent by the server.

If the user is adjusting pitch manually, either with lookup/lookdown,
mlook and mouse, or klook and keyboard, pitch drifting is constantly stopped.
===============
*/
void V_DriftPitch( void )
{
	float		delta, move;

	if (noclip_anglehack || !cl.onground || cls.demoplayback || cl.spectator)
	{
		cl.driftmove = 0;
		cl.pitchvel = 0;
		return;
	}

// don't count small mouse motion
	if (cl.nodrift)
	{
		if (fabs(cl.cmd.forwardmove) < cl_forwardspeed.value)
			cl.driftmove = 0;
		else
			cl.driftmove += host_frametime;

		if (cl.driftmove > v_centermove.value)
		{
			V_StartPitchDrift();
		}
		return;
	}

	delta = cl.idealpitch - cl.viewangles[PITCH];

	if (!delta)
	{
		cl.pitchvel = 0;
		return;
	}

	move = host_frametime * cl.pitchvel;
	cl.pitchvel += host_frametime * v_centerspeed.value;

//Con_Printf("move: %f (%f)\n", move, host_frametime);

	if (delta > 0)
	{
		if (move > delta)
		{
			cl.pitchvel = 0;
			move = delta;
		}
		cl.viewangles[PITCH] += move;
	}
	else if (delta < 0)
	{
		if (move > -delta)
		{
			cl.pitchvel = 0;
			move = -delta;
		}
		cl.viewangles[PITCH] -= move;
	}	
}





/*
=============
V_CalcBlend
=============
*/
#ifdef	GLQUAKE
void V_CalcBlend( void )
{
	float	r, g, b, a;

	r = 0;
	g = 0;
	b = 0;
	a = 0;

	v_blend[0] = r / 255.0;
	v_blend[1] = g / 255.0;
	v_blend[2] = b / 255.0;
	v_blend[3] = a;
	if (v_blend[3] > 1)
		v_blend[3] = 1;
	if (v_blend[3] < 0)
		v_blend[3] = 0;
}
#endif

/*
=============
V_UpdatePalette
=============
*/
#ifdef	GLQUAKE
void V_UpdatePalette( void )
{
	// TODO: Implement
}
#endif

/*
==============
V_AddIdle

Idle swaying
==============
*/
void V_AddIdle( void )
{
	// TODO: Implement
}


/*
==============
V_CalcViewRoll

Roll is induced by movement and damage
==============
*/
void V_CalcViewRoll( void )
{
	// TODO: Implement
}


/*
==================
V_CalcIntermissionRefdef

==================
*/
void V_CalcIntermissionRefdef( void )
{
	// TODO: Implement
}

/*
==================
V_CalcRefdef

==================
*/
void V_CalcRefdef( void )
{
	float			bob, waterOffset;

	V_DriftPitch();

	bob = V_CalcBob();

// refresh position from simulated origin
	VectorCopy(cl.simorg, r_refdef.vieworg);
	r_refdef.vieworg[2] += bob + cl.viewheight;

	VectorCopy(cl.viewangles, r_refdef.viewangles);

	V_CalcShake();
	V_ApplyShake(r_refdef.vieworg, r_refdef.viewangles, 1.0);

	// never let view origin sit exactly on a node line, because a water plane can
	// dissapear when viewed with the eye exactly on it.
	// FIXME, we send origin at 1/128 now, change this?
	// the server protocol only specifies to 1/16 pixel, so add 1/32 in each axis

	r_refdef.vieworg[0] += 1.0 / 32;
	r_refdef.vieworg[1] += 1.0 / 32;
	r_refdef.vieworg[2] += 1.0 / 32;

	// Check for problems around water, move the viewer artificially if necessary 
	// -- this prevents drawing errors in GL due to waves

	waterOffset = 0;
	if (cl.waterlevel >= 2)
	{
		// TODO: Implement
	}

	r_refdef.vieworg[2] += waterOffset;

	V_CalcViewRoll();
	V_AddIdle();

	// TODO: Implement
}

/*
==================
V_RenderView

The player's clipping box goes from (-16 -16 -24) to (16 16 32) from
the entity origin, so any view position inside that will be valid
==================
*/
extern vrect_t	scr_vrect;

void V_RenderView( void )
{
	if (con_forcedup)
		return;

	if (cls.state != ca_active)
		return;

// don't allow cheats in multiplayer
	if (cl.maxclients > 1)
	{
		Cvar_Set("scr_ofsx", "0");
		Cvar_Set("scr_ofsy", "0");
		Cvar_Set("scr_ofsz", "0");
	}

	if (cl.intermission)
	{
		V_CalcIntermissionRefdef();
	}
	else if (!cl.paused /* && (sv.maxclients > 1 || key_dest == key_game) */)
	{
		V_CalcRefdef();
	}

	R_PushDlights();

	if (lcd_x.value)
	{
		//
		// render two interleaved views
		//
		int		i;

		vid.rowbytes <<= 1;
		vid.aspect *= 0.5;

		r_refdef.viewangles[YAW] -= lcd_yaw.value;
		for (i = 0; i < 3; i++)
			r_refdef.vieworg[i] -= right[i] * lcd_x.value;
		R_RenderView();

		vid.buffer += vid.rowbytes >> 1;

		R_PushDlights();

		r_refdef.viewangles[YAW] += lcd_yaw.value * 2;
		for (i = 0; i < 3; i++)
			r_refdef.vieworg[i] += 2 * right[i] * lcd_x.value;
		R_RenderView();

		vid.buffer -= vid.rowbytes >> 1;

		r_refdef.vrect.height <<= 1;

		vid.rowbytes >>= 1;
		vid.aspect *= 2;
	}
	else
	{
		R_RenderView();
	}
}

/*
=================
V_CalcShake

Apply noise to the eye position.
UNDONE: Feedback a bit of this into the view model position.  It shakes too much
=================
*/
void V_CalcShake( void )
{
	// TODO: Implement
}

/*
=================
V_ApplyShake

Apply the current screen shake to this origin/angles.  Factor is the amount to apply
This is so you can blend in part of the shake
=================
*/
void V_ApplyShake( float* origin, float* angles, float factor )
{
	// TODO: Implement
}


//============================================================================

/*
=============
V_Init
=============
*/
void V_Init( void )
{
	Cmd_AddCommand("centerview", V_StartPitchDrift);

	Cvar_RegisterVariable(&lcd_x);
	Cvar_RegisterVariable(&lcd_yaw);

	Cvar_RegisterVariable(&v_centermove);
	Cvar_RegisterVariable(&v_centerspeed);

	// TODO: Implement

	Cvar_RegisterVariable(&scr_ofsx);
	Cvar_RegisterVariable(&scr_ofsy);
	Cvar_RegisterVariable(&scr_ofsz);
	Cvar_RegisterVariable(&cl_rollspeed);
	Cvar_RegisterVariable(&cl_rollangle);
	Cvar_RegisterVariable(&cl_bob);
	Cvar_RegisterVariable(&cl_bobcycle);
	Cvar_RegisterVariable(&cl_bobup);

	// TODO: Implement
}

/*
=================
V_InitLevel

Initialize sceen fade/shake data
=================
*/
void V_InitLevel( void )
{
	// TODO: Implement
}