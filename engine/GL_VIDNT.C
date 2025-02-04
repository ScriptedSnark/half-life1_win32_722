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

	Cvar_RegisterVariable(&gl_ztrick);
	Cvar_RegisterVariable(&vid_d3d);

	// TODO: Implement

	Cmd_AddCommand("gl_log", GLimp_EnableLogging);

	VID_GetVID(&vid);

	return 1;
}


void VID_WriteBuffer( const char* pFilename )
{
	// TODO: Implement
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