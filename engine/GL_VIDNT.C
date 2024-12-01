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

cvar_t	vid_d3d = { "vid_d3d", "0" };


// TODO: Implement

viddef_t	vid;				// global video state


int			window_center_x, window_center_y;
RECT		window_rect;



extern void	(*VID_GetVID)( struct viddef_s* pvid );


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

	// TODO: Implement

	GL_Config();
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
	// TODO: Implement

	Cvar_RegisterVariable(&vid_d3d);

	// TODO: Implement

	VID_GetVID(&vid);

	return 1;
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