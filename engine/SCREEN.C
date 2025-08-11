// screen.c -- master for refresh, status bar, console, chat, notify, etc

#include "quakedef.h"
#include "winquake.h"

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

float		oldscreensize, oldfov;
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
float		downloadpercent = -1.0;

qboolean	scr_initialized;		// ready to draw

qpic_t* scr_ram;
qpic_t* scr_net;
qpic_t* scr_paused;

int			scr_fullupdate;

int			clearconsole;
int			clearnotify;

int			sb_lines;

viddef_t	vid;				// global video state

vrect_t* pconupdate;
vrect_t		scr_vrect;

qboolean	scr_disabled_for_loading;
qboolean	scr_drawloading;
float		scr_disabled_time;
qboolean	scr_skipupdate;

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

void SCR_EraseCenterString( void )
{
	int		y;

	if (scr_erase_center++ > vid.numpages)
	{
		scr_erase_lines = 0;
		return;
	}

	if (scr_center_lines <= 4)
		y = vid.height * 0.35;
	else
		y = 48;

	scr_copytop = 1;
	Draw_TileClear(0, y, vid.width, 8 * scr_erase_lines);
}

void SCR_DrawCenterString( void )
{
	char* start;
	int		l;
	int		j;
	int		x, y;
	int		remaining;
	char	string[80];

// the finale prints the characters one at a time
	if (cl.intermission)
		remaining = scr_printspeed.value * (cl.time - scr_centertime_start);
	else
		remaining = 9999;

	scr_erase_center = 0;
	start = scr_centerstring;

	if (scr_center_lines <= 4)
		y = vid.height * 0.35;
	else
		y = 48;

	do
	{
	// scan the width of the line
		for (l = 0; l < 40; l++)
			if (start[l] == '\n' || !start[l])
				break;
		strncpy(string, start, l);
		string[l] = 0;
		x = (vid.width - Draw_StringLen(string)) / 2;
		for (j = 0; j < l; j++)
		{
			x += Draw_Character(x, y, start[j]);
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
	vrect_t		vrect;
	float		size;

	scr_fullupdate = 0;		// force a background redraw
	vid.recalc_refdef = 0;

//========================================

// bound viewsize
	if (scr_viewsize.value < 30)
		Cvar_Set("viewsize", "30");
	if (scr_viewsize.value > 120)
		Cvar_Set("viewsize", "120");

// bound field of view
	if (scr_fov_value < 10)
		scr_fov_value = 10;
	if (scr_fov_value > 170)
		scr_fov_value = 170;

// intermission is always full screen	
	if (cl.intermission)
		size = 120;
	else
		size = scr_viewsize.value;

	if (size >= 120)
		sb_lines = 0;		// no status bar at all
	else if (size >= 110)
		sb_lines = 24;		// no inventory
	else
		sb_lines = 24 + 16 + 8;

// these calculations mirror those in R_Init() for r_refdef, but take no
// account of water warping
	vrect.x = 0;
	vrect.y = 0;
	vrect.width = vid.width;
	vrect.height = vid.height;

	R_SetVrect(&vrect, &scr_vrect, sb_lines);

// guard against going from one mode to another that's less than half the
// vertical resolution
	if (scr_con_current > (float)vid.height)
		scr_con_current = (float)vid.height;

	R_ViewChanged(&vrect, sb_lines, vid.aspect);
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
	Cmd_AddCommand("screenshot", SCR_ScreenShot_f);
	Cmd_AddCommand("sizeup", SCR_SizeUp_f);
	Cmd_AddCommand("sizedown", SCR_SizeDown_f);

	scr_ram = Draw_PicFromWad("lambda");
	scr_net = Draw_PicFromWad("lambda");
	scr_paused = Draw_PicFromWad("paused");

	scr_initialized = TRUE;
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
	qpic_t* pic;

	if (!scr_showpause.value)		// turn off for screenshots
		return;

	if (!cl.paused)
		return;

	pic = Draw_PicFromWad("paused");
	Draw_TransPic((vid.width - pic->width) / 2,
		(vid.height - pic->height - 48) / 2, pic);
}



/*
==============
SCR_DrawLoading
==============
*/
void SCR_DrawLoading( void )
{
	qpic_t* pic;

	if (!scr_drawloading)
		return;

	pic = Draw_PicFromWad("lambda");
	Draw_TransPic((vid.width - pic->width) / 2,
		(vid.height - pic->height - 48) / 2, pic);
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
		scr_copytop = 1;
		Draw_TileClear(0, (int)scr_con_current, vid.width, vid.height - scr_con_current);
	}
	else if (clearnotify++ < vid.numpages)
	{
		scr_copytop = 1;
		Draw_TileClear(0, 0, vid.width, con_notifylines);
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
	if (scr_con_current)
	{
		scr_copyeverything = 1;
		Con_DrawConsole(scr_con_current, TRUE);
		clearconsole = 0;
	}
	else
	{
		if (key_dest == key_game || key_dest == key_message)
			Con_DrawNotify();	// only draw notify in game
	}
}


/*
==============================================================================

						SCREEN SHOTS

==============================================================================
*/


typedef struct
{
	char	manufacturer;
	char	version;
	char	encoding;
	char	bits_per_pixel;
	unsigned short	xmin, ymin, xmax, ymax;
	unsigned short	hres, vres;
	unsigned char	palette[48];
	char	reserved;
	char	color_planes;
	unsigned short	bytes_per_line;
	unsigned short	palette_type;
	char	filler[58];
	unsigned char	data;			// unbounded
} pcx_t;

/*
==============
WritePCXfile
==============
*/
void WritePCXfile( char* filename, byte* data, int width, int height,
	int rowbytes, word* palette )
{
	int		i, j, length;
	pcx_t* pcx;
	byte* pack;
	byte* pal;

	pcx = (pcx_t*)Hunk_TempAlloc(width * height * 2 + 1000);
	if (pcx == NULL)
	{
		Con_Printf("SCR_ScreenShot_f: not enough memory\n");
		return;
	}

	pcx->manufacturer = 0x0A;	// PCX id
	pcx->version = 5;			// 256 color
	pcx->encoding = 1;		// uncompressed
	pcx->bits_per_pixel = 8;		// 256 color
	pcx->xmin = 0;
	pcx->ymin = 0;
	pcx->xmax = LittleShort((short)(width - 1));
	pcx->ymax = LittleShort((short)(height - 1));
	pcx->hres = LittleShort((short)width);
	pcx->vres = LittleShort((short)height);
	Q_memset(pcx->palette, 0, sizeof(pcx->palette));
	pcx->color_planes = 1;		// chunky image
	pcx->bytes_per_line = LittleShort((short)width);
	pcx->palette_type = LittleShort(2);		// not a grey scale
	Q_memset(pcx->filler, 0, sizeof(pcx->filler));

// pack the image
	pack = &pcx->data;

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			if ((*data & 0xC0) != 0xC0)
				*pack++ = *data++;
			else
			{
				*pack++ = 0xC1;
				*pack++ = *data++;
			}
		}

		data += rowbytes - width;
	}

// write the palette
	*pack++ = 0x0C;	// palette ID byte
	pal = (byte*)palette;
	for (i = 0; i < 256; i++)
	{
		pack[0] = pal[2];
		pack[1] = pal[1];
		pack[2] = pal[0];
		pack += 3;
		pal += 8;
	}

// write output file 
	length = pack - (byte*)pcx;
	COM_WriteFile(filename, pcx, length);
}



/*
==================
SCR_ScreenShot_f
==================
*/
void SCR_ScreenShot_f( void )
{
	int     i;
	char		pcxname[80];
	char		checkname[MAX_OSPATH];

// 
// find a file name to save it to 
// 
	strcpy(pcxname, "HalfLife.pcx");

	for (i = 0; i <= 99; i++)
	{
		pcxname[5] = i / 10 + '0';
		pcxname[6] = i % 10 + '0';
		sprintf(checkname, "%s/%s", com_gamedir, pcxname);
		if (Sys_FileTime(checkname) == -1)
			break;	// file doesn't exist
	}

	if (i == 100)
	{
		Con_Printf("SCR_ScreenShot_f: Couldn't create a PCX file\n");
		return;
	}

// 
// save the pcx file 
// 
	D_EnableBackBufferAccess();	// enable direct drawing of console to back
								//  buffer

	WritePCXfile(pcxname, vid.buffer, vid.width, vid.height, vid.rowbytes,
		host_basepal);

	D_DisableBackBufferAccess();	// for adapters that can't stay mapped in
									//  for linear writes all the time

	Con_Printf("Wrote %s\n", pcxname);
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

	if (!(cls.state == ca_connected || cls.state == ca_uninitialized || cls.state == ca_active))
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

	y = vid.height * 0.35;

	do
	{
	// scan the width of the line
		for (l = 0; l < 40; l++)
			if (start[l] == '\n' || !start[l])
				break;
		x = (vid.width - l * 8) / 2;
		for (j = 0; j < l; j++)
			x += Draw_Character(x, y, start[j]);

		y += 12;

		while (*start && *start != '\n')
			start++;

		if (!*start)
			return;
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

	static float	oldscr_viewsize;
	static float	oldlcd_x;
	vrect_t		vrect;

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
		if (realtime - scr_disabled_time > 60)
		{
			scr_disabled_for_loading = FALSE;
			Con_Printf("load failed.\n");
		}
		else
			return;
	}

	if (cls.state == ca_dedicated)
		return;				// stdout only

	if (!scr_initialized || !con_initialized)
		return;				// not initialized yet

	if (scr_viewsize.value != oldscr_viewsize)
	{
		oldscr_viewsize = scr_viewsize.value;
		vid.recalc_refdef = 1;
	}

//
// check for vid changes
//
	if (oldfov != scr_fov_value)
	{
		oldfov = scr_fov_value;
		vid.recalc_refdef = TRUE;
	}

	if (oldlcd_x != lcd_x.value)
	{
		vid.recalc_refdef = 1;
		oldlcd_x = lcd_x.value;
	}

	if (oldscreensize != scr_viewsize.value)
	{
		oldscreensize = scr_viewsize.value;
		vid.recalc_refdef = TRUE;
	}

	if (vid.recalc_refdef)
	{
	// something changed, so reorder the screen
		SCR_CalcRefdef();
	}

//
// do 3D refresh drawing, and then update the screen
//
	D_EnableBackBufferAccess();	// of all overlay stuff if drawing directly


	if (scr_fullupdate++ < vid.numpages)
	{	// clear the entire screen
		scr_copyeverything = 1;
		Draw_TileClear(0, 0, vid.width, vid.height);
	}

	pconupdate = NULL;


	SCR_SetUpToDrawConsole();
	SCR_EraseCenterString();

	D_DisableBackBufferAccess();	// for adapters that can't stay mapped in
									//  for linear writes all the time

	VID_LockBuffer();

	V_RenderView();

	VID_UnlockBuffer();

	D_EnableBackBufferAccess();	// of all overlay stuff if drawing directly

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
	else if (cl.intermission == 3 && key_dest == key_game)
	{
		SCR_CheckDrawCenterString();
	}
	else
	{
		if ((float)vid.height > scr_con_current && cls.state == ca_active)
			Sbar_Draw();

		SCR_DrawNet();
		SCR_DrawPause();
		SCR_CheckDrawCenterString();
		SCR_DrawConsole();
	}

	if (scr_netusage.value)
	{
		if ((float)vid.height > scr_con_current)
			SCR_NetUsage();
	}

	if (r_netgraph.value)
		R_NetGraph();

	CL_ShowSizes();

	SCR_DrawDownloadInfo();
	SCR_DrawDownloadProgress();

	D_DisableBackBufferAccess();	// for adapters that can't stay mapped in
									//  for linear writes all the time
	if (pconupdate)
	{
		D_UpdateRects(pconupdate);
	}

//
// update one of three areas
//

	if (scr_copyeverything)
	{
		vrect.x = 0;
		vrect.y = 0;
		vrect.width = vid.width;
		vrect.height = vid.height;
		vrect.pnext = NULL;

		VID_Update(&vrect);
	}
	else if (scr_copytop)
	{
		vrect.x = 0;
		vrect.y = 0;
		vrect.width = vid.width;
		vrect.height = vid.height - sb_lines;
		vrect.pnext = NULL;

		VID_Update(&vrect);
	}
	else
	{
		vrect.x = scr_vrect.x;
		vrect.y = scr_vrect.y;
		vrect.width = scr_vrect.width;
		vrect.height = scr_vrect.height;
		vrect.pnext = NULL;

		VID_Update(&vrect);
	}
}


/*
==================
SCR_UpdateWholeScreen
==================
*/
void SCR_UpdateWholeScreen( void )
{
	scr_fullupdate = 0;
	SCR_UpdateScreen();
}

/*
=================
SCR_DrawDownloadText

=================
*/
void SCR_DrawDownloadText( void )
{
	char	szStatusText[128];
	int		i;
	int		w, h, x, y;
	int		recieved;
	int		bytes = 0;
	float	speed = 0.0;
	float	time = 0.0;
	float	remaining;
	vrect_t rcFill;
	byte	color[3];
	downloadtime_t* dt1;
	downloadtime_t* dt2;

	recieved = cls.nTotalToTransfer - cls.nRemainingToTransfer;

	for (i = 0; i < MAX_DL_STATS; i++)
	{
		dt1 = &cls.rgDownloads[(cls.downloadnumber - i - 2) & (MAX_DL_STATS - 1)];
		dt2 = &cls.rgDownloads[(cls.downloadnumber - i - 1) & (MAX_DL_STATS - 1)];

		if (dt1->bUsed && dt2->bUsed && dt2->fTime >= dt1->fTime)
		{
			bytes += dt1->nBytesRemaining - dt2->nBytesRemaining;
			time += dt2->fTime - dt1->fTime;
		}
	}

	if (time != 0)
		speed = bytes / time;

	if (speed != 0)
	{
		w = min(scr_vrect.width - 2, 250);
		h = min(scr_vrect.height - 2, 10);

		x = scr_vrect.x + (scr_vrect.width - w) / 2 + 1;
		y = scr_vrect.y + scr_vrect.height - 12;
		if (cls.state != ca_active)
			y = scr_vrect.y + scr_vrect.height - 92;
		y = max(y, 22);

		rcFill.x = x;
		rcFill.y = y;
		rcFill.width = w;
		rcFill.height = h;

		color[0] = color[1] = color[2] = 0;
		D_FillRect(&rcFill, color);

		remaining = cls.nRemainingToTransfer / speed;
		sprintf(szStatusText, "%iK received, %i seconds remaining...\n", recieved / 1024, (int)remaining);
		Draw_String(x, y, szStatusText);
	}
}

/*
=================
SCR_DrawDownloadInfo

=================
*/
void SCR_DrawDownloadInfo( void )
{
	int		percent;
	int		w, h, x, y;
	float	progress;
	vrect_t rcFill;
	byte	color[3];

	if (scr_downloading.value < 0)
		return;

	if (!cls.download)
	{
		scr_downloading.value = -1.0;
		return;
	}

	percent = (int)scr_downloading.value;
	percent = min(100, max(0, percent));

	w = min(scr_vrect.width - 2, 250);
	h = min(scr_vrect.height - 2, 10);

	x = scr_vrect.x + (scr_vrect.width - w) / 2 + 1;
	y = scr_vrect.y + (scr_vrect.height - 22);
	if (cls.state != ca_active)
		y = scr_vrect.y + scr_vrect.height - 102;
	y = max(y, 2);

	// Background
	rcFill.x = x;
	rcFill.y = y;
	rcFill.width = w;
	rcFill.height = h;

	color[0] = 63;
	color[1] = 63;
	color[2] = 63;
	D_FillRect(&rcFill, color);

	// Progress bar
	progress = percent / 100.0;
	rcFill.x += 2;
	rcFill.y += 2;
	rcFill.width = progress * (w - 4) + 0.5;
	rcFill.height -= 4;

	color[0] = 127;
	color[1] = 63;
	color[2] = 0;
	D_FillRect(&rcFill, color);

	// Remaining space
	rcFill.x += rcFill.width;
	rcFill.width = w - rcFill.width - 4;

	color[0] = 0;
	color[1] = 0;
	color[2] = 0;
	D_FillRect(&rcFill, color);

	// Draw text info
	SCR_DrawDownloadText();
}

/*
=================
SCR_DrawDownloadProgress

=================
*/
void SCR_DrawDownloadProgress( void )
{
	int		percent;
	int		offset;
	int		w, h, x, y;
	float	scale;
	float	progress;
	vrect_t rcFill;
	byte	color[3];

	if (scr_disabled_for_loading || downloadpercent < 0)
		return;

	if (cls.state != ca_connected && cls.state != ca_uninitialized)
	{
		downloadpercent = -1.0;
		return;
	}

	percent = (int)downloadpercent;
	percent = min(100, max(0, percent));

	scale = scr_vrect.height / 240.0;
	if (scale < 1.0)
		scale = 1.0;

	w = scr_vrect.width / 2;
	h = scale * 8;

	offset = h / 4;
	offset = max(offset, 2);

	x = scr_vrect.x + w / 2;
	y = scr_vrect.height * 0.6;

	// Background
	rcFill.x = x;
	rcFill.y = y;
	rcFill.width = w;
	rcFill.height = h;

	color[0] = color[1] = color[2] = 0;
	D_FillRect(&rcFill, color);

	// Progress bar
	progress = percent / 100.0;
	rcFill.x += offset;
	rcFill.y += offset;
	rcFill.height -= 2 * offset;
	rcFill.width = progress * (w - 2 * offset) + 0.5;

	color[0] = 255;
	color[1] = 180;
	color[2] = 56;
	D_FillRect(&rcFill, color);
}