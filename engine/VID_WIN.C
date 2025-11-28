// vid_win.c -- Win32 video driver

#include "quakedef.h"
#include "d_local.h"

int			window_center_x, window_center_y;
RECT		window_rect;

byte* vid_surfcache;
int			vid_surfcachesize;
static int	VID_highhunkmark;

static HANDLE	hMovieFile = INVALID_HANDLE_VALUE;

#define MODE_WINDOWED			0
#define MODE_SETTABLE_WINDOW	2
#define NO_MODE					(MODE_WINDOWED - 1)
#define MODE_FULLSCREEN_DEFAULT	(MODE_WINDOWED + 3)

// Note that 0 is MODE_WINDOWED
cvar_t		vid_mode = { "vid_mode", "0" };
// Note that 0 is MODE_WINDOWED
cvar_t		_vid_default_mode = { "_vid_default_mode", "0", TRUE };
// Note that 3 is MODE_FULLSCREEN_DEFAULT
cvar_t		_vid_default_mode_win = { "_vid_default_mode_win", "3", TRUE };
cvar_t		vid_wait = { "vid_wait", "0" };
cvar_t		vid_nopageflip = { "vid_nopageflip", "0", TRUE };
cvar_t		_vid_wait_override = { "_vid_wait_override", "0", TRUE };
cvar_t		vid_config_x = { "vid_config_x", "800", TRUE };
cvar_t		vid_config_y = { "vid_config_y", "600", TRUE };
cvar_t		vid_stretch_by_2 = { "vid_stretch_by_2", "0", TRUE };
cvar_t		_windowed_mouse = { "_windowed_mouse", "0", TRUE };

extern char* (*VID_GetExtModeDescription)( int mode );
extern void	(*VID_GetVID)( struct viddef_s* pvid );

DLL_EXPORT void VID_UpdateWindowVars( void* prc, int x, int y )
{
	RECT* rect = (RECT*)prc;
	window_rect.left = rect->left;
	window_rect.top = rect->top;
	window_rect.right = rect->right;
	window_rect.bottom = rect->bottom;
	window_center_x = x;
	window_center_y = y;
}

DLL_EXPORT void VID_UpdateVID( viddef_t* pvid )
{
	vid = *pvid;
}

byte* gppal = NULL;

/*
================
VID_AllocBuffers
================
*/
DLL_EXPORT qboolean VID_AllocBuffers( void )
{
	int		tsize, tbuffersize;

	vid.maxwarpwidth = WARP_WIDTH;
	vid.maxwarpheight = WARP_HEIGHT;
	vid.colormap = gppal;
	vid.fullbright = 256;

	is15bit = vid.is15bit;

	tbuffersize = vid.width * vid.height * sizeof(*d_pzbuffer);

	tsize = D_SurfaceCacheForRes(vid.width, vid.height);

	tbuffersize += tsize;

// see if there's enough memory, allowing for the normal mode 0x13 pixel,
// z, and surface buffers
	if ((host_parms.memsize - tbuffersize + SURFCACHE_SIZE_AT_320X200 +
		 0x10000 * 3) < minimum_memory)
	{
		Con_SafePrintf("Not enough memory for video mode\n");
		return FALSE;		// not enough memory for mode
	}

	vid_surfcachesize = tsize;

	if (d_pzbuffer)
	{
		D_FlushCaches();
		Hunk_FreeToHighMark(VID_highhunkmark);
		d_pzbuffer = NULL;
	}

	VID_highhunkmark = Hunk_HighMark();

	d_pzbuffer = (short*)Hunk_HighAllocName(tbuffersize, "video");

	vid_surfcache = (byte*)(d_pzbuffer + vid.width * vid.height);

	D_InitCaches();

	return TRUE;
}

int VID_Init( word* palette )
{
	Cvar_RegisterVariable(&vid_mode);
	Cvar_RegisterVariable(&vid_wait);
	Cvar_RegisterVariable(&vid_nopageflip);
	Cvar_RegisterVariable(&_vid_wait_override);
	Cvar_RegisterVariable(&_vid_default_mode);
	Cvar_RegisterVariable(&_vid_default_mode_win);
	Cvar_RegisterVariable(&vid_config_x);
	Cvar_RegisterVariable(&vid_config_y);
	Cvar_RegisterVariable(&vid_stretch_by_2);
	Cvar_RegisterVariable(&_windowed_mouse);

	gppal = (byte*)palette;

	VID_GetVID(&vid);

	if (!VID_AllocBuffers())
	{
		return 0;
	}

	S_Init();
	return 1;
}

/*
=================
VID_DescribeMode_f
=================
*/
void VID_DescribeMode_f( void )
{
	int		modenum;

	modenum = Q_atoi(Cmd_Argv(1));

	Con_Printf("%s\n", VID_GetExtModeDescription(modenum));
}

/*
===================
VID_TakeSnapshot

Write vid.buffer out as a windows bitmap file
*/
void VID_TakeSnapshot( const char* pFilename )
{
	int				i, j;
	HANDLE			file;
	BITMAPFILEHEADER hdr;
	BITMAPINFOHEADER bi;
	int				imageSize;
	color24			row[2048];
	word* pStart, * pDest;
	DWORD			dwSize;


	imageSize = vid.width * vid.height * 3;

	file = CreateFile(pFilename, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file == INVALID_HANDLE_VALUE)
		Sys_Error("Couldn't create file for snapshot.\n");

	// file header
	hdr.bfType = 0x4D42;	// 'BM'
	hdr.bfSize = imageSize + sizeof(hdr) + sizeof(bi);
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;
	hdr.bfOffBits = sizeof(hdr) + sizeof(bi);

	if (!WriteFile(file, &hdr, sizeof(hdr), &dwSize, 0))
		Sys_Error("Couldn't write file header to snapshot.\n");

	bi.biWidth = vid.width;
	bi.biHeight = vid.height;
	bi.biSize = sizeof(bi);
	bi.biPlanes = 1;
	bi.biBitCount = 24;
	bi.biCompression = 0;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	if (!WriteFile(file, &bi, sizeof(bi), &dwSize, 0))
		Sys_Error("Couldn't write bitmap header to snapshot.\n");

	pStart = (word*)(vid.buffer + vid.height * vid.rowbytes);
	for (i = 0; i < (int)vid.height; i++)
	{
		pDest = (word*)((byte*)pStart - vid.rowbytes);

		for (j = 0; j < (int)vid.width; j++)
		{
			if (is15bit)
			{
				row[j].b = (*pDest >> 7) & 0xF8;
				row[j].g = (*pDest >> 2) & 0xF8;
			}
			else
			{
				row[j].b = (*pDest >> 8) & 0xF8;
				row[j].g = (*pDest >> 3) & 0xFC;
			}
			row[j].r = (*pDest) << 3;

			pDest++;
		}

		if (!WriteFile(file, row, vid.width * 3, &dwSize, 0) || vid.width * 3 != dwSize)
			Sys_Error("Couldn't write bitmap data snapshot.\n");

		pStart = (word*)((byte*)pDest - vid.rowbytes);
	}

	// close the file
	if (!CloseHandle(file))
		Sys_Error("Couldn't close file for snapshot.\n");
}

void VID_WriteBuffer( const char* pFilename )
{
	int				imageSize;
	int				createType;
	DWORD			dwSize;
	WORD			frameHeader[3];
	int				blockHeader[2];
	static char		basefilename[256];

	imageSize = vid.height * vid.rowbytes;

	createType = OPEN_EXISTING;

	if (pFilename)
	{
		strcpy(basefilename, pFilename);
		if (hMovieFile != INVALID_HANDLE_VALUE)
			CloseHandle(hMovieFile);

		createType = CREATE_ALWAYS;
	}

	hMovieFile = CreateFile(basefilename, GENERIC_WRITE, 0, NULL, createType, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hMovieFile == INVALID_HANDLE_VALUE)
		Sys_Error("Couldn't open movie file.\n");

	dwSize = 0;
	SetFilePointer(hMovieFile, 0, &dwSize, FILE_END);

	frameHeader[0] = vid.width;
	frameHeader[1] = vid.height;
	if (is15bit)
		frameHeader[2] = 15;
	else
		frameHeader[2] = 16;

	blockHeader[0] = 0x4D46524D;	// 'MFRM'
	blockHeader[1] = imageSize + sizeof(frameHeader);

	if (!WriteFile(hMovieFile, blockHeader, sizeof(blockHeader), &dwSize, 0))
		Sys_Error("Couldn't write block header.\n");

	if (!WriteFile(hMovieFile, frameHeader, sizeof(frameHeader), &dwSize, 0))
		Sys_Error("Couldn't write frame header.\n");

	if (!WriteFile(hMovieFile, vid.buffer, imageSize, &dwSize, 0))
		Sys_Error("Couldn't write frame data.\n");

	// close the file
	if (!CloseHandle(hMovieFile))
		Sys_Error("Couldn't close file for movie.\n");

	hMovieFile = INVALID_HANDLE_VALUE;
}