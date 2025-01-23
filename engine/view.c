// view.c -- player eye positioning

#include "quakedef.h"
#include "r_local.h"

/*

The view is allowed to move slightly from it's true position for bobbing,
but if it exceeds 8 pixels linear distance (spherical, not box), the list of
entities sent from the server may not include everything in the pvs, especially
when crossing a water boudnary.

*/

cvar_t		lcd_x = { "lcd_x", "0" };
cvar_t		lcd_yaw = { "lcd_yaw", "0" };

cvar_t	scr_ofsx = { "scr_ofsx", "0", FALSE };
cvar_t	scr_ofsy = { "scr_ofsy", "0", FALSE };
cvar_t	scr_ofsz = { "scr_ofsz", "0", FALSE };


byte		texgammatable[256];	// palette is sent through this to convert to screen gamma

vec3_t		r_soundOrigin;
vec3_t		r_playerViewportAngles;


// TODO: Implement


/*
===============
V_CalcRoll

Used by view and sv_user
===============
*/
vec3_t	forward, right, up;



// TODO: Implement








void V_StartPitchDrift( void )
{
}

void V_StopPitchDrift( void )
{
}

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



//============================================================================

/*
=============
V_Init
=============
*/
void V_Init( void )
{
	// TODO: Implement

	Cvar_RegisterVariable(&lcd_x);
	Cvar_RegisterVariable(&lcd_yaw);

	// TODO: Implement

	Cvar_RegisterVariable(&scr_ofsx);
	Cvar_RegisterVariable(&scr_ofsy);
	Cvar_RegisterVariable(&scr_ofsz);

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