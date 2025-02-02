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





cvar_t	v_dark = { "v_dark", "0" };





byte		texgammatable[256];	// palette is sent through this to convert to screen gamma
int			lightgammatable[1024];
int			lineargammatable[1024];
int			screengammatable[1024];

#ifdef	GLQUAKE
byte		ramps[3][256];
float		v_blend[4];		// rgba 0.0 - 1.0
#endif	// GLQUAKE


// TODO: Implement


vec3_t	v_forward, v_right, v_up;



// TODO: Implement





/*
===============
V_CalcRoll
Used by view and sv_user
===============
*/
float V_CalcRoll( float* angles, float* velocity )
{
	float	sign;
	float	side;
	float	value;

	AngleVectors(angles, v_forward, v_right, v_up);
	side = DotProduct(velocity, v_right);
	sign = side < 0 ? -1 : 1;
	side = fabs(side);

	value = cl_rollangle.value;

	if (side < cl_rollspeed.value)
		side = side * value / cl_rollspeed.value;
	else
		side = value;

	return side * sign;
}

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

cvar_t v_gamma = { "gamma", "2.5", TRUE };		// monitor gamma
cvar_t v_brightness = { "brightness", "0.0", TRUE };	// low level light adjustment
cvar_t v_lightgamma = { "lightgamma", "2.5" };
cvar_t v_texgamma = { "texgamma", "2.0" };		// source gamma of textures
cvar_t v_lambert = { "lambert", "1.5" };
cvar_t v_direct = { "direct", "0.9" };

void BuildGammaTable( float g )
{
	int		i, inf;
	float	g1, g3;

	if (g <= 0) // prevent division by zero
	{
		g3 = 0.0;
		g1 = g;
	}

	if (g > 3.0)
		g = 3.0;

	g = 1.0 / g;

	g1 = g * v_texgamma.value;

	if (v_brightness.value <= 0.0)
	{
		g3 = 0.125;
	}
	else if (v_brightness.value > 1.0)
	{
		g3 = 0.05;
	}
	else
	{
		g3 = 0.125 - (v_brightness.value * v_brightness.value) * 0.075;
	}

	for (i = 0; i < 256; i++)
	{
		inf = 255 * pow(i / 255.0, g1);
		if (inf < 0)
			inf = 0;
		if (inf > 255)
			inf = 255;
		texgammatable[i] = inf;
	}

	for (i = 0; i < 1024; i++)
	{
		float f;

		f = pow(i / 1023.0, v_lightgamma.value);

		// scale up
		if (v_brightness.value > 1.0)
			f = f * v_brightness.value;

		// shift up
		if (f <= g3)
			f = (f / g3) * 0.125;
		else
			f = 0.125 + ((f - g3) / (1.0 - g3)) * 0.875;

		// convert
		inf = 1023 * pow(f, g);

		if (inf < 0)
			inf = 0;
		if (inf > 1023)
			inf = 1023;
		lightgammatable[i] = inf;
	}

	for (i = 0; i < 1024; i++)
	{
		// convert from screen gamma space to linear space
		lineargammatable[i] = 1023 * pow(i / 1023.0, v_gamma.value);
		// convert from linear gamma space to screen space
		screengammatable[i] = 1023 * pow(i / 1023.0, 1.0 / v_gamma.value);
	}
}

/*
=================
V_CheckGamma

FIXME:  Define this as a change function to the cvar's below rather than polling it
 every frame.  Note, still need to make sure it gets called very first time through frame loop.
=================
*/
qboolean V_CheckGamma( void )
{
	static float oldgammavalue, oldlightgamma, oldtexgamma, oldbrightness;
#if !defined ( GLQUAKE )
	static float ambientr, ambientg, ambientb;
#endif

	if ((v_gamma.value == oldgammavalue) &&
		(v_lightgamma.value == oldlightgamma) &&
		(v_texgamma.value == oldtexgamma) &&
		(v_brightness.value == oldbrightness)
#if !defined ( GLQUAKE )
		&& (r_ambient_r.value == ambientr) &&
		(r_ambient_g.value == ambientg) &&
		(r_ambient_b.value == ambientb)
#endif
		)
	{
		return FALSE;
	}

	oldgammavalue = v_gamma.value;
	oldlightgamma = v_lightgamma.value;
	oldtexgamma = v_texgamma.value;
	oldbrightness = v_brightness.value;

#if !defined ( GLQUAKE )
	ambientr = r_ambient_r.value;
	ambientg = r_ambient_g.value;
	ambientb = r_ambient_b.value;
#endif

	BuildGammaTable(v_gamma.value);

	D_FlushCaches();
	vid.recalc_refdef = 1;				// force a surface cache flush

	return TRUE;
}

/*
=============
V_CalcPowerupCshift
=============
*/
void V_CalcPowerupCshift( void )
{
	// TODO: Implement
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
	qboolean	newFlag;
	qboolean force;

	V_CalcPowerupCshift();

	newFlag = FALSE;

	force = V_CheckGamma();
	if (!newFlag && !force)
		return;

	V_CalcBlend();

	// TODO: Implement
}
#endif


/*
==============================================================================

						VIEW RENDERING

==============================================================================
*/

float angledelta( float a )
{
	a = anglemod(a);
	if (a > 180)
		a -= 360;
	return a;
}


/*
==================
V_CalcGunAngle
==================
*/
void V_CalcGunAngle( void )
{
	// TODO: Implement
}

// TODO: Implement

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
	float		side;

	side = V_CalcRoll(cl_entities[cl.viewentity].angles, cl.simvel);

	r_refdef.viewangles[ROLL] += side;

	// TODO: Implement

	if (cl.stats[STAT_HEALTH] <= 0)
	{
		r_refdef.viewangles[ROLL] = 80;	// dead view angle
		return;
	}
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
	int				i;
	vec3_t			angles;
	vec3_t			forward, right, up;
	float			bob, waterOffset;
	static float oldz = 0;

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

	// offsets
	VectorCopy(cl.viewangles, angles);

	AngleVectors(angles, forward, right, up);

	for (i = 0; i < 3; i++)
		r_refdef.vieworg[i] += scr_ofsx.value * forward[i]
		+ scr_ofsy.value * right[i]
		+ scr_ofsz.value * up[i];


	// TODO: Implement



	// Give gun our viewangles
	VectorCopy(cl.viewangles, cl.viewent.angles);

	// set up gun position
	V_CalcGunAngle();

	// Use predicted origin as view origin.
	VectorCopy(cl.simorg, cl.viewent.origin);
	cl.viewent.origin[2] += (waterOffset + cl.viewheight);

	// Let the viewmodel shake at about 10% of the amplitude
	V_ApplyShake(cl.viewent.origin, cl.viewent.angles, 0.9);

	for (i = 0; i < 3; i++)
	{
		cl.viewent.origin[i] += bob * 0.4 * forward[i];
	}
	cl.viewent.origin[2] += bob;

	// throw in a little tilt.
	cl.viewent.angles[YAW] -= bob * 0.5;
	cl.viewent.angles[ROLL] -= bob * 1;
	cl.viewent.angles[PITCH] -= bob * 0.3;

	// pushing the view origin down off of the same X/Z plane as the ent's origin will give the
	// gun a very nice 'shifting' effect when the player looks up/down. If there is a problem
	// with view model distortion, this may be a cause. (SJB). 
	cl.viewent.origin[2] -= 1;

	// fudge position around to keep amount of weapon visible
	// roughly equal with different FOV
	if (scr_viewsize.value == 110)
	{
		cl.viewent.origin[2] += 1;
	}
	else if (scr_viewsize.value == 100)
	{
		cl.viewent.origin[2] += 2;
	}
	else if (scr_viewsize.value == 90)
	{
		cl.viewent.origin[2] += 1;
	}
	else if (scr_viewsize.value == 80)
	{
		cl.viewent.origin[2] += 0.5;
	}
	
	cl.viewent.model = cl.model_precache[cl.stats[STAT_WEAPON]];
	cl.viewent.frame = 0.0;
	cl.viewent.colormap = (byte*)vid.colormap;
	cl.viewent.index = cl.playernum + 1;

// set up the refresh position
	VectorAdd(r_refdef.viewangles, cl.punchangle, r_refdef.viewangles);

// smooth out stair step ups
	if (cl.onground && (cl.simorg[2] - oldz) > 0)
	{
		float steptime;

		steptime = cl.time - cl.oldtime;
		if (steptime < 0)
//FIXME		I_Error ("steptime < 0");
			steptime = 0;

		oldz += steptime * 80;
		if (oldz > cl.simorg[2])
			oldz = cl.simorg[2];
		if (cl.simorg[2] - oldz > 12)
			oldz = cl.simorg[2] - 12;
		r_refdef.vieworg[2] += oldz - cl.simorg[2];
		cl.viewent.origin[2] += oldz - cl.simorg[2];
	}
	else
		oldz = cl.simorg[2];

	// TODO: Implement

	// override all previous settings if the viewent isn't the client
	if (cl.viewentity > cl.maxclients)
	{
		cl_entity_t* viewentity;

		viewentity = &cl_entities[cl.viewentity];
		VectorCopy(viewentity->origin, r_refdef.vieworg);
		VectorCopy(viewentity->angles, r_refdef.viewangles);
	}
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
			r_refdef.vieworg[i] -= v_right[i] * lcd_x.value;
		R_RenderView();

		vid.buffer += vid.rowbytes >> 1;

		R_PushDlights();

		r_refdef.viewangles[YAW] += lcd_yaw.value * 2;
		for (i = 0; i < 3; i++)
			r_refdef.vieworg[i] += 2 * v_right[i] * lcd_x.value;
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

// Screen shake variables
typedef struct
{
	float time;
	float duration;
	float amplitude;
	float frequency;
	float nextShake;
	vec3_t offset;
	float angle;
	vec3_t appliedOffset;
	float appliedAngle;
} screenshake_t;

screenshake_t gVShake;

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
	if (origin)
		VectorMA(origin, factor, gVShake.appliedOffset, origin);

	if (angles)
		angles[ROLL] += gVShake.appliedAngle * factor;
}

/*
=================
V_ScreenFade

Message hook to parse ScreenFade messages
=================
*/
int V_ScreenFade( const char* pszName, int iSize, void* pbuf )
{
	ScreenFade* pFade = (ScreenFade*)pbuf;

	cl.sf.fadeEnd = pFade->duration * (1.0 / 4096.0);
	cl.sf.fadeReset = pFade->holdTime * (1.0 / 4096.0);

	cl.sf.fader = pFade->r;
	cl.sf.fadeg = pFade->g;
	cl.sf.fadeb = pFade->b;
	cl.sf.fadealpha = pFade->a;

	cl.sf.fadeFlags = pFade->fadeFlags;
	cl.sf.fadeSpeed = 0.0;

	// Calc fade speed
	if (pFade->duration)
	{
		// Fade out (reversed fade in)
		if (pFade->fadeFlags & FFADE_OUT)
		{
			if (cl.sf.fadeEnd)
			{
				cl.sf.fadeSpeed = -(cl.sf.fadealpha / cl.sf.fadeEnd);
			}

			cl.sf.fadeEnd += cl.time;
			cl.sf.fadeReset += cl.sf.fadeEnd;
		}
		else
		{
			if (cl.sf.fadeEnd)
			{
				cl.sf.fadeSpeed = cl.sf.fadealpha / cl.sf.fadeEnd;
			}

			cl.sf.fadeReset += cl.time;
			cl.sf.fadeEnd += cl.sf.fadeReset;
		}
	}

	return 1;
}

/*
=============
V_FadeAlpha

Compute the overall color & alpha of the fades
=============
*/
int V_FadeAlpha( void )
{
	int alpha;

	if (cl.sf.fadeReset < cl.time && cl.sf.fadeEnd < cl.time)
		return 0;

	alpha = (cl.sf.fadeEnd - cl.time) * cl.sf.fadeSpeed;

	if (cl.sf.fadeFlags & FFADE_OUT)
		alpha += cl.sf.fadealpha;

	// clamp it
	if (alpha > cl.sf.fadealpha)
		alpha = cl.sf.fadealpha;
	else if (alpha < 0)
		alpha = 0;

	return alpha;
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
	
	Cvar_RegisterVariable(&v_dark);

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
	
	BuildGammaTable(2.5);

	Cvar_RegisterVariable(&v_gamma);
	Cvar_RegisterVariable(&v_lightgamma);
	Cvar_RegisterVariable(&v_texgamma);
	Cvar_RegisterVariable(&v_brightness);
	Cvar_RegisterVariable(&v_lambert);
	Cvar_RegisterVariable(&v_direct);

	// TODO: Implement
	
	HookServerMsg("ScreenFade", V_ScreenFade);

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
	memset(&gVShake, 0, sizeof(gVShake));

	cl.sf.fadeFlags = 0;
	cl.sf.fader = 0;
	cl.sf.fadeg = 0;
	cl.sf.fadeb = 0;

	if (v_dark.value)
	{
		cl.sf.fadealpha = 255;
		cl.sf.fadeSpeed = 51.0;
		cl.sf.fadeReset = cl.time + 5.0;
		cl.sf.fadeEnd = cl.sf.fadeReset + 5.0;
		v_dark.value = 0.0;
	}
	else
	{
		cl.sf.fadealpha = 0;
		cl.sf.fadeSpeed = 0.0;
		cl.sf.fadeReset = 0.0;
		cl.sf.fadeEnd = 0.0;
	}
}