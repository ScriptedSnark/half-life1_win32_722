// gl_vidnt.c -- NT GL vid component

#include "quakedef.h"
#include "winquake.h"
#include "gl_hw.h"
#include "opengl2d3d.h"

BOOL gfMiniDriver = FALSE;


qboolean		scr_skipupdate;

int gGLHardwareType = GL_HW_UNKNOWN;

const char* gl_vendor;
const char* gl_renderer;
const char* gl_version;
const char* gl_extensions;

cvar_t	gl_ztrick = { "gl_ztrick", "1" };

cvar_t	vid_d3d = { "vid_d3d", "0" };

// TODO: Implement

cvar_t	_windowed_mouse = { "_windowed_mouse", "0", TRUE };

// TODO: Implement

viddef_t	vid;				// global video state

PROC qglArrayElementEXT;
PROC qglColorPointerEXT;
PROC qglTexCoordPointerEXT;
PROC qglVertexPointerEXT;

//int		texture_mode = GL_NEAREST;
//int		texture_mode = GL_NEAREST_MIPMAP_NEAREST;
//int		texture_mode = GL_NEAREST_MIPMAP_LINEAR;
int		texture_mode = GL_LINEAR;
//int		texture_mode = GL_LINEAR_MIPMAP_NEAREST;
//int		texture_mode = GL_LINEAR_MIPMAP_LINEAR;

int		texture_extension_number = 1;

qboolean gl_mtexable = FALSE;

//====================================

float		gldepthmin, gldepthmax;

int			window_center_x, window_center_y;
RECT		window_rect;



extern void	(*VID_GetVID)( struct viddef_s* pvid );
extern int GlideReadPixels(int x, int y, int width, int height, word* pixels);

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

// TODO: Implement

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
	// TODO: Implement

	Cvar_RegisterVariable(&_windowed_mouse);

	Cvar_RegisterVariable(&gl_ztrick);
	Cvar_RegisterVariable(&vid_d3d);

	// TODO: Implement



	Cmd_AddCommand("gl_log", GLimp_EnableLogging);

	VID_GetVID(&vid);

	return 1;
}

void VID_TakeSnapshot(const char* pFilename)
{
	HANDLE				fp;
	BITMAPFILEHEADER	hdr;
	BITMAPINFOHEADER	bi;
	int					imageSize = 3 * vid.height * vid.width;
	DWORD				dwWritten;
	byte				*hp;
	byte				b;
	int					i;

	fp = CreateFile(pFilename, GENERIC_READ | GENERIC_WRITE, 0, 0, 2u, 0x80u, 0);
	if (fp == (HANDLE)-1)
		Sys_Error("Couldn't create file for snapshot.\n");

	hdr.bfType = (long)'MB';
	hdr.bfSize = imageSize + sizeof(hdr) + sizeof(bi);
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;
	hdr.bfOffBits = sizeof(hdr) + sizeof(bi);

	if (!WriteFile(fp, &hdr.bfType, sizeof(hdr), &dwWritten, 0))
		Sys_Error("Couldn't write file header to snapshot.\n");

	bi.biWidth = vid.width;
	bi.biHeight = vid.height;
	bi.biSize = sizeof(bi);
	bi.biPlanes = 1;
	bi.biBitCount = 24;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;
	bi.biCompression = 0;

	if (!WriteFile(fp, &bi, sizeof(bi), &dwWritten, 0))
		Sys_Error("Couldn't write bitmap header to snapshot.\n");

	hp = (byte *)malloc(imageSize);
	if (!hp)
		Sys_Error("Couldn't allocate bitmap header to snapshot.\n");

	if ( gGLHardwareType != GL_HW_3Dfx || !GlideReadPixels(0, 0, vid.width, vid.height, (word *)hp) )
		qglReadPixels(0, 0, vid.width, vid.height, GL_RGB, GL_UNSIGNED_BYTE, hp);

	// swap rgb to bgr
	for ( i = sizeof(hdr) + sizeof(bi); i < imageSize; i += sizeof(byte) * sizeof(RGBTRIPLE) )
	{
		b = hp[i];
		hp[i] = hp[i + 2];
		hp[i + 2] = b;
	}

	if ( !WriteFile(fp, hp, imageSize, &dwWritten, 0) )
		Sys_Error("Couldn't write bitmap data snapshot.\n");

	free(hp);

	if ( !CloseHandle(fp) )
		Sys_Error("Couldn't close file for snapshot.\n");
}


void VID_TakeSnapshotRect( const char* pFilename, int x, int y, int w, int h )
{
	// TODO: Implement
}

void VID_WriteBuffer( const char* pFilename )
{
	static char		szFileName[256];
	static HANDLE	hFile;
	int				imageSize = 3 * vid.height * vid.width;
	WORD			frameHeader[3];
	int				blockHeader[2];
	DWORD			dwWritten;
	void*			pFrameData;

	frameHeader[0] = vid.width;
	frameHeader[1] = vid.height;
	frameHeader[2] = 24;

	blockHeader[0] = 'MFRM';
	blockHeader[1] = imageSize + (sizeof(short) * 3);

	if ( pFilename && pFilename[0] )
	{
		strcpy( szFileName, pFilename );
		if (hFile != INVALID_HANDLE_VALUE)
			CloseHandle( hFile );
	}

	hFile = CreateFile( szFileName, GENERIC_WRITE, 0, NULL,
						  pFilename ? OPEN_EXISTING : CREATE_ALWAYS,
						  FILE_ATTRIBUTE_NORMAL, NULL );

	if ( hFile == INVALID_HANDLE_VALUE )
		Sys_Error("Couldn't open movie file.\n");

	SetFilePointer( hFile, 0, NULL, FILE_END );

	pFrameData = malloc( imageSize );
	if ( !pFrameData )
		Sys_Error("Couldn't allocate frame buffer.\n");

	if ( gGLHardwareType != GL_HW_3Dfx || !GlideReadPixels(0, 0, vid.width, vid.height, pFrameData) )
		qglReadPixels( 0, 0, vid.width, vid.height, GL_RGB, GL_UNSIGNED_BYTE, pFrameData );

	if (!WriteFile(hFile, blockHeader, sizeof(blockHeader), &dwWritten, NULL) )
		Sys_Error("Couldn't write block header.\n");

	if (!WriteFile(hFile, frameHeader, sizeof(frameHeader), &dwWritten, NULL) )
		Sys_Error("Couldn't write frame header.\n");

	if (!WriteFile(hFile, pFrameData, imageSize, &dwWritten, NULL) )
		Sys_Error("Couldn't write frame data.\n");

	free( pFrameData );

	if (!CloseHandle(hFile))
		Sys_Error("Couldn't close file for movie.\n");

	hFile = INVALID_HANDLE_VALUE;
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