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

static void APIENTRY logColorPointer( GLint size, GLenum type, GLsizei stride, const void *pointer )
{
	SIG("glColorPointer");
	dllColorPointer(size, type, stride, pointer);
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




	// TODO: Implement
}

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

	qglAccum					= dllAccum = GPA("glAccum");
	qglAlphaFunc				= dllAlphaFunc = GPA("glAlphaFunc");
	qglAreTexturesResident		= dllAreTexturesResident = GPA("glAreTexturesResident");
	qglArrayElement				= dllArrayElement = GPA("glArrayElement");
	qglBegin					= dllBegin = GPA("glBegin");
	qglBindTexture				= dllBindTexture = GPA("glBindTexture");
	qglBitmap					= dllBitmap = GPA("glBitmap");
	qglBlendFunc				= dllBlendFunc = GPA("glBlendFunc");
	qglCallList					= dllCallList = GPA("glCallList");
	qglCallLists				= dllCallLists = GPA("glCallLists");
	qglClear					= dllClear = GPA("glClear");
	qglClearAccum				= dllClearAccum = GPA("glClearAccum");
	qglClearColor				= dllClearColor = GPA("glClearColor");
	qglClearDepth				= dllClearDepth = GPA("glClearDepth");
	qglClearIndex				= dllClearIndex = GPA("glClearIndex");
	qglClearStencil				= dllClearStencil = GPA("glClearStencil");
	qglClipPlane				= dllClipPlane = GPA("glClipPlane");
	qglColor3b					= dllColor3b = GPA("glColor3b");
	qglColor3bv					= dllColor3bv = GPA("glColor3bv");
	qglColor3d					= dllColor3d = GPA("glColor3d");
	qglColor3dv					= dllColor3dv = GPA("glColor3dv");
	qglColor3f					= dllColor3f = GPA("glColor3f");
	qglColor3fv					= dllColor3fv = GPA("glColor3fv");
	qglColor3i					= dllColor3i = GPA("glColor3i");
	qglColor3iv					= dllColor3iv = GPA("glColor3iv");
	qglColor3s					= dllColor3s = GPA("glColor3s");
	qglColor3sv					= dllColor3sv = GPA("glColor3sv");
	qglColor3ub					= dllColor3ub = GPA("glColor3ub");
	qglColor3ubv				= dllColor3ubv = GPA("glColor3ubv");
	qglColor3ui					= dllColor3ui = GPA("glColor3ui");
	qglColor3uiv				= dllColor3uiv = GPA("glColor3uiv");
	qglColor3us					= dllColor3us = GPA("glColor3us");
	qglColor3usv				= dllColor3usv = GPA("glColor3usv");
	qglColor4b					= dllColor4b = GPA("glColor4b");
	qglColor4bv					= dllColor4bv = GPA("glColor4bv");
	qglColor4d					= dllColor4d = GPA("glColor4d");
	qglColor4dv					= dllColor4dv = GPA("glColor4dv");
	qglColor4f					= dllColor4f = GPA("glColor4f");
	qglColor4fv					= dllColor4fv = GPA("glColor4fv");
	qglColor4i					= dllColor4i = GPA("glColor4i");
	qglColor4iv					= dllColor4iv = GPA("glColor4iv");
	qglColor4s					= dllColor4s = GPA("glColor4s");
	qglColor4sv					= dllColor4sv = GPA("glColor4sv");
	qglColor4ub					= dllColor4ub = GPA("glColor4ub");
	qglColor4ubv				= dllColor4ubv = GPA("glColor4ubv");
	qglColor4ui					= dllColor4ui = GPA("glColor4ui");
	qglColor4uiv				= dllColor4uiv = GPA("glColor4uiv");
	qglColor4us					= dllColor4us = GPA("glColor4us");
	qglColor4usv				= dllColor4usv = GPA("glColor4usv");
	qglColorMask				= dllColorMask = GPA("glColorMask");
	qglColorMaterial			= dllColorMaterial = GPA("glColorMaterial");
	qglColorPointer				= dllColorPointer = GPA("glColorPointer");













	// TODO: Implement


	return glw_state.hinstOpenGL;
}