#include "quakedef.h"
#include "r_triangle.h"

#if defined( GLQUAKE )
float	gGlR, gGlG, gGlB, gGlW;
#else

// TODO: Implement

#endif

#if defined( GLQUAKE )
void tri_GL_Color4f( float x, float y, float z, float w )
{
	qglColor4f(x * w, y * w, z * w, 1);
	gGlR = x;
	gGlG = y;
	gGlB = z;
	gGlW = w;
}

void tri_GL_Color4ub( byte r, byte g, byte b, byte a )
{
	gGlR = r / 255.0;
	gGlG = g / 255.0;
	gGlB = b / 255.0;
	gGlW = a / 255.0;
	qglColor4f(gGlR, gGlG, gGlB, 1);
}

void tri_GL_Brightness( float x )
{
	qglColor4f(gGlR * gGlW * x, gGlG * gGlW * x, gGlB * gGlW * x, 1);
}

// Set the rendering mode
void tri_GL_RenderMode( int mode )
{
	switch (mode)
	{
	case kRenderNormal:
		qglDisable(GL_BLEND);
		qglDepthMask(GL_TRUE);
		qglShadeModel(GL_FLAT);
		break;
	case kRenderTransColor:
	case kRenderTransTexture:
		qglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		qglEnable(GL_BLEND);
		qglShadeModel(GL_SMOOTH);
		break;
	case kRenderTransAdd:
		qglBlendFunc(GL_ONE, GL_ONE);
		qglEnable(GL_BLEND);
		qglDepthMask(GL_FALSE);
		qglShadeModel(GL_SMOOTH);
		break;
	default:
		break;
	}
}

void tri_GL_CullFace( TRICULLSTYLE style )
{
	switch (style)
	{
	case TRI_FRONT:
		qglEnable(GL_CULL_FACE);
		qglCullFace(GL_FRONT);
		break;
	case TRI_NONE:
		qglDisable(GL_CULL_FACE);
		break;
	}
}

int R_TriangleSpriteTexture( model_t* pSpriteModel, int frame )
{
	mspriteframe_t* pSpriteFrame;

	pSpriteFrame = R_GetSpriteFrame((msprite_t*)pSpriteModel->cache.data, frame);
	if (!pSpriteFrame)
		return FALSE;

	GL_Bind(pSpriteFrame->gl_texturenum);
	return TRUE;
}
#else
void tri_Soft_Begin( int primitiveCode )
{
	// TODO: Implement
}

void tri_Soft_CullFace( int style )
{
	// TODO: Implement
}

// Set RGBA color (range is [0..1])
void tri_Soft_Color4f( float r, float g, float b, float a )
{
	// TODO: Implement
}

void tri_Soft_Color4ub( byte r, byte g, byte b, byte a )
{
	// TODO: Implement
}

// Set the rendering mode
void tri_Soft_RenderMode( int mode )
{
	// TODO: Implement
}

void tri_Soft_TexCoord2f( float u, float v )
{
	// TODO: Implement
}

void tri_Soft_Brightness( float brightness )
{
	// TODO: Implement
}

void tri_Soft_Vertex3f( float x, float y, float z )
{
	// TODO: Implement
}

void tri_Soft_Vertex3fv( float* worldPnt )
{
	// TODO: Implement
}

void tri_Soft_End( void )
{
	// TODO: Implement
}

void R_TriangleSetTexture( byte* pTexture, short width, short height, word* pPalette )
{
	// TODO: Implement
}

int R_TriangleSpriteTexture( struct model_s* pSpriteModel, int frame )
{
	// TODO: Implement
	return 0;
}

/*
================
R_TriangleProjectFinalVert
================
*/
void R_TriangleProjectFinalVert( finalvert_t* fv, vec_t* av )
{
	// TODO: Implement
}

void R_TriangleDraw( int i0, int i1, int i2 )
{
	// TODO: Implement
}

#endif