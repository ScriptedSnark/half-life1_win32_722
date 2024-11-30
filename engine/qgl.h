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
#  define APIENTRY
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



#endif // QGL_H