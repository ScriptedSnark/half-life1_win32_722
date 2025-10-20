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
extern	void ( APIENTRY * qglTexGend )( GLenum coord, GLenum pname, GLdouble param );
extern	void ( APIENTRY * qglTexGendv )( GLenum coord, GLenum pname, const GLdouble* params );
extern	void ( APIENTRY * qglTexGenf )( GLenum coord, GLenum pname, GLfloat param );
extern	void ( APIENTRY * qglTexGenfv )( GLenum coord, GLenum pname, const GLfloat* params );
extern	void ( APIENTRY * qglTexGeni )( GLenum coord, GLenum pname, GLint param );
extern	void ( APIENTRY * qglTexGeniv )( GLenum coord, GLenum pname, const GLint* params );
extern	void ( APIENTRY * qglTexImage1D )( GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid* pixels );
extern	void ( APIENTRY * qglTexImage2D )( GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* pixels );
extern	void ( APIENTRY * qglTexParameterf )( GLenum target, GLenum pname, GLfloat param );
extern	void ( APIENTRY * qglTexParameterfv )( GLenum target, GLenum pname, const GLfloat* params );
extern	void ( APIENTRY * qglTexParameteri )( GLenum target, GLenum pname, GLint param );
extern	void ( APIENTRY * qglTexParameteriv )( GLenum target, GLenum pname, const GLint* params );
extern	void ( APIENTRY * qglTexSubImage1D )( GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid* pixels );
extern	void ( APIENTRY * qglTexSubImage2D )( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels );
extern	void ( APIENTRY * qglTranslated )( GLdouble x, GLdouble y, GLdouble z );
extern	void ( APIENTRY * qglTranslatef )( GLfloat x, GLfloat y, GLfloat z );
extern	void ( APIENTRY * qglVertex2d )( GLdouble x, GLdouble y );
extern	void ( APIENTRY * qglVertex2dv )( const GLdouble* v );
extern	void ( APIENTRY * qglVertex2f )( GLfloat x, GLfloat y );
extern	void ( APIENTRY * qglVertex2fv )( const GLfloat* v );
extern	void ( APIENTRY * qglVertex2i )( GLint x, GLint y );
extern	void ( APIENTRY * qglVertex2iv )( const GLint* v );
extern	void ( APIENTRY * qglVertex2s )( GLshort x, GLshort y );
extern	void ( APIENTRY * qglVertex2sv )( const GLshort* v );
extern	void ( APIENTRY * qglVertex3d )( GLdouble x, GLdouble y, GLdouble z );
extern	void ( APIENTRY * qglVertex3dv )( const GLdouble* v );
extern	void ( APIENTRY * qglVertex3f )( GLfloat x, GLfloat y, GLfloat z );
extern	void ( APIENTRY * qglVertex3fv )( const GLfloat* v );
extern	void ( APIENTRY * qglVertex3i )( GLint x, GLint y, GLint z );
extern	void ( APIENTRY * qglVertex3iv )( const GLint* v );
extern	void ( APIENTRY * qglVertex3s )( GLshort x, GLshort y, GLshort z );
extern	void ( APIENTRY * qglVertex3sv )( const GLshort* v );
extern	void ( APIENTRY * qglVertex4d )( GLdouble x, GLdouble y, GLdouble z, GLdouble w );
extern	void ( APIENTRY * qglVertex4dv )( const GLdouble* v );
extern	void ( APIENTRY * qglVertex4f )( GLfloat x, GLfloat y, GLfloat z, GLfloat w );
extern	void ( APIENTRY * qglVertex4fv )( const GLfloat* v );
extern	void ( APIENTRY * qglVertex4i )( GLint x, GLint y, GLint z, GLint w );
extern	void ( APIENTRY * qglVertex4iv )( const GLint* v );
extern	void ( APIENTRY * qglVertex4s )( GLshort x, GLshort y, GLshort z, GLshort w );
extern	void ( APIENTRY * qglVertex4sv )( const GLshort* v );
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

extern	void ( APIENTRY * dllTexGend )( GLenum coord, GLenum pname, GLdouble param );
extern	void ( APIENTRY * dllTexGendv )( GLenum coord, GLenum pname, const GLdouble* params );
extern	void ( APIENTRY * dllTexGenf )( GLenum coord, GLenum pname, GLfloat param );
extern	void ( APIENTRY * dllTexGenfv )( GLenum coord, GLenum pname, const GLfloat* params );
extern	void ( APIENTRY * dllTexGeni )( GLenum coord, GLenum pname, GLint param );
extern	void ( APIENTRY * dllTexGeniv )( GLenum coord, GLenum pname, const GLint* params );
extern	void ( APIENTRY * dllTexImage1D )( GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid* pixels );
extern	void ( APIENTRY * dllTexImage2D )( GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* pixels );
extern	void ( APIENTRY * dllTexParameterf )( GLenum target, GLenum pname, GLfloat param );
extern	void ( APIENTRY * dllTexParameterfv )( GLenum target, GLenum pname, const GLfloat* params );
extern	void ( APIENTRY * dllTexParameteri )( GLenum target, GLenum pname, GLint param );
extern	void ( APIENTRY * dllTexParameteriv )( GLenum target, GLenum pname, const GLint* params );
extern	void ( APIENTRY * dllTexSubImage1D )( GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid* pixels );
extern	void ( APIENTRY * dllTexSubImage2D )( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels );
extern	void ( APIENTRY * dllTranslated )( GLdouble x, GLdouble y, GLdouble z );
extern	void ( APIENTRY * dllTranslatef )( GLfloat x, GLfloat y, GLfloat z );
extern	void ( APIENTRY * dllVertex2d )( GLdouble x, GLdouble y );
extern	void ( APIENTRY * dllVertex2dv )( const GLdouble* v );
extern	void ( APIENTRY * dllVertex2f )( GLfloat x, GLfloat y );
extern	void ( APIENTRY * dllVertex2fv )( const GLfloat* v );
extern	void ( APIENTRY * dllVertex2i )( GLint x, GLint y );
extern	void ( APIENTRY * dllVertex2iv )( const GLint* v );
extern	void ( APIENTRY * dllVertex2s )( GLshort x, GLshort y );
extern	void ( APIENTRY * dllVertex2sv )( const GLshort* v );
extern	void ( APIENTRY * dllVertex3d )( GLdouble x, GLdouble y, GLdouble z );
extern	void ( APIENTRY * dllVertex3dv )( const GLdouble* v );
extern	void ( APIENTRY * dllVertex3f )( GLfloat x, GLfloat y, GLfloat z );
extern	void ( APIENTRY * dllVertex3fv )( const GLfloat* v );
extern	void ( APIENTRY * dllVertex3i )( GLint x, GLint y, GLint z );
extern	void ( APIENTRY * dllVertex3iv )( const GLint* v );
extern	void ( APIENTRY * dllVertex3s )( GLshort x, GLshort y, GLshort z );
extern	void ( APIENTRY * dllVertex3sv )( const GLshort* v );
extern	void ( APIENTRY * dllVertex4d )( GLdouble x, GLdouble y, GLdouble z, GLdouble w );
extern	void ( APIENTRY * dllVertex4dv )( const GLdouble* v );
extern	void ( APIENTRY * dllVertex4f )( GLfloat x, GLfloat y, GLfloat z, GLfloat w );
extern	void ( APIENTRY * dllVertex4fv )( const GLfloat* v );
extern	void ( APIENTRY * dllVertex4i )( GLint x, GLint y, GLint z, GLint w );
extern	void ( APIENTRY * dllVertex4iv )( const GLint* v );
extern	void ( APIENTRY * dllVertex4s )( GLshort x, GLshort y, GLshort z, GLshort w );
extern	void ( APIENTRY * dllVertex4sv )( const GLshort* v );
extern	void ( APIENTRY * dllVertexPointer )( GLint size, GLenum type, GLsizei stride, const GLvoid* pointer );
extern	void ( APIENTRY * dllViewport )( GLint x, GLint y, GLsizei width, GLsizei height );
}

typedef struct D3D_s
{
	int				wndWidth;
	int				wndHeight;
	HWND			hWnd;	// Window handle
	HDC				hDC;	// Device context
	void*			lpDD4;
	int				bFullscreen;
	BOOL			f4444; // RGBA4444 format
} D3D_t;

#endif // _OPENGL32_H_