#include "quakedef.h"
#include "pr_cmds.h"
#include "cmodel.h"
#include "gl_water.h"

#define	BLOCK_WIDTH		128
#define	BLOCK_HEIGHT	128

#define	MAX_LIGHTMAPS	64

#define MAX_DECALSURFS		500

int		lightmap_bytes;		// 1, 2, or 4
GLint	lightmap_used;

int		lightmap_textures;
#define MAX_BLOCK_LIGHTS	(18 * 18)
colorVec blocklights[MAX_BLOCK_LIGHTS];

int			active_lightmaps;

typedef struct
{
	int l, t, w, h;
} glRect_t;

glpoly_t* lightmap_polys[MAX_LIGHTMAPS];
qboolean	lightmap_modified[MAX_LIGHTMAPS];
glRect_t	lightmap_rectchange[MAX_LIGHTMAPS];


int			allocated[MAX_LIGHTMAPS][BLOCK_WIDTH];

msurface_t* gDecalSurfs[MAX_DECALSURFS];
int gDecalSurfCount;

// the lightmap texture data needs to be kept in
// main memory so texsubimage can update properly
byte		lightmaps[4 * MAX_LIGHTMAPS * BLOCK_WIDTH * BLOCK_HEIGHT];
// For gl_texsort 0
msurface_t* skychain;
msurface_t* waterchain;

extern colorVec gWaterColor;

/*
===============
R_AddDynamicLights
===============
*/
void R_AddDynamicLights( msurface_t* surf )
{
	// TODO: Implement
}


/*
===============
R_BuildLightMap

Build the blocklights array for a given surface and copy to dest
Combine and scale multiple lightmaps into the 8.8 format in blocklights
===============
*/
void R_BuildLightMap( msurface_t* psurf, byte* dest, int stride )
{
	int			smax, tmax;
	int			t;
	int			i, j, k, size;
	color24* lightmap;
	colorVec* bl;
	unsigned	scale;
	int			maps;
	int			maxSize;

	psurf->cached_dlight = (psurf->dlightbits & r_dlightactive);
	psurf->dlightbits &= r_dlightactive;

	smax = (psurf->extents[0] >> 4) + 1;
	tmax = (psurf->extents[1] >> 4) + 1;
	size = smax * tmax;
	lightmap = psurf->samples;

// set to full bright if no light data
	if (r_fullbright.value || !cl.worldmodel->lightdata)
	{
		for (i = 0; i < size; i++)
		{
			blocklights[i].r = 255 * 256;
			blocklights[i].g = 255 * 256;
			blocklights[i].b = 255 * 256;
		}
		goto store;
	}

// clear to no light
	for (i = 0; i < size; i++)
	{
		blocklights[i].r = 0;
		blocklights[i].g = 0;
		blocklights[i].b = 0;
	}

// add all the lightmaps
	if (lightmap)
	{
		for (maps = 0; maps < MAXLIGHTMAPS && psurf->styles[maps] != 255;
			maps++)
		{
			scale = d_lightstylevalue[psurf->styles[maps]];
			psurf->cached_light[maps] = scale;	// 8.8 fraction
			for (i = 0; i < size; i++)
			{
				blocklights[i].r += lightmap[i].r * scale;
				blocklights[i].g += lightmap[i].g * scale;
				blocklights[i].b += lightmap[i].b * scale;
			}
			lightmap += size;	// skip to next lightmap
		}
	}

// add all the dynamic lights
	if (psurf->dlightframe == r_framecount)
		R_AddDynamicLights(psurf);

// bound, invert, and shift
store:
	switch (gl_lightmap_format)
	{
	case GL_RGBA:
		stride -= (smax << 2);
		bl = blocklights;
		for (i = 0; i < tmax; i++, dest += stride)
		{
			for (j = 0; j < smax; j++)
			{
				for (k = 0; k < 3; k++)
				{
					maxSize = ((unsigned int*)bl)[k] >> 6;
					if (maxSize > 1023)
						maxSize = 1023;
					dest[k] = lightgammatable[maxSize] >> 2;
				}
				bl++;

				dest[3] = 255;
				dest += 4;

			}
		}
		break;
	case GL_ALPHA:
	case GL_LUMINANCE:
	case GL_INTENSITY:
		bl = blocklights;
		for (i = 0; i < tmax; i++, dest += stride)
		{
			for (j = 0; j < smax; j++)
			{
				t = bl->r;
				t >>= 8;
				if (t > 255)
					t = 255;
				dest[j] = 255 - t;
				bl++;
			}
		}
		break;
	default:
		Sys_Error("Bad lightmap format");
	}
}


/*
===============
R_TextureAnimation

Returns the proper texture for a given time and base texture
===============
*/
texture_t* R_TextureAnimation( msurface_t* s )
{
	texture_t* base;
	int		reletive;
	int		count;
	static int rtable[20][20];
	int		tu, tv;

	base = s->texinfo->texture;

	if (!rtable[0][0])
	{
		for (tu = 0; tu < 20; tu++)
		{
			for (tv = 0; tv < 20; tv++)
			{
				rtable[tu][tv] = RandomLong(0, 0x7FFF);
			}
		}
	}

	if (currententity->frame)
	{
		if (base->alternate_anims)
			base = base->alternate_anims;
	}

	if (!base->anim_total)
		return base;

	if (base->name[0] == '-')
	{
		tu = (int)((s->texturemins[0] + (base->width << 16)) / base->width) % 20;
		tv = (int)((s->texturemins[1] + (base->height << 16)) / base->height) % 20;
		reletive = rtable[tu][tv] % base->anim_total;
	}
	else
	{
		reletive = (int)(cl.time * 10.0) % base->anim_total;
	}

	count = 0;
	while (base->anim_min > reletive || base->anim_max <= reletive)
	{
		base = base->anim_next;
		if (!base)
			Sys_Error("R_TextureAnimation: broken cycle");
		if (++count > 100)
			Sys_Error("R_TextureAnimation: infinite cycle");
	}

	return base;
}

/*
=============================================================

	BRUSH MODELS

=============================================================
*/

qboolean mtexenabled = FALSE;

void GL_SelectTexture( GLenum target );

void GL_DisableMultitexture( void )
{
	if (mtexenabled)
	{
		qglDisable(GL_TEXTURE_2D);
		GL_SelectTexture(TEXTURE0_SGIS);
		mtexenabled = FALSE;
	}
}

void GL_EnableMultitexture( void )
{
	if (gl_mtexable)
	{
		GL_SelectTexture(TEXTURE1_SGIS);
		qglEnable(GL_TEXTURE_2D);
		mtexenabled = TRUE;
	}
}

// TODO: Implement

/*
================
R_DrawSequentialPoly

Systems that have fast state and texture changes can
just do everything as it passes with no need to sort
================
*/
void R_DrawSequentialPoly( msurface_t* s, int face )
{
	// TODO: Implement
}

/*
================
R_RenderDynamicLightmaps
Multitexture
================
*/
void R_RenderDynamicLightmaps( msurface_t* fa )
{
	// TODO: Implement
}

/*
================
DrawGLWaterPoly

Warp the vertex coordinates
================
*/
void DrawGLWaterPoly( glpoly_t* p )
{
	// TODO: Implement
}

void DrawGLWaterPolyLightmap( glpoly_t* p )
{
	// TODO: Implement
}

/*
================
DrawGLPoly
================
*/
void DrawGLPoly( glpoly_t* p )
{
	int		i;
	float*	v;

	qglBegin(GL_POLYGON);
	v = p->verts[0];
	for (i = 0; i < p->numverts; i++, v += VERTEXSIZE)
	{
		qglTexCoord2f(v[3], v[4]);
		qglVertex3fv(v);
	}
	qglEnd();
}

/*
================
ScrollOffset
================
*/
float ScrollOffset( msurface_t* psurface, cl_entity_t* pEntity )
{
	// TODO: Implement
	return 0.0f;
}

/*
================
DrawGLPolyScroll
================
*/
void DrawGLPolyScroll( msurface_t* psurface, cl_entity_t* pEntity )
{
	// TODO: Implement
}

/*
================
DrawGLSolidPoly
================
*/
void DrawGLSolidPoly( glpoly_t* p )
{
	// TODO: Implement
}

/*
================
R_BlendLightmaps
================
*/
void R_BlendLightmaps( void )
{
	int			i, j;
	glpoly_t* p;
	float* v;

	if (r_fullbright.value)
		return;
	if (!gl_texsort.value)
		return;

	qglDepthMask(0);		// don't bother writing Z

	if (gl_lightmap_format == GL_LUMINANCE)
		qglBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
	else if (gl_lightmap_format == GL_INTENSITY)
	{
		qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		qglColor4f(0, 0, 0, 1);
		qglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else if (gl_lightmap_format == GL_RGBA)
	{
		qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		if (gl_overbright.value)
		{
			qglBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);
			qglColor4f(170 / 255.0, 170 / 255.0, 170 / 255.0, 1);
		}
		else
		{
			qglBlendFunc(GL_ZERO, GL_SRC_COLOR);
		}
	}

	if (!r_lightmap.value)
	{
		qglEnable(GL_BLEND);
	}

	for (i = 0; i < MAX_LIGHTMAPS; i++)
	{
		p = lightmap_polys[i];
		if (!p)
			continue;
		GL_Bind(lightmap_textures + i);
		qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (lightmap_modified[i])
		{
			lightmap_modified[i] = FALSE;
			qglTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
				BLOCK_WIDTH, BLOCK_HEIGHT, gl_lightmap_format, GL_UNSIGNED_BYTE,
				lightmaps + i * BLOCK_WIDTH * BLOCK_HEIGHT * lightmap_bytes);
		}
		for (; p; p = p->chain)
		{
			if (p->flags & SURF_UNDERWATER)
			{
				DrawGLWaterPolyLightmap(p);
			}
			else if (p->flags & SURF_DRAWTURB)
			{
				float tempVert[3];
				glpoly_t* wp;

				for (wp = p; wp; wp = wp->next)
				{
					qglBegin(GL_POLYGON);
					qglColor3f(1, 1, 1);
					v = wp->verts[0];
					for (j = 0; j < wp->numverts; j++, v += VERTEXSIZE)
					{
						qglTexCoord2f(v[5], v[6]);
						VectorCopy(v, tempVert);
						tempVert[2] += turbsin[(int)(cl.time * 160.0 + v[0] + v[1]) & 0xFF] * gl_wateramp.value;
						tempVert[2] += turbsin[(int)(cl.time * 171.0 + v[0] * 5.0 - v[1]) & 0xFF] * gl_wateramp.value * 0.8;
						qglVertex3fv(tempVert);
					}
					qglEnd();
				}
			}
			else
			{
				qglBegin(GL_POLYGON);
				v = p->verts[0];
				for (j = 0; j < p->numverts; j++, v += VERTEXSIZE)
				{
					qglTexCoord2f(v[5], v[6]);
					qglVertex3fv(v);
				}
				qglEnd();
			}
		}
	}

	qglDisable(GL_BLEND);
	if (gl_lightmap_format == GL_LUMINANCE)
		qglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	else if (gl_lightmap_format == GL_INTENSITY)
	{
		qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		qglColor4f(1, 1, 1, 1);
	}

	qglDepthMask(1);		// back to normal Z buffering
}

/*
================
R_RenderBrushPoly
================
*/
void R_RenderBrushPoly( msurface_t* fa )
{
	texture_t*	t;
	byte*		base;
	int			maps;

	c_brush_polys++;

	if (fa->flags & SURF_DRAWSKY)
	{
		return;
	}

	t = R_TextureAnimation(fa);
	GL_Bind(t->gl_texturenum);

	if (fa->flags & SURF_DRAWTURB)
	{	// warp texture, no lightmaps
		EmitWaterPolys(fa, SIDE_FRONT);
		return;
	}

	if (fa->flags & SURF_UNDERWATER)
	{
		DrawGLWaterPoly(fa->polys);
	}
	else if (currententity->rendermode == kRenderTransColor)
	{
		qglDisable(GL_TEXTURE_2D);
		DrawGLSolidPoly(fa->polys);
		qglEnable(GL_TEXTURE_2D);
	}
	else if (fa->flags & SURF_DRAWTILED)
	{
		DrawGLPolyScroll(fa, currententity);
	}
	else
	{
		DrawGLPoly(fa->polys);
	}

	// add the poly to the proper lightmap chain

	fa->polys->chain = lightmap_polys[fa->lightmaptexturenum];
	lightmap_polys[fa->lightmaptexturenum] = fa->polys;

	if (fa->pdecals)
	{
		gDecalSurfs[gDecalSurfCount] = fa;
		gDecalSurfCount++;

		if (gDecalSurfCount > MAX_DECALSURFS)
			Sys_Error("Too many decal surfaces!\n");
	}

	// check for lightmap modification
	for (maps = 0; maps < MAXLIGHTMAPS && fa->styles[maps] != 255;
		 maps++)
	{
		if (d_lightstylevalue[fa->styles[maps]] != fa->cached_light[maps])
			goto dynamic;
	}

	if (fa->dlightframe == r_framecount	// dynamic this frame
		|| fa->cached_dlight)			// dynamic previously
	{
	dynamic:
		if (r_dynamic.value)
		{
			lightmap_modified[fa->lightmaptexturenum] = TRUE;
			base = lightmaps + fa->lightmaptexturenum * lightmap_bytes * BLOCK_WIDTH * BLOCK_HEIGHT;
			base += fa->light_t * BLOCK_WIDTH * lightmap_bytes + fa->light_s * lightmap_bytes;
			R_BuildLightMap(fa, base, BLOCK_WIDTH * lightmap_bytes);
		}
	}
}

/*
================
R_MirrorChain
================
*/
void R_MirrorChain( msurface_t* s )
{
	if (mirror)
		return;
	mirror = TRUE;
	mirror_plane = s->plane;
}

/*
================
R_DrawWaterChain
================
*/
void R_DrawWaterChain( msurface_t* pChain )
{
	// TODO: Implement
}

/*
================
DrawTextureChains
================
*/
void DrawTextureChains( void )
{
	int		i;
	msurface_t* s;
	texture_t* t;
	int iSounds;

	currententity = cl_entities;

	iSounds = 100;
	if (!gl_texsort.value)
	{
		GL_DisableMultitexture();

		if (skychain)
		{
			R_DrawSkyChain(skychain);
			skychain = NULL;
		}

		if (waterchain)
		{
			R_DrawWaterChain(waterchain);
			waterchain = NULL;
		}

		// JAY: Disable this return to turn on water lightmaps (broken)
		//return;
	}

	for (i = 0; i < cl.worldmodel->numtextures; i++)
	{
		t = cl.worldmodel->textures[i];
		if (!t)
			continue;
		s = t->texturechain;
		if (!s)
			continue;
		if (i == skytexturenum)
		{
			R_DrawSkyChain(t->texturechain);
		}
		else if (i == mirrortexturenum && r_mirroralpha.value != 1.0)
		{
			R_MirrorChain(s);
			continue;
		}
		else
		{
			if ((s->flags & SURF_DRAWTURB) && r_wateralpha.value != 1.0)
				continue;	// draw translucent water later
			for (; s; s = s->texturechain)
				R_RenderBrushPoly(s);
		}

		t->texturechain = NULL;

		if (iSounds-- == 0)
		{
			S_ExtraUpdate();
			iSounds = 100;
		}
	}
}

// TODO: Implement

/*
=============================================================

	WORLD MODEL

=============================================================
*/

/*
================
R_RecursiveWorldNode
================
*/
void R_RecursiveWorldNode( mnode_t* node )
{
	int			c, side;
	mplane_t* plane;
	msurface_t* surf, ** mark;
	mleaf_t* pleaf;
	double		dot;

	if (node->contents == CONTENTS_SOLID)
		return;		// solid

	if (node->visframe != r_visframecount)
		return;
	if (R_CullBox(node->minmaxs, node->minmaxs + 3))
		return;

	// if a leaf node, draw stuff
	if (node->contents < 0)
	{
		pleaf = (mleaf_t*)node;

		mark = pleaf->firstmarksurface;
		c = pleaf->nummarksurfaces;

		if (c)
		{
			do
			{
				(*mark)->visframe = r_framecount;
				mark++;
			} while (--c);
		}

		// deal with model fragments in this leaf
		if (pleaf->efrags)
			R_StoreEfrags(&pleaf->efrags);

		return;
	}

// node is just a decision point, so go down the apropriate sides

// find which side of the node we are on
	plane = node->plane;

	switch (plane->type)
	{
	case PLANE_X:
		dot = modelorg[0] - plane->dist;
		break;
	case PLANE_Y:
		dot = modelorg[1] - plane->dist;
		break;
	case PLANE_Z:
		dot = modelorg[2] - plane->dist;
		break;
	default:
		dot = DotProduct(modelorg, plane->normal) - plane->dist;
		break;
	}

	if (dot >= 0)
		side = 0;
	else
		side = 1;

// recurse down the children, front side first
	R_RecursiveWorldNode(node->children[side]);

// draw stuff
	c = node->numsurfaces;

	if (c)
	{
		surf = cl.worldmodel->surfaces + node->firstsurface;

		if (dot < 0 - BACKFACE_EPSILON)
			side = SURF_PLANEBACK;
		else if (dot > BACKFACE_EPSILON)
			side = 0;
		{
			for (; c; c--, surf++)
			{
				if (surf->visframe != r_framecount)
					continue;

				// don't backface underwater surfaces, because they warp
				if (!(surf->flags & SURF_UNDERWATER) && ((dot < 0) ^ !!(surf->flags & SURF_PLANEBACK)))
					continue;		// wrong side

				// if sorting by texture, just store it out
				if (gl_texsort.value)
				{
					if (!mirror
						|| surf->texinfo->texture != cl.worldmodel->textures[mirrortexturenum])
					{
						surf->texturechain = surf->texinfo->texture->texturechain;
						surf->texinfo->texture->texturechain = surf;
					}
				}
				else if (surf->flags & SURF_DRAWSKY)
				{
					surf->texturechain = skychain;
					skychain = surf;
				}
				else if (surf->flags & SURF_DRAWTURB)
				{
					surf->texturechain = waterchain;
					waterchain = surf;
				}
				else
				{
					R_DrawSequentialPoly(surf, SIDE_FRONT);
				}
			}
		}

	}

// recurse down the back side
	R_RecursiveWorldNode(node->children[!side]);
}



/*
=============
R_DrawWorld
=============
*/
void R_DrawWorld( void )
{
	cl_entity_t ent;

	memset(&ent, 0, sizeof(ent));
	ent.model = cl.worldmodel;

	VectorCopy(r_refdef.vieworg, modelorg);

	ent.rendercolor.r = gWaterColor.r;
	ent.rendercolor.g = gWaterColor.g;
	ent.rendercolor.b = gWaterColor.b;

	currententity = &ent;
	currenttexture = -1;

	qglColor3f(1, 1, 1);
	memset(lightmap_polys, 0, sizeof(lightmap_polys));

	R_ClearSkyBox();

	if (!gl_texsort.value)
	{
		switch (gl_lightmap_format)
		{
		case GL_LUMINANCE:
			qglBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
			break;
		case GL_INTENSITY:
			qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			qglColor4f(0, 0, 0, 1);
			qglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			break;
		case GL_RGBA:
			if (gl_overbright.value)
			{
				qglBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);
				qglColor4f(170 / 255.0, 170 / 255.0, 170 / 255.0, 1);
			}
			else
			{
				qglBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
			}
			break;
		}
	}

	R_RecursiveWorldNode(cl.worldmodel->nodes);
	gDecalSurfCount = 0;

	DrawTextureChains();
	S_ExtraUpdate();

	R_DrawDecals();

	R_BlendLightmaps();
}

/*
===============
R_MarkLeaves
===============
*/
void R_MarkLeaves( void )
{
	byte* vis;
	mnode_t* node;
	int		i;
	byte	solid[4096];

	if (r_oldviewleaf == r_viewleaf && !r_novis.value)
		return;

	if (mirror)
		return;

	r_visframecount++;
	r_oldviewleaf = r_viewleaf;

	if (r_novis.value)
	{
		vis = solid;
		memset(solid, 0xff, (cl.worldmodel->numleafs + 7) >> 3);
	}
	else
		vis = Mod_LeafPVS(r_viewleaf, cl.worldmodel);

	for (i = 0; i < cl.worldmodel->numleafs; i++)
	{
		if (vis[i >> 3] & (1 << (i & 7)))
		{
			node = (mnode_t*)&cl.worldmodel->leafs[i + 1];
			do
			{
				if (node->visframe == r_visframecount)
					break;
				node->visframe = r_visframecount;
				node = node->parent;
			} while (node);
		}
	}
}

// TODO: Implement

/*
=============================================================================

  LIGHTMAP ALLOCATION

=============================================================================
*/

// returns a texture number and the position inside it
int AllocBlock( int w, int h, int* x, int* y )
{
	int		i, j;
	int		best, best2;
	int		texnum;

	for (texnum = 0; texnum < MAX_LIGHTMAPS; texnum++)
	{
		best = BLOCK_HEIGHT;

		for (i = 0; i < BLOCK_WIDTH - w; i++)
		{
			best2 = 0;

			for (j = 0; j < w; j++)
			{
				if (allocated[texnum][i + j] >= best)
					break;
				if (allocated[texnum][i + j] > best2)
					best2 = allocated[texnum][i + j];
			}
			if (j == w)
			{	// this is a valid spot
				*x = i;
				*y = best = best2;
			}
		}

		if (best + h > BLOCK_HEIGHT)
			continue;

		for (i = 0; i < w; i++)
			allocated[texnum][*x + i] = best + h;

		return texnum;
	}

	Sys_Error("AllocBlock: full");
	return 0;
}


mvertex_t* r_pcurrentvertbase;
model_t* currentmodel;

int	nColinElim;

/*
================
BuildSurfaceDisplayList
================
*/
void BuildSurfaceDisplayList( msurface_t* fa )
{
	int			i, lindex, lnumverts;
	medge_t*	pedges, * r_pedge;
	int			vertpage;
	float*		vec;
	float		s, t;
	glpoly_t* poly;

// reconstruct the polygon
	pedges = currentmodel->edges;
	lnumverts = fa->numedges;
	vertpage = 0;

	//
	// draw texture
	//
	poly = (glpoly_t*)Hunk_Alloc(sizeof(glpoly_t) + (lnumverts - 4) * VERTEXSIZE * sizeof(float));
	poly->next = fa->polys;
	poly->flags = fa->flags;
	fa->polys = poly;
	poly->numverts = lnumverts;

	for (i = 0; i < lnumverts; i++)
	{
		lindex = currentmodel->surfedges[fa->firstedge + i];

		if (lindex > 0)
		{
			r_pedge = &pedges[lindex];
			vec = r_pcurrentvertbase[r_pedge->v[0]].position;
		}
		else
		{
			r_pedge = &pedges[-lindex];
			vec = r_pcurrentvertbase[r_pedge->v[1]].position;
		}
		s = DotProduct(vec, fa->texinfo->vecs[0]) + fa->texinfo->vecs[0][3];
		s /= fa->texinfo->texture->width;

		t = DotProduct(vec, fa->texinfo->vecs[1]) + fa->texinfo->vecs[1][3];
		t /= fa->texinfo->texture->height;

		VectorCopy(vec, poly->verts[i]);
		poly->verts[i][3] = s;
		poly->verts[i][4] = t;

		//
		// lightmap texture coordinates
		//
		s = DotProduct(vec, fa->texinfo->vecs[0]) + fa->texinfo->vecs[0][3];
		s -= fa->texturemins[0];
		s += fa->light_s * 16;
		s += 8;
		s /= BLOCK_WIDTH * 16; //fa->texinfo->texture->width;

		t = DotProduct(vec, fa->texinfo->vecs[1]) + fa->texinfo->vecs[1][3];
		t -= fa->texturemins[1];
		t += fa->light_t * 16;
		t += 8;
		t /= BLOCK_HEIGHT * 16; //fa->texinfo->texture->height;

		poly->verts[i][5] = s;
		poly->verts[i][6] = t;
	}

	//
	// remove co-linear points - Ed
	//
	if (!gl_keeptjunctions.value && !(fa->flags & SURF_UNDERWATER))
	{
		for (i = 0; i < lnumverts; ++i)
		{
			vec3_t v1, v2;
			float* prev, * thisPoint, * next;

			prev = poly->verts[(i + lnumverts - 1) % lnumverts];
			thisPoint = poly->verts[i];
			next = poly->verts[(i + 1) % lnumverts];

			VectorSubtract(thisPoint, prev, v1);
			VectorNormalize(v1);
			VectorSubtract(next, prev, v2);
			VectorNormalize(v2);

			// skip co-linear points
#define COLINEAR_EPSILON 0.001
			if ((fabs(v1[0] - v2[0]) <= COLINEAR_EPSILON) &&
				(fabs(v1[1] - v2[1]) <= COLINEAR_EPSILON) &&
				(fabs(v1[2] - v2[2]) <= COLINEAR_EPSILON))
			{
				int j;
				for (j = i + 1; j < lnumverts; ++j)
				{
					int k;
					for (k = 0; k < VERTEXSIZE; ++k)
						poly->verts[j - 1][k] = poly->verts[j][k];
				}
				--lnumverts;
				++nColinElim;
				// retry next vertex next time, which is now current vertex
				--i;
			}
		}
	}
	poly->numverts = lnumverts;
}

/*
========================
GL_CreateSurfaceLightmap
========================
*/
void GL_CreateSurfaceLightmap( msurface_t* surf )
{
	int		smax, tmax;
	byte* base;

	if (surf->flags & (SURF_DRAWSKY | SURF_DRAWTURB))
		return;

	if ((surf->flags & SURF_DRAWTILED) && (surf->texinfo->flags & TEX_SPECIAL))
		return;

	smax = (surf->extents[0] >> 4) + 1;
	tmax = (surf->extents[1] >> 4) + 1;

	surf->lightmaptexturenum = AllocBlock(smax, tmax, &surf->light_s, &surf->light_t);
	base = lightmaps + surf->lightmaptexturenum * lightmap_bytes * BLOCK_WIDTH * BLOCK_HEIGHT;
	base += (surf->light_t * BLOCK_WIDTH + surf->light_s) * lightmap_bytes;
	R_BuildLightMap(surf, base, BLOCK_WIDTH * lightmap_bytes);
}

/*
==================
GL_BuildLightmaps

Builds the lightmap texture
with all the surfaces from all brush models
==================
*/
void GL_BuildLightmaps( void )
{
	int		i, j;
	model_t* m;

	memset(allocated, 0, sizeof(allocated));

	r_framecount = 1;		// no dlightcache

	if (!lightmap_textures)
	{
		lightmap_textures = texture_extension_number;
		texture_extension_number += MAX_LIGHTMAPS;
	}

	gl_lightmap_format = GL_RGBA;

	if (COM_CheckParm("-lm_1"))
		gl_lightmap_format = GL_LUMINANCE;
	if (COM_CheckParm("-lm_a"))
		gl_lightmap_format = GL_ALPHA;
	if (COM_CheckParm("-lm_i"))
		gl_lightmap_format = GL_INTENSITY;
	if (COM_CheckParm("-lm_2"))
		gl_lightmap_format = GL_RGBA4;
	if (COM_CheckParm("-lm_4"))
		gl_lightmap_format = GL_RGBA;

	switch (gl_lightmap_format)
	{
	case GL_RGBA:
		lightmap_bytes = 4;
		lightmap_used = 3;
		break;
	case GL_RGBA4:
		lightmap_bytes = 2;
		lightmap_used = 2;
		break;
	case GL_LUMINANCE:
	case GL_INTENSITY:
	case GL_ALPHA:
		lightmap_bytes = 1;
		lightmap_used = 1;
		break;
	}

	for (j = 1; j < MAX_MODELS; j++)
	{
		m = cl.model_precache[j];
		if (!m)
			break;
		if (m->name[0] == '*')
			continue;
		r_pcurrentvertbase = m->vertexes;
		currentmodel = m;
		for (i = 0; i < m->numsurfaces; i++)
		{
			GL_CreateSurfaceLightmap(m->surfaces + i);
			if (m->surfaces[i].flags & SURF_DRAWTURB)
				continue;

			BuildSurfaceDisplayList(m->surfaces + i);
		}
	}

	if (!gl_texsort.value)
		GL_SelectTexture(TEXTURE1_SGIS);

	//
	// upload all lightmaps that were filled
	//
	for (i = 0; i < MAX_LIGHTMAPS; i++)
	{
		if (!allocated[i][0])
			break;		// no more used
		lightmap_modified[i] = FALSE;
		GL_Bind(lightmap_textures + i);
		qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		qglTexImage2D(GL_TEXTURE_2D, 0, lightmap_used,
			BLOCK_WIDTH, BLOCK_HEIGHT, 0,
			gl_lightmap_format, GL_UNSIGNED_BYTE, lightmaps + i * BLOCK_WIDTH * BLOCK_HEIGHT * lightmap_bytes);
	}

	if (!gl_texsort.value)
		GL_SelectTexture(TEXTURE0_SGIS);
}

// TODO: Implement


// Init the decal pool
void R_DecalInit( void )
{
	// TODO: Implement
}






void R_DecalRemoveAll( int textureIndex )
{
	// TODO: Implement
}



// Renders all decals
void R_DrawDecals( void )
{
	// TODO: Implement
}