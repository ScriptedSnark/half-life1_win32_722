/*
** qgl.c
**
** This file implements the operating system binding of GL to QGL function
** pointers.  When doing a port of Quake3 you must implement the following
** two functions:
**
** QGL_Init() - loads libraries, assigns function pointers, etc.
** QGL_Shutdown() - unloads libraries, NULLs function pointers
*/

#include <time.h>

#include "quakedef.h"

typedef struct glwstate_s
{
	HINSTANCE	hInstance;
	void* wndproc;

	HINSTANCE hinstOpenGL;	// HINSTANCE for the OpenGL library

	qboolean minidriver;
	qboolean allowdisplaydepthchange;
	qboolean mcd_accelerated;

	FILE* log_fp;
} glwstate_t;

glwstate_t glw_state;

int   ( WINAPI * qwglChoosePixelFormat )( HDC, CONST PIXELFORMATDESCRIPTOR * );
int   ( WINAPI * qwglDescribePixelFormat )( HDC, int, UINT, LPPIXELFORMATDESCRIPTOR );
int   ( WINAPI * qwglGetPixelFormat )( HDC );
BOOL ( WINAPI * qwglSetPixelFormat )( HDC, int, CONST PIXELFORMATDESCRIPTOR* );
BOOL ( WINAPI * qwglSwapBuffers )( HDC );

BOOL ( WINAPI * qwglCopyContext )( HGLRC, HGLRC, UINT );
HGLRC ( WINAPI * qwglCreateContext )( HDC );
HGLRC ( WINAPI * qwglCreateLayerContext )( HDC, int );
BOOL ( WINAPI * qwglDeleteContext )( HGLRC );
HGLRC ( WINAPI * qwglGetCurrentContext )( VOID );
HDC ( WINAPI * qwglGetCurrentDC )( VOID );
PROC ( WINAPI * qwglGetProcAddress )( LPCSTR );
BOOL ( WINAPI * qwglMakeCurrent )( HDC, HGLRC );
BOOL ( WINAPI * qwglShareLists )( HGLRC, HGLRC );
BOOL ( WINAPI * qwglUseFontBitmaps )( HDC, DWORD, DWORD, DWORD );

BOOL ( WINAPI * qwglUseFontOutlines )( HDC, DWORD, DWORD, DWORD, FLOAT,
										   FLOAT, int, LPGLYPHMETRICSFLOAT );

BOOL ( WINAPI * qwglDescribeLayerPlane )( HDC, int, int, UINT,
											LPLAYERPLANEDESCRIPTOR );
int  ( WINAPI * qwglSetLayerPaletteEntries )( HDC, int, int, int,
												CONST COLORREF* );
int  ( WINAPI * qwglGetLayerPaletteEntries )(HDC, int, int, int,
												COLORREF* );
BOOL( WINAPI * qwglRealizeLayerPalette )( HDC, int, BOOL );
BOOL( WINAPI * qwglSwapLayerBuffers )( HDC, UINT );




void ( APIENTRY * qglAccum )( GLenum op, GLfloat value );
void ( APIENTRY * qglAlphaFunc )( GLenum func, GLclampf ref );
GLboolean ( APIENTRY * qglAreTexturesResident )( GLsizei n, const GLuint* textures, GLboolean* residences );
void ( APIENTRY * qglArrayElement )( GLint i );
void ( APIENTRY * qglBegin )( GLenum mode );
void ( APIENTRY * qglBindTexture )( GLenum target, GLuint texture );
void ( APIENTRY * qglBitmap )( GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte* bitmap );
void ( APIENTRY * qglBlendFunc )( GLenum sfactor, GLenum dfactor );
void ( APIENTRY * qglCallList )( GLuint list );
void ( APIENTRY * qglCallLists )( GLsizei n, GLenum type, const GLvoid* lists );
void ( APIENTRY * qglClear )( GLbitfield mask );
void ( APIENTRY * qglClearAccum )( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha );
void ( APIENTRY * qglClearColor )( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha );
void ( APIENTRY * qglClearDepth )( GLclampd depth );
void ( APIENTRY * qglClearIndex )( GLfloat c );
void ( APIENTRY * qglClearStencil )( GLint s );
void ( APIENTRY * qglClipPlane )( GLenum plane, const GLdouble* equation );
void ( APIENTRY * qglColor3b )( GLbyte red, GLbyte green, GLbyte blue );
void ( APIENTRY * qglColor3bv )( const GLbyte* v );
void ( APIENTRY * qglColor3d )( GLdouble red, GLdouble green, GLdouble blue );
void ( APIENTRY * qglColor3dv )( const GLdouble* v );
void ( APIENTRY * qglColor3f )( GLfloat red, GLfloat green, GLfloat blue );
void ( APIENTRY * qglColor3fv )( const GLfloat* v );
void ( APIENTRY * qglColor3i )( GLint red, GLint green, GLint blue );
void ( APIENTRY * qglColor3iv )( const GLint* v );
void ( APIENTRY * qglColor3s )( GLshort red, GLshort green, GLshort blue );
void ( APIENTRY * qglColor3sv )( const GLshort* v );
void ( APIENTRY * qglColor3ub )( GLubyte red, GLubyte green, GLubyte blue );
void ( APIENTRY * qglColor3ubv )( const GLubyte* v );
void ( APIENTRY * qglColor3ui )( GLuint red, GLuint green, GLuint blue );
void ( APIENTRY * qglColor3uiv )( const GLuint* v );
void ( APIENTRY * qglColor3us )( GLushort red, GLushort green, GLushort blue );
void ( APIENTRY * qglColor3usv )( const GLushort* v );
void ( APIENTRY * qglColor4b )( GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha );
void ( APIENTRY * qglColor4bv )( const GLbyte* v );
void ( APIENTRY * qglColor4d )( GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha );
void ( APIENTRY * qglColor4dv )( const GLdouble* v );
void ( APIENTRY * qglColor4f )( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha );
void ( APIENTRY * qglColor4fv )( const GLfloat* v );
void ( APIENTRY * qglColor4i )( GLint red, GLint green, GLint blue, GLint alpha );
void ( APIENTRY * qglColor4iv )( const GLint* v );
void ( APIENTRY * qglColor4s )( GLshort red, GLshort green, GLshort blue, GLshort alpha );
void ( APIENTRY * qglColor4sv )( const GLshort* v );
void ( APIENTRY * qglColor4ub )( GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha );
void ( APIENTRY * qglColor4ubv )( const GLubyte* v );
void ( APIENTRY * qglColor4ui )( GLuint red, GLuint green, GLuint blue, GLuint alpha );
void ( APIENTRY * qglColor4uiv )( const GLuint* v );
void ( APIENTRY * qglColor4us )( GLushort red, GLushort green, GLushort blue, GLushort alpha );
void ( APIENTRY * qglColor4usv )( const GLushort* v );
void ( APIENTRY * qglColorMask )( GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha );
void ( APIENTRY * qglColorMaterial )( GLenum face, GLenum mode );
void ( APIENTRY * qglColorPointer )( GLint size, GLenum type, GLsizei stride, const GLvoid* pointer );
void ( APIENTRY * qglCopyPixels )( GLint x, GLint y, GLsizei width, GLsizei height, GLenum type );
void ( APIENTRY * qglCopyTexImage1D )( GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLint border );
void ( APIENTRY * qglCopyTexImage2D )( GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border );
void ( APIENTRY * qglCopyTexSubImage1D )( GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width );
void ( APIENTRY * qglCopyTexSubImage2D )( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height );
void ( APIENTRY * qglCullFace )( GLenum mode );
void ( APIENTRY * qglDeleteLists )( GLuint list, GLsizei range );
void ( APIENTRY * qglDeleteTextures )( GLsizei n, const GLuint* textures );
void ( APIENTRY * qglDepthFunc )( GLenum func );
void ( APIENTRY * qglDepthMask )( GLboolean flag );
void ( APIENTRY * qglDepthRange )( GLclampd zNear, GLclampd zFar );
void ( APIENTRY * qglDisable )( GLenum cap );
void ( APIENTRY * qglDisableClientState )( GLenum array );
void ( APIENTRY * qglDrawArrays )( GLenum mode, GLint first, GLsizei count );
void ( APIENTRY * qglDrawBuffer )( GLenum mode );
void ( APIENTRY * qglDrawElements )( GLenum mode, GLsizei count, GLenum type, const GLvoid* indices );
void ( APIENTRY * qglDrawPixels )( GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels );
void ( APIENTRY * qglEdgeFlag )( GLboolean flag );
void ( APIENTRY * qglEdgeFlagPointer )( GLsizei stride, const GLvoid* pointer );
void ( APIENTRY * qglEdgeFlagv )( const GLboolean* flag );
void ( APIENTRY * qglEnable )( GLenum cap );
void ( APIENTRY * qglEnableClientState )( GLenum array );
void ( APIENTRY * qglEnd )( void );
void ( APIENTRY * qglEndList )( void );
void ( APIENTRY * qglEvalCoord1d )( GLdouble u );
void ( APIENTRY * qglEvalCoord1dv )( const GLdouble* u );
void ( APIENTRY * qglEvalCoord1f )( GLfloat u );
void ( APIENTRY * qglEvalCoord1fv )( const GLfloat* u );
void ( APIENTRY * qglEvalCoord2d )( GLdouble u, GLdouble v );
void ( APIENTRY * qglEvalCoord2dv )( const GLdouble* u );
void ( APIENTRY * qglEvalCoord2f )( GLfloat u, GLfloat v );
void ( APIENTRY * qglEvalCoord2fv )( const GLfloat* u );
void ( APIENTRY * qglEvalMesh1 )( GLenum mode, GLint i1, GLint i2 );
void ( APIENTRY * qglEvalMesh2 )( GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2 );
void ( APIENTRY * qglEvalPoint1 )( GLint i );
void ( APIENTRY * qglEvalPoint2 )( GLint i, GLint j );
void ( APIENTRY * qglFeedbackBuffer )( GLsizei size, GLenum type, GLfloat* buffer );
void ( APIENTRY * qglFinish )( void );
void ( APIENTRY * qglFlush )( void );
void ( APIENTRY * qglFogf )( GLenum pname, GLfloat param );
void ( APIENTRY * qglFogfv )( GLenum pname, const GLfloat* params );
void ( APIENTRY * qglFogi )( GLenum pname, GLint param );
void ( APIENTRY * qglFogiv )( GLenum pname, const GLint* params );
void ( APIENTRY * qglFrontFace )( GLenum mode );
void ( APIENTRY * qglFrustum )( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar );
GLuint ( APIENTRY * qglGenLists )( GLsizei range );
void ( APIENTRY * qglGenTextures )( GLsizei n, GLuint* textures );
void ( APIENTRY * qglGetBooleanv )( GLenum pname, GLboolean* params );
void ( APIENTRY * qglGetClipPlane )( GLenum plane, GLdouble* equation );
void ( APIENTRY * qglGetDoublev )( GLenum pname, GLdouble* params );
GLenum ( APIENTRY * qglGetError )( void );
void ( APIENTRY * qglGetFloatv )( GLenum pname, GLfloat* params );
void ( APIENTRY * qglGetIntegerv )( GLenum pname, GLint* params );
void ( APIENTRY * qglGetLightfv )( GLenum light, GLenum pname, GLfloat* params );
void ( APIENTRY * qglGetLightiv )( GLenum light, GLenum pname, GLint* params );
void ( APIENTRY * qglGetMapdv )( GLenum target, GLenum query, GLdouble* v );
void ( APIENTRY * qglGetMapfv )( GLenum target, GLenum query, GLfloat* v );
void ( APIENTRY * qglGetMapiv )( GLenum target, GLenum query, GLint* v );
void ( APIENTRY * qglGetMaterialfv )( GLenum face, GLenum pname, GLfloat* params);
void ( APIENTRY * qglGetMaterialiv )( GLenum face, GLenum pname, GLint* params);
void ( APIENTRY * qglGetPixelMapfv )( GLenum map, GLfloat* values );
void ( APIENTRY * qglGetPixelMapuiv )( GLenum map, GLuint* values );
void ( APIENTRY * qglGetPixelMapusv )( GLenum map, GLushort* values );
void ( APIENTRY * qglGetPointerv )( GLenum pname, GLvoid** params );
void ( APIENTRY * qglGetPolygonStipple )( GLubyte* mask );
const GLubyte * ( APIENTRY * qglGetString )( GLenum name );
void ( APIENTRY * qglGetTexEnvfv )( GLenum target, GLenum pname, GLfloat* params );
void ( APIENTRY * qglGetTexEnviv )( GLenum target, GLenum pname, GLint* params );
void ( APIENTRY * qglGetTexGendv )( GLenum coord, GLenum pname, GLdouble* params );
void ( APIENTRY * qglGetTexGenfv )( GLenum coord, GLenum pname, GLfloat* params );
void ( APIENTRY * qglGetTexGeniv )( GLenum coord, GLenum pname, GLint* params );
void ( APIENTRY * qglGetTexImage )( GLenum target, GLint level, GLenum format, GLenum type, GLvoid* pixels );
















//void ( APIENTRY * qglViewport )( GLint x, GLint y, GLsizei width, GLsizei height );

BOOL ( WINAPI * qwglSwapIntervalEXT )( int interval );
BOOL ( WINAPI * qwglGetDeviceGammaRampEXT )( unsigned char* , unsigned char* , unsigned char* );
BOOL ( WINAPI * qwglSetDeviceGammaRampEXT )( const unsigned char* , const unsigned char* , const unsigned char* );

void ( APIENTRY * qglPointParameterfEXT )( GLenum param, GLfloat value );
void ( APIENTRY * qglPointParameterfvEXT )( GLenum param, const GLfloat* value );
void ( APIENTRY * qglColorTableEXT )( int, int, int, int, int, const void* );
void ( APIENTRY * qglSelectTextureSGIS )( GLenum );
void ( APIENTRY * qglMTexCoord2fSGIS )( GLenum, GLfloat, GLfloat );








static void ( APIENTRY * dllAccum )( GLenum op, GLfloat value );
static void ( APIENTRY * dllAlphaFunc )( GLenum func, GLclampf ref );
GLboolean ( APIENTRY * dllAreTexturesResident )( GLsizei n, const GLuint* textures, GLboolean* residences );
static void ( APIENTRY * dllArrayElement )( GLint i );
static void ( APIENTRY * dllBegin )( GLenum mode );
static void ( APIENTRY * dllBindTexture )( GLenum target, GLuint texture );
static void ( APIENTRY * dllBitmap )( GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte* bitmap );
static void ( APIENTRY * dllBlendFunc )( GLenum sfactor, GLenum dfactor );
static void ( APIENTRY * dllCallList )( GLuint list );
static void ( APIENTRY * dllCallLists )( GLsizei n, GLenum type, const GLvoid* lists );
static void ( APIENTRY * dllClear )(GLbitfield mask);
static void ( APIENTRY * dllClearAccum )( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha );
static void ( APIENTRY * dllClearColor )( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha );
static void ( APIENTRY * dllClearDepth )( GLclampd depth );
static void ( APIENTRY * dllClearIndex )( GLfloat c );
static void ( APIENTRY * dllClearStencil )( GLint s );
static void ( APIENTRY * dllClipPlane )( GLenum plane, const GLdouble* equation );
static void ( APIENTRY * dllColor3b )( GLbyte red, GLbyte green, GLbyte blue );
static void ( APIENTRY * dllColor3bv )( const GLbyte* v );
static void ( APIENTRY * dllColor3d )( GLdouble red, GLdouble green, GLdouble blue );
static void ( APIENTRY * dllColor3dv )( const GLdouble* v );
static void ( APIENTRY * dllColor3f )( GLfloat red, GLfloat green, GLfloat blue );
static void ( APIENTRY * dllColor3fv )( const GLfloat* v );
static void ( APIENTRY * dllColor3i )( GLint red, GLint green, GLint blue );
static void ( APIENTRY * dllColor3iv )( const GLint* v );
static void ( APIENTRY * dllColor3s )( GLshort red, GLshort green, GLshort blue );
static void ( APIENTRY * dllColor3sv )( const GLshort* v );
static void ( APIENTRY * dllColor3ub )( GLubyte red, GLubyte green, GLubyte blue );
static void ( APIENTRY * dllColor3ubv )( const GLubyte* v );
static void ( APIENTRY * dllColor3ui )( GLuint red, GLuint green, GLuint blue );
static void ( APIENTRY * dllColor3uiv )( const GLuint* v );
static void ( APIENTRY * dllColor3us )( GLushort red, GLushort green, GLushort blue );
static void ( APIENTRY * dllColor3usv )( const GLushort* v );
static void ( APIENTRY * dllColor4b )( GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha );
static void ( APIENTRY * dllColor4bv )( const GLbyte* v );
static void ( APIENTRY * dllColor4d )( GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha );
static void ( APIENTRY * dllColor4dv )( const GLdouble* v );
static void ( APIENTRY * dllColor4f )( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha );
static void ( APIENTRY * dllColor4fv )( const GLfloat* v );
static void ( APIENTRY * dllColor4i )( GLint red, GLint green, GLint blue, GLint alpha );
static void ( APIENTRY * dllColor4iv )( const GLint* v );
static void ( APIENTRY * dllColor4s )( GLshort red, GLshort green, GLshort blue, GLshort alpha );
static void ( APIENTRY * dllColor4sv )( const GLshort* v );
static void ( APIENTRY * dllColor4ub )( GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha );
static void ( APIENTRY * dllColor4ubv )( const GLubyte* v );
static void ( APIENTRY * dllColor4ui )( GLuint red, GLuint green, GLuint blue, GLuint alpha );
static void ( APIENTRY * dllColor4uiv )( const GLuint* v );
static void ( APIENTRY * dllColor4us )( GLushort red, GLushort green, GLushort blue, GLushort alpha );
static void ( APIENTRY * dllColor4usv )( const GLushort* v );
static void ( APIENTRY * dllColorMask )( GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha );
static void ( APIENTRY * dllColorMaterial )( GLenum face, GLenum mode );
static void ( APIENTRY * dllColorPointer )( GLint size, GLenum type, GLsizei stride, const GLvoid* pointer );
static void ( APIENTRY * dllCopyPixels )( GLint x, GLint y, GLsizei width, GLsizei height, GLenum type );
static void ( APIENTRY * dllCopyTexImage1D )( GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLint border );
static void ( APIENTRY * dllCopyTexImage2D )( GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border );
static void ( APIENTRY * dllCopyTexSubImage1D )( GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width );
static void ( APIENTRY * dllCopyTexSubImage2D )( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height );
static void ( APIENTRY * dllCullFace )( GLenum mode );
static void ( APIENTRY * dllDeleteLists )( GLuint list, GLsizei range );
static void ( APIENTRY * dllDeleteTextures )( GLsizei n, const GLuint* textures );
static void ( APIENTRY * dllDepthFunc )( GLenum func );
static void ( APIENTRY * dllDepthMask )( GLboolean flag );
static void ( APIENTRY * dllDepthRange )( GLclampd zNear, GLclampd zFar );
static void ( APIENTRY * dllDisable )( GLenum cap );
static void ( APIENTRY * dllDisableClientState )( GLenum array );
static void ( APIENTRY * dllDrawArrays )( GLenum mode, GLint first, GLsizei count );
static void ( APIENTRY * dllDrawBuffer )( GLenum mode );
static void ( APIENTRY * dllDrawElements )( GLenum mode, GLsizei count, GLenum type, const GLvoid* indices );
static void ( APIENTRY * dllDrawPixels )( GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels );
static void ( APIENTRY * dllEdgeFlag )( GLboolean flag );
static void ( APIENTRY * dllEdgeFlagPointer )( GLsizei stride, const GLvoid* pointer );
static void ( APIENTRY * dllEdgeFlagv )( const GLboolean* flag );
static void ( APIENTRY * dllEnable )( GLenum cap );
static void ( APIENTRY * dllEnableClientState )( GLenum array );
static void ( APIENTRY * dllEnd )( void );
static void ( APIENTRY * dllEndList )( void );
static void ( APIENTRY * dllEvalCoord1d )( GLdouble u );
static void ( APIENTRY * dllEvalCoord1dv )( const GLdouble* u );
static void ( APIENTRY * dllEvalCoord1f )( GLfloat u );
static void ( APIENTRY * dllEvalCoord1fv )( const GLfloat* u );
static void ( APIENTRY * dllEvalCoord2d )( GLdouble u, GLdouble v );
static void ( APIENTRY * dllEvalCoord2dv )( const GLdouble* u );
static void ( APIENTRY * dllEvalCoord2f )( GLfloat u, GLfloat v );
static void ( APIENTRY * dllEvalCoord2fv )( const GLfloat* u );
static void ( APIENTRY * dllEvalMesh1 )( GLenum mode, GLint i1, GLint i2 );
static void ( APIENTRY * dllEvalMesh2 )( GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2 );
static void ( APIENTRY * dllEvalPoint1 )( GLint i );
static void ( APIENTRY * dllEvalPoint2 )( GLint i, GLint j );
static void ( APIENTRY * dllFeedbackBuffer )( GLsizei size, GLenum type, GLfloat* buffer );
static void ( APIENTRY * dllFinish )( void );
static void ( APIENTRY * dllFlush )( void );
static void ( APIENTRY * dllFogf )( GLenum pname, GLfloat param );
static void ( APIENTRY * dllFogfv )( GLenum pname, const GLfloat* params );
static void ( APIENTRY * dllFogi )( GLenum pname, GLint param );
static void ( APIENTRY * dllFogiv )( GLenum pname, const GLint* params );
static void ( APIENTRY * dllFrontFace )( GLenum mode );
static void ( APIENTRY * dllFrustum )( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar );
GLuint ( APIENTRY * dllGenLists )( GLsizei range );
static void ( APIENTRY * dllGenTextures )( GLsizei n, GLuint* textures );
static void ( APIENTRY * dllGetBooleanv )( GLenum pname, GLboolean* params );
static void ( APIENTRY * dllGetClipPlane )( GLenum plane, GLdouble* equation );
static void ( APIENTRY * dllGetDoublev )( GLenum pname, GLdouble* params );
GLenum ( APIENTRY * dllGetError )( void );
static void ( APIENTRY * dllGetFloatv )( GLenum pname, GLfloat* params );
static void ( APIENTRY * dllGetIntegerv )( GLenum pname, GLint* params );
static void ( APIENTRY * dllGetLightfv )( GLenum light, GLenum pname, GLfloat* params );
static void ( APIENTRY * dllGetLightiv )( GLenum light, GLenum pname, GLint* params );
static void ( APIENTRY * dllGetMapdv )( GLenum target, GLenum query, GLdouble* v );
static void ( APIENTRY * dllGetMapfv )( GLenum target, GLenum query, GLfloat* v );
static void ( APIENTRY * dllGetMapiv )( GLenum target, GLenum query, GLint* v );
static void ( APIENTRY * dllGetMaterialfv )( GLenum face, GLenum pname, GLfloat* params );
static void ( APIENTRY * dllGetMaterialiv )( GLenum face, GLenum pname, GLint* params );
static void ( APIENTRY * dllGetPixelMapfv )( GLenum map, GLfloat* values );
static void ( APIENTRY * dllGetPixelMapuiv )( GLenum map, GLuint* values );
static void ( APIENTRY * dllGetPixelMapusv )( GLenum map, GLushort* values );
static void ( APIENTRY * dllGetPointerv )( GLenum pname, GLvoid** params );
static void ( APIENTRY * dllGetPolygonStipple )( GLubyte* mask );
const GLubyte * ( APIENTRY * dllGetString )( GLenum name );
static void ( APIENTRY * dllGetTexEnvfv )( GLenum target, GLenum pname, GLfloat* params );
static void ( APIENTRY * dllGetTexEnviv )( GLenum target, GLenum pname, GLint* params );
static void ( APIENTRY * dllGetTexGendv )( GLenum coord, GLenum pname, GLdouble* params );
static void ( APIENTRY * dllGetTexGenfv )( GLenum coord, GLenum pname, GLfloat* params );
static void ( APIENTRY * dllGetTexGeniv )( GLenum coord, GLenum pname, GLint* params );
static void ( APIENTRY * dllGetTexImage )( GLenum target, GLint level, GLenum format, GLenum type, GLvoid* pixels );







static void APIENTRY logAccum( GLenum op, GLfloat value )
{
	fprintf(glw_state.log_fp, "glAccum\n");
	dllAccum(op, value);
}

static void APIENTRY logAlphaFunc( GLenum func, GLclampf ref )
{
	fprintf(glw_state.log_fp, "glAlphaFunc( 0x%x, %f )\n", func, ref);
	dllAlphaFunc(func, ref);
}

static GLboolean APIENTRY logAreTexturesResident( GLsizei n, const GLuint *textures, GLboolean *residences )
{
	fprintf(glw_state.log_fp, "glAreTexturesResident\n");
	return dllAreTexturesResident(n, textures, residences);
}

static void APIENTRY logArrayElement( GLint i )
{
	fprintf(glw_state.log_fp, "glArrayElement\n");
	dllArrayElement(i);
}

static void APIENTRY logBegin( GLenum mode )
{
	fprintf(glw_state.log_fp, "glBegin( 0x%x )\n", mode);
	dllBegin(mode);
}

static void APIENTRY logBindTexture( GLenum target, GLuint texture )
{
	fprintf(glw_state.log_fp, "glBindTexture( 0x%x, %u )\n", target, texture);
	dllBindTexture(target, texture);
}

static void APIENTRY logBitmap( GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap )
{
	fprintf(glw_state.log_fp, "glBitmap\n");
	dllBitmap(width, height, xorig, yorig, xmove, ymove, bitmap);
}

static void APIENTRY logBlendFunc( GLenum sfactor, GLenum dfactor )
{
	fprintf(glw_state.log_fp, "glBlendFunc( 0x%x, 0x%x )\n", sfactor, dfactor);
	dllBlendFunc(sfactor, dfactor);
}

static void APIENTRY logCallList( GLuint list )
{
	fprintf(glw_state.log_fp, "glCallList( %u )\n", list);
	dllCallList(list);
}

static void APIENTRY logCallLists( GLsizei n, GLenum type, const void *lists )
{
	fprintf(glw_state.log_fp, "glCallLists\n");
	dllCallLists(n, type, lists);
}

static void APIENTRY logClear( GLbitfield mask )
{
	fprintf(glw_state.log_fp, "glClear\n");
	dllClear(mask);
}

static void APIENTRY logClearAccum( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha )
{
	fprintf(glw_state.log_fp, "glClearAccum\n");
	dllClearAccum(red, green, blue, alpha);
}

static void APIENTRY logClearColor( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha )
{
	fprintf(glw_state.log_fp, "glClearColor\n");
	dllClearColor(red, green, blue, alpha);
}

static void APIENTRY logClearDepth( GLclampd depth )
{
	fprintf(glw_state.log_fp, "glClearDepth\n");
	dllClearDepth(depth);
}

static void APIENTRY logClearIndex( GLfloat c )
{
	fprintf(glw_state.log_fp, "glClearIndex\n");
	dllClearIndex(c);
}

static void APIENTRY logClearStencil( GLint s )
{
	fprintf(glw_state.log_fp, "glClearStencil\n");
	dllClearStencil(s);
}

static void APIENTRY logClipPlane( GLenum plane, const GLdouble* equation )
{
	fprintf(glw_state.log_fp, "glClipPlane\n");
	dllClipPlane(plane, equation);
}

static void APIENTRY logColor3b( GLbyte red, GLbyte green, GLbyte blue )
{
	fprintf(glw_state.log_fp, "glColor3b\n");
	dllColor3b(red, green, blue);
}

static void APIENTRY logColor3bv( const GLbyte* v )
{
	fprintf(glw_state.log_fp, "glColor3bv\n");
	dllColor3bv(v);
}

static void APIENTRY logColor3d( GLdouble red, GLdouble green, GLdouble blue )
{
	fprintf(glw_state.log_fp, "glColor3d\n");
	dllColor3d(red, green, blue);
}

static void APIENTRY logColor3dv( const GLdouble* v )
{
	fprintf(glw_state.log_fp, "glColor3dv\n");
	dllColor3dv(v);
}

static void APIENTRY logColor3f( GLfloat red, GLfloat green, GLfloat blue )
{
	fprintf(glw_state.log_fp, "glColor3f\n");
	dllColor3f(red, green, blue);
}

static void APIENTRY logColor3fv( const GLfloat* v )
{
	fprintf(glw_state.log_fp, "glColor3fv\n");
	dllColor3fv(v);
}

static void APIENTRY logColor3i( GLint red, GLint green, GLint blue )
{
	fprintf(glw_state.log_fp, "glColor3i\n");
	dllColor3i(red, green, blue);
}

static void APIENTRY logColor3iv( const GLint* v )
{
	fprintf(glw_state.log_fp, "glColor3iv\n");
	dllColor3iv(v);
}

static void APIENTRY logColor3s( GLshort red, GLshort green, GLshort blue )
{
	fprintf(glw_state.log_fp, "glColor3s\n");
	dllColor3s(red, green, blue);
}

static void APIENTRY logColor3sv( const GLshort* v )
{
	fprintf(glw_state.log_fp, "glColor3sv\n");
	dllColor3sv(v);
}

static void APIENTRY logColor3ub( GLubyte red, GLubyte green, GLubyte blue )
{
	fprintf(glw_state.log_fp, "glColor3ub\n");
	dllColor3ub(red, green, blue);
}

static void APIENTRY logColor3ubv( const GLubyte* v )
{
	fprintf(glw_state.log_fp, "glColor3ubv\n");
	dllColor3ubv(v);
}

#define SIG( x ) fprintf(glw_state.log_fp, x "\n")

static void APIENTRY logColor3ui( GLuint red, GLuint green, GLuint blue )
{
	SIG("glColor3ui");
	dllColor3ui(red, green, blue);
}

static void APIENTRY logColor3uiv( const GLuint* v )
{
	SIG("glColor3uiv");
	dllColor3uiv(v);
}

static void APIENTRY logColor3us( GLushort red, GLushort green, GLushort blue )
{
	SIG("glColor3us");
	dllColor3us(red, green, blue);
}

static void APIENTRY logColor3usv( const GLushort* v )
{
	SIG("glColor3usv");
	dllColor3usv(v);
}

static void APIENTRY logColor4b( GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha )
{
	SIG("glColor4b");
	dllColor4b(red, green, blue, alpha);
}

static void APIENTRY logColor4bv( const GLbyte* v )
{
	SIG("glColor4bv");
	dllColor4bv(v);
}

static void APIENTRY logColor4d( GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha )
{
	SIG("glColor4d");
	dllColor4d(red, green, blue, alpha);
}
static void APIENTRY logColor4dv( const GLdouble* v )
{
	SIG("glColor4dv");
	dllColor4dv(v);
}
static void APIENTRY logColor4f( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha )
{
	fprintf(glw_state.log_fp, "glColor4f( %f,%f,%f,%f )\n", red, green, blue, alpha);
	dllColor4f(red, green, blue, alpha);
}
static void APIENTRY logColor4fv( const GLfloat* v )
{
	fprintf(glw_state.log_fp, "glColor4fv( %f,%f,%f,%f )\n", v[0], v[1], v[2], v[3]);
	dllColor4fv(v);
}
static void APIENTRY logColor4i( GLint red, GLint green, GLint blue, GLint alpha )
{
	SIG("glColor4i");
	dllColor4i(red, green, blue, alpha);
}
static void APIENTRY logColor4iv( const GLint* v )
{
	SIG("glColor4iv");
	dllColor4iv(v);
}
static void APIENTRY logColor4s( GLshort red, GLshort green, GLshort blue, GLshort alpha )
{
	SIG("glColor4s");
	dllColor4s(red, green, blue, alpha);
}
static void APIENTRY logColor4sv( const GLshort* v )
{
	SIG("glColor4sv");
	dllColor4sv(v);
}
static void APIENTRY logColor4ub( GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha )
{
	SIG("glColor4b");
	dllColor4b(red, green, blue, alpha);
}
static void APIENTRY logColor4ubv( const GLubyte* v )
{
	SIG("glColor4ubv");
	dllColor4ubv(v);
}
static void APIENTRY logColor4ui( GLuint red, GLuint green, GLuint blue, GLuint alpha )
{
	SIG("glColor4ui");
	dllColor4ui(red, green, blue, alpha);
}
static void APIENTRY logColor4uiv( const GLuint* v )
{
	SIG("glColor4uiv");
	dllColor4uiv(v);
}
static void APIENTRY logColor4us( GLushort red, GLushort green, GLushort blue, GLushort alpha )
{
	SIG("glColor4us");
	dllColor4us(red, green, blue, alpha);
}
static void APIENTRY logColor4usv( const GLushort* v )
{
	SIG("glColor4usv");
	dllColor4usv(v);
}
static void APIENTRY logColorMask( GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha )
{
	SIG("glColorMask");
	dllColorMask(red, green, blue, alpha);
}
static void APIENTRY logColorMaterial( GLenum face, GLenum mode )
{
	SIG("glColorMaterial");
	dllColorMaterial(face, mode);
}

static void APIENTRY logColorPointer( GLint size, GLenum type, GLsizei stride, const void* pointer )
{
	SIG("glColorPointer");
	dllColorPointer(size, type, stride, pointer);
}

static void APIENTRY logCopyPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum type )
{
	SIG("glCopyPixels");
	dllCopyPixels(x, y, width, height, type);
}

static void APIENTRY logCopyTexImage1D( GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLint border )
{
	SIG("glCopyTexImage1D");
	dllCopyTexImage1D(target, level, internalFormat, x, y, width, border);
}

static void APIENTRY logCopyTexImage2D( GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border )
{
	SIG("glCopyTexImage2D");
	dllCopyTexImage2D(target, level, internalFormat, x, y, width, height, border);
}

static void APIENTRY logCopyTexSubImage1D( GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width )
{
	SIG("glCopyTexSubImage1D");
	dllCopyTexSubImage1D(target, level, xoffset, x, y, width);
}

static void APIENTRY logCopyTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height )
{
	SIG("glCopyTexSubImage2D");
	dllCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
}

static void APIENTRY logCullFace( GLenum mode )
{
	SIG("glCullFace");
	dllCullFace(mode);
}

static void APIENTRY logDeleteLists( GLuint list, GLsizei range )
{
	SIG("glDeleteLists");
	dllDeleteLists(list, range);
}

static void APIENTRY logDeleteTextures( GLsizei n, const GLuint* textures )
{
	SIG("glDeleteTextures");
	dllDeleteTextures(n, textures);
}

static void APIENTRY logDepthFunc( GLenum func )
{
	SIG("glDepthFunc");
	dllDepthFunc(func);
}

static void APIENTRY logDepthMask( GLboolean flag )
{
	SIG("glDepthMask");
	dllDepthMask(flag);
}

static void APIENTRY logDepthRange( GLclampd zNear, GLclampd zFar )
{
	SIG("glDepthRange");
	dllDepthRange(zNear, zFar);
}

static void APIENTRY logDisable( GLenum cap )
{
	fprintf(glw_state.log_fp, "glDisable( 0x%x )\n", cap);
	dllDisable(cap);
}

static void APIENTRY logDisableClientState( GLenum array )
{
	SIG("glDisableClientState");
	dllDisableClientState(array);
}

static void APIENTRY logDrawArrays( GLenum mode, GLint first, GLsizei count )
{
	SIG("glDrawArrays");
	dllDrawArrays(mode, first, count);
}

static void APIENTRY logDrawBuffer( GLenum mode )
{
	SIG("glDrawBuffer");
	dllDrawBuffer(mode);
}

static void APIENTRY logDrawElements( GLenum mode, GLsizei count, GLenum type, const void* indices )
{
	SIG("glDrawElements");
	dllDrawElements(mode, count, type, indices);
}

static void APIENTRY logDrawPixels( GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels )
{
	SIG("glDrawPixels");
	dllDrawPixels(width, height, format, type, pixels);
}

static void APIENTRY logEdgeFlag( GLboolean flag )
{
	SIG("glEdgeFlag");
	dllEdgeFlag(flag);
}

static void APIENTRY logEdgeFlagPointer( GLsizei stride, const void* pointer )
{
	SIG("glEdgeFlagPointer");
	dllEdgeFlagPointer(stride, pointer);
}

static void APIENTRY logEdgeFlagv( const GLboolean* flag )
{
	SIG("glEdgeFlagv");
	dllEdgeFlagv(flag);
}

static void APIENTRY logEnable( GLenum cap )
{
	fprintf(glw_state.log_fp, "glEnable( 0x%x )\n", cap);
	dllEnable(cap);
}

static void APIENTRY logEnableClientState( GLenum array )
{
	SIG("glEnableClientState");
	dllEnableClientState(array);
}

static void APIENTRY logEnd( void )
{
	SIG("glEnd");
	dllEnd();
}

static void APIENTRY logEndList( void )
{
	SIG("glEndList");
	dllEndList();
}

static void APIENTRY logEvalCoord1d( GLdouble u )
{
	SIG("glEvalCoord1d");
	dllEvalCoord1d(u);
}

static void APIENTRY logEvalCoord1dv( const GLdouble* u )
{
	SIG("glEvalCoord1dv");
	dllEvalCoord1dv(u);
}

static void APIENTRY logEvalCoord1f( GLfloat u )
{
	SIG("glEvalCoord1f");
	dllEvalCoord1f(u);
}

static void APIENTRY logEvalCoord1fv( const GLfloat* u )
{
	SIG("glEvalCoord1fv");
	dllEvalCoord1fv(u);
}
static void APIENTRY logEvalCoord2d( GLdouble u, GLdouble v )
{
	SIG("glEvalCoord2d");
	dllEvalCoord2d(u, v);
}
static void APIENTRY logEvalCoord2dv( const GLdouble* u )
{
	SIG("glEvalCoord2dv");
	dllEvalCoord2dv(u);
}
static void APIENTRY logEvalCoord2f( GLfloat u, GLfloat v )
{
	SIG("glEvalCoord2f");
	dllEvalCoord2f(u, v);
}
static void APIENTRY logEvalCoord2fv( const GLfloat* u )
{
	SIG("glEvalCoord2fv");
	dllEvalCoord2fv(u);
}

static void APIENTRY logEvalMesh1( GLenum mode, GLint i1, GLint i2 )
{
	SIG("glEvalMesh1");
	dllEvalMesh1(mode, i1, i2);
}
static void APIENTRY logEvalMesh2( GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2 )
{
	SIG("glEvalMesh2");
	dllEvalMesh2(mode, i1, i2, j1, j2);
}
static void APIENTRY logEvalPoint1( GLint i )
{
	SIG("glEvalPoint1");
	dllEvalPoint1(i);
}
static void APIENTRY logEvalPoint2( GLint i, GLint j )
{
	SIG("glEvalPoint2");
	dllEvalPoint2(i, j);
}

static void APIENTRY logFeedbackBuffer( GLsizei size, GLenum type, GLfloat* buffer )
{
	SIG("glFeedbackBuffer");
	dllFeedbackBuffer(size, type, buffer);
}

static void APIENTRY logFinish( void )
{
	SIG("glFinish");
	dllFinish();
}

static void APIENTRY logFlush( void )
{
	SIG("glFlush");
	dllFlush();
}

static void APIENTRY logFogf( GLenum pname, GLfloat param )
{
	SIG("glFogf");
	dllFogf(pname, param);
}

static void APIENTRY logFogfv( GLenum pname, const GLfloat* params )
{
	SIG("glFogfv");
	dllFogfv(pname, params);
}

static void APIENTRY logFogi( GLenum pname, GLint param )
{
	SIG("glFogi");
	dllFogi(pname, param);
}

static void APIENTRY logFogiv( GLenum pname, const GLint* params )
{
	SIG("glFogiv");
	dllFogiv(pname, params);
}

static void APIENTRY logFrontFace( GLenum mode )
{
	SIG("glFrontFace");
	dllFrontFace(mode);
}

static void APIENTRY logFrustum( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar )
{
	SIG("glFrustum");
	dllFrustum(left, right, bottom, top, zNear, zFar);
}

static GLuint APIENTRY logGenLists( GLsizei range )
{
	SIG("glGenLists");
	return dllGenLists(range);
}

static void APIENTRY logGenTextures( GLsizei n, GLuint* textures )
{
	SIG("glGenTextures");
	dllGenTextures(n, textures);
}

static void APIENTRY logGetBooleanv( GLenum pname, GLboolean* params )
{
	SIG("glGetBooleanv");
	dllGetBooleanv(pname, params);
}

static void APIENTRY logGetClipPlane( GLenum plane, GLdouble* equation )
{
	SIG("glGetClipPlane");
	dllGetClipPlane(plane, equation);
}

static void APIENTRY logGetDoublev( GLenum pname, GLdouble* params )
{
	SIG("glGetDoublev");
	dllGetDoublev(pname, params);
}

static GLenum APIENTRY logGetError( void )
{
	SIG("glGetError");
	return dllGetError();
}

static void APIENTRY logGetFloatv( GLenum pname, GLfloat* params )
{
	SIG("glGetFloatv");
	dllGetFloatv(pname, params);
}

static void APIENTRY logGetIntegerv( GLenum pname, GLint* params )
{
	SIG("glGetIntegerv");
	dllGetIntegerv(pname, params);
}

static void APIENTRY logGetLightfv( GLenum light, GLenum pname, GLfloat* params )
{
	SIG("glGetLightfv");
	dllGetLightfv(light, pname, params);
}

static void APIENTRY logGetLightiv( GLenum light, GLenum pname, GLint* params )
{
	SIG("glGetLightiv");
	dllGetLightiv(light, pname, params);
}

static void APIENTRY logGetMapdv( GLenum target, GLenum query, GLdouble* v )
{
	SIG("glGetMapdv");
	dllGetMapdv(target, query, v);
}

static void APIENTRY logGetMapfv( GLenum target, GLenum query, GLfloat* v )
{
	SIG("glGetMapfv");
	dllGetMapfv(target, query, v);
}

static void APIENTRY logGetMapiv( GLenum target, GLenum query, GLint* v )
{
	SIG("glGetMapiv");
	dllGetMapiv(target, query, v);
}

static void APIENTRY logGetMaterialfv( GLenum face, GLenum pname, GLfloat* params )
{
	SIG("glGetMaterialfv");
	dllGetMaterialfv(face, pname, params);
}

static void APIENTRY logGetMaterialiv( GLenum face, GLenum pname, GLint* params )
{
	SIG("glGetMaterialiv");
	dllGetMaterialiv(face, pname, params);
}

static void APIENTRY logGetPixelMapfv( GLenum map, GLfloat* values )
{
	SIG("glGetPixelMapfv");
	dllGetPixelMapfv(map, values);
}

static void APIENTRY logGetPixelMapuiv( GLenum map, GLuint* values )
{
	SIG("glGetPixelMapuiv");
	dllGetPixelMapuiv(map, values);
}

static void APIENTRY logGetPixelMapusv( GLenum map, GLushort* values )
{
	SIG("glGetPixelMapusv");
	dllGetPixelMapusv(map, values);
}

static void APIENTRY logGetPointerv( GLenum pname, GLvoid** params )
{
	SIG("glGetPointerv");
	dllGetPointerv(pname, params);
}

static void APIENTRY logGetPolygonStipple( GLubyte* mask )
{
	SIG("glGetPolygonStipple");
	dllGetPolygonStipple(mask);
}

static const GLubyte * APIENTRY logGetString( GLenum name )
{
	SIG("glGetString");
	return dllGetString(name);
}

static void APIENTRY logGetTexEnvfv( GLenum target, GLenum pname, GLfloat* params )
{
	SIG("glGetTexEnvfv");
	dllGetTexEnvfv(target, pname, params);
}

static void APIENTRY logGetTexEnviv( GLenum target, GLenum pname, GLint* params )
{
	SIG("glGetTexEnviv");
	dllGetTexEnviv(target, pname, params);
}

static void APIENTRY logGetTexGendv( GLenum coord, GLenum pname, GLdouble* params )
{
	SIG("glGetTexGendv");
	dllGetTexGendv(coord, pname, params);
}

static void APIENTRY logGetTexGenfv( GLenum coord, GLenum pname, GLfloat* params )
{
	SIG("glGetTexGenfv");
	dllGetTexGenfv(coord, pname, params);
}

static void APIENTRY logGetTexGeniv( GLenum coord, GLenum pname, GLint* params )
{
	SIG("glGetTexGeniv");
	dllGetTexGeniv(coord, pname, params);
}

static void APIENTRY logGetTexImage( GLenum target, GLint level, GLenum format, GLenum type, void* pixels )
{
	SIG("glGetTexImage");
	dllGetTexImage(target, level, format, type, pixels);
}











/*
** QGL_Shutdown
**
** Unloads the specified DLL then nulls out all the proc pointers.
*/
void QGL_Shutdown( void )
{
	if (glw_state.hinstOpenGL)
		FreeLibrary(glw_state.hinstOpenGL);

	glw_state.hinstOpenGL = NULL;

	qglAccum					= NULL;
	qglAlphaFunc				= NULL;
	qglAreTexturesResident		= NULL;
	qglArrayElement				= NULL;
	qglBegin					= NULL;
	qglBindTexture				= NULL;
	qglBitmap					= NULL;
	qglBlendFunc				= NULL;
	qglCallList					= NULL;
	qglCallLists				= NULL;
	qglClear					= NULL;
	qglClearAccum				= NULL;
	qglClearColor				= NULL;
	qglClearDepth				= NULL;
	qglClearIndex				= NULL;
	qglClearStencil				= NULL;
	qglClipPlane				= NULL;
	qglColor3b					= NULL;
	qglColor3bv					= NULL;
	qglColor3d					= NULL;
	qglColor3dv					= NULL;
	qglColor3f					= NULL;
	qglColor3fv					= NULL;
	qglColor3i					= NULL;
	qglColor3iv					= NULL;
	qglColor3s					= NULL;
	qglColor3sv					= NULL;
	qglColor3ub					= NULL;
	qglColor3ubv				= NULL;
	qglColor3ui					= NULL;
	qglColor3uiv				= NULL;
	qglColor3us					= NULL;
	qglColor3usv				= NULL;
	qglColor4b					= NULL;
	qglColor4bv					= NULL;
	qglColor4d					= NULL;
	qglColor4dv					= NULL;
	qglColor4f					= NULL;
	qglColor4fv					= NULL;
	qglColor4i					= NULL;
	qglColor4iv					= NULL;
	qglColor4s					= NULL;
	qglColor4sv					= NULL;
	qglColor4ub					= NULL;
	qglColor4ubv				= NULL;
	qglColor4ui					= NULL;
	qglColor4uiv				= NULL;
	qglColor4us					= NULL;
	qglColor4usv				= NULL;
	qglColorMask				= NULL;
	qglColorMaterial			= NULL;
	qglColorPointer				= NULL;
	qglCopyPixels				= NULL;
	qglCopyTexImage1D			= NULL;
	qglCopyTexImage2D			= NULL;
	qglCopyTexSubImage1D		= NULL;
	qglCopyTexSubImage2D		= NULL;
	qglCullFace					= NULL;
	qglDeleteLists				= NULL;
	qglDeleteTextures			= NULL;
	qglDepthFunc				= NULL;
	qglDepthMask				= NULL;
	qglDepthRange				= NULL;
	qglDisable					= NULL;
	qglDisableClientState		= NULL;
	qglDrawArrays				= NULL;
	qglDrawBuffer				= NULL;
	qglDrawElements				= NULL;
	qglDrawPixels				= NULL;
	qglEdgeFlag					= NULL;
	qglEdgeFlagPointer			= NULL;
	qglEdgeFlagv				= NULL;
	qglEnable					= NULL;
	qglEnableClientState		= NULL;
	qglEnd						= NULL;
	qglEndList					= NULL;
	qglEvalCoord1d				= NULL;
	qglEvalCoord1dv				= NULL;
	qglEvalCoord1f				= NULL;
	qglEvalCoord1fv				= NULL;
	qglEvalCoord2d				= NULL;
	qglEvalCoord2dv				= NULL;
	qglEvalCoord2f				= NULL;
	qglEvalCoord2fv				= NULL;
	qglEvalMesh1				= NULL;
	qglEvalMesh2				= NULL;
	qglEvalPoint1				= NULL;
	qglEvalPoint2				= NULL;
	qglFeedbackBuffer			= NULL;
	qglFinish					= NULL;
	qglFlush					= NULL;
	qglFogf						= NULL;
	qglFogfv					= NULL;
	qglFogi						= NULL;
	qglFogiv					= NULL;
	qglFrontFace				= NULL;
	qglFrustum					= NULL;
	qglGenLists					= NULL;
	qglGenTextures				= NULL;
	qglGetBooleanv				= NULL;
	qglGetClipPlane				= NULL;
	qglGetDoublev				= NULL;
	qglGetError					= NULL;
	qglGetFloatv				= NULL;
	qglGetIntegerv				= NULL;
	qglGetLightfv				= NULL;
	qglGetLightiv				= NULL;
	qglGetMapdv					= NULL;
	qglGetMapfv					= NULL;
	qglGetMapiv					= NULL;
	qglGetMaterialfv			= NULL;
	qglGetMaterialiv			= NULL;
	qglGetPixelMapfv			= NULL;
	qglGetPixelMapuiv			= NULL;
	qglGetPixelMapusv			= NULL;
	qglGetPointerv				= NULL;
	qglGetPolygonStipple		= NULL;
	qglGetString				= NULL;
	qglGetTexEnvfv				= NULL;
	qglGetTexEnviv				= NULL;
	qglGetTexGendv				= NULL;
	qglGetTexGenfv				= NULL;
	qglGetTexGeniv				= NULL;
	qglGetTexImage				= NULL;















	// TODO: Implement






	qwglCopyContext				= NULL;
	qwglCreateContext			= NULL;
	qwglCreateLayerContext		= NULL;
	qwglDeleteContext			= NULL;
	qwglDescribeLayerPlane		= NULL;
	qwglGetCurrentContext		= NULL;
	qwglGetCurrentDC			= NULL;
	qwglGetLayerPaletteEntries	= NULL;
	qwglGetProcAddress			= NULL;
	qwglMakeCurrent				= NULL;
	qwglRealizeLayerPalette		= NULL;
	qwglSetLayerPaletteEntries	= NULL;
	qwglShareLists				= NULL;
	qwglSwapLayerBuffers		= NULL;
	qwglUseFontBitmaps			= NULL;
	qwglUseFontOutlines			= NULL;

	qwglChoosePixelFormat		= NULL;
	qwglDescribePixelFormat		= NULL;
	qwglGetPixelFormat			= NULL;
	qwglSetPixelFormat			= NULL;
	qwglSwapBuffers				= NULL;

	qwglSwapIntervalEXT			= NULL;

	qwglGetDeviceGammaRampEXT	= NULL;
	qwglSetDeviceGammaRampEXT	= NULL;
}

#pragma warning (disable : 4113 4133 4047 )
#define GPA( a ) GetProcAddress( glw_state.hinstOpenGL, a )

/*
** QGL_Init
**
** This is responsible for binding our qgl function pointers to
** the appropriate GL stuff.  In Windows this means doing a
** LoadLibrary and a bunch of calls to GetProcAddress.  On other
** operating systems we need to do the right thing, whatever that
** might be.
**
*/
HINSTANCE QGL_Init( char* pdllname )
{
	if (!pdllname)
	{
		pdllname = "opengl32.dll";
	}

	// update 3Dfx gamma irrespective of underlying DLL
	{
		char envbuffer[1024];
		float g;

		g = 1.0;
		sprintf(envbuffer, "SSTV2_GAMMA=%f", g);
		_putenv(envbuffer);
		sprintf(envbuffer, "SST_GAMMA=%f", g);
		_putenv(envbuffer);
		sprintf(envbuffer, "SSTH3_GAMMA=%f", g);
		_putenv(envbuffer);
		_putenv("FX_GLIDE_NO_SPLASH=1");
	}

	if ((glw_state.hinstOpenGL = LoadLibrary(pdllname)) == 0)
	{
		char* buf = NULL;

		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&buf, 0, NULL);
		Con_Printf("%s\n", buf);
		return NULL;
	}

	qglAccum					= dllAccum						= GPA("glAccum");
	qglAlphaFunc				= dllAlphaFunc					= GPA("glAlphaFunc");
	qglAreTexturesResident		= dllAreTexturesResident		= GPA("glAreTexturesResident");
	qglArrayElement				= dllArrayElement				= GPA("glArrayElement");
	qglBegin					= dllBegin						= GPA("glBegin");
	qglBindTexture				= dllBindTexture				= GPA("glBindTexture");
	qglBitmap					= dllBitmap						= GPA("glBitmap");
	qglBlendFunc				= dllBlendFunc					= GPA("glBlendFunc");
	qglCallList					= dllCallList					= GPA("glCallList");
	qglCallLists				= dllCallLists					= GPA("glCallLists");
	qglClear					= dllClear						= GPA("glClear");
	qglClearAccum				= dllClearAccum					= GPA("glClearAccum");
	qglClearColor				= dllClearColor					= GPA("glClearColor");
	qglClearDepth				= dllClearDepth					= GPA("glClearDepth");
	qglClearIndex				= dllClearIndex					= GPA("glClearIndex");
	qglClearStencil				= dllClearStencil				= GPA("glClearStencil");
	qglClipPlane				= dllClipPlane					= GPA("glClipPlane");
	qglColor3b					= dllColor3b					= GPA("glColor3b");
	qglColor3bv					= dllColor3bv					= GPA("glColor3bv");
	qglColor3d					= dllColor3d					= GPA("glColor3d");
	qglColor3dv					= dllColor3dv					= GPA("glColor3dv");
	qglColor3f					= dllColor3f					= GPA("glColor3f");
	qglColor3fv					= dllColor3fv					= GPA("glColor3fv");
	qglColor3i					= dllColor3i					= GPA("glColor3i");
	qglColor3iv					= dllColor3iv					= GPA("glColor3iv");
	qglColor3s					= dllColor3s					= GPA("glColor3s");
	qglColor3sv					= dllColor3sv					= GPA("glColor3sv");
	qglColor3ub					= dllColor3ub					= GPA("glColor3ub");
	qglColor3ubv				= dllColor3ubv					= GPA("glColor3ubv");
	qglColor3ui					= dllColor3ui					= GPA("glColor3ui");
	qglColor3uiv				= dllColor3uiv					= GPA("glColor3uiv");
	qglColor3us					= dllColor3us					= GPA("glColor3us");
	qglColor3usv				= dllColor3usv					= GPA("glColor3usv");
	qglColor4b					= dllColor4b					= GPA("glColor4b");
	qglColor4bv					= dllColor4bv					= GPA("glColor4bv");
	qglColor4d					= dllColor4d					= GPA("glColor4d");
	qglColor4dv					= dllColor4dv					= GPA("glColor4dv");
	qglColor4f					= dllColor4f					= GPA("glColor4f");
	qglColor4fv					= dllColor4fv					= GPA("glColor4fv");
	qglColor4i					= dllColor4i					= GPA("glColor4i");
	qglColor4iv					= dllColor4iv					= GPA("glColor4iv");
	qglColor4s					= dllColor4s					= GPA("glColor4s");
	qglColor4sv					= dllColor4sv					= GPA("glColor4sv");
	qglColor4ub					= dllColor4ub					= GPA("glColor4ub");
	qglColor4ubv				= dllColor4ubv					= GPA("glColor4ubv");
	qglColor4ui					= dllColor4ui					= GPA("glColor4ui");
	qglColor4uiv				= dllColor4uiv					= GPA("glColor4uiv");
	qglColor4us					= dllColor4us					= GPA("glColor4us");
	qglColor4usv				= dllColor4usv					= GPA("glColor4usv");
	qglColorMask				= dllColorMask					= GPA("glColorMask");
	qglColorMaterial			= dllColorMaterial				= GPA("glColorMaterial");
	qglColorPointer				= dllColorPointer				= GPA("glColorPointer");
	qglCopyPixels				= dllCopyPixels					= GPA("glCopyPixels");
	qglCopyTexImage1D			= dllCopyTexImage1D				= GPA("glCopyTexImage1D");
	qglCopyTexImage2D			= dllCopyTexImage2D				= GPA("glCopyTexImage2D");
	qglCopyTexSubImage1D		= dllCopyTexSubImage1D			= GPA("glCopyTexSubImage1D");
	qglCopyTexSubImage2D		= dllCopyTexSubImage2D			= GPA("glCopyTexSubImage2D");
	qglCullFace					= dllCullFace					= GPA("glCullFace");
	qglDeleteLists				= dllDeleteLists				= GPA("glDeleteLists");
	qglDeleteTextures			= dllDeleteTextures				= GPA("glDeleteTextures");
	qglDepthFunc				= dllDepthFunc					= GPA("glDepthFunc");
	qglDepthMask				= dllDepthMask					= GPA("glDepthMask");
	qglDepthRange				= dllDepthRange					= GPA("glDepthRange");
	qglDisable					= dllDisable					= GPA("glDisable");
	qglDisableClientState		= dllDisableClientState			= GPA("glDisableClientState");
	qglDrawArrays				= dllDrawArrays					= GPA("glDrawArrays");
	qglDrawBuffer				= dllDrawBuffer					= GPA("glDrawBuffer");
	qglDrawElements				= dllDrawElements				= GPA("glDrawElements");
	qglDrawPixels				= dllDrawPixels					= GPA("glDrawPixels");
	qglEdgeFlag					= dllEdgeFlag					= GPA("glEdgeFlag");
	qglEdgeFlagPointer			= dllEdgeFlagPointer			= GPA("glEdgeFlagPointer");
	qglEdgeFlagv				= dllEdgeFlagv					= GPA("glEdgeFlagv");
	qglEnable					= dllEnable						= GPA("glEnable");
	qglEnableClientState		= dllEnableClientState			= GPA("glEnableClientState");
	qglEnd						= dllEnd						= GPA("glEnd");
	qglEndList					= dllEndList					= GPA("glEndList");
	qglEvalCoord1d				= dllEvalCoord1d				= GPA("glEvalCoord1d");
	qglEvalCoord1dv				= dllEvalCoord1dv				= GPA("glEvalCoord1dv");
	qglEvalCoord1f				= dllEvalCoord1f				= GPA("glEvalCoord1f");
	qglEvalCoord1fv				= dllEvalCoord1fv				= GPA("glEvalCoord1fv");
	qglEvalCoord2d				= dllEvalCoord2d				= GPA("glEvalCoord2d");
	qglEvalCoord2dv				= dllEvalCoord2dv				= GPA("glEvalCoord2dv");
	qglEvalCoord2f				= dllEvalCoord2f				= GPA("glEvalCoord2f");
	qglEvalCoord2fv				= dllEvalCoord2fv				= GPA("glEvalCoord2fv");
	qglEvalMesh1				= dllEvalMesh1					= GPA("glEvalMesh1");
	qglEvalMesh2				= dllEvalMesh2					= GPA("glEvalMesh2");
	qglEvalPoint1				= dllEvalPoint1					= GPA("glEvalPoint1");
	qglEvalPoint2				= dllEvalPoint2					= GPA("glEvalPoint2");
	qglFeedbackBuffer			= dllFeedbackBuffer				= GPA("glFeedbackBuffer");
	qglFinish					= dllFinish						= GPA("glFinish");
	qglFlush					= dllFlush						= GPA("glFlush");
	qglFogf						= dllFogf						= GPA("glFogf");
	qglFogfv					= dllFogfv						= GPA("glFogfv");
	qglFogi						= dllFogi						= GPA("glFogi");
	qglFogiv					= dllFogiv						= GPA("glFogiv");
	qglFrontFace				= dllFrontFace					= GPA("glFrontFace");
	qglFrustum					= dllFrustum					= GPA("glFrustum");
	qglGenLists					= dllGenLists					= GPA("glGenLists");
	qglGenTextures				= dllGenTextures				= GPA("glGenTextures");
	qglGetBooleanv				= dllGetBooleanv				= GPA("glGetBooleanv");
	qglGetClipPlane				= dllGetClipPlane				= GPA("glGetClipPlane");
	qglGetDoublev				= dllGetDoublev					= GPA("glGetDoublev");
	qglGetError					= dllGetError					= GPA("glGetError");
	qglGetFloatv				= dllGetFloatv					= GPA("glGetFloatv");
	qglGetIntegerv				= dllGetIntegerv				= GPA("glGetIntegerv");
	qglGetLightfv				= dllGetLightfv					= GPA("glGetLightfv");
	qglGetLightiv				= dllGetLightiv					= GPA("glGetLightiv");
	qglGetMapdv					= dllGetMapdv					= GPA("glGetMapdv");
	qglGetMapfv					= dllGetMapfv					= GPA("glGetMapfv");
	qglGetMapiv					= dllGetMapiv					= GPA("glGetMapiv");
	qglGetMaterialfv			= dllGetMaterialfv				= GPA("glGetMaterialfv");
	qglGetMaterialiv			= dllGetMaterialiv				= GPA("glGetMaterialiv");
	qglGetPixelMapfv			= dllGetPixelMapfv				= GPA("glGetPixelMapfv");
	qglGetPixelMapuiv			= dllGetPixelMapuiv				= GPA("glGetPixelMapuiv");
	qglGetPixelMapusv			= dllGetPixelMapusv				= GPA("glGetPixelMapusv");
	qglGetPointerv				= dllGetPointerv				= GPA("glGetPointerv");
	qglGetPolygonStipple		= dllGetPolygonStipple			= GPA("glGetPolygonStipple");
	qglGetString				= dllGetString					= GPA("glGetString");
	qglGetTexEnvfv				= dllGetTexEnvfv				= GPA("glGetTexEnvfv");
	qglGetTexEnviv				= dllGetTexEnviv				= GPA("glGetTexEnviv");
	qglGetTexGendv				= dllGetTexGendv				= GPA("glGetTexGendv");
	qglGetTexGenfv				= dllGetTexGenfv				= GPA("glGetTexGenfv");
	qglGetTexGeniv				= dllGetTexGeniv				= GPA("glGetTexGeniv");
	qglGetTexImage				= dllGetTexImage				= GPA("glGetTexImage");









	// TODO: Implement





	qwglCopyContext				= GPA("wglCopyContext");
	qwglCreateContext			= GPA("wglCreateContext");
	qwglCreateLayerContext		= GPA("wglCreateLayerContext");
	qwglDeleteContext			= GPA("wglDeleteContext");
	qwglDescribeLayerPlane		= GPA("wglDescribeLayerPlane");
	qwglGetCurrentContext		= GPA("wglGetCurrentContext");
	qwglGetCurrentDC			= GPA("wglGetCurrentDC");
	qwglGetLayerPaletteEntries	= GPA("wglGetLayerPaletteEntries");
	qwglGetProcAddress			= GPA("wglGetProcAddress");
	qwglMakeCurrent				= GPA("wglMakeCurrent");
	qwglRealizeLayerPalette		= GPA("wglRealizeLayerPalette");
	qwglSetLayerPaletteEntries	= GPA("wglSetLayerPaletteEntries");
	qwglShareLists				= GPA("wglShareLists");
	qwglSwapLayerBuffers		= GPA("wglSwapLayerBuffers");
	qwglUseFontBitmaps			= GPA("wglUseFontBitmapsA");
	qwglUseFontOutlines			= GPA("wglUseFontOutlinesA");

	qwglChoosePixelFormat		= GPA("wglChoosePixelFormat");
	qwglDescribePixelFormat		= GPA("wglDescribePixelFormat");
	qwglGetPixelFormat			= GPA("wglGetPixelFormat");
	qwglSetPixelFormat			= GPA("wglSetPixelFormat");
	qwglSwapBuffers				= GPA("wglSwapBuffers");

	qwglSwapIntervalEXT			= NULL;
	qglPointParameterfEXT		= NULL;
	qglPointParameterfvEXT		= NULL;
	qglColorTableEXT			= NULL;
	qglSelectTextureSGIS		= NULL;
	qglMTexCoord2fSGIS			= NULL;

	return glw_state.hinstOpenGL;
}

void GLimp_EnableLogging( void )
{
	// TODO: Implement
}


void GLimp_LogNewFrame( void )
{
	if (glw_state.log_fp)
		fprintf(glw_state.log_fp, "*** R_BeginFrame ***\n");
}

#pragma warning (default : 4113 4133 4047 )