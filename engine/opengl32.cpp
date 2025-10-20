//-----------------------------------------------------------------------------
// Quake GL to DirectX wrapper
//-----------------------------------------------------------------------------

#include "opengl32.h"
#include "d3d_structs.h"

#ifdef _WIN32
#define DLL_EXPORT extern "C" __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

D3D_t gD3D;

// TODO: Implement

DLL_EXPORT void APIENTRY glSelectTextureSGIS( GLenum target )
{
	// TODO: Implement
}

DLL_EXPORT void APIENTRY glMTexCoord2fSGIS( GLenum target, GLfloat s, GLfloat t )
{
	// TODO: Implement
}



// TODO: Implement




























DLL_EXPORT void APIENTRY glTexGend( GLenum coord, GLenum pname, GLdouble param )
{
}

DLL_EXPORT void APIENTRY glTexGendv( GLenum coord, GLenum pname, const GLdouble* params )
{
}

DLL_EXPORT void APIENTRY glTexGenf( GLenum coord, GLenum pname, GLfloat param )
{
}

DLL_EXPORT void APIENTRY glTexGenfv( GLenum coord, GLenum pname, const GLfloat* params )
{
}

DLL_EXPORT void APIENTRY glTexGeni( GLenum coord, GLenum pname, GLint param )
{
}

DLL_EXPORT void APIENTRY glTexGeniv( GLenum coord, GLenum pname, const GLint* params )
{
}

DLL_EXPORT void APIENTRY glTexImage1D( GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid* pixels )
{
}

DLL_EXPORT void APIENTRY glTexImage2D( GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* pixels )
{
	// TODO: Implement
}

DLL_EXPORT void APIENTRY glTexParameterf( GLenum target, GLenum pname, GLfloat param )
{
	// TODO: Implement
}

DLL_EXPORT void APIENTRY glTexParameterfv( GLenum target, GLenum pname, const GLfloat* params )
{
}

DLL_EXPORT void APIENTRY glTexParameteri( GLenum target, GLenum pname, GLint param )
{
}

DLL_EXPORT void APIENTRY glTexParameteriv( GLenum target, GLenum pname, const GLint* params )
{
}

DLL_EXPORT void APIENTRY glTexSubImage1D( GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid* pixels )
{
}

DLL_EXPORT void APIENTRY glTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels )
{
	// TODO: Implement
}

DLL_EXPORT void APIENTRY glTranslated( GLdouble x, GLdouble y, GLdouble z )
{
}

DLL_EXPORT void APIENTRY glTranslatef( GLfloat x, GLfloat y, GLfloat z )
{
	// TODO: Implement
}

DLL_EXPORT void APIENTRY glVertex2d( GLdouble x, GLdouble y )
{
}

DLL_EXPORT void APIENTRY glVertex2dv( const GLdouble* v )
{
}

DLL_EXPORT void APIENTRY glVertex2f( GLfloat x, GLfloat y )
{
	// TODO: Implement
}

DLL_EXPORT void APIENTRY glVertex2fv( const GLfloat* v )
{
}

DLL_EXPORT void APIENTRY glVertex2i( GLint x, GLint y )
{
}

DLL_EXPORT void APIENTRY glVertex2iv( const GLint* v )
{
}

DLL_EXPORT void APIENTRY glVertex2s( GLshort x, GLshort y )
{
}

DLL_EXPORT void APIENTRY glVertex2sv( const GLshort* v )
{
}

DLL_EXPORT void APIENTRY glVertex3d( GLdouble x, GLdouble y, GLdouble z )
{
}

DLL_EXPORT void APIENTRY glVertex3dv( const GLdouble* v )
{
}

DLL_EXPORT void APIENTRY glVertex3f( GLfloat x, GLfloat y, GLfloat z )
{
	// TODO: Implement
}

DLL_EXPORT void APIENTRY glVertex3fv( const GLfloat* v )
{
	// TODO: Implement
}

DLL_EXPORT void APIENTRY glVertex3i( GLint x, GLint y, GLint z )
{
}

DLL_EXPORT void APIENTRY glVertex3iv( const GLint* v )
{
}

DLL_EXPORT void APIENTRY glVertex3s( GLshort x, GLshort y, GLshort z )
{
}

DLL_EXPORT void APIENTRY glVertex3sv( const GLshort* v )
{
}

DLL_EXPORT void APIENTRY glVertex4d( GLdouble x, GLdouble y, GLdouble z, GLdouble w )
{
}

DLL_EXPORT void APIENTRY glVertex4dv( const GLdouble* v )
{
}

DLL_EXPORT void APIENTRY glVertex4f( GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
}

DLL_EXPORT void APIENTRY glVertex4fv( const GLfloat* v )
{
}

DLL_EXPORT void APIENTRY glVertex4i( GLint x, GLint y, GLint z, GLint w )
{
}

DLL_EXPORT void APIENTRY glVertex4iv( const GLint* v )
{
}

DLL_EXPORT void APIENTRY glVertex4s( GLshort x, GLshort y, GLshort z, GLshort w )
{
}

DLL_EXPORT void APIENTRY glVertex4sv( const GLshort* v )
{
}

DLL_EXPORT void APIENTRY glVertexPointer( GLint size, GLenum type, GLsizei stride, const GLvoid* pointer )
{
}

DLL_EXPORT void APIENTRY glViewport( GLint x, GLint y, GLsizei width, GLsizei height )
{
	// TODO: Implement
}

DLL_EXPORT BOOL WINAPI wglCopyContext( HGLRC hglrcSrc, HGLRC hglrcDst, UINT mask )
{
	return FALSE;
}

DLL_EXPORT HGLRC WINAPI wglCreateContext( HDC hdc )
{
	LPDIRECTDRAW lpDD;
	DDSURFACEDESC ddsd;
	RECT rect;

	gD3D.hDC = hdc;
	gD3D.hWnd = WindowFromDC(hdc);

	GetClientRect(gD3D.hWnd, &rect);

	gD3D.wndWidth = (USHORT)rect.right;
	gD3D.wndHeight = (USHORT)rect.bottom;

	if (FAILED(CoInitialize(NULL)))
		return NULL;

	if (FAILED(DirectDrawCreate(NULL, &lpDD, NULL)))
	{
		CoUninitialize();
		return NULL;
	}

	ddsd.dwSize = sizeof(ddsd);
	if (FAILED(lpDD->lpVtbl->GetDisplayMode(lpDD, &ddsd)))
	{
		lpDD->lpVtbl->Release(lpDD);
		CoUninitialize();
		return NULL;
	}

	lpDD->lpVtbl->Release(lpDD);

	// TODO: Implement

	return (HGLRC)1;
}

DLL_EXPORT HGLRC WINAPI wglCreateLayerContext( HDC hdc, int iLayerPlan )
{
	return (HGLRC)1;
}

DLL_EXPORT BOOL WINAPI wglDeleteContext( HGLRC hglrc )
{
	// TODO: Implement

	CoUninitialize();

	return TRUE;
}

DLL_EXPORT BOOL WINAPI wglDescribeLayerPlane( HDC hdc, int iPixelFormat, int iLayerPlane, UINT nBytes, LPLAYERPLANEDESCRIPTOR plpd )
{
	return FALSE;
}

DLL_EXPORT HGLRC WINAPI wglGetCurrentContext( void )
{
	return (HGLRC)1;
}

DLL_EXPORT HDC WINAPI wglGetCurrentDC( void )
{
	return gD3D.hDC;
}

DLL_EXPORT int WINAPI wglGetLayerPaletteEntries( HDC hdc, int iLayerPlane, int iStart, int cEntries, COLORREF* pcr )
{
	return 0;
}

DLL_EXPORT PROC WINAPI wglGetProcAddress( LPCSTR lpszProc )
{
	if (!strcmp(lpszProc, "glMTexCoord2fSGIS"))
		return (PROC)glMTexCoord2fSGIS;
	else if (!strcmp(lpszProc, "glSelectTextureSGIS"))
		return (PROC)glSelectTextureSGIS;

	return NULL;
}

DLL_EXPORT BOOL WINAPI wglMakeCurrent( HDC hdc, HGLRC hglrc )
{
	return TRUE;
}

DLL_EXPORT BOOL WINAPI wglRealizeLayerPalette( HDC hdc, int iLayerPlane, BOOL bRealize )
{
	return FALSE;
}

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

	qglTexGend					= dllTexGend					= glTexGend;
	qglTexGendv					= dllTexGendv					= glTexGendv;
	qglTexGenf					= dllTexGenf					= glTexGenf;
	qglTexGenfv					= dllTexGenfv					= glTexGenfv;
	qglTexGeni					= dllTexGeni					= glTexGeni;
	qglTexGeniv					= dllTexGeniv					= glTexGeniv;
	qglTexImage1D				= dllTexImage1D					= glTexImage1D;
	qglTexImage2D				= dllTexImage2D					= glTexImage2D;
	qglTexParameterf			= dllTexParameterf				= glTexParameterf;
	qglTexParameterfv			= dllTexParameterfv				= glTexParameterfv;
	qglTexParameteri			= dllTexParameteri				= glTexParameteri;
	qglTexParameteriv			= dllTexParameteriv				= glTexParameteriv;
	qglTexSubImage1D			= dllTexSubImage1D				= glTexSubImage1D;
	qglTexSubImage2D			= dllTexSubImage2D				= glTexSubImage2D;
	qglTranslated				= dllTranslated					= glTranslated;
	qglTranslatef				= dllTranslatef					= glTranslatef;
	qglVertex2d					= dllVertex2d					= glVertex2d;
	qglVertex2dv				= dllVertex2dv					= glVertex2dv;
	qglVertex2f					= dllVertex2f					= glVertex2f;
	qglVertex2fv				= dllVertex2fv					= glVertex2fv;
	qglVertex2i					= dllVertex2i					= glVertex2i;
	qglVertex2iv				= dllVertex2iv					= glVertex2iv;
	qglVertex2s					= dllVertex2s					= glVertex2s;
	qglVertex2sv				= dllVertex2sv					= glVertex2sv;
	qglVertex3d					= dllVertex3d					= glVertex3d;
	qglVertex3dv				= dllVertex3dv					= glVertex3dv;
	qglVertex3f					= dllVertex3f					= glVertex3f;
	qglVertex3fv				= dllVertex3fv					= glVertex3fv;
	qglVertex3i					= dllVertex3i					= glVertex3i;
	qglVertex3iv				= dllVertex3iv					= glVertex3iv;
	qglVertex3s					= dllVertex3s					= glVertex3s;
	qglVertex3sv				= dllVertex3sv					= glVertex3sv;
	qglVertex4d					= dllVertex4d					= glVertex4d;
	qglVertex4dv				= dllVertex4dv					= glVertex4dv;
	qglVertex4f					= dllVertex4f					= glVertex4f;
	qglVertex4fv				= dllVertex4fv					= glVertex4fv;
	qglVertex4i					= dllVertex4i					= glVertex4i;
	qglVertex4iv				= dllVertex4iv					= glVertex4iv;
	qglVertex4s					= dllVertex4s					= glVertex4s;
	qglVertex4sv				= dllVertex4sv					= glVertex4sv;
	qglVertexPointer			= dllVertexPointer				= glVertexPointer;
	qglViewport					= dllViewport					= glViewport;

	qwglCopyContext				= wglCopyContext;
	qwglCreateContext			= wglCreateContext;
	qwglCreateLayerContext		= wglCreateLayerContext;
	qwglDeleteContext			= wglDeleteContext;
	qwglDescribeLayerPlane		= wglDescribeLayerPlane;
	qwglGetCurrentContext		= wglGetCurrentContext;
	qwglGetCurrentDC			= wglGetCurrentDC;
	qwglGetLayerPaletteEntries	= wglGetLayerPaletteEntries;
	qwglGetProcAddress			= wglGetProcAddress;
	qwglMakeCurrent				= wglMakeCurrent;
	qwglRealizeLayerPalette		= wglRealizeLayerPalette;
	qwglSetLayerPaletteEntries	= wglSetLayerPaletteEntries;
	qwglShareLists				= wglShareLists;
	qwglSwapLayerBuffers		= wglSwapLayerBuffers;
	qwglUseFontBitmaps			= wglUseFontBitmaps;
	qwglUseFontOutlines			= wglUseFontOutlines;
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