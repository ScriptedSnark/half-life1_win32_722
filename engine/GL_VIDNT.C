// gl_vidnt.c -- NT GL vid component

#include "quakedef.h"
#include "winquake.h"
#include "gl_hw.h"
#include "opengl32.h"

BOOL gfMiniDriver = FALSE;

int gGLHardwareType = GL_HW_UNKNOWN;

const char* gl_vendor;
const char* gl_renderer;
const char* gl_version;
const char* gl_extensions;

static HANDLE	hMovieFile = INVALID_HANDLE_VALUE;

cvar_t	gl_ztrick = { "gl_ztrick", "1" };

viddef_t	vid;				// global video state

float		gldepthmin, gldepthmax;

PROC qglArrayElementEXT;
PROC qglColorPointerEXT;
PROC qglTexCoordPointerEXT;
PROC qglVertexPointerEXT;

qboolean gl_mtexable = FALSE;

//====================================

cvar_t		vid_d3d = { "vid_d3d", "0" };
cvar_t		vid_mode = { "vid_mode", "0" };
// Note that 0 is MODE_WINDOWED
cvar_t		_vid_default_mode = { "_vid_default_mode", "0", TRUE };
// Note that 3 is MODE_FULLSCREEN_DEFAULT
cvar_t		_vid_default_mode_win = { "_vid_default_mode_win", "3", TRUE };
cvar_t		vid_wait = { "vid_wait", "0" };
cvar_t		vid_nopageflip = { "vid_nopageflip", "0", TRUE };
cvar_t		vid_wait_override = { "_vid_wait_override", "0", TRUE };
cvar_t		vid_config_x = { "vid_config_x", "800", TRUE };
cvar_t		vid_config_y = { "vid_config_y", "600", TRUE };
cvar_t		vid_stretch_by_2 = { "vid_stretch_by_2", "1", TRUE };
cvar_t		_windowed_mouse = { "_windowed_mouse", "0", TRUE };

int			window_center_x, window_center_y;
RECT		window_rect;

extern void	(*VID_GetVID)( struct viddef_s* pvid );
extern char* (*VID_GetExtModeDescription)( int mode );

extern int GlideReadPixels( int x, int y, int width, int height, word* pixels );

void CheckTextureExtensions( void )
{
	qboolean	texture_ext = FALSE;

	if (vid_d3d.value)
		return;

	if (strstr(gl_extensions, "GL_EXT_paletted_texture") &&
		strstr(gl_extensions, "GL_EXT_shared_texture_palette"))
	{
		qglColorTableEXT = (void*)qwglGetProcAddress("glColorTableEXT");
		Con_Printf("Found paletted texture extension.\n");
	}
	else
	{
		qglColorTableEXT = NULL;
	}

	if (strstr(gl_extensions, "GL_EXT_texture_object "))
		texture_ext = TRUE;

	if (!texture_ext || COM_CheckParm("-gl11"))
	{
		return;
	}

	if ((qglBindTexture = (void*)qwglGetProcAddress("glBindTextureEXT")) == NULL)
	{
		Sys_Error("GetProcAddress for BindTextureEXT failed");
		return;
	}
}

void CheckArrayExtensions( void )
{
	char* tmp;

	/* check for texture extension */
	tmp = (char*)(qglGetString(GL_EXTENSIONS));
	while (*tmp)
	{
		if (strncmp(tmp, "GL_EXT_vertex_array", strlen("GL_EXT_vertex_array")) == 0)
		{
			if (
				((qglArrayElementEXT = qwglGetProcAddress("glArrayElementEXT")) == NULL) ||
				((qglColorPointerEXT = qwglGetProcAddress("glColorPointerEXT")) == NULL) ||
				((qglTexCoordPointerEXT = qwglGetProcAddress("glTexCoordPointerEXT")) == NULL) ||
				((qglVertexPointerEXT = qwglGetProcAddress("glVertexPointerEXT")) == NULL))
			{
				Sys_Error("GetProcAddress for vertex extension failed");
				return;
			}
			return;
		}
		tmp++;
	}

	Sys_Error("Vertex array extension not present");
}

//int		texture_mode = GL_NEAREST;
//int		texture_mode = GL_NEAREST_MIPMAP_NEAREST;
//int		texture_mode = GL_NEAREST_MIPMAP_LINEAR;
int		texture_mode = GL_LINEAR;
//int		texture_mode = GL_LINEAR_MIPMAP_NEAREST;
//int		texture_mode = GL_LINEAR_MIPMAP_LINEAR;

int		texture_extension_number = 1;

void CheckMultiTextureExtensions( void )
{
	if (vid_d3d.value)
		return;
	
	if (strstr(gl_extensions, "GL_SGIS_multitexture "))
	{
		Con_Printf("Multitexture extensions found.\n");
		qglMTexCoord2fSGIS = (void*)qwglGetProcAddress("glMTexCoord2fSGIS");
		qglSelectTextureSGIS = (void*)qwglGetProcAddress("glSelectTextureSGIS");
		gl_mtexable = TRUE;
		GL_SelectTexture(TEXTURE0_SGIS);
	}
	else
	{
		Con_Printf("NO Multitexture extensions found.\n");
		return;
	}
}

void GL_Config( void )
{
	if (strstr(gl_vendor, "3Dfx"))
	{
		gGLHardwareType = GL_HW_3Dfx;
		Cbuf_InsertText("exec hw/3Dfx.cfg\n");
	}
	else if (strstr(gl_vendor, "NVIDIA"))
	{
		if (strstr(gl_renderer, "RIVA 128"))
		{
			gGLHardwareType = GL_HW_RIVA128;
			Cbuf_InsertText("exec hw/riva128.cfg\n");
		}
		else if (strstr(gl_renderer, "TNT"))
		{
			gGLHardwareType = GL_HW_RIVATNT;
			Cbuf_InsertText("exec hw/rivaTNT.cfg\n");
		}
	}
	else if (strstr(gl_vendor, "PCX2"))
	{
		Cbuf_InsertText("exec hw/PowerVRPCX2.cfg\n");
		gGLHardwareType = GL_HW_PCX2;
	}
	else if (strstr(gl_vendor, "PowerVR"))
	{
		Cbuf_InsertText("exec hw/PowerVRSG.cfg\n");
		gGLHardwareType = GL_HW_PVRSG;
	}
	else if (strstr(gl_vendor, "V2200"))
	{
		Cbuf_InsertText("exec hw/V2200.cfg\n");
		gGLHardwareType = GL_HW_RENDITIONV2200;
	}
	else if (strstr(gl_vendor, "3Dlabs"))
	{
		Cbuf_InsertText("exec hw/3Dlabs.cfg\n");
		gGLHardwareType = GL_HW_3DLABS;
	}
	else
	{
		gGLHardwareType = GL_HW_UNKNOWN;
	}
}

/*
===============
GL_Init
===============
*/
void GL_Init( void )
{
	gl_vendor = (const char*)qglGetString(GL_VENDOR);
	Con_DPrintf("GL_VENDOR: %s\n", gl_vendor);
	gl_renderer = (const char*)qglGetString(GL_RENDERER);
	Con_DPrintf("GL_RENDERER: %s\n", gl_renderer);

	gl_version = (const char*)qglGetString(GL_VERSION);
	Con_DPrintf("GL_VERSION: %s\n", gl_version);
	gl_extensions = (const char*)qglGetString(GL_EXTENSIONS);
	Con_DPrintf("GL_EXTENSIONS: %s\n", gl_extensions);

	CheckTextureExtensions();
	CheckMultiTextureExtensions();

	qglClearColor(1, 0, 0, 0);
	qglCullFace(GL_FRONT);
	qglEnable(GL_TEXTURE_2D);

	qglEnable(GL_ALPHA_TEST);

	qglAlphaFunc(GL_NOTEQUAL, 0.0);

	qglPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	qglShadeModel(GL_FLAT);

	qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	qglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	GL_Config();
}

/*
=================
GL_BeginRendering

=================
*/
void GL_BeginRendering( int* x, int* y, int* width, int* height )
{
	*x = *y = 0;
	*width = window_rect.right - window_rect.left;
	*height = window_rect.bottom - window_rect.top;
	vid.width = vid.conwidth = *width;
	vid.height = vid.conheight = *height;
	GLimp_LogNewFrame();
}


void GL_EndRendering( void )
{
	VID_Update(NULL);
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

#if defined (_WIN32)
//==========================================================================


BOOL bSetupPixelFormat( HDC hDC )
{
	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),		// size of this pfd
		1,									// version number
		PFD_DRAW_TO_WINDOW 					// support window
		| PFD_SUPPORT_OPENGL 				// support OpenGL
		| PFD_DOUBLEBUFFER,					// double buffered
		PFD_TYPE_RGBA,						// RGBA type
		24,									// 24-bit color depth
		0, 0, 0, 0, 0, 0,					// color bits ignored
		0,									// no alpha buffer
		0,									// shift bit ignored
		0,									// no accumulation buffer
		0, 0, 0, 0, 						// accum bits ignored
		32,									// 32-bit z-buffer	
		0,									// no stencil buffer
		0,									// no auxiliary buffer
		PFD_MAIN_PLANE,						// main layer
		0,									// reserved
		0, 0, 0								// layer masks ignored
	};

	int pixelformat;

	if (gfMiniDriver || vid_d3d.value)
	{
		if ((pixelformat = qwglChoosePixelFormat(hDC, &pfd)) == 0)
		{
			MessageBox(NULL, "ChoosePixelFormat failed", "Error", MB_OK);
			return FALSE;
		}

		if (!qwglSetPixelFormat(hDC, pixelformat, &pfd))
		{
			MessageBox(NULL, "SetPixelFormat failed", "Error", MB_OK);
			return FALSE;
		}

		qwglDescribePixelFormat(hDC, pixelformat, sizeof(pfd), &pfd);
		return TRUE;
	}

	if ((pixelformat = ChoosePixelFormat(hDC, &pfd)) == 0)
	{
		MessageBox(NULL, "ChoosePixelFormat failed", "Error", MB_OK);
		return FALSE;
	}

	if (SetPixelFormat(hDC, pixelformat, &pfd) == FALSE)
	{
		MessageBox(NULL, "SetPixelFormat failed", "Error", MB_OK);
		return FALSE;
	}

	DescribePixelFormat(hDC, pixelformat, sizeof(pfd), &pfd);

	return TRUE;
}
#endif

DLL_EXPORT int GL_SetMode( HWND mainwindow, HDC* pmaindc, HGLRC* pbaseRC, int fD3D, char* pszDriver )
{
	HMODULE hDll;

	gfMiniDriver = FALSE;

	vid_d3d.value = fD3D;

	*pmaindc = NULL;
	*pbaseRC = NULL;

	if (!vid_d3d.value)
	{
		if (pszDriver)
		{
			gfMiniDriver = strstr(pszDriver, "opengl32") == NULL;
		}
		hDll = QGL_Init(pszDriver);
	}
	else
	{
		hDll = QGL_D3DInit();
	}

	if (!hDll)
	{
		if (pszDriver)
			Sys_Error("Error initializing gl driver, check that the file '%s' exists", pszDriver);
		else
			Sys_Error("Error initializing gl driver, check that the GL driver file opengl32.dll exists");
	}
	else
	{
		HDC hDC = GetDC(mainwindow);
		*pmaindc = hDC;
		if (bSetupPixelFormat(hDC))
		{
			HGLRC context = qwglCreateContext(hDC);
			*pbaseRC = context;
			if (context)
			{
				if (qwglMakeCurrent(*pmaindc, *pbaseRC))
					return TRUE;
			}
		}
	}

	if (hDll)
	{
		qwglMakeCurrent(NULL, NULL);
		if (*pbaseRC)
			qwglDeleteContext(*pbaseRC);
		if (*pmaindc)
			ReleaseDC(mainwindow, *pmaindc);

		FreeLibrary(hDll);
	}

	return FALSE;
}

DLL_EXPORT void GL_Shutdown( HWND hwnd, HDC hdc, HGLRC hglrc )
{
	qwglMakeCurrent(NULL, NULL);
	if (hglrc)
		qwglDeleteContext(hglrc);
	ReleaseDC(hwnd, hdc);
}

/*
================
VID_Init
================
*/
int VID_Init( word* palette )
{
	Cvar_RegisterVariable(&vid_mode);
	Cvar_RegisterVariable(&vid_wait);
	Cvar_RegisterVariable(&vid_nopageflip);
	Cvar_RegisterVariable(&vid_wait_override);
	Cvar_RegisterVariable(&_vid_default_mode);
	Cvar_RegisterVariable(&_vid_default_mode_win);
	Cvar_RegisterVariable(&vid_config_x);
	Cvar_RegisterVariable(&vid_config_y);
	Cvar_RegisterVariable(&vid_stretch_by_2);
	Cvar_RegisterVariable(&_windowed_mouse);
	Cvar_RegisterVariable(&gl_ztrick);
	Cvar_RegisterVariable(&vid_d3d);

	Cmd_AddCommand("vid_describemode", VID_DescribeMode_f);
	Cmd_AddCommand("gl_log", GLimp_EnableLogging);

	VID_GetVID(&vid);

	return TRUE;
}

/*
===================
VID_TakeSnapshot

Write vid.buffer out as a windows bitmap file
*/
void VID_TakeSnapshot( const char* pFilename )
{
	int				i;
	HANDLE			fp;
	BITMAPFILEHEADER hdr;
	BITMAPINFOHEADER bi;
	int				imageSize;
	DWORD			dwSize;

	unsigned char* hp = NULL;
	unsigned char b;

	imageSize = vid.width * vid.height * 3;

	fp = CreateFile(pFilename, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fp == INVALID_HANDLE_VALUE)
		Sys_Error("Couldn't create file for snapshot.\n");

	// file header
	hdr.bfType = 0x4D42;	// 'BM'
	hdr.bfSize = imageSize + sizeof(hdr) + sizeof(bi);
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;
	hdr.bfOffBits = sizeof(hdr) + sizeof(bi);

	if (!WriteFile(fp, &hdr, sizeof(hdr), &dwSize, 0))
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

	if (!WriteFile(fp, &bi, sizeof(bi), &dwSize, 0))
		Sys_Error("Couldn't write bitmap header to snapshot.\n");

	hp = (unsigned char*)malloc(imageSize);
	if (!hp)
		Sys_Error("Couldn't allocate bitmap header to snapshot.\n");

	if (gGLHardwareType != GL_HW_3Dfx || !GlideReadPixels(0, 0, vid.width, vid.height, (word*)hp))
		qglReadPixels(0, 0, vid.width, vid.height, GL_RGB, GL_UNSIGNED_BYTE, hp);

	// swap RGB to BGR
	for (i = 0; i < imageSize; i += 3)
	{
		b = hp[i + 0];
		hp[i + 0] = hp[i + 2];
		hp[i + 2] = b;
	}

	// write to the file
	if (!WriteFile(fp, hp, imageSize, &dwSize, 0))
		Sys_Error("Couldn't write bitmap data snapshot.\n");

	free(hp);

	// close the file
	if (!CloseHandle(fp))
		Sys_Error("Couldn't close file for snapshot.\n");
}


void VID_TakeSnapshotRect( const char* pFilename, int x, int y, int w, int h )
{
	int				i;
	HANDLE			file;
	BITMAPFILEHEADER hdr;
	BITMAPINFOHEADER bi;
	int				imageSize;
	DWORD			dwSize;

	unsigned char* hp = NULL;
	unsigned char b;

	imageSize = w * h * 3;

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

	bi.biWidth = w;
	bi.biHeight = h;
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

	hp = (unsigned char*)malloc(imageSize);
	if (!hp)
		Sys_Error("Couldn't allocate bitmap header to snapshot.\n");

	if (gGLHardwareType != GL_HW_3Dfx || !GlideReadPixels(x, y, w, h, (word*)hp))
		qglReadPixels(x, y, w, h, GL_RGB, GL_UNSIGNED_BYTE, hp);

	// swap RGB to BGR
	for (i = 0; i < imageSize; i += 3)
	{
		b = hp[i + 0];
		hp[i + 0] = hp[i + 2];
		hp[i + 2] = b;
	}

	// write to the file
	if (!WriteFile(file, hp, imageSize, &dwSize, 0))
		Sys_Error("Couldn't write bitmap data snapshot.\n");

	free(hp);

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

	unsigned char* hp = NULL;

	imageSize = vid.width * vid.height * 3;

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
	frameHeader[2] = 24;

	blockHeader[0] = 0x4D46524D;	// 'MFRM'
	blockHeader[1] = imageSize + sizeof(frameHeader);

	hp = (unsigned char*)malloc(imageSize);
	if (!hp)
		Sys_Error("Couldn't allocate bitmap header to snapshot.\n");

	if (gGLHardwareType != GL_HW_3Dfx || !GlideReadPixels(0, 0, vid.width, vid.height, (word*)hp))
		qglReadPixels(0, 0, vid.width, vid.height, GL_RGB, GL_UNSIGNED_BYTE, hp);

	if (!WriteFile(hMovieFile, blockHeader, sizeof(blockHeader), &dwSize, 0))
		Sys_Error("Couldn't write block header.\n");

	if (!WriteFile(hMovieFile, frameHeader, sizeof(frameHeader), &dwSize, 0))
		Sys_Error("Couldn't write frame header.\n");

	// write to the file
	if (!WriteFile(hMovieFile, hp, imageSize, &dwSize, 0))
		Sys_Error("Couldn't write frame data.\n");

	free(hp);

	// close the file
	if (!CloseHandle(hMovieFile))
		Sys_Error("Couldn't close file for movie.\n");

	hMovieFile = INVALID_HANDLE_VALUE;
}

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

DLL_EXPORT int VID_AllocBuffers( void )
{
	return TRUE;
}