#ifndef QGL_H
#define QGL_H
#ifdef _WIN32
#pragma once
#endif

#if !defined ( GLQUAKE )
#error This should not be called by sw/swds.
#endif

#include "quakedef.h"

#ifdef WIN32
#include <Windows.h>
#else
typedef void* HDC;
typedef void* HGLRC;
#endif

#include <GL/gl.h>

HINSTANCE QGL_Init( char* pdllname );
void QGL_Shutdown( void );

void GLimp_EnableLogging( void );
void GLimp_LogNewFrame( void );

void GL_Config( void );
DLL_EXPORT void GL_Init( void );

#ifndef APIENTRY
#define APIENTRY
#endif

// windows systems use a function pointer for each call so we can load minidrivers

extern  void ( APIENTRY * qglAccum )( GLenum op, GLfloat value );
extern  void ( APIENTRY * qglAlphaFunc )( GLenum func, GLclampf ref );
extern  GLboolean ( APIENTRY * qglAreTexturesResident )( GLsizei n, const GLuint* textures, GLboolean* residences );
extern  void ( APIENTRY * qglArrayElement )( GLint i );
extern  void ( APIENTRY * qglBegin )( GLenum mode );
extern  void ( APIENTRY * qglBindTexture )( GLenum target, GLuint texture );
extern  void ( APIENTRY * qglBitmap )( GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte* bitmap );
extern  void ( APIENTRY * qglBlendFunc )( GLenum sfactor, GLenum dfactor );
extern  void ( APIENTRY * qglCallList )( GLuint list );
extern  void ( APIENTRY * qglCallLists )( GLsizei n, GLenum type, const GLvoid* lists );
extern	void ( APIENTRY * qglClear )( GLbitfield mask );
extern	void ( APIENTRY * qglClearAccum )( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha );
extern	void ( APIENTRY * qglClearColor )( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha );
extern	void ( APIENTRY * qglClearDepth )( GLclampd depth );
extern	void ( APIENTRY * qglClearIndex )( GLfloat c );
extern	void ( APIENTRY * qglClearStencil )( GLint s );
extern	void ( APIENTRY * qglClipPlane )( GLenum plane, const GLdouble* equation );
extern	void ( APIENTRY * qglColor3b )( GLbyte red, GLbyte green, GLbyte blue );
extern	void ( APIENTRY * qglColor3bv )( const GLbyte* v );
extern	void ( APIENTRY * qglColor3d )( GLdouble red, GLdouble green, GLdouble blue );
extern	void ( APIENTRY * qglColor3dv )( const GLdouble* v );
extern	void ( APIENTRY * qglColor3f )( GLfloat red, GLfloat green, GLfloat blue );
extern	void ( APIENTRY * qglColor3fv )( const GLfloat* v );
extern	void ( APIENTRY * qglColor3i )( GLint red, GLint green, GLint blue );
extern	void ( APIENTRY * qglColor3iv )( const GLint* v );
extern	void ( APIENTRY * qglColor3s )( GLshort red, GLshort green, GLshort blue );
extern	void ( APIENTRY * qglColor3sv )( const GLshort* v );
extern	void ( APIENTRY * qglColor3ub )( GLubyte red, GLubyte green, GLubyte blue );
extern	void ( APIENTRY * qglColor3ubv )( const GLubyte* v );
extern	void ( APIENTRY * qglColor3ui )( GLuint red, GLuint green, GLuint blue );
extern	void ( APIENTRY * qglColor3uiv )( const GLuint* v );
extern	void ( APIENTRY * qglColor3us )( GLushort red, GLushort green, GLushort blue );
extern	void ( APIENTRY * qglColor3usv )( const GLushort* v );
extern	void ( APIENTRY * qglColor4b )( GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha );
extern	void ( APIENTRY * qglColor4bv )( const GLbyte* v );
extern	void ( APIENTRY * qglColor4d )( GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha );
extern	void ( APIENTRY * qglColor4dv )( const GLdouble* v );
extern	void ( APIENTRY * qglColor4f )( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha );
extern	void ( APIENTRY * qglColor4fv )( const GLfloat* v );
extern	void ( APIENTRY * qglColor4i )( GLint red, GLint green, GLint blue, GLint alpha );
extern	void ( APIENTRY * qglColor4iv )( const GLint* v );
extern	void ( APIENTRY * qglColor4s )( GLshort red, GLshort green, GLshort blue, GLshort alpha );
extern	void ( APIENTRY * qglColor4sv )( const GLshort* v );
extern	void ( APIENTRY * qglColor4ub )( GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha );
extern	void ( APIENTRY * qglColor4ubv )( const GLubyte* v );
extern	void ( APIENTRY * qglColor4ui )( GLuint red, GLuint green, GLuint blue, GLuint alpha );
extern	void ( APIENTRY * qglColor4uiv )( const GLuint* v );
extern	void ( APIENTRY * qglColor4us )( GLushort red, GLushort green, GLushort blue, GLushort alpha );
extern	void ( APIENTRY * qglColor4usv )( const GLushort* v );
extern	void ( APIENTRY * qglColorMask )( GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha );
extern	void ( APIENTRY * qglColorMaterial )( GLenum face, GLenum mode );
extern	void ( APIENTRY * qglColorPointer )( GLint size, GLenum type, GLsizei stride, const GLvoid* pointer );
extern	void ( APIENTRY * qglCopyPixels )( GLint x, GLint y, GLsizei width, GLsizei height, GLenum type );
extern	void ( APIENTRY * qglCopyTexImage1D )( GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLint border );
extern	void ( APIENTRY * qglCopyTexImage2D )( GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border );
extern	void ( APIENTRY * qglCopyTexSubImage1D )( GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width );
extern	void ( APIENTRY * qglCopyTexSubImage2D )( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height );
extern	void ( APIENTRY * qglCullFace )( GLenum mode );
extern	void ( APIENTRY * qglDeleteLists )( GLuint list, GLsizei range );
extern	void ( APIENTRY * qglDeleteTextures )( GLsizei n, const GLuint* textures );
extern	void ( APIENTRY * qglDepthFunc )( GLenum func );
extern	void ( APIENTRY * qglDepthMask )( GLboolean flag );
extern	void ( APIENTRY * qglDepthRange )( GLclampd zNear, GLclampd zFar );
extern	void ( APIENTRY * qglDisable )( GLenum cap );
extern	void ( APIENTRY * qglDisableClientState )( GLenum array );
extern	void ( APIENTRY * qglDrawArrays )( GLenum mode, GLint first, GLsizei count );
extern	void ( APIENTRY * qglDrawBuffer )( GLenum mode );
extern	void ( APIENTRY * qglDrawElements )( GLenum mode, GLsizei count, GLenum type, const GLvoid* indices );
extern	void ( APIENTRY * qglDrawPixels )( GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels );
extern	void ( APIENTRY * qglEdgeFlag )( GLboolean flag );
extern	void ( APIENTRY * qglEdgeFlagPointer )( GLsizei stride, const GLvoid* pointer );
extern	void ( APIENTRY * qglEdgeFlagv )( const GLboolean* flag );
extern	void ( APIENTRY * qglEnable )( GLenum cap );
extern	void ( APIENTRY * qglEnableClientState )( GLenum array );
extern	void ( APIENTRY * qglEnd )( void );
extern	void ( APIENTRY * qglEndList )( void );








//void ( APIENTRY * qglViewport )( GLint x, GLint y, GLsizei width, GLsizei height );

extern	void ( APIENTRY * qglPointParameterfEXT )( GLenum param, GLfloat value );
extern	void ( APIENTRY * qglPointParameterfvEXT )( GLenum param, const GLfloat* value );
extern	void ( APIENTRY * qglColorTableEXT )( int, int, int, int, int, const void* );
extern	void ( APIENTRY * qglSelectTextureSGIS )( GLenum );
extern	void ( APIENTRY * qglMTexCoord2fSGIS )( GLenum, GLfloat, GLfloat );

#ifdef _WIN32

extern	int   ( WINAPI * qwglChoosePixelFormat )( HDC, CONST PIXELFORMATDESCRIPTOR * );
extern	int   ( WINAPI * qwglDescribePixelFormat )( HDC, int, UINT, LPPIXELFORMATDESCRIPTOR );
extern	int   ( WINAPI * qwglGetPixelFormat )( HDC );
extern	BOOL ( WINAPI * qwglSetPixelFormat )( HDC, int, CONST PIXELFORMATDESCRIPTOR* );
extern	BOOL ( WINAPI * qwglSwapBuffers )( HDC );

extern	BOOL ( WINAPI * qwglCopyContext )( HGLRC, HGLRC, UINT );
extern	HGLRC ( WINAPI * qwglCreateContext )( HDC );
extern	HGLRC ( WINAPI * qwglCreateLayerContext )( HDC, int );
extern	BOOL ( WINAPI * qwglDeleteContext )( HGLRC );
extern	HGLRC ( WINAPI * qwglGetCurrentContext )( VOID );
extern	HDC ( WINAPI * qwglGetCurrentDC )( VOID );
extern	PROC ( WINAPI * qwglGetProcAddress )( LPCSTR );
extern	BOOL ( WINAPI * qwglMakeCurrent )( HDC, HGLRC );
extern	BOOL ( WINAPI * qwglShareLists )( HGLRC, HGLRC );
extern	BOOL ( WINAPI * qwglUseFontBitmaps )( HDC, DWORD, DWORD, DWORD );

extern	BOOL ( WINAPI * qwglUseFontOutlines )( HDC, DWORD, DWORD, DWORD, FLOAT,
										   FLOAT, int, LPGLYPHMETRICSFLOAT );

extern	BOOL ( WINAPI * qwglDescribeLayerPlane )( HDC, int, int, UINT,
											LPLAYERPLANEDESCRIPTOR );
extern	int  ( WINAPI * qwglSetLayerPaletteEntries )( HDC, int, int, int,
												CONST COLORREF* );
extern	int  ( WINAPI * qwglGetLayerPaletteEntries )(HDC, int, int, int,
												COLORREF* );
extern	BOOL( WINAPI * qwglRealizeLayerPalette )( HDC, int, BOOL );
extern	BOOL( WINAPI * qwglSwapLayerBuffers )( HDC, UINT );

extern	BOOL ( WINAPI * qwglSwapIntervalEXT )( int interval );

extern	BOOL ( WINAPI * qwglGetDeviceGammaRampEXT )( unsigned char* , unsigned char* , unsigned char* );
extern	BOOL ( WINAPI * qwglSetDeviceGammaRampEXT )( const unsigned char* , const unsigned char* , const unsigned char* );

#endif

#endif // QGL_H