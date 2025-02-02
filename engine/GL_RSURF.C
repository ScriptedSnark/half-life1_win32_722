#include "quakedef.h"
#include "cmodel.h"
#include "decal.h"
#include "draw.h"
#include "pr_cmds.h"
#include "gl_water.h"

#define MAX_DECALSURFS		500

int		lightmap_bytes;		// 1, 2, or 4
GLint	lightmap_used;

int		lightmap_textures;
#define MAX_BLOCK_LIGHTS	(18 * 18)
colorVec blocklights[MAX_BLOCK_LIGHTS];

#define	BLOCK_WIDTH		128
#define	BLOCK_HEIGHT	128

#define	MAX_LIGHTMAPS	64
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
	int			lnum;
	int			sd, td;
	float		dist, rad, minlight;
	vec3_t		impact, local;
	int			s, t;
	int			smax, tmax;
	mtexinfo_t* tex;

	smax = (surf->extents[0] >> 4) + 1;
	tmax = (surf->extents[1] >> 4) + 1;
	tex = surf->texinfo;

	for (lnum = 0; lnum < MAX_DLIGHTS; lnum++)
	{
		if (!(surf->dlightbits & (1 << lnum)))
			continue;		// not lit by this light

		VectorSubtract(cl_dlights[lnum].origin, currententity->origin, impact);

		rad = cl_dlights[lnum].radius;
		dist = DotProduct(impact, surf->plane->normal)
			- surf->plane->dist;
		rad -= fabs(dist);
		minlight = cl_dlights[lnum].minlight;
		if (rad < minlight)
			continue;
		minlight = rad - minlight;

		local[0] = DotProduct(impact, tex->vecs[0]) + tex->vecs[0][3];
		local[1] = DotProduct(impact, tex->vecs[1]) + tex->vecs[1][3];

		local[0] -= surf->texturemins[0];
		local[1] -= surf->texturemins[1];

		for (t = 0; t < tmax; t++)
		{
			td = local[1] - t * 16;
			if (td < 0)
				td = -td;
			for (s = 0; s < smax; s++)
			{
				sd = local[0] - s * 16;
				if (sd < 0)
					sd = -sd;
				if (sd > td)
					dist = sd + (td >> 1);
				else
					dist = td + (sd >> 1);
				if (dist < minlight)
				{
					unsigned delta;
					delta = (rad - dist) * 256;

					blocklights[t * smax + s].r += (delta * cl_dlights[lnum].color.r) >> 8;
					blocklights[t * smax + s].g += (delta * cl_dlights[lnum].color.g) >> 8;
					blocklights[t * smax + s].b += (delta * cl_dlights[lnum].color.b) >> 8;
				}
			}
		}
	}
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
	int		i;
	float*	v;
	vec3_t	nv;

	qglBegin(GL_TRIANGLE_FAN);
	v = p->verts[0];
	for (i = 0; i < p->numverts; i++, v += VERTEXSIZE)
	{
		qglTexCoord2f(v[3], v[4]);
		nv[0] = v[0] + 8 * sin(v[1] * 0.05 + realtime) * sin(v[2] * 0.05 + realtime);
		nv[1] = v[1] + 8 * sin(v[0] * 0.05 + realtime) * sin(v[2] * 0.05 + realtime);
		nv[2] = v[2];
		qglVertex3fv(nv);
	}
	qglEnd();
}

void DrawGLWaterPolyLightmap( glpoly_t* p )
{
	int		i;
	float*	v;
	vec3_t	nv;

	qglBegin(GL_TRIANGLE_FAN);
	v = p->verts[0];
	for (i = 0; i < p->numverts; i++, v += VERTEXSIZE)
	{
		qglTexCoord2f(v[5], v[6]);
		nv[0] = v[0] + 8 * sin(v[1] * 0.05 + realtime) * sin(v[2] * 0.05 + realtime);
		nv[1] = v[1] + 8 * sin(v[0] * 0.05 + realtime) * sin(v[2] * 0.05 + realtime);
		nv[2] = v[2];
		qglVertex3fv(nv);
	}
	qglEnd();
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
	float speed;
	float sOffset;

	sOffset = (float)(pEntity->rendercolor.b + (pEntity->rendercolor.g << 8)) / 16.0f;
	if (!pEntity->rendercolor.r)
		sOffset = -sOffset;

	speed = (1.0 / psurface->texinfo->texture->width) * sOffset * cl.time;

	if (speed < 0.0)
		return fmod(speed, -1);
	else
		return fmod(speed, 1);
}

/*
================
DrawGLPolyScroll
================
*/
void DrawGLPolyScroll( msurface_t* psurface, cl_entity_t* pEntity )
{
	int		i;
	float*	v;
	float sOffset;
	glpoly_t* p;

	sOffset = ScrollOffset(psurface, pEntity);
	p = psurface->polys;

	qglBegin(GL_POLYGON);
	v = p->verts[0];
	for (i = 0; i < p->numverts; i++, v += VERTEXSIZE)
	{
		qglTexCoord2f(v[3] + sOffset, v[4]);
		qglVertex3fv(v);
	}
	qglEnd();
}

/*
================
DrawGLSolidPoly
================
*/
void DrawGLSolidPoly( glpoly_t* p )
{
	int		i;
	float*	v;

	qglColor4f(currententity->rendercolor.r / 256.0,
		currententity->rendercolor.g / 256.0,
		currententity->rendercolor.b / 256.0, r_blend);

	qglBegin(GL_POLYGON);
	v = p->verts[0];
	for (i = 0; i < p->numverts; i++, v += VERTEXSIZE)
	{
		qglVertex3fv(v);
	}
	qglEnd();
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
	msurface_t* s;

	for (s = pChain; s; s = s->texturechain)
	{
		GL_Bind(s->texinfo->texture->gl_texturenum);
		EmitWaterPolys(s, 0);
	}
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

/*
=================
R_SetRenderMode
=================
*/
void R_SetRenderMode( cl_entity_t* pEntity )
{
	switch (pEntity->rendermode)
	{
	case kRenderNormal:
		qglColor4f(1, 1, 1, 1);
		break;
	case kRenderTransColor:
		qglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		qglTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ALPHA);
		qglEnable(GL_BLEND);
		break;
	case kRenderTransAlpha:
		qglEnable(GL_ALPHA_TEST);
		qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		qglColor4f(1, 1, 1, 1);
		qglDisable(GL_BLEND);
		qglAlphaFunc(GL_GREATER, gl_alphamin.value);
		break;
	case kRenderTransAdd:
		qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		qglBlendFunc(GL_ONE, GL_ONE);
		qglColor4f(r_blend, r_blend, r_blend, 1);
		qglDepthMask(GL_FALSE);
		qglEnable(GL_BLEND);
		break;
	default:
		qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		qglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		qglColor4f(1, 1, 1, r_blend);
		qglDepthMask(GL_FALSE);
		qglEnable(GL_BLEND);
		break;
	}
}

/*
=================
R_DrawBrushModel
=================
*/
void R_DrawBrushModel( cl_entity_t* e )
{
	int			i;
	int			k;
	vec3_t		mins, maxs;
	msurface_t* psurf;
	float		dot;
	mplane_t*	pplane;
	model_t*	clmodel;
	qboolean	rotated;

	currententity = e;
	currenttexture = -1;

	clmodel = e->model;

	if (e->angles[0] || e->angles[1] || e->angles[2])
	{
		rotated = TRUE;
		for (i = 0; i < 3; i++)
		{
			mins[i] = e->origin[i] - clmodel->radius;
			maxs[i] = e->origin[i] + clmodel->radius;
		}
	}
	else
	{
		rotated = FALSE;
		VectorAdd(e->origin, clmodel->mins, mins);
		VectorAdd(e->origin, clmodel->maxs, maxs);
	}

	if (R_CullBox(mins, maxs))
		return;

	qglColor3f(1, 1, 1);
	memset(lightmap_polys, 0, sizeof(lightmap_polys));

	VectorSubtract(r_refdef.vieworg, e->origin, modelorg);
	if (rotated)
	{
		vec3_t	temp;
		vec3_t	forward, right, up;

		VectorCopy(modelorg, temp);
		AngleVectors(e->angles, forward, right, up);
		modelorg[0] = DotProduct(temp, forward);
		modelorg[1] = -DotProduct(temp, right);
		modelorg[2] = DotProduct(temp, up);
	}

	psurf = &clmodel->surfaces[clmodel->firstmodelsurface];

// calculate dynamic lighting for bmodel if it's not an
// instanced model
	if (clmodel->firstmodelsurface != 0 && !gl_flashblend.value)
	{
		for (k = 0; k < MAX_DLIGHTS; k++)
		{
			if ((cl_dlights[k].die < cl.time) ||
				(!cl_dlights[k].radius))
				continue;

			vec3_t saveOrigin;
			VectorCopy(cl_dlights[k].origin, saveOrigin);
			VectorSubtract(cl_dlights[k].origin, e->origin, cl_dlights[k].origin);

			R_MarkLights(&cl_dlights[k], 1 << k,
				clmodel->nodes + clmodel->hulls[0].firstclipnode);

			VectorCopy(saveOrigin, cl_dlights[k].origin);
		}
	}

	qglPushMatrix();

	R_RotateForEntity(e);
	R_SetRenderMode(e);
	
	//
	// draw texture
	//
	for (i = 0; i < clmodel->nummodelsurfaces; i++, psurf++)
	{	
		// find which side of the node we are on
		pplane = psurf->plane;

		if (psurf->flags & SURF_DRAWTURB)
		{
			if (pplane->type != PLANE_Z && !gl_watersides.value)
				continue;
			if ((mins[2] + 1.0) >= pplane->dist)
				continue;
		}

		dot = DotProduct(modelorg, pplane->normal) - pplane->dist;

		// draw the polygon
		if (((psurf->flags & SURF_PLANEBACK) && (dot < -BACKFACE_EPSILON)) ||
			(!(psurf->flags & SURF_PLANEBACK) && (dot > BACKFACE_EPSILON)))
		{
			if (gl_texsort.value)
			{
				R_RenderBrushPoly(psurf);
			}
			else
			{
				R_SetRenderMode(e);
				R_DrawSequentialPoly(psurf, SIDE_FRONT);
			}
		}
		else
		{
			if (psurf->flags & SURF_DRAWTURB)
			{
				R_SetRenderMode(e);
				R_DrawSequentialPoly(psurf, SIDE_BACK);
			}
		}
	}

	if (currententity->rendermode != kRenderNormal)
	{
		qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		qglDisable(GL_BLEND);
	}

	if (gl_texsort.value || currententity->rendermode == kRenderTransColor)
	{
		if (currententity->rendermode == kRenderTransAlpha)
		{
			if (gl_lightholes.value)
			{
				qglDepthFunc(GL_EQUAL);
				R_BlendLightmaps();

				if (!gl_ztrick.value || gldepthmin < 0.5)
					qglDepthFunc(GL_LEQUAL);
				else
					qglDepthFunc(GL_GEQUAL);
			}
		}
		else
		{
			R_DrawDecals();

			if (currententity->rendermode == kRenderNormal)
			{
				R_BlendLightmaps();
			}
		}
	}

	qglPopMatrix();

	qglDepthMask(GL_TRUE);
	qglDisable(GL_ALPHA_TEST);
	qglAlphaFunc(GL_NOTEQUAL, 0);
	qglDisable(GL_BLEND);
}

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

//-----------------------------------------------------------------------------
//
// Decal system
//
//-----------------------------------------------------------------------------

// UNDONE: Compress this???  256K here?
static decal_t			gDecalPool[MAX_DECALS];
static int				gDecalCount;					// Pool index
static vec3_t			gDecalPos;

static model_t*			gDecalModel = NULL;
static texture_t*		gDecalTexture = NULL;
static int				gDecalSize, gDecalIndex;
static int				gDecalFlags, gDecalEntity;

int R_DecalUnProject( decal_t* pdecal, vec_t* position );
void R_DecalCreate( msurface_t* psurface, int textureIndex, float scale, float x, float y );
void R_DecalShoot( int textureIndex, int entity, int modelIndex, vec_t* position, int flags );
void R_InvalidateSurface( msurface_t* surface );

#define DECAL_DISTANCE			4

// Empirically determined constants for minimizing overalpping decals
#define MAX_OVERLAP_DECALS		4
#define DECAL_OVERLAP_DIST		8


// Init the decal pool
void R_DecalInit( void )
{
	memset(gDecalPool, 0, sizeof(gDecalPool));
	gDecalCount = 0;
}


// Unlink pdecal from any surface it's attached to
void R_DecalUnlink( decal_t* pdecal )
{
	decal_t* tmp;

	if (pdecal->psurface)
	{
		if (pdecal->psurface->pdecals == pdecal)
		{
			pdecal->psurface->pdecals = pdecal->pnext;
		}
		else
		{
			tmp = pdecal->psurface->pdecals;
			if (!tmp)
			{
				Sys_Error("Bad decal list");
				return;
			}

			while (tmp->pnext)
			{
				if (tmp->pnext == pdecal)
				{
					tmp->pnext = pdecal->pnext;
					break;
				}

				tmp = tmp->pnext;
			}
		}
	}

	pdecal->psurface = NULL;
}


// Just reuse next decal in list
// A decal that spans multiple surfaces will use multiple decal_t pool entries, as each surface needs
// it's own.
decal_t* R_DecalAlloc( decal_t* pdecal )
{
	if (!pdecal)
	{
		int count;

		count = 0;		// Check for the odd possiblity of infinte loop
		do
		{
			gDecalCount++;
			if (gDecalCount >= MAX_DECALS)
				gDecalCount = 0;
			pdecal = gDecalPool + gDecalCount;	// reuse next decal
			count++;
		} while ((pdecal->flags & FDECAL_PERMANENT) && count < MAX_DECALS);
	}

	// If decal is already linked to a surface, unlink it.
	R_DecalUnlink(pdecal);

	return pdecal;
}


// remove all decals
void R_DecalRemoveAll( int textureIndex )
{
	int i;
	decal_t* pDecal;

	for (i = 0; i < MAX_DECALS; i++)
	{
		pDecal = &gDecalPool[i];

		if (pDecal->texture == textureIndex)
		{
			R_DecalUnlink(pDecal);
			memset(pDecal, 0, sizeof(decal_t));
		}
	}
}

// iterate over all surfaces on a node, looking for surfaces to decal
void R_DecalNode( mnode_t* node )
{
	mplane_t* splitplane;
	float		dist;

	if (!node)
		return;

	if (node->contents < 0)
		return;

	splitplane = node->plane;
	dist = DotProduct(gDecalPos, splitplane->normal) - splitplane->dist;

	// This is arbitrarily set to 10 right now.  In an ideal world we'd have the 
	// exact surface but we don't so, this tells me which planes are "sort of 
	// close" to the gunshot -- the gunshot is actually 4 units in front of the 
	// wall (see dlls\weapons.cpp). We also need to check to see if the decal 
	// actually intersects the texture space of the surface, as this method tags
	// parallel surfaces in the same node always.
	// JAY: This still tags faces that aren't correct at edges because we don't 
	// have a surface normal

	if (dist > gDecalSize)
	{
		R_DecalNode(node->children[0]);
	}
	else if (dist < -gDecalSize)
	{
		R_DecalNode(node->children[1]);
	}
	else
	{
		if (dist < DECAL_DISTANCE && dist > -DECAL_DISTANCE)
		{
			int			w, h;
			float		s, t, scale;
			msurface_t* surf;
			int			i;
			mtexinfo_t* tex;

			surf = gDecalModel->surfaces + node->firstsurface;

			// iterate over all surfaces in the node
			for (i = 0; i < node->numsurfaces; i++, surf++)
			{
				if (surf->flags & (SURF_DRAWTILED | SURF_DRAWTURB))
					continue;

				tex = surf->texinfo;

				scale = Length(tex->vecs[0]);
				if (scale == 0)
					continue;

				// project decal center into the texture space of the surface
				s = DotProduct(gDecalPos, tex->vecs[0]) + tex->vecs[0][3] - surf->texturemins[0];
				t = DotProduct(gDecalPos, tex->vecs[1]) + tex->vecs[1][3] - surf->texturemins[1];

				w = gDecalTexture->width * scale;
				h = gDecalTexture->height * scale;

				// move s,t to upper left corner
				s -= (w * 0.5);
				t -= (h * 0.5);

				if (s <= -w || t <= -h ||
					s > (surf->extents[0] + w) || t > (surf->extents[1] + h))
				{
					continue; // nope
				}

				scale = 1.0 / scale;
				s = (surf->texturemins[0] + s) / (float)tex->texture->width;
				t = (surf->texturemins[1] + t) / (float)tex->texture->height;

				// stamp it
				R_DecalCreate(surf, gDecalIndex, scale, s, t);
			}
		}

		R_DecalNode(node->children[0]);
		R_DecalNode(node->children[1]);
	}
}

int DecalListAdd( DECALLIST* pList, int count )
{
	int			i;
	vec3_t		tmp;
	DECALLIST* pdecal;

	pdecal = pList + count;
	for (i = 0; i < count; i++)
	{
		if (pdecal->name == pList[i].name &&
			pdecal->entityIndex == pList[i].entityIndex)
		{
			VectorSubtract(pdecal->position, pList[i].position, tmp);	// Merge
			if (Length(tmp) < 2)	// UNDONE: Tune this '2' constant
				return count;
		}
	}

	// This is a new decal
	return count + 1;
}


typedef int (*qsortFunc_t)( const void *, const void * );

int DecalDepthCompare( const DECALLIST* elem1, const DECALLIST* elem2 )
{
	if (elem1->depth > elem2->depth)
		return -1;
	if (elem1->depth < elem2->depth)
		return 1;

	return 0;
}

int DecalListCreate( DECALLIST* pList )
{
	int total = 0;
	int i, depth;

	for (i = 0; i < MAX_DECALS; i++)
	{
		decal_t* decal = &gDecalPool[i];

		// Decal is in use and is not a custom decal
		if (!decal->psurface || (decal->flags & FDECAL_CUSTOM))
			continue;

		decal_t* pdecals;
		texture_t* ptexture;

		// compute depth
		pdecals = decal->psurface->pdecals;
		depth = 0;
		while (pdecals && pdecals != decal)
		{
			depth++;
			pdecals = pdecals->pnext;
		}
		pList[total].depth = depth;
		pList[total].flags = decal->flags;

		pList[total].entityIndex = R_DecalUnProject(decal, pList[total].position);

		ptexture = Draw_DecalTexture(decal->texture);
		pList[total].name = ptexture->name;

		// Check to see if the decal should be addedo
		total = DecalListAdd(pList, total);
	}

	// Sort the decals lowest depth first, so they can be re-applied in order
	qsort(pList, total, sizeof(DECALLIST), (qsortFunc_t)DecalDepthCompare);

	return total;
}
// ---------------------------------------------------------

int R_DecalUnProject( decal_t* pdecal, vec_t* position )
{
	float s, t;
	float scale;
	float inverseScale;
	mtexinfo_t* pTexinfo;
	texture_t* ptexture;
	int entityIndex = 0;

	if (!pdecal || !pdecal->psurface)
		return -1;

	pTexinfo = pdecal->psurface->texinfo;

	s = (float)pTexinfo->texture->width * pdecal->dx - (float)pdecal->psurface->texturemins[0];
	t = (float)pTexinfo->texture->height * pdecal->dy - (float)pdecal->psurface->texturemins[1];

	scale = Length(pTexinfo->vecs[0]) * 0.5;
	ptexture = Draw_DecalTexture(pdecal->texture);

	s = (float)ptexture->width * scale + s + (float)pdecal->psurface->texturemins[0] - pTexinfo->vecs[0][3];
	t = (float)ptexture->height * scale + t + (float)pdecal->psurface->texturemins[1] - pTexinfo->vecs[1][3];

	inverseScale = fabs(Length(pTexinfo->vecs[0]));

	if (inverseScale != 0.0)
		inverseScale = (1.0 / inverseScale) * (1.0 / inverseScale);

	VectorScale(pTexinfo->vecs[0], s * inverseScale, position);

	VectorMA(position, t * inverseScale, pTexinfo->vecs[1], position);
	VectorMA(position, pdecal->psurface->plane->dist, pdecal->psurface->plane->normal, position);

	entityIndex = pdecal->entityIndex;

	if (pdecal->entityIndex)
	{
		hull_t* phull;
		vec3_t temp;
		edict_t* pEdict;
		model_t* pModel = NULL;

		pEdict = &sv.edicts[entityIndex];
		if (pEdict->v.modelindex)
			pModel = sv.models[pEdict->v.modelindex];

		// Make sure it's a brush model
		if (!pModel || pModel->type != mod_brush)
			return 0;

		if (pEdict->v.angles[0] || pEdict->v.angles[1] || pEdict->v.angles[2])
		{
			vec3_t forward, right, up;
			AngleVectorsTranspose(pEdict->v.angles, forward, right, up);

			position[0] = DotProduct(position, forward);
			position[1] = DotProduct(position, right);
			position[2] = DotProduct(position, up);
		}

		if (pModel->firstmodelsurface)
		{
			phull = &pModel->hulls[0]; // always use #0 hull
			VectorAdd(pEdict->v.origin, phull->clip_mins, temp);
			VectorAdd(temp, position, position);
		}
	}

	return entityIndex;
}


// Shoots a decal onto the surface of the BSP.  position is the center of the decal in world coords
void R_DecalShoot_( texture_t* ptexture, int index, int entity, int modelIndex, vec_t* position, int flags )
{
	mnode_t* pnodes;
	cl_entity_t* pent;

	VectorCopy(position, gDecalPos);	// Pass position in global

	pent = &cl_entities[entity];

	// Try all ways to get the model
	if (pent)
	{
		gDecalModel = pent->model;
		if (!gDecalModel)
		{
			if (modelIndex)
				gDecalModel = cl.model_precache[modelIndex];

			if (!gDecalModel)
			{
				if (sv.active)
					gDecalModel = sv.models[sv.edicts[entity].v.modelindex];
			}
		}
	}
	else
	{
		gDecalModel = NULL;
	}

	if (!pent || !gDecalModel || gDecalModel->type != mod_brush || !ptexture)
	{
		Con_DPrintf("Decals must hit mod_brush!\n");
		return;
	}

	pnodes = gDecalModel->nodes;

	if (entity)
	{
		hull_t* phull;
		vec3_t temp;

		if (gDecalModel->firstmodelsurface)
		{
			phull = &gDecalModel->hulls[0]; // always use #0 hull

			VectorSubtract(position, phull->clip_mins, temp);
			VectorSubtract(temp, pent->origin, gDecalPos);
			pnodes = pnodes + phull->firstclipnode;
		}

		if (pent->angles[0] || pent->angles[1] || pent->angles[2])
		{
			vec3_t forward, right, up;
			AngleVectors(pent->angles, forward, right, up);
			VectorCopy(gDecalPos, temp);

			gDecalPos[0] = DotProduct(temp, forward);
			gDecalPos[1] = -DotProduct(temp, right);
			gDecalPos[2] = DotProduct(temp, up);
		}
	}

	// More state used by R_DecalNode()
	gDecalEntity = entity;
	gDecalTexture = ptexture;
	gDecalIndex = index;
	gDecalFlags = flags;
	gDecalSize = ptexture->width >> 1;

	if (gDecalSize < (int)(ptexture->height >> 1))
		gDecalSize = ptexture->height >> 1;

	R_DecalNode(pnodes);
}

// Shoots a decal onto the surface of the BSP.  position is the center of the decal in world coords
// This is called from cl_parse.c, cl_tent.c
void R_DecalShoot( int textureIndex, int entity, int modelIndex, vec_t* position, int flags )
{
	texture_t* ptexture;

	ptexture = Draw_DecalTexture(textureIndex);
	R_DecalShoot_(ptexture, textureIndex, entity, modelIndex, position, flags);
}

void R_CustomDecalShoot( texture_t* ptexture, int playernum, int entity, int modelIndex, vec_t* position, int flags )
{
	int plindex = ~playernum;
	R_DecalShoot_(ptexture, plindex, entity, modelIndex, position, flags);
}

// Check for intersecting decals on this surface
decal_t* R_DecalIntersect( msurface_t* psurf, int* pcount, float x, float y )
{
	decal_t* plist;
	decal_t* plast;
	int			dist;
	int			lastDist;
	int			dx, dy;
	texture_t* ptexture;
	float		w, h;
	float		maxWidth;
	qboolean	bPermanent;

	plast = NULL;

	lastDist = 0xFFFF;
	*pcount = 0;

	maxWidth = (float)(gDecalTexture->width) * 1.5;

	plist = psurf->pdecals;
	while (plist)
	{
		ptexture = Draw_DecalTexture(plist->texture);

		// Don't steal bigger decals and replace them with smaller decals
		// Don't steal permanent decals
		bPermanent = (plist->flags & FDECAL_PERMANENT);
		if (!bPermanent)
		{
			if (maxWidth >= (float)ptexture->width)
			{
				w = abs((int)((gDecalTexture->width >> 1) + psurf->texinfo->texture->width * x
					- (psurf->texinfo->texture->width * plist->dx + (ptexture->width >> 1))));
				h = abs((int)((gDecalTexture->height >> 1) + psurf->texinfo->texture->height * y
					- (psurf->texinfo->texture->height * plist->dy + (ptexture->height >> 1))));

				// Now figure out the part of the projection that intersects plist's
				// clip box [0,0,1,1].
				if (h <= w)
				{
					dx = w;
					dy = h;
				}
				else
				{
					dx = h;
					dy = w;
				}

				// Figure out how much of this intersects the (0,0) - (1,1) bbox
				dist = (float)dx + (float)dy * 0.5;
				if ((dist * plist->scale) < 8)
				{
					*pcount += 1;

					if (!plast || dist <= lastDist)
					{
						lastDist = dist;
						plast = plist;
					}
				}
			}
		}

		plist = plist->pnext;
	}

	return plast;
}

// Allocate and initialize a decal from the pool, on surface with offsets x, y
void R_DecalCreate( msurface_t* psurface, int textureIndex, float scale, float x, float y )
{
	decal_t* pdecal;
	decal_t* pold;
	int				count;

	pold = R_DecalIntersect(psurface, &count, x, y);

	if (count < MAX_OVERLAP_DECALS)
		pold = NULL;

	pdecal = R_DecalAlloc(pold);
	pdecal->texture = textureIndex;
	pdecal->flags = gDecalFlags;
	pdecal->dx = x;
	pdecal->dy = y;
	pdecal->pnext = NULL;

	if (psurface->pdecals)
	{
		pold = psurface->pdecals;

		while (pold->pnext)
			pold = pold->pnext;

		pold->pnext = pdecal;
	}
	else
	{
		psurface->pdecals = pdecal;
	}

	// Tag surface
	pdecal->psurface = psurface;

	// Set scaling
	pdecal->scale = scale;
	pdecal->entityIndex = gDecalEntity;	

	R_InvalidateSurface(psurface);
}

// clip edges
#define LEFT_EDGE			0
#define RIGHT_EDGE			1
#define TOP_EDGE			2
#define BOTTOM_EDGE			3

// Quick and dirty sutherland Hodgman clipper
// Clip polygon to decal in texture space
// JAY: This code is lame, change it later.  It does way too much work per frame
// It can be made to recursively call the clipping code and only copy the vertex list once
int Inside( float* vert, int edge )
{
	switch (edge)
	{
		case LEFT_EDGE:
			if (vert[3] > 0.0)
				return 1;
			return 0;

		case RIGHT_EDGE:
			if (vert[3] < 1.0)
				return 1;
			return 0;

		case TOP_EDGE:
			if (vert[4] > 0.0)
				return 1;
			return 0;

		case BOTTOM_EDGE:
			if (vert[4] < 1.0)
				return 1;
			return 0;
	}

	return 0;
}

void Intersect( float* one, float* two, int edge, float* out )
{
	float t;

	// t is the parameter of the line between one and two clipped to the edge
	// or the fraction of the clipped point between one & two
	// vert[3] is u
	// vert[4] is v
	// vert[5] is lightmap u
	// vert[6] is lightmap v
	// vert[0], vert[1], vert[2] is X, Y, Z


	if (edge < TOP_EDGE)
	{
		if (edge == LEFT_EDGE)
		{
			// left
			t = ((one[3] - 0) / (one[3] - two[3]));
			out[3] = 0;
		}
		else
		{
			// right
			t = ((one[3] - 1) / (one[3] - two[3]));
			out[3] = 1;
		}

		out[4] = one[4] + (two[4] - one[4]) * t;
	}
	else
	{
		if (edge == TOP_EDGE)
		{
			// top
			t = ((one[4] - 0) / (one[4] - two[4]));
			out[4] = 0;
		}
		else
		{
			// bottom
			t = ((one[4] - 1) / (one[4] - two[4]));
			out[4] = 1;
		}

		out[3] = one[3] + (two[3] - one[3]) * t;
	}

	out[0] = one[0] + (two[0] - one[0]) * t;
	out[1] = one[1] + (two[1] - one[1]) * t;
	out[2] = one[2] + (two[2] - one[2]) * t;
}

int SHClip( float* vert, int vertCount, float* out, int edge )
{
	int j, outCount;
	float* s, * p;


	outCount = 0;

	s = &vert[(vertCount - 1) * VERTEXSIZE];
	for (j = 0; j < vertCount; j++)
	{
		p = &vert[j * VERTEXSIZE];
		if (Inside(p, edge))
		{
			if (Inside(s, edge))
			{
				memcpy(out, p, sizeof(*out) * VERTEXSIZE);
				outCount++;
				out += VERTEXSIZE;
			}
			else
			{
				Intersect(s, p, edge, out);
				out += VERTEXSIZE;
				outCount++;
				memcpy(out, p, sizeof(*out) * VERTEXSIZE);
				outCount++;
				out += VERTEXSIZE;
			}
		}
		else
		{
			if (Inside(s, edge))
			{
				Intersect(p, s, edge, out);
				out += VERTEXSIZE;
				outCount++;
			}
		}
		s = p;
	}

	return outCount;
}

#define MAX_DECALCLIPVERT		32

// Renders all decals
void R_DrawDecals( void )
{
	float vert[MAX_DECALCLIPVERT][VERTEXSIZE];
	float outvert[MAX_DECALCLIPVERT][VERTEXSIZE];

	decal_t* plist; // decal list
	int	i, j, k, outCount = 0;
	texture_t* ptexture;
	msurface_t* psurf;

	float* v = NULL;
	float* vlist;

	if (gDecalSurfCount == 0)
		return;

	qglEnable(GL_BLEND);
	qglEnable(GL_ALPHA_TEST);
	qglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	qglDepthMask(GL_FALSE);

	if (gl_polyoffset.value)
	{
		qglEnable(GL_POLYGON_OFFSET_FILL);

		if (!gl_ztrick.value || gldepthmin < 0.5)
		{
			qglPolygonOffset(1.0, -gl_polyoffset.value);
		}
		else
		{
			qglPolygonOffset(1.0, gl_polyoffset.value);
		}
	}

	for (i = 0; i < gDecalSurfCount; i++)
	{
		psurf = gDecalSurfs[i];

		// Draw all decals
		for (plist = psurf->pdecals; plist; plist = plist->pnext)
		{
			float scalex, scaley;

			ptexture = Draw_DecalTexture(plist->texture);

			scalex = (psurf->texinfo->texture->width * plist->scale) / (float)ptexture->width;
			scaley = (psurf->texinfo->texture->height * plist->scale) / (float)ptexture->height;

			v = psurf->polys->verts[0];
			for (j = 0; j < psurf->polys->numverts; j++, v += VERTEXSIZE)
			{
				VectorCopy(v, vert[j]);
				vert[j][3] = (v[3] - plist->dx) * scalex;
				vert[j][4] = (v[4] - plist->dy) * scaley;
			}

			// Clip the polygon to the decal texture space
			outCount = SHClip(vert[0], psurf->polys->numverts, outvert[0], LEFT_EDGE);
			outCount = SHClip(outvert[0], outCount, vert[0], RIGHT_EDGE);
			outCount = SHClip(vert[0], outCount, outvert[0], TOP_EDGE);
			outCount = SHClip(outvert[0], outCount, vert[0], BOTTOM_EDGE);

			if (outCount)
			{
				GL_Bind(ptexture->gl_texturenum);

				qglBegin(GL_POLYGON);
				vlist = vert[0];
				for (k = 0; k < outCount; k++, vlist += VERTEXSIZE)
				{
					qglTexCoord2f(vlist[3], vlist[4]);
					qglVertex3fv(vlist);
				}
				qglEnd();
			}
		}
	}

	if (gl_polyoffset.value)
		qglDisable(GL_POLYGON_OFFSET_FILL);

	qglDisable(GL_ALPHA_TEST);
	qglDisable(GL_BLEND);
	qglDepthMask(GL_TRUE);

	gDecalSurfCount = 0;
}

// Renders all decals in multitexture mode
void R_DrawMTexDecals( void )
{
	// TODO: Implement
}

void R_InvalidateSurface( msurface_t* surface )
{
}