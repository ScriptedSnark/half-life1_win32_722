#include "quakedef.h"
#include "r_triangle.h"
#if !defined ( GLQUAKE )
#include "d_local.h"
#endif

#if defined( GLQUAKE )
float gGlR, gGlG, gGlB, gGlW;
#else
int	gRenderMode;
int	gPrimitiveCode;

TRICULLSTYLE gFaceRule;

void R_TriangleProjectFinalVert( finalvert_t* fv, vec_t* av );
void R_TriangleDraw( int i0, int i1, int i2 );
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
	r_affinetridesc.numtriangles = 0;
	r_anumverts = 0;

	R_StudioVertBuffer();
	gPrimitiveCode = primitiveCode;
	pfinalverts->v[2] = 0;
	pfinalverts->v[3] = 0;
	pfinalverts->v[4] = 0xFFFF;
}

void tri_Soft_CullFace( int style )
{
	gFaceRule = style;
}

// Set RGBA color (range is [0..1])
void tri_Soft_Color4f( float r, float g, float b, float a )
{
	if (r_fullbright.value == 1.0)
	{
		r = g = b = 1.0;
	}

	r_icolormix.r = (int)(r * 0xC080) & 0xFF00;
	r_icolormix.g = (int)(g * 0xC080) & 0xFF00;
	r_icolormix.b = (int)(b * 0xC080) & 0xFF00;

	r_blend = (int)(a * 255.0);
}

void tri_Soft_Color4ub( byte r, byte g, byte b, byte a )
{
	r_icolormix.r = (r * 192) & 0xFF00;
	r_icolormix.g = (g * 192) & 0xFF00;
	r_icolormix.b = (b * 192) & 0xFF00;

	r_blend = a;
}

// Set the rendering mode
void tri_Soft_RenderMode( int mode )
{
	switch (mode)
	{
	case kRenderNormal:
		polysetdraw = D_PolysetDrawSpans8;
		break;
	case kRenderTransColor:
	case kRenderTransTexture:
		polysetdraw = D_PolysetDrawSpansTrans;
		break;
	case kRenderTransAdd:
		polysetdraw = D_PolysetDrawSpansTransAdd;
		break;
	}

	gRenderMode = mode;
}

void tri_Soft_TexCoord2f( float u, float v )
{
	if (u < 0.0 || v < 0.0)
		Con_DPrintf("Underflow: %f %f\n", u, v);
	pfinalverts[r_anumverts].v[2] = (r_affinetridesc.skinwidth - 1) * u * 0x10000;
	pfinalverts[r_anumverts].v[3] = (r_affinetridesc.skinheight - 1) * v * 0x10000;
}

void tri_Soft_Brightness( float brightness )
{
	int i;

	if (r_fullbright.value == 1.0)
	{
		pfinalverts[r_anumverts].v[4] = 0xC080;
	}
	else
	{
		i = brightness * 0xC080;
		if (i > 0xFFFF)
			i = 0xFFFF;

		pfinalverts[r_anumverts].v[4] = i;
	}
}

void tri_Soft_Vertex3fv( float* worldPnt )
{
	finalvert_t* fv;
	vec3_t	aa;
	vec3_t	localPnt;

	VectorSubtract(worldPnt, r_origin, aa);
	TransformVector(aa, localPnt);

	fv = &pfinalverts[r_anumverts];
	fv->flags = 0;

	if (localPnt[2] < ALIAS_Z_CLIP_PLANE)
		fv->flags = ALIAS_Z_CLIP;
	else
	{
		R_TriangleProjectFinalVert(fv, localPnt);

		if (fv->v[0] < r_refdef.aliasvrect.x)
			fv->flags |= ALIAS_LEFT_CLIP;
		if (fv->v[1] < r_refdef.aliasvrect.y)
			fv->flags |= ALIAS_TOP_CLIP;
		if (fv->v[0] > r_refdef.aliasvrectright)
			fv->flags |= ALIAS_RIGHT_CLIP;
		if (fv->v[1] > r_refdef.aliasvrectbottom)
			fv->flags |= ALIAS_BOTTOM_CLIP;
	}

	r_anumverts++;

	if (gPrimitiveCode == TRI_QUADS && !(r_anumverts & 3) && !(fv->flags & ALIAS_Z_CLIP))
	{
		if (!(pfinalverts[r_anumverts - 1].flags & ALIAS_Z_CLIP) &&
			!(pfinalverts[r_anumverts - 2].flags & ALIAS_Z_CLIP) &&
			!(pfinalverts[r_anumverts - 3].flags & ALIAS_Z_CLIP))
		{
			int i;
			float s, t;
			int vmax;

			vmax = (r_affinetridesc.skinheight - 1) << 16;
			if (pfinalverts[r_anumverts - 1].v[3] >= vmax)
			{
				pfinalverts[r_anumverts + 3] = pfinalverts[r_anumverts - 1];
				pfinalverts[r_anumverts + 2] = pfinalverts[r_anumverts - 2];

				s = (float)(vmax - pfinalverts[r_anumverts - 4].v[3]);
				s /= (float)(pfinalverts[r_anumverts - 1].v[3] - pfinalverts[r_anumverts - 4].v[3]);
				t = 1.0 - s;

				for (i = 0; i < 6; i++)
				{
					pfinalverts[r_anumverts + 0].v[i] = pfinalverts[r_anumverts - 1].v[i] * s + pfinalverts[r_anumverts - 4].v[i] * t;
					pfinalverts[r_anumverts + 1].v[i] = pfinalverts[r_anumverts - 2].v[i] * s + pfinalverts[r_anumverts - 3].v[i] * t;
				}

				pfinalverts[r_anumverts - 1] = pfinalverts[r_anumverts + 0];
				pfinalverts[r_anumverts - 2] = pfinalverts[r_anumverts + 1];

				for (i = 0; i < 4; i++)
				{
					fv = &pfinalverts[r_anumverts - 2 + i];
					fv->flags = 0;

					if (fv->v[0] < r_refdef.aliasvrect.x)
						fv->flags |= ALIAS_LEFT_CLIP;
					if (fv->v[1] < r_refdef.aliasvrect.y)
						fv->flags |= ALIAS_TOP_CLIP;
					if (fv->v[0] > r_refdef.aliasvrectright)
						fv->flags |= ALIAS_RIGHT_CLIP;
					if (fv->v[1] > r_refdef.aliasvrectbottom)
						fv->flags |= ALIAS_BOTTOM_CLIP;
				}

				pfinalverts[r_anumverts + 3].v[3] -= vmax;
				pfinalverts[r_anumverts + 2].v[3] -= vmax;
				pfinalverts[r_anumverts + 3].v[3] &= 0xFFFF0000;
				pfinalverts[r_anumverts + 2].v[3] &= 0xFFFF0000;
				pfinalverts[r_anumverts + 1].v[3] = 0;
				pfinalverts[r_anumverts + 0].v[3] = 0;
				pfinalverts[r_anumverts - 1].v[3] = vmax - 1;
				pfinalverts[r_anumverts - 2].v[3] = vmax - 1;
				r_anumverts += 4;
			}
		}
	}

	pfinalverts[r_anumverts].v[2] = pfinalverts[r_anumverts - 1].v[2];
	pfinalverts[r_anumverts].v[3] = pfinalverts[r_anumverts - 1].v[3];
	pfinalverts[r_anumverts].v[4] = pfinalverts[r_anumverts - 1].v[4];
}

void tri_Soft_Vertex3f( float x, float y, float z )
{
	vec3_t tmp;

	tmp[0] = x;
	tmp[1] = y;
	tmp[2] = z;
	tri_Soft_Vertex3fv(tmp);
}

void tri_Soft_End( void )
{
	int i;

	switch (gPrimitiveCode)
	{
	case TRI_TRIANGLES:
	{
		for (i = 0; i < r_anumverts; i += 3)
		{
			R_TriangleDraw(i, i + 1, i + 2);
		}
		break;
	}
	case TRI_TRIANGLE_FAN:
	{
		for (i = 0; i < r_anumverts; i += 2)
		{
			R_TriangleDraw(0, i, i + 1);
		}
		break;
	}
	case TRI_QUADS:
	{
		for (i = 0; i < r_anumverts; i += 4)
		{
			R_TriangleDraw(i, i + 1, i + 2);
			R_TriangleDraw(i, i + 2, i + 3);
		}
		break;
	}
	case TRI_TRIANGLE_STRIP:
	{
		int last1, last2;
		for (i = 2, last1 = 0, last2 = 1; i < r_anumverts; i++)
		{
			R_TriangleDraw(i, last2, last1);
			if (r_anumverts - 1 > i)
			{
				last1 = i++;
				R_TriangleDraw(i, last2, last1);
				last2 = i;
			}
		}
		break;
	}
	case TRI_QUAD_STRIP:
	{
		for (i = 2; i < r_anumverts; i += 2)
		{
			R_TriangleDraw(i - 2, i - 1, i);
			R_TriangleDraw(i - 1, i + 1, i);
		}
		break;
	}
	}

	r_anumverts = 0;
}

void R_TriangleSetTexture( byte* pTexture, short width, short height, word* pPalette )
{
	r_affinetridesc.pskindesc = NULL;
	r_affinetridesc.pskin = (void*)pTexture;
	r_affinetridesc.skinwidth = width;
	r_affinetridesc.seamfixupX16 = (width >> 1) << 16;
	r_affinetridesc.skinheight = height;
	r_palette = pPalette;
	D_PolysetUpdateTables();
}

int R_TriangleSpriteTexture( model_t* pSpriteModel, int frame )
{
	msprite_t* pSprite;
	mspriteframe_t* pSpriteFrame;

	pSprite = (msprite_t*)pSpriteModel->cache.data;
	pSpriteFrame = (mspriteframe_t*)R_GetSpriteFrame(pSprite, frame);
	if (!pSpriteFrame)
		return FALSE;

	R_TriangleSetTexture(pSpriteFrame->pixels, pSpriteFrame->width, pSpriteFrame->height, (word*)((byte*)pSprite + pSprite->paloffset));
	return TRUE;
}

void R_TriangleProjectFinalVert( finalvert_t* fv, vec_t* av )
{
	float	zi;

// project points
	if (av[2] == 0.0)
		zi = 0.0;
	else
		zi = 1.0 / av[2];

	fv->v[5] = zi * (float)0x8000 * (float)0x10000;

	fv->v[0] = xcenter + (av[0] * xscale * zi);
	fv->v[1] = ycenter - (av[1] * yscale * zi);
}

void R_TriangleDraw( int i0, int i1, int i2 )
{
	mtriangle_t tri;
	int flags;

	if (!(pfinalverts[i0].flags & pfinalverts[i1].flags & pfinalverts[i2].flags & (ALIAS_XY_CLIP_MASK | ALIAS_Z_CLIP)) &&
		!((pfinalverts[i0].flags | pfinalverts[i1].flags | pfinalverts[i2].flags) & ALIAS_Z_CLIP))
	{
		r_affinetridesc.numtriangles = 1;

		tri.facesfront = 1;

		// Determine triangle orientation
		if (gFaceRule == TRI_NONE &&
			((pfinalverts[i0].v[0] - pfinalverts[i2].v[0]) * (pfinalverts[i0].v[1] - pfinalverts[i1].v[1]) -
			 (pfinalverts[i0].v[0] - pfinalverts[i1].v[0]) * (pfinalverts[i0].v[1] - pfinalverts[i2].v[1])) >= 0)
		{
			tri.vertindex[0] = i2;
			tri.vertindex[1] = i1;
			tri.vertindex[2] = i0;
		}
		else
		{
			tri.vertindex[0] = i0;
			tri.vertindex[1] = i1;
			tri.vertindex[2] = i2;
		}

		flags = pfinalverts[i0].flags | pfinalverts[i1].flags | pfinalverts[i2].flags;
		if (!(flags & (ALIAS_XY_CLIP_MASK | ALIAS_Z_CLIP)))
		{	// totally unclipped
			r_affinetridesc.pfinalverts = pfinalverts;
			r_affinetridesc.ptriangles = &tri;
			D_PolysetDraw();
		}
		else
		{	// partially clipped
			R_AliasClipTriangle(&tri);
		}
	}
}

#endif