//-----------------------------------------------------------------------------
// Quake GL to DirectX wrapper
//-----------------------------------------------------------------------------

#ifdef DECLSPEC_IMPORT
#undef DECLSPEC_IMPORT
#endif
#define DECLSPEC_IMPORT __declspec(dllexport)

#define CINTERFACE
#include <../dx6sdk/include/ddraw.h>
#include <../dx6sdk/include/d3d.h>

#undef DECLSPEC_IMPORT
#define DECLSPEC_IMPORT __declspec(dllimport)

#include "d3d_structs.h"
#include "opengl32.h"

#ifdef _WIN32
#define DLL_EXPORT extern "C" __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

D3D_t gD3D;

// TODO: Implement

DLL_EXPORT void APIENTRY glTexImage2D( GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* pixels )
{
	// TODO: Implement
}

// TODO: Implement

DLL_EXPORT void APIENTRY glSelectTextureSGIS( GLenum target )
{
	// TODO: Implement
}

DLL_EXPORT void APIENTRY glMTexCoord2fSGIS( GLenum target, GLfloat s, GLfloat t )
{
	// TODO: Implement
}


DLL_EXPORT void APIENTRY glVertexPointer( GLint size, GLenum type, GLsizei stride, const GLvoid* pointer )
{
}

DLL_EXPORT void APIENTRY glViewport( GLint x, GLint y, GLsizei width, GLsizei height )
{
	// TODO: Implement
}



// TODO: Implement














DLL_EXPORT int WINAPI wglSetLayerPaletteEntries( HDC, int, int, int, CONST COLORREF* )
{
	return 0;
}

DLL_EXPORT BOOL APIENTRY wglShareLists( HGLRC, HGLRC )
{
	return FALSE;
}

DLL_EXPORT BOOL WINAPI wglSwapLayerBuffers( HDC, UINT )
{
	return FALSE;
}

DLL_EXPORT BOOL WINAPI wglUseFontBitmapsA( HDC, DWORD, DWORD, DWORD )
{
	return FALSE;
}

DLL_EXPORT BOOL WINAPI wglUseFontBitmapsW( HDC, DWORD, DWORD, DWORD )
{
	return FALSE;
}

DLL_EXPORT BOOL WINAPI wglUseFontOutlinesA( HDC, DWORD, DWORD, DWORD, FLOAT,
	FLOAT, int, LPGLYPHMETRICSFLOAT )
{
	return FALSE;
}

DLL_EXPORT BOOL WINAPI wglUseFontOutlinesW( HDC, DWORD, DWORD, DWORD, FLOAT,
	FLOAT, int, LPGLYPHMETRICSFLOAT )
{
	return FALSE;
}

DLL_EXPORT int APIENTRY wglChoosePixelFormat( HDC hdc, CONST PIXELFORMATDESCRIPTOR* ppfd )
{
	return 1;
}

DLL_EXPORT int APIENTRY wglDescribePixelFormat( HDC hdc, int iPixelFormat, UINT nBytes, LPPIXELFORMATDESCRIPTOR ppfd )
{
	ppfd->nSize = sizeof(PIXELFORMATDESCRIPTOR);	// size of this pfd
	ppfd->nVersion = 1;								// version number
	ppfd->dwFlags = PFD_GENERIC_ACCELERATED			// support GDI acceleration
		| PFD_DRAW_TO_WINDOW						// support window
		| PFD_SUPPORT_OPENGL						// support OpenGL
		| PFD_DOUBLEBUFFER;							// double buffered
	ppfd->iPixelType = PFD_TYPE_RGBA;				// RGBA type
	ppfd->cColorBits = 15;							// 15-bit color depth
	ppfd->cRedBits = 5;								// bits of red
	ppfd->cRedShift = 0;							// shift for red
	ppfd->cGreenBits = 5;							// bits of green
	ppfd->cGreenShift = 0;							// shift for green
	ppfd->cBlueBits = 5;							// bits of blue
	ppfd->cBlueShift = 0;							// shift for blue
	ppfd->cAlphaBits = 1;							// bits of alpha
	ppfd->cAlphaShift = 0;							// shift bit ignored
	ppfd->cAccumBits = 0;							// no accumulation buffer
	ppfd->cAccumRedBits = 0;						// accum bits ignored
	ppfd->cAccumGreenBits = 0;						// accum bits ignored
	ppfd->cAccumBlueBits = 0;						// accum bits ignored
	ppfd->cAccumAlphaBits = 0;						// accum bits ignored
	ppfd->cDepthBits = 16;							// 16-bit z-buffer
	ppfd->cStencilBits = 0;							// no stencil buffer
	ppfd->cAuxBuffers = 0;							// no auxiliary buffer
	ppfd->iLayerType = PFD_MAIN_PLANE;				// main layer
	ppfd->bReserved = 0;							// reserved
	ppfd->dwLayerMask = 0;							// layer masks ignored
	ppfd->dwVisibleMask = 0;						// layer masks ignored
	ppfd->dwDamageMask = 0;							// layer masks ignored

	return 1;
}

DLL_EXPORT int APIENTRY wglGetPixelFormat( HDC hdc )
{
	return 1;
}

DLL_EXPORT BOOL APIENTRY wglSetPixelFormat( HDC hdc, int iPixelFormat, CONST PIXELFORMATDESCRIPTOR* ppfd )
{
	return TRUE;
}

DLL_EXPORT BOOL APIENTRY wglSwapBuffers( HDC hdc )
{
	// TODO: Implement
	return TRUE;
}

DLL_EXPORT void Download4444( void )
{
	gD3D.f4444 = TRUE;
}

DLL_EXPORT void QGL_D3DShared( D3DGLOBALS* d3dGShared )
{
	gD3D.lpDD4 = d3dGShared->lpDD4;
	gD3D.bFullscreen = d3dGShared->bFullscreen;
	OutputDebugString("setting dd ipntr in dll\n");
}

/*
** QGL_D3DInit
*/
DLL_EXPORT HINSTANCE QGL_D3DInit( void )
{
	// TODO: Implement



	


















	qglVertexPointer			= dllVertexPointer				= glVertexPointer;
	qglViewport					= dllViewport					= glViewport;

//	qwglCopyContext				= wglCopyContext;
//	qwglCreateContext			= wglCreateContext;
//	qwglCreateLayerContext		= wglCreateLayerContext;
//	qwglDeleteContext			= wglDeleteContext;
//	qwglDescribeLayerPlane		= wglDescribeLayerPlane;
//	qwglGetCurrentContext		= wglGetCurrentContext;
//	qwglGetCurrentDC			= wglGetCurrentDC;
//	qwglGetLayerPaletteEntries	= wglGetLayerPaletteEntries;
//	qwglGetProcAddress			= wglGetProcAddress;
//	qwglMakeCurrent				= wglMakeCurrent;
//	qwglRealizeLayerPalette		= wglRealizeLayerPalette;
//	qwglSetLayerPaletteEntries	= wglSetLayerPaletteEntries;
//	qwglShareLists				= wglShareLists;
//	qwglSwapLayerBuffers		= wglSwapLayerBuffers;
//	qwglUseFontBitmaps			= wglUseFontBitmaps;
//	qwglUseFontOutlines			= wglUseFontOutlines;
	qwglChoosePixelFormat		= wglChoosePixelFormat;
	qwglDescribePixelFormat		= wglDescribePixelFormat;
	qwglGetPixelFormat			= wglGetPixelFormat;
	qwglSetPixelFormat			= wglSetPixelFormat;
	qwglSwapBuffers				= wglSwapBuffers;
	qwglSwapIntervalEXT			= NULL;

	qglPointParameterfEXT		= NULL;
	qglPointParameterfvEXT		= NULL;
	qglColorTableEXT			= NULL;
	qglSelectTextureSGIS		= NULL;
	qglMTexCoord2fSGIS			= NULL;

	return (HINSTANCE)1;
}

// TODO: Implement