// HUD.c - heads up display code

#include "quakedef.h"
#include "screen.h"

int			giHudLevel = 1;

void HudSizeUp( void )
{
	if (scr_viewsize.value < 120.0)
	{
		Cvar_SetValue("viewsize", scr_viewsize.value + 10.0);
		return;
	}

	giHudLevel--;
	if (giHudLevel < 0)
	{
		giHudLevel = 0;
	}
}

void HudSizeDown( void )
{
	giHudLevel++;
	if (giHudLevel > 3)
	{
		giHudLevel = 3;
		Cvar_SetValue("viewsize", scr_viewsize.value - 10.0);
	}
}


void Sbar_Draw( void )
{
	if (giHudLevel == 0)
		return;

	if (scr_con_current == vid.height)
		return; // console is full screen

	scr_copyeverything = TRUE;

	// TODO: Implement

	ClientDLL_HudRedraw(0);
}