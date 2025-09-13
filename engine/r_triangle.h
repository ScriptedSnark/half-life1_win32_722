// r_triangle.h
#if !defined( R_TRIANGLE_H )
#define R_TRIANGLE_H
#ifdef _WIN32
#pragma once
#endif

typedef enum
{
	TRI_FRONT = 0,
	TRI_NONE = 1,
} TRICULLSTYLE;

#if defined( GLQUAKE )
#define TRI_TRIANGLES		GL_TRIANGLES
#define TRI_TRIANGLE_FAN	GL_TRIANGLE_FAN
#define TRI_QUADS			GL_QUADS
#define TRI_POLYGON			GL_POLYGON
#define TRI_LINES			GL_LINES
#define TRI_TRIANGLE_STRIP	GL_TRIANGLE_STRIP
#define TRI_QUAD_STRIP		GL_QUAD_STRIP
#else
#define TRI_TRIANGLES		0
#define TRI_TRIANGLE_FAN	1
#define TRI_QUADS			2
#define TRI_POLYGON			3
#define TRI_LINES			4	
#define TRI_TRIANGLE_STRIP	5
#define TRI_QUAD_STRIP		6
#endif

// GL
void tri_GL_Color4f( float x, float y, float z, float w );
void tri_GL_Color4ub( byte r, byte g, byte b, byte a );
void tri_GL_Brightness( float x );
void tri_GL_RenderMode( int mode );
void tri_GL_CullFace( TRICULLSTYLE style );

// Software
void tri_Soft_Begin( int primitiveCode );
void tri_Soft_Color4f( float r, float g, float b, float a );
void tri_Soft_Color4ub( byte r, byte g, byte b, byte a );
void tri_Soft_TexCoord2f( float u, float v );
void tri_Soft_Vertex3f(float x, float y, float z);
void tri_Soft_Vertex3fv( float* worldPnt );
void tri_Soft_Brightness( float brightness );
void tri_Soft_RenderMode( int mode );
void tri_Soft_CullFace( int style );
void tri_Soft_End( void );

void R_TriangleSetTexture( byte* pTexture, short width, short height, word* pPalette );
int R_TriangleSpriteTexture( model_t* pSpriteModel, int frame );

#if defined( GLQUAKE )
#define tri_Begin(mode)				qglBegin(mode)
#define tri_Color4f(x, y, z, w)		tri_GL_Color4f(x, y, z, w)
#define tri_Color4ub(r, g, b, a)	tri_GL_Color4ub(r, g, b, a)
#define tri_TexCoord2f(u, v)		qglTexCoord2f(u, v)
#define tri_Vertex3f(x, y, z)		qglVertex3f(x, y, z)
#define tri_Vertex3fv(v)			qglVertex3fv(v)
#define tri_Brightness(brightness)	tri_GL_Brightness(brightness)
#define tri_RenderMode(mode)		tri_GL_RenderMode(mode)
#define tri_CullFace(style)			tri_GL_CullFace(style)
#define tri_End()					qglEnd()
#else
#define tri_Begin(mode)				tri_Soft_Begin(mode)
#define tri_Color4f(r, g, b, a)		tri_Soft_Color4f(r, g, b, a)
#define tri_Color4ub(r, g, b, a)	tri_Soft_Color4ub(r, g, b, a)
#define tri_TexCoord2f(u, v)		tri_Soft_TexCoord2f(u, v)
#define tri_Vertex3f(x, y, z)		tri_Soft_Vertex3f(x, y, z)
#define tri_Vertex3fv(v)			tri_Soft_Vertex3fv(v)
#define tri_Brightness(brightness)	tri_Soft_Brightness(brightness)
#define tri_RenderMode(mode)		tri_Soft_RenderMode(mode)
#define tri_CullFace(style)			tri_Soft_CullFace(style)
#define tri_End()					tri_Soft_End()
#endif

#endif // R_TRIANGLE_H