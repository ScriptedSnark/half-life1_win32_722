// HUD.c - heads up display code

#include "quakedef.h"
#include "screen.h"

int			giHudLevel = 1;

extern cvar_t crosshair;
extern vrect_t scr_vrect;

void DrawCrosshair( int x, int y );

void HudSizeUp( void )
{
	if (scr_viewsize.value < 120)
	{
		Cvar_SetValue("viewsize", scr_viewsize.value + 10);
		return;
	}

	giHudLevel--;
	if (giHudLevel < 0)
		giHudLevel = 0;
}

void HudSizeDown( void )
{
	giHudLevel++;
	if (giHudLevel > 3)
	{
		giHudLevel = 3;
		Cvar_SetValue("viewsize", scr_viewsize.value - 10);
	}
}


/*
===============
Sbar_Draw
===============
*/
void Sbar_Draw( void )
{
	float x, y;
	vec3_t angles;
	vec3_t forward;
	vec3_t point, screen;

	if (giHudLevel == 0)
		return;

	if (scr_con_current == vid.height)
		return; // console is full screen

	scr_copyeverything = TRUE;

	if (crosshair.value)
	{
		x = scr_vrect.x + (scr_vrect.width / 2);
		y = scr_vrect.y + (scr_vrect.height / 2);

		VectorAdd(r_refdef.viewangles, cl.crosshairangle, angles);
		AngleVectors(angles, forward, NULL, NULL);
		VectorAdd(r_origin, forward, point);

		ScreenTransform(point, screen);

#if defined( GLQUAKE )
		DrawCrosshair(0.5 * screen[0] * scr_vrect.width + x + 0.5, 0.5 * screen[1] * scr_vrect.height + y + 0.5);
#else
		DrawCrosshair(xscale * screen[0] + x + 0.5, yscale * screen[1] + y + 0.5);
#endif
	}

	ClientDLL_HudRedraw(0);
}