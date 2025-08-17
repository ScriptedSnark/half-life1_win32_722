// HUD.c - heads up display code

#include "quakedef.h"
#include "screen.h"
#include "cl_tent.h"
#include "pr_cmds.h"

int	gHealth = 100;
double gfFade;
int	gAmmo;
double gfAmmoFade;

int giHudLevel = 1;
int giSelectMode = 1;
int giSelAmmo1 = -1;
int giSelAmmo2 = -1;
int giClipAmmo;
int giTotalAmmo;
int giSecondAmmo;
int giAmmoDigits;
int giHealthWidth;
int giAmmoWidth;
int giGeigerRange = -1;
double gflGeigerDelay = -1.0;

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
Sbar_Geiger
===============
*/
void Sbar_Geiger( void )
{
	int pct;
	float flvol;
	sfx_t* rgsfx[3] = { NULL };
	int i;

	if (cl.time < gflGeigerDelay)
		return;

	gflGeigerDelay = cl.time + 0.1;

	if (giGeigerRange < 1000 && giGeigerRange > 0)
	{
		// piecewise linear is better than continuous formula for this
		if (giGeigerRange > 800)
		{
			pct = 0;			//Con_Printf("range > 800\n");
			flvol = 0.0;
			i = 0;
		}
		else if (giGeigerRange > 600)
		{
			pct = 2;
			flvol = 0.4;		//Con_Printf("range > 600\n");
			rgsfx[0] = cl_sfx_geiger1;
			rgsfx[1] = cl_sfx_geiger1;
			i = 2;
		}
		else if (giGeigerRange > 500)
		{
			pct = 4;
			flvol = 0.5;		//Con_Printf("range > 500\n");
			rgsfx[0] = cl_sfx_geiger1;
			rgsfx[1] = cl_sfx_geiger2;
			i = 2;
		}
		else if (giGeigerRange > 400)
		{
			pct = 8;
			flvol = 0.6;		//Con_Printf("range > 400\n");
			rgsfx[0] = cl_sfx_geiger1;
			rgsfx[1] = cl_sfx_geiger2;
			rgsfx[2] = cl_sfx_geiger3;
			i = 3;
		}
		else if (giGeigerRange > 300)
		{
			pct = 8;
			flvol = 0.7;		//Con_Printf("range > 300\n");
			rgsfx[0] = cl_sfx_geiger2;
			rgsfx[1] = cl_sfx_geiger3;
			rgsfx[2] = cl_sfx_geiger4;
			i = 3;
		}
		else if (giGeigerRange > 200)
		{
			pct = 28;
			flvol = 0.78;		//Con_Printf("range > 200\n");
			rgsfx[0] = cl_sfx_geiger2;
			rgsfx[1] = cl_sfx_geiger3;
			rgsfx[2] = cl_sfx_geiger4;
			i = 3;
		}
		else if (giGeigerRange > 150)
		{
			pct = 40;
			flvol = 0.80;		//Con_Printf("range > 150\n");
			rgsfx[0] = cl_sfx_geiger3;
			rgsfx[1] = cl_sfx_geiger4;
			rgsfx[2] = cl_sfx_geiger5;
			i = 3;
		}
		else if (giGeigerRange > 100)
		{
			pct = 60;
			flvol = 0.85;		//Con_Printf("range > 100\n");
			rgsfx[0] = cl_sfx_geiger3;
			rgsfx[1] = cl_sfx_geiger4;
			rgsfx[2] = cl_sfx_geiger5;
			i = 3;
		}
		else if (giGeigerRange > 75)
		{
			pct = 80;
			flvol = 0.9;		//Con_Printf("range > 75\n");
			//gflGeigerDelay = cl.time + GEIGERDELAY * 0.75;
			rgsfx[0] = cl_sfx_geiger4;
			rgsfx[1] = cl_sfx_geiger5;
			rgsfx[2] = cl_sfx_geiger6;
			i = 3;
		}
		else if (giGeigerRange > 50)
		{
			pct = 90;
			flvol = 0.95;		//Con_Printf("range > 50\n");
			rgsfx[0] = cl_sfx_geiger5;
			rgsfx[1] = cl_sfx_geiger6;
			i = 2;
		}
		else
		{
			pct = 95;
			flvol = 1.0;		//Con_Printf("range < 50\n");
			rgsfx[0] = cl_sfx_geiger5;
			rgsfx[1] = cl_sfx_geiger6;
			i = 2;
		}

		flvol = flvol * RandomFloat(0.0, 0.5) + 0.25;

		if (RandomLong(0, 127) < pct || RandomLong(0, 127) < pct)
		{
			S_StartDynamicSound(-1, CHAN_AUTO, rgsfx[RandomLong(0, i - 1)], r_origin, flvol, 1.0, 0, PITCH_NORM);
		}
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

#if !defined( GLQUAKE )
	FillBackGround();
#endif

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

#if !defined( GLQUAKE )
/*
===============
FillBackGround
===============
*/
void FillBackGround( void )
{
	if (scr_vrect.width == vid.width &&
		scr_vrect.height == vid.height)
		return;

	if (scr_vrect.height != vid.height)
		Draw_Fill(0, scr_vrect.height + scr_vrect.y, vid.width, vid.height - (scr_vrect.height + scr_vrect.y), 0);

	if (scr_vrect.x)
	{
		Draw_Fill(0, 0, scr_vrect.x, scr_vrect.height + scr_vrect.y, 0);
		Draw_Fill(scr_vrect.x + scr_vrect.width, 0, scr_vrect.x, scr_vrect.height + scr_vrect.y, 0);
	}

	if (scr_vrect.height != vid.height)
		Draw_Fill(scr_vrect.x, 0, vid.width, scr_vrect.y, 0);
}
#endif