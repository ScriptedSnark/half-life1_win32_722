//-----------------------------------------------------------------------------
// Quake GL to DirectX wrapper
//-----------------------------------------------------------------------------

#ifndef _OPENGL32_H_
#define _OPENGL32_H_

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
	void*			lpDD4;
	int				bFullscreen;
	BOOL			f4444; // RGBA4444 format
} D3D_t;

#endif // _OPENGL32_H_