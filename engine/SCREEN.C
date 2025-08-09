// screen.c -- master for refresh, status bar, console, chat, notify, etc

#include "quakedef.h"

/*

background clear
rendering
turtle/net/ram icons
sbar
centerprint / slow centerprint
notify lines
intermission / finale overlay
loading plaque
console
menu

required background clears
required update regions


syncronous draw mode or async
One off screen buffer, with updates either copied or xblited
Need to double buffer?


async draw will require the refresh area to be cleared, because it will be
xblited, but sync draw can just ignore it.

sync
draw

CenterPrint()
SlowPrint()
Screen_Update();
Con_Printf();

net
turn off messages option

the refresh is allways rendered, unless the console is full screen


console is:
	notify lines
	half
	full


*/


// In other C files.
qboolean V_CheckGamma( void );

// only the refresh window will be updated unless these variables are flagged 
int			scr_copytop;
int			scr_copyeverything;

float		scr_con_current;
float		scr_conlines;		// lines of console to display

cvar_t		scr_viewsize = { "viewsize", "120", TRUE };
float		scr_fov_value = 90;	// 10 - 170
cvar_t		scr_conspeed = { "scr_conspeed", "600" };
cvar_t		scr_centertime = { "scr_centertime", "2" };
cvar_t		scr_showram = { "showram", "1" };
cvar_t		scr_showpause = { "showpause", "1" };
cvar_t		scr_printspeed = { "scr_printspeed", "8" };
cvar_t		scr_netusage = { "netusage", "0" };
cvar_t		scr_graphheight = { "graphheight", "64.0" };
cvar_t		scr_graphmedian = { "graphmedian", "128.0" };
cvar_t		scr_graphhigh = { "graphhigh", "512.0" };
cvar_t		scr_graphmean = { "graphmean", "1" };
cvar_t		scr_downloading = { "scr_downloading", "-1.0" };
float		downloading = -1.0;

qboolean	scr_initialized;		// ready to draw

qpic_t* scr_ram;
qpic_t* scr_net;
qpic_t* scr_paused;







int			clearconsole;
int			clearnotify;



vrect_t		scr_vrect;

qboolean	scr_disabled_for_loading;
qboolean	scr_skipupdate;






float		scr_centertime_off;




/*
=================
SCR_CenterPrint

Called for important messages that should stay in the center of the screen
for a few moments
=================
*/
void SCR_CenterPrint( char* str )
{
	// TODO: Implement
}


//============================================================================

/*
==================
SCR_Init
==================
*/
void SCR_Init( void )
{
	Cvar_RegisterVariable(&scr_viewsize);
	Cvar_RegisterVariable(&scr_conspeed);
	Cvar_RegisterVariable(&scr_showram);
	Cvar_RegisterVariable(&scr_showpause);
	Cvar_RegisterVariable(&scr_centertime);
	Cvar_RegisterVariable(&scr_printspeed);
	Cvar_RegisterVariable(&scr_netusage);
	Cvar_RegisterVariable(&scr_graphheight);
	Cvar_RegisterVariable(&scr_graphmedian);
	Cvar_RegisterVariable(&scr_graphhigh);
	Cvar_RegisterVariable(&scr_graphmean);

	NET_InitColors();

//
// register our commands
//

	// TODO: Implement

	scr_ram = Draw_PicFromWad("lambda");
	scr_net = Draw_PicFromWad("lambda");
	scr_paused = Draw_PicFromWad("paused");

	scr_initialized = TRUE;
}


/*
===============
SCR_BeginLoadingPlaque

================
*/
void SCR_BeginLoadingPlaque( void )
{
	// TODO: Implement
}

/*
===============
SCR_EndLoadingPlaque

================
*/
void SCR_EndLoadingPlaque( void )
{
	// TODO: Implement
}

/*
==================
SCR_UpdateScreen

This is called every frame, and can also be called explicitly to flush
text to the screen.

WARNING: be very careful calling this from elsewhere, because the refresh
needs almost the entire 256k of stack space!
==================
*/
void SCR_UpdateScreen( void )
{
	// TODO: Implement
}