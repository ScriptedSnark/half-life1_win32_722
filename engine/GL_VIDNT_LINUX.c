// gl_vidnt_linux.c -- NT GL vid component adapted for Linux

#include "quakedef.h"
#include "winquake.h"
#include "gl_hw.h"
#include <SDL2/SDL.h>

// Store global window/context if needed
SDL_GLContext g_GLContext = NULL;
SDL_Window* gWindow = NULL;

BOOL gfMiniDriver = FALSE;

extern qboolean		scr_skipupdate;

int gGLHardwareType = GL_HW_UNKNOWN;

const char* gl_vendor;
const char* gl_renderer;
const char* gl_version;
const char* gl_extensions;

cvar_t	gl_ztrick = { "gl_ztrick", "1" };

viddef_t	vid;				// global video state

float		gldepthmin, gldepthmax;

void* qglArrayElementEXT;
void* qglColorPointerEXT;
void* qglTexCoordPointerEXT;
void* qglVertexPointerEXT;

qboolean gl_mtexable = FALSE;

//====================================

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

void CheckTextureExtensions( void )
{
	qboolean texture_ext = FALSE;

	if (strstr(gl_extensions, "GL_EXT_paletted_texture") &&
		strstr(gl_extensions, "GL_EXT_shared_texture_palette"))
	{
		qglColorTableEXT = (void (*)(int, int, int, int, int, const void*))SDL_GL_GetProcAddress("glColorTableEXT");
		Con_Printf("Found paletted texture extension.\n");
	}
	else
	{
		qglColorTableEXT = NULL;
	}

	if (strstr(gl_extensions, "GL_EXT_texture_object"))
		texture_ext = TRUE;

	if (!texture_ext || COM_CheckParm("-gl11"))
		return;

	if ((qglBindTexture = (void (*)(GLenum, GLuint))SDL_GL_GetProcAddress("glBindTexture")) == NULL)
	{
		Sys_Error("SDL_GL_GetProcAddress for glBindTexture failed");
	}
}

void CheckArrayExtensions( void )
{
	const char* tmp = (const char*)qglGetString(GL_EXTENSIONS);

	if (strstr(tmp, "GL_EXT_vertex_array"))
	{
		if (
			((qglArrayElementEXT    = SDL_GL_GetProcAddress("glArrayElementEXT"))    == NULL) ||
			((qglColorPointerEXT    = SDL_GL_GetProcAddress("glColorPointerEXT"))    == NULL) ||
			((qglTexCoordPointerEXT = SDL_GL_GetProcAddress("glTexCoordPointerEXT")) == NULL) ||
			((qglVertexPointerEXT   = SDL_GL_GetProcAddress("glVertexPointerEXT"))   == NULL))
		{
			Sys_Error("SDL_GL_GetProcAddress for vertex array extension failed");
		}
		return;
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
	if (strstr(gl_extensions, "GL_SGIS_multitexture"))
	{
		Con_Printf("Multitexture extensions found.\n");
		qglMTexCoord2fSGIS    = (void (*)(GLenum, float, float))SDL_GL_GetProcAddress("glMTexCoord2fSGIS");
		qglSelectTextureSGIS  = (void (*)(GLenum))SDL_GL_GetProcAddress("glSelectTextureSGIS");
		gl_mtexable = TRUE;
		GL_SelectTexture(TEXTURE0_SGIS);
	}
	else
	{
		Con_Printf("NO Multitexture extensions found.\n");
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
VID_DescribeCurrentMode_f
=================
*/
void VID_DescribeMode_f( void )
{
	int		modenum;

	modenum = Q_atoi(Cmd_Argv(1));

	Con_Printf("%s\n", VID_GetExtModeDescription(modenum));
}

// This replaces GL_SetMode for SDL2
DLL_EXPORT int GL_SetMode(SDL_Window* window)
{
    if (!window)
    {
        Sys_Error("GL_SetMode: NULL window");
        return FALSE;
    }

    // Create OpenGL context
	g_GLContext = SDL_GL_CreateContext(window);
	gWindow = window;
	if (!g_GLContext)
	{
		Sys_Error("SDL_GL_CreateContext failed: %s", SDL_GetError());
		return FALSE;
	}

    // Make context current
    if (SDL_GL_MakeCurrent(window, g_GLContext) != 0)
    {
        Sys_Error("SDL_GL_MakeCurrent failed: %s", SDL_GetError());
        SDL_GL_DeleteContext(g_GLContext);
        g_GLContext = NULL;
        return FALSE;
    }

	QGL_Init("NULL");
    //SDL_GL_SetSwapInterval(1); // Enable vsync if desired
    return TRUE;
}

// This replaces GL_Shutdown for SDL2
void GL_Shutdown(SDL_Window* window)
{
    if (g_GLContext)
    {
        SDL_GL_MakeCurrent(window, NULL);
        SDL_GL_DeleteContext(g_GLContext);
        g_GLContext = NULL;
    }
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
void VID_TakeSnapshot(const char* pFilename)
{
	FILE* fp;
	BITMAPFILEHEADER hdr;
	BITMAPINFOHEADER bi;
	int imageSize = vid.width * vid.height * 3;
	unsigned char* hp = NULL;
	unsigned char b;

	fp = fopen(pFilename, "wb");
	if (!fp)
		Sys_Error("Couldn't create file for snapshot.\n");

	// File header
	hdr.bfType = 0x4D42; // 'BM'
	hdr.bfSize = imageSize + sizeof(hdr) + sizeof(bi);
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;
	hdr.bfOffBits = sizeof(hdr) + sizeof(bi);

	fwrite(&hdr, sizeof(hdr), 1, fp);

	// Info header
	bi.biSize = sizeof(bi);
	bi.biWidth = vid.width;
	bi.biHeight = vid.height;
	bi.biPlanes = 1;
	bi.biBitCount = 24;
	bi.biCompression = 0;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	fwrite(&bi, sizeof(bi), 1, fp);

	// Read pixels
	hp = (unsigned char*)malloc(imageSize);
	if (!hp)
		Sys_Error("Couldn't allocate memory for snapshot.\n");

	qglReadPixels(0, 0, vid.width, vid.height, GL_RGB, GL_UNSIGNED_BYTE, hp);

	// Swap RGB to BGR
	for (int i = 0; i < imageSize; i += 3)
	{
		b = hp[i];
		hp[i] = hp[i + 2];
		hp[i + 2] = b;
	}

	fwrite(hp, imageSize, 1, fp);
	free(hp);
	fclose(fp);
}



void VID_TakeSnapshotRect(const char* pFilename, int x, int y, int w, int h)
{
	FILE* fp;
	BITMAPFILEHEADER hdr;
	BITMAPINFOHEADER bi;
	int imageSize = w * h * 3;
	unsigned char* hp = NULL;
	unsigned char b;

	fp = fopen(pFilename, "wb");
	if (!fp)
		Sys_Error("Couldn't create file for snapshot.\n");

	// File header
	hdr.bfType = 0x4D42; // 'BM'
	hdr.bfSize = imageSize + sizeof(hdr) + sizeof(bi);
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;
	hdr.bfOffBits = sizeof(hdr) + sizeof(bi);

	fwrite(&hdr, sizeof(hdr), 1, fp);

	// Info header
	bi.biSize = sizeof(bi);
	bi.biWidth = w;
	bi.biHeight = h;
	bi.biPlanes = 1;
	bi.biBitCount = 24;
	bi.biCompression = 0;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	fwrite(&bi, sizeof(bi), 1, fp);

	hp = (unsigned char*)malloc(imageSize);
	if (!hp)
		Sys_Error("Couldn't allocate memory for snapshot.\n");

	qglReadPixels(x, y, w, h, GL_RGB, GL_UNSIGNED_BYTE, hp);

	// Swap RGB to BGR
	for (int i = 0; i < imageSize; i += 3)
	{
		b = hp[i];
		hp[i] = hp[i + 2];
		hp[i + 2] = b;
	}

	fwrite(hp, imageSize, 1, fp);
	free(hp);
	fclose(fp);
}

void VID_WriteBuffer(const char* pFilename)
{
	int imageSize = vid.width * vid.height * 3;
	WORD frameHeader[3] = { vid.width, vid.height, 24 };
	int blockHeader[2] = { 0x4D46524D, imageSize + sizeof(frameHeader) }; // 'MFRM'
	static char basefilename[256];
	static FILE* file = NULL;
	unsigned char* hp = NULL;

	if (pFilename)
	{
		strcpy(basefilename, pFilename);
		if (file)
			fclose(file);

		file = fopen(basefilename, "wb");
	}
	else
	{
		file = fopen(basefilename, "ab");
	}

	if (!file)
		Sys_Error("Couldn't open movie file.\n");

	hp = (unsigned char*)malloc(imageSize);
	if (!hp)
		Sys_Error("Couldn't allocate memory for snapshot.\n");

	qglReadPixels(0, 0, vid.width, vid.height, GL_RGB, GL_UNSIGNED_BYTE, hp);

	fwrite(blockHeader, sizeof(blockHeader), 1, file);
	fwrite(frameHeader, sizeof(frameHeader), 1, file);
	fwrite(hp, imageSize, 1, file);

	free(hp);
	fclose(file);
	file = NULL;
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