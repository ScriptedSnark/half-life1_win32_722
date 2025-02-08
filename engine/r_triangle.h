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

// GL
void tri_GL_Color4f( float x, float y, float z, float w );
void tri_GL_Color4ub( byte r, byte g, byte b, byte a );
void tri_GL_Brightness( float x );
void tri_GL_RenderMode( int mode );
void tri_GL_CullFace( TRICULLSTYLE style );

// Software


int R_TriangleSpriteTexture( struct model_s* pSpriteModel, int frame );

#endif // R_TRIANGLE_H