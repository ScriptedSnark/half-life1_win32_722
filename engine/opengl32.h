//-----------------------------------------------------------------------------
// Quake GL to DirectX wrapper
//-----------------------------------------------------------------------------

#ifndef _OPENGL32_H_
#define _OPENGL32_H_

#ifdef DECLSPEC_IMPORT
#undef DECLSPEC_IMPORT
#endif
#define DECLSPEC_IMPORT __declspec(dllexport)

#define CINTERFACE
#include <../dx6sdk/include/ddraw.h>
#include <../dx6sdk/include/d3d.h>

#undef DECLSPEC_IMPORT
#define DECLSPEC_IMPORT __declspec(dllimport)

typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef signed char GLbyte;
typedef short GLshort;
typedef int GLint;
typedef int GLsizei;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned int GLuint;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef double GLclampd;
typedef void GLvoid;

extern "C" {
extern	void ( APIENTRY * qglVertexPointer )( GLint size, GLenum type, GLsizei stride, const GLvoid* pointer );
extern	void ( APIENTRY * qglViewport )( GLint x, GLint y, GLsizei width, GLsizei height );

extern	BOOL ( APIENTRY * qwglCopyContext )( HGLRC hglrcSrc, HGLRC hglrcDst, UINT mask );
extern	HGLRC ( APIENTRY * qwglCreateContext )( HDC hdc );
extern	HGLRC ( APIENTRY * qwglCreateLayerContext )( HDC hdc, int iLayerPlan );
extern	BOOL ( APIENTRY * qwglDeleteContext )( HGLRC hglrc );
extern	BOOL ( APIENTRY * qwglDescribeLayerPlane )( HDC hdc, int iPixelFormat, int iLayerPlane, UINT nBytes, LPLAYERPLANEDESCRIPTOR plpd );
extern	HGLRC ( APIENTRY * qwglGetCurrentContext )( VOID );
extern	HDC  ( APIENTRY * qwglGetCurrentDC )( VOID );
extern	int  ( APIENTRY * qwglGetLayerPaletteEntries )( HDC hdc, int iLayerPlane, int iStart, int cEntries, COLORREF* pcr );
extern	PROC ( APIENTRY * qwglGetProcAddress )( LPCSTR lpszProc );
extern	BOOL ( APIENTRY * qwglMakeCurrent )( HDC hdc, HGLRC hglrc );
extern	BOOL ( APIENTRY * qwglRealizeLayerPalette )( HDC hdc, int iLayerPlane, BOOL bRealize );
extern	int  ( APIENTRY * qwglSetLayerPaletteEntries )( HDC hdc, int iLayerPlane, int iStart, int cEntries, CONST COLORREF* pcr );
extern	BOOL ( APIENTRY * qwglShareLists )( HGLRC hglrc1, HGLRC hglrc2 );
extern	BOOL ( APIENTRY * qwglSwapLayerBuffers )( HDC hdc, UINT fuPlanes );
extern	BOOL ( APIENTRY * qwglUseFontBitmaps )( HDC hdc, DWORD first, DWORD count, DWORD listBase );
extern	BOOL ( APIENTRY * qwglUseFontOutlines )( HDC hdc, DWORD first, DWORD count, DWORD listBase, FLOAT deviation, FLOAT extrusion, int format, LPGLYPHMETRICSFLOAT lpgmf );
extern	int  ( APIENTRY * qwglChoosePixelFormat )( HDC hdc, CONST PIXELFORMATDESCRIPTOR* ppfd );
extern	int  ( APIENTRY * qwglDescribePixelFormat )( HDC hdc, int iPixelFormat, UINT nBytes, LPPIXELFORMATDESCRIPTOR ppfd );
extern	int  ( APIENTRY * qwglGetPixelFormat )( HDC hdc );
extern	BOOL ( APIENTRY * qwglSetPixelFormat )( HDC hdc, int iPixelFormat, CONST PIXELFORMATDESCRIPTOR* ppfd );
extern	BOOL ( APIENTRY * qwglSwapBuffers )( HDC hdc );

extern	BOOL ( APIENTRY * qwglSwapIntervalEXT )( int interval );
extern	void ( APIENTRY * qglPointParameterfEXT )( GLenum param, GLfloat value );
extern	void ( APIENTRY * qglPointParameterfvEXT )( GLenum param, const GLfloat* value );
extern	void ( APIENTRY * qglColorTableEXT )( int, int, int, int, int, const void* );
extern	void ( APIENTRY * qglSelectTextureSGIS )( GLenum );
extern	void ( APIENTRY * qglMTexCoord2fSGIS )( GLenum, GLfloat, GLfloat );

extern	void ( APIENTRY * dllVertexPointer )( GLint size, GLenum type, GLsizei stride, const GLvoid* pointer );
extern	void ( APIENTRY * dllViewport )( GLint x, GLint y, GLsizei width, GLsizei height );
}

typedef struct D3D_s
{
	HWND			hWnd;	// Window handle
	HDC				hDC;	// Device context
	void*			lpDD4;
	int				bFullscreen;
	BOOL			f4444; // RGBA4444 format
} D3D_t;

#endif // _OPENGL32_H_