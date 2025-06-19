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

CenterPrint ()
SlowPrint ()
Screen_Update ();
Con_Printf ();

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

int			glx, gly, glwidth, glheight;

// only the refresh window will be updated unless these variables are flagged 
int			scr_copytop;
int			scr_copyeverything;

float		scr_con_current;
float		scr_conlines;		// lines of console to display

cvar_t		scr_viewsize = { "viewsize", "120", TRUE };
float		scr_fov_value = 90;	// 10 - 170
cvar_t		scr_conspeed = { "scr_conspeed", "600" };
cvar_t		scr_centertime = { "scr_centertime", "2" };
cvar_t		scr_showram = { "showram", "0" };
cvar_t		scr_showpause = { "showpause", "1" };
cvar_t		scr_printspeed = { "scr_printspeed", "8" };
cvar_t		scr_netusage = { "netusage", "0" };
cvar_t		scr_graphheight = { "graphheight", "64.0" };
cvar_t		scr_graphmedian = { "graphmedian", "128.0" };
cvar_t		scr_graphhigh = { "graphhigh", "512.0" };
cvar_t		scr_graphmean = { "graphmean", "1" };
cvar_t		scr_downloading = { "scr_downloading", "-1.0" };

qboolean	scr_initialized;		// ready to draw

//qpic_t* scr_ram;
//qpic_t* scr_net;
qpic_t* scr_paused;

int			scr_fullupdate;

int			clearconsole;
int			clearnotify;

int			sb_lines;

viddef_t	vid;				// global video state

vrect_t		scr_vrect;

qboolean	scr_disabled_for_loading;
qboolean	scr_drawloading;
float		scr_disabled_time;

void SCR_ScreenShot_f( void );

/*
===============================================================================

CENTER PRINTING

===============================================================================
*/

char            scr_centerstring[1024];
float           scr_centertime_start;   // for slow victory printing
float           scr_centertime_off;
int             scr_center_lines;
int             scr_erase_lines;
int             scr_erase_center;

/*
=================
SCR_CenterPrint

Called for important messages that should stay in the center of the screen
for a few moments
=================
*/
void SCR_CenterPrint( char* str )
{
	strncpy(scr_centerstring, str, sizeof(scr_centerstring) - 1);
	scr_centertime_off = scr_centertime.value;
	scr_centertime_start = cl.time;

// count the number of lines for centering
	scr_center_lines = 1;
	while (*str)
	{
		if (*str == '\n')
			scr_center_lines++;
		str++;
	}
}


void SCR_DrawCenterString( void )
{
	char	*start;
	int		l;
	int		j;
	int		x, y;
	int		remaining;

// the finale prints the characters one at a time
	if (cl.intermission)
		remaining = scr_printspeed.value * (cl.time - scr_centertime_start);
	else
		remaining = 9999;

	scr_erase_center = 0;
	start = scr_centerstring;

	if (scr_center_lines <= 4)
		y = glheight * 0.35;
	else
		y = 48;

	do
	{
	// scan the width of the line
		for (l = 0; l < 40; l++)
			if (start[l] == '\n' || !start[l])
				break;
		x = (glwidth - l * 8) / 2;
		for (j = 0; j < l; j++, x += 8)
		{
			Draw_Character(x, y, start[j]);
			if (!remaining--)
				return;
		}

		y += 12;

		while (*start && *start != '\n')
			start++;

		if (!*start)
			break;
		start++;		// skip the \n
	} while (1);
}

void SCR_CheckDrawCenterString( void )
{
	scr_copytop = 1;
	if (scr_center_lines > scr_erase_lines)
		scr_erase_lines = scr_center_lines;

	scr_centertime_off -= host_frametime;

	if (scr_centertime_off <= 0 && !cl.intermission)
		return;
	if (key_dest != key_game)
		return;

	SCR_DrawCenterString();
}

//=============================================================================

/*
=================
SCR_CalcRefdef

Must be called whenever vid changes
Internal use only
=================
*/
static void SCR_CalcRefdef( void )
{
	float           size;
	int             h;

	scr_fullupdate = 0;             // force a background redraw
	vid.recalc_refdef = 0;

	scr_viewsize.value = 120.0;

// bound field of view
	if (scr_fov_value < 10.0)
		scr_fov_value = 10.0;
	if (scr_fov_value > 170.0)
		scr_fov_value = 170.0;

// intermission is always full screen
	if (cl.intermission)
		size = 100.0;
	else
		size = 100.0;

	sb_lines = 0;			// no status bar at all

	if (cl.intermission)
	{
		size = 100.0;
		sb_lines = 0;
	}
	size /= 100.0;

	h = glheight - sb_lines;

	r_refdef.vrect.width = glwidth * size;
	if (r_refdef.vrect.width < 96)
	{
		size = 96.0 / r_refdef.vrect.width;
		r_refdef.vrect.width = 96;		// min for icons
	}

	r_refdef.vrect.height = glheight * size;
	if (r_refdef.vrect.height > glheight)
		r_refdef.vrect.height = glheight;

	r_refdef.vrect.x = (glwidth - r_refdef.vrect.width) / 2;
	r_refdef.vrect.y = (h - r_refdef.vrect.height) / 2;

	scr_vrect = r_refdef.vrect;
}


/*
=================
SCR_SizeUp_f

Keybinding command
=================
*/
void SCR_SizeUp_f( void )
{
	HudSizeUp();
	vid.recalc_refdef = 1;
}


/*
=================
SCR_SizeDown_f

Keybinding command
=================
*/
void SCR_SizeDown_f( void )
{
	HudSizeDown();
	vid.recalc_refdef = 1;
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
	Cvar_RegisterVariable(&scr_showpause);
	Cvar_RegisterVariable(&scr_conspeed);
	Cvar_RegisterVariable(&scr_centertime);
	Cvar_RegisterVariable(&scr_printspeed);
	Cvar_RegisterVariable(&scr_showram);
	Cvar_RegisterVariable(&scr_netusage);
	Cvar_RegisterVariable(&scr_graphheight);
	Cvar_RegisterVariable(&scr_graphmedian);
	Cvar_RegisterVariable(&scr_graphhigh);
	Cvar_RegisterVariable(&scr_graphmean);

	NET_InitColors();

//
// register our commands
//
	Cmd_AddCommand("screenshot", SCR_ScreenShot_f);
	Cmd_AddCommand("sizeup", SCR_SizeUp_f);
	Cmd_AddCommand("sizedown", SCR_SizeDown_f);

	scr_paused = Draw_PicFromWad("paused");

	scr_initialized = TRUE;
}

/*
==============
Draw_CenterPic
==============
*/
void Draw_CenterPic( qpic_t* pPic )
{
	// set to projection matrix
	qglMatrixMode(GL_PROJECTION);
	qglPushMatrix();
	qglLoadIdentity();
	qglOrtho(0, 640, 480, 0, -99999, 99999);

	qglMatrixMode(GL_MODELVIEW);
	qglPushMatrix();
	qglLoadIdentity();

	qglDisable(GL_DEPTH_TEST);
	qglDisable(GL_BLEND);
	qglEnable(GL_ALPHA_TEST);

	qglColor4f(1, 1, 1, 1);

	Draw_Pic(320 - pPic->width / 2, 240 - pPic->height / 2, pPic);

	qglPopMatrix();
	qglMatrixMode(GL_PROJECTION);
	qglPopMatrix();
}

/*
==============
SCR_DrawNet
==============
*/
void SCR_DrawNet( void )
{
}

/*
==============
DrawPause
==============
*/
void SCR_DrawPause( void )
{
	if (!scr_showpause.value)		// turn off for screenshots
		return;

	if (!cl.paused)
		return;

	Draw_CenterPic(scr_paused);
}



/*
==============
SCR_DrawLoading
==============
*/
void SCR_DrawLoading( void )
{
	if (!scr_drawloading)
		return;

	if (!draw_disc)
		return;

	Draw_CenterPic(draw_disc);
}


//=============================================================================


/*
==================
SCR_SetUpToDrawConsole
==================
*/
void SCR_SetUpToDrawConsole( void )
{
	Con_CheckResize();

	if (scr_drawloading)
		return;		// never a console with loading plaque

// decide on the height of the console
	con_forcedup = !cl.worldmodel || cls.signon != SIGNONS;

	if (con_forcedup)
	{
		scr_conlines = vid.height;		// full screen
		scr_con_current = vid.height;
	}
	else if (key_dest == key_console)
		scr_conlines = vid.height / 2;	// half screen
	else
		scr_conlines = 0;				// none visible

	if (scr_conlines < scr_con_current)
	{
		scr_con_current -= scr_conspeed.value * host_frametime;
		if (scr_conlines > scr_con_current)
			scr_con_current = scr_conlines;

	}
	else if (scr_conlines > scr_con_current)
	{
		scr_con_current += scr_conspeed.value * host_frametime;
		if (scr_conlines < scr_con_current)
			scr_con_current = scr_conlines;
	}

	if (clearconsole++ < vid.numpages)
	{
	}
	else if (clearnotify++ < vid.numpages)
	{
	}
	else
		con_notifylines = 0;
}

/*
==================
SCR_DrawConsole
==================
*/
void SCR_DrawConsole( void )
{
	Con_DrawNotify();
}


/*
==============================================================================

						SCREEN SHOTS

==============================================================================
*/

typedef struct _TargaHeader {
	unsigned char 	id_length, colormap_type, image_type;
	unsigned short	colormap_index, colormap_length;
	unsigned char	colormap_size;
	unsigned short	x_origin, y_origin, width, height;
	unsigned char	pixel_size, attributes;
} TargaHeader;


/*
==================
SCR_ScreenShot_f
==================
*/
void SCR_ScreenShot_f( void )
{
	// TODO: Implement
}


//=============================================================================


/*
===============
SCR_BeginLoadingPlaque

================
*/
void SCR_BeginLoadingPlaque( void )
{
	S_StopAllSounds(TRUE);

	if (cls.state != ca_connected && cls.state != ca_uninitialized && cls.state != ca_active)
		return;
	if (cls.signon != SIGNONS)
		return;
// redraw with no console and the loading plaque
	Con_ClearNotify();
	scr_centertime_off = 0;
	scr_con_current = 0;

	scr_drawloading = TRUE;
	scr_fullupdate = 0;
	SCR_UpdateScreen();
	SCR_UpdateScreen();

	scr_disabled_for_loading = TRUE;
	scr_disabled_time = realtime;
	scr_fullupdate = 0;
}

/*
===============
SCR_EndLoadingPlaque

================
*/
void SCR_EndLoadingPlaque( void )
{
	scr_disabled_for_loading = FALSE;
	scr_fullupdate = 0;
	scr_drawloading = FALSE;
	Con_ClearNotify();
}

//=============================================================================

char* scr_notifystring;
qboolean	scr_drawdialog;

void SCR_DrawNotifyString( void )
{
	char* start;
	int		l;
	int		j;
	int		x, y;

	start = scr_notifystring;

	y = glheight * 0.35;

	do
	{
	// scan the width of the line
		for (l = 0; l < 40; l++)
			if (start[l] == '\n' || !start[l])
				break;
		x = (glwidth - l * 8) / 2;
		for (j = 0; j < l; j++, x += 8)
			Draw_Character(x, y, start[j]);

		y += 8;

		while (*start && *start != '\n')
			start++;

		if (!*start)
			break;
		start++;		// skip the \n
	} while (1);
}

/*
==================
SCR_ModalMessage

Displays a text string in the center of the screen and waits for a Y or N
keypress.
==================
*/
int SCR_ModalMessage( char* text )
{
	if (cls.state == ca_dedicated)
		return TRUE;

	scr_notifystring = text;

// draw a fresh screen
	scr_fullupdate = 0;
	scr_drawdialog = TRUE;
	scr_notifystring = text;
	SCR_UpdateScreen();
	scr_drawdialog = FALSE;

	S_ClearBuffer();		// so dma doesn't loop current sound

	do
	{
		key_count = -1;		// wait for a key down and up
		Sys_SendKeyEvents();
	} while (key_lastpress != 'y' && key_lastpress != 'n' && key_lastpress != K_ESCAPE);

	scr_fullupdate = 0;
	SCR_UpdateScreen();

	return key_lastpress == 'y';
}


//=============================================================================

/*
===============
SCR_BringDownConsole

Brings the console down and fades the palettes back to normal
================
*/
void SCR_BringDownConsole( void )
{
	int		i;

	scr_centertime_off = 0;

	for (i = 0; i < 20 && scr_conlines != scr_con_current; i++)
		SCR_UpdateScreen();
}

void SCR_TileClear( void )
{
	if (r_refdef.vrect.x > 0)
	{
		Draw_TileClear(0, 0, r_refdef.vrect.x, 152);
		Draw_TileClear(r_refdef.vrect.x + r_refdef.vrect.width, 0, r_refdef.vrect.width - r_refdef.vrect.x + 320, 152);
	}

	if (r_refdef.vrect.height < 152)
	{
		Draw_TileClear(r_refdef.vrect.x, 0, r_refdef.vrect.width, r_refdef.vrect.y);
		Draw_TileClear(r_refdef.vrect.x, r_refdef.vrect.height + r_refdef.vrect.y,
			r_refdef.vrect.width,
			152 - (r_refdef.vrect.height + r_refdef.vrect.y));
	}
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
	static qboolean recursionGuard = FALSE;

	// Always force the Gamma Table to be rebuilt. Otherwise,
	// we'll load textures with an all white gamma lookup table.
	V_UpdatePalette();

	if (gfBackground)
		return;

	if (scr_skipupdate)
		return;

	if (recursionGuard)
	{
		recursionGuard = FALSE;
		return;
	}

	scr_copytop = 0;
	scr_copyeverything = 0;

	if (scr_disabled_for_loading)
	{
		if (realtime - scr_disabled_time <= 60)
			return;

		scr_disabled_for_loading = FALSE;
		Con_Printf("load failed.\n");
	}

	// No screen refresh on dedicated servers
	if (cls.state == ca_dedicated)
		return;

	if (!scr_initialized || !con_initialized)
		return;				// not initialized yet

	GL_BeginRendering(&glx, &gly, &glwidth, &glheight);

	//
	// determine size of refresh window
	//
	if (vid.recalc_refdef)
		SCR_CalcRefdef();

//
// do 3D refresh drawing, and then update the screen
//
	SCR_SetUpToDrawConsole();

	// Draw world, etc.
	V_RenderView();

	GLBeginHud();

	//
	// draw any areas not covered by the refresh
	//
	SCR_TileClear();

	if (scr_drawdialog)
	{
		Sbar_Draw();
		Draw_FadeScreen();
		SCR_DrawNotifyString();
		scr_copyeverything = TRUE;
	}
	else if (scr_drawloading)
	{
		SCR_DrawLoading();
		Sbar_Draw();
	}
	else if (cl.intermission == 1 && key_dest == key_game)
	{
		ClientDLL_HudRedraw(1);
	}
	else if (cl.intermission == 2 && key_dest == key_game)
	{
		SCR_CheckDrawCenterString();
	}
	else
	{
		GL_Bind(r_notexture_mip->gl_texturenum);

		if (vid.height > scr_con_current)
			Sbar_Draw();

		SCR_DrawNet();
		SCR_DrawPause();
		SCR_CheckDrawCenterString();
		Con_DrawNotify();
	}

	// TODO: Implement
	
	if (r_netgraph.value)
		R_NetGraph();

	// TODO: Implement

	GLFinishHud();

	GL_EndRendering();
}

/*
================
D_FillRect
================
*/
void D_FillRect( vrect_t* r, byte* color )
{
	qglDisable(GL_TEXTURE_2D);
	qglEnable(GL_BLEND);
	qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	qglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	qglEnable(GL_ALPHA_TEST);

	qglColor4f(color[0] / 255.0, color[1] / 255.0, color[2] / 255.0, 1.0);

	qglDisable(GL_DEPTH_TEST);

	qglBegin(GL_QUADS);
	qglVertex2f(r->x, r->y);
	qglVertex2f(r->x + r->width, r->y);
	qglVertex2f(r->x + r->width, r->y + r->height);
	qglVertex2f(r->x, r->y + r->height);
	qglEnd();

	qglDisable(GL_ALPHA_TEST);
	qglEnable(GL_DEPTH_TEST);

	qglColor3f(1, 1, 1);

	qglEnable(GL_TEXTURE_2D);
	qglDisable(GL_BLEND);
}