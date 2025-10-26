// r_main.c

#include "quakedef.h"
#include "r_studio.h"
#include "r_trans.h"
#include "shake.h"

cl_entity_t	r_worldentity;

qboolean	r_cache_thrash;		// compatability

vec3_t		modelorg, r_entorigin;
cl_entity_t* currententity;

int			r_visframecount;
int			r_framecount;

mplane_t	frustum[4];

int			c_brush_polys, c_alias_polys;

qboolean	envmap;				// true during envmap command capture
int			currenttexture = -1;	// to avoid unnecessary texture sets
int			cnttextures[2] = { -1, -1 };     // cached

int			particletexture;	// little dot for particles
int			playertextures;		// up to 16 color translated skins

int			mirrortexturenum;	// quake texturenum, not gltexturenum
qboolean	mirror;
mplane_t*	mirror_plane;

alight_t	r_viewlighting;
vec3_t		r_plightvec; // light vector in model reference frame

//
// view origin
//
vec3_t	vup;
vec3_t	vpn;
vec3_t	vright;
vec3_t	r_origin;

float	r_world_matrix[16];
float	r_base_world_matrix[16];
float	gProjectionMatrix[16];
float	gWorldToScreen[16];

//
// screen size info
//
refdef_t	r_refdef;

mleaf_t*	r_viewleaf, * r_oldviewleaf;

texture_t*	r_notexture_mip;

int			d_lightstylevalue[256];	// 8.8 fraction of base light value


void R_MarkLeaves( void );

extern cshift_t	cshift_water;

/*-----------------------------------------------------------------*/

#define DEG2RAD( a ) ( a * M_PI ) / 180.0F

void ProjectPointOnPlane( vec_t* dst, const vec_t* p, const vec_t* normal )
{
	float d;
	vec3_t n;
	float inv_denom;

	inv_denom = 1.0F / DotProduct(normal, normal);

	d = DotProduct(normal, p) * inv_denom;

	n[0] = normal[0] * inv_denom;
	n[1] = normal[1] * inv_denom;
	n[2] = normal[2] * inv_denom;

	dst[0] = p[0] - d * n[0];
	dst[1] = p[1] - d * n[1];
	dst[2] = p[2] - d * n[2];
}

/*
** assumes "src" is normalized
*/
void PerpendicularVector( vec_t* dst, const vec_t* src )
{
	int pos;
	int i;
	float minelem = 1.0F;
	vec3_t tempvec;

	/*
	** find the smallest magnitude axially aligned vector
	*/
	for (pos = 0, i = 0; i < 3; i++)
	{
		if (fabs(src[i]) < minelem)
		{
			pos = i;
			minelem = fabs(src[i]);
		}
	}
	tempvec[0] = tempvec[1] = tempvec[2] = 0.0F;
	tempvec[pos] = 1.0F;

	/*
	** project the point onto the plane defined by src
	*/
	ProjectPointOnPlane(dst, tempvec, src);

	/*
	** normalize the result
	*/
	VectorNormalize(dst);
}


void RotatePointAroundVector( vec_t* dst, const vec_t* dir, const vec_t* point, float degrees )
{
	float	m[3][3];
	float	im[3][3];
	float	zrot[3][3];
	float	tmpmat[3][3];
	float	rot[3][3];
	int	i;
	vec3_t vr, vup, vf;

	vf[0] = dir[0];
	vf[1] = dir[1];
	vf[2] = dir[2];

	PerpendicularVector(vr, dir);
	CrossProduct(vr, vf, vup);

	m[0][0] = vr[0];
	m[1][0] = vr[1];
	m[2][0] = vr[2];

	m[0][1] = vup[0];
	m[1][1] = vup[1];
	m[2][1] = vup[2];

	m[0][2] = vf[0];
	m[1][2] = vf[1];
	m[2][2] = vf[2];

	memcpy(im, m, sizeof(im));

	im[0][1] = m[1][0];
	im[0][2] = m[2][0];
	im[1][0] = m[0][1];
	im[1][2] = m[2][1];
	im[2][0] = m[0][2];
	im[2][1] = m[1][2];

	memset(zrot, 0, sizeof(zrot));
	zrot[0][0] = zrot[1][1] = zrot[2][2] = 1.0F;

	zrot[0][0] = cos(DEG2RAD(degrees));
	zrot[0][1] = sin(DEG2RAD(degrees));
	zrot[1][0] = -sin(DEG2RAD(degrees));
	zrot[1][1] = cos(DEG2RAD(degrees));

	R_ConcatRotations(m, zrot, tmpmat);
	R_ConcatRotations(tmpmat, im, rot);

	for (i = 0; i < 3; i++)
	{
		dst[i] = rot[i][0] * point[0] + rot[i][1] * point[1] + rot[i][2] * point[2];
	}
}

/*
=================
R_CullBox

Returns true if the box is completely outside the frustom
=================
*/
qboolean R_CullBox( vec_t* mins, vec_t* maxs )
{
	int		i;

	for (i = 0; i < 4; i++)
		if (BoxOnPlaneSide(mins, maxs, &frustum[i]) == 2)
			return TRUE;
	return FALSE;
}


void R_RotateForEntity( cl_entity_t* e )
{
	int		i;
	vec3_t	angles;
	vec3_t	modelpos;

	VectorCopy(e->origin, modelpos);
	VectorCopy(e->angles, angles);

	if (e->movetype != MOVETYPE_NONE)
	{
		float f = 0.0;
		float d;
		if (e->animtime + 0.2 > cl.time && e->animtime != e->prevanimtime)
			f = (cl.time - e->animtime) / (e->animtime - e->prevanimtime);

		for (i = 0; i < 3; i++)
		{
			d = e->prevorigin[i] - e->origin[i];
			modelpos[i] -= d * f;
		}

		if (f > 0.0 && f < 1.5)
		{
			f = 1.0 - f;

			for (i = 0; i < 3; i++)
			{
				d = e->prevangles[i] - e->angles[i];
				if (d > 180.0)
					d -= 360.0;
				else if (d < -180.0)
					d += 360.0;

				angles[i] += d * f;
			}
		}
	}

	qglTranslatef(modelpos[0], modelpos[1], modelpos[2]);

	qglRotatef(angles[1], 0, 0, 1);
	qglRotatef(angles[0], 0, 1, 0);

	//ZOID: fixed z angle
	qglRotatef(angles[2], 1, 0, 0);
}

/*
=============================================================

  SPRITE MODELS

=============================================================
*/

/*
=================
R_DrawSpriteModel

=================
*/
void R_DrawSpriteModel( cl_entity_t* e )
{
	vec3_t	point, forward, right, up;
	mspriteframe_t* frame;
	float			scale;
	msprite_t* psprite;
	colorVec		color;

	psprite = (msprite_t*)e->model->cache.data;

	// don't even bother culling, because it's just a single
	// polygon without a surface cache
	frame = R_GetSpriteFrame(psprite, e->frame);
	if (e->scale > 0)
		scale = e->scale;
	else
		scale = 1;

	if (e->rendermode == kRenderNormal)
		r_blend = 1;

	R_SpriteColor(&color, e, (int)(r_blend * 255.0));

	if (gl_spriteblend.value || e->rendermode != kRenderNormal)
	{
		switch (e->rendermode)
		{
		case kRenderTransColor:
			qglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			qglTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ALPHA);
			qglColor4ub(color.r, color.g, color.b, (int)(r_blend * 255.0));
			break;
		case kRenderTransAdd:
			qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			qglBlendFunc(GL_ONE, GL_ONE);
			qglColor4ub(color.r, color.g, color.b, 255);
			qglDepthMask(GL_FALSE);
			break;
		case kRenderGlow:
			qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			qglBlendFunc(GL_ONE, GL_ONE);
			qglColor4ub(color.r, color.g, color.b, 255);
			qglDisable(GL_DEPTH_TEST);
			qglDepthMask(GL_FALSE);
			break;
		case kRenderTransAlpha:
			qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			qglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			qglColor4ub(color.r, color.g, color.b, (int)(r_blend * 255.0));
			qglDepthMask(GL_FALSE);
			break;
		default:
			qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			qglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			qglColor4ub(color.r, color.g, color.b, (int)(r_blend * 255.0));
			break;
		}
		qglEnable(GL_BLEND);
	}
	else
	{
		qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		qglColor4ub(color.r, color.g, color.b, 255);
		qglDisable(GL_BLEND);
	}

	// Get orthonormal bases
	R_GetSpriteAxes(e, psprite->type, forward, right, up);

	GL_DisableMultitexture();

	GL_Bind(frame->gl_texturenum);

	qglEnable(GL_ALPHA_TEST);
	qglBegin(GL_QUADS);

	qglTexCoord2f(0, 1);
	VectorMA(r_entorigin, frame->down * scale, up, point);
	VectorMA(point, frame->left * scale, right, point);
	qglVertex3fv(point);

	qglTexCoord2f(0, 0);
	VectorMA(r_entorigin, frame->up * scale, up, point);
	VectorMA(point, frame->left * scale, right, point);
	qglVertex3fv(point);

	qglTexCoord2f(1, 0);
	VectorMA(r_entorigin, frame->up * scale, up, point);
	VectorMA(point, frame->right * scale, right, point);
	qglVertex3fv(point);

	qglTexCoord2f(1, 1);
	VectorMA(r_entorigin, frame->down * scale, up, point);
	VectorMA(point, frame->right * scale, right, point);
	qglVertex3fv(point);

	qglEnd();

	qglDisable(GL_ALPHA_TEST);

	qglDepthMask(GL_TRUE);

	if (gl_spriteblend.value || e->rendermode != kRenderNormal)
	{
		qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		qglDisable(GL_BLEND);
		qglEnable(GL_DEPTH_TEST);
	}
}

/*
=============================================================

  ALIAS MODELS

=============================================================
*/


#define NUMVERTEXNORMALS	162
float	r_avertexnormals[NUMVERTEXNORMALS][3] = {
#include "anorms.h"
};

vec3_t	shadevector;
float	shadelight, ambientlight;

// precalculated dot products for quantized angles
#define SHADEDOT_QUANT 16
float	r_avertexnormal_dots[SHADEDOT_QUANT][256] = {
#include "anorm_dots.h"
};

float* shadedots = r_avertexnormal_dots[0];

int	lastposenum;

/*
=============
GL_DrawAliasFrame
=============
*/
void GL_DrawAliasFrame( aliashdr_t* paliashdr, int posenum )
{
	float	l;
	trivertx_t* verts;
	int* order;
	int		count;

	lastposenum = posenum;

	verts = (trivertx_t*)((byte*)paliashdr + paliashdr->posedata);
	verts += posenum * paliashdr->poseverts;
	order = (int*)((byte*)paliashdr + paliashdr->commands);

	while (1)
	{
		// get the vertex count and primitive type
		count = *order++;
		if (!count)
			break;		// done
		if (count < 0)
		{
			count = -count;
			qglBegin(GL_TRIANGLE_FAN);
		}
		else
			qglBegin(GL_TRIANGLE_STRIP);

		do
		{
			// texture coordinates come from the draw list
			qglTexCoord2f(((float*)order)[0], ((float*)order)[1]);
			order += 2;

			// normals and vertexes come from the frame list
			l = shadedots[verts->lightnormalindex] * shadelight;
			qglColor3f(l, l, l);
			qglVertex3f(verts->v[0], verts->v[1], verts->v[2]);
			verts++;
		} while (--count);

		qglEnd();
	}
}


/*
=============
GL_DrawAliasShadow
=============
*/
extern	vec3_t			lightspot;

void GL_DrawAliasShadow( aliashdr_t* paliashdr, int posenum )
{
	trivertx_t* verts;
	int* order;
	vec3_t	point;
	float	height, lheight;
	int		count;

	lheight = currententity->origin[2] - lightspot[2];

	height = 0;
	verts = (trivertx_t*)((byte*)paliashdr + paliashdr->posedata);
	verts += posenum * paliashdr->poseverts;
	order = (int*)((byte*)paliashdr + paliashdr->commands);

	height = -lheight + 1.0;

	while (1)
	{
		// get the vertex count and primitive type
		count = *order++;
		if (!count)
			break;		// done
		if (count < 0)
		{
			count = -count;
			qglBegin(GL_TRIANGLE_FAN);
		}
		else
			qglBegin(GL_TRIANGLE_STRIP);

		do
		{
			// texture coordinates come from the draw list
			// (skipped for shadows) qglTexCoord2fv ((float*)order);
			order += 2;

			// normals and vertexes come from the frame list
			point[0] = verts->v[0] * paliashdr->scale[0] + paliashdr->scale_origin[0];
			point[1] = verts->v[1] * paliashdr->scale[1] + paliashdr->scale_origin[1];
			point[2] = verts->v[2] * paliashdr->scale[2] + paliashdr->scale_origin[2];

			point[0] -= shadevector[0] * (point[2] + lheight);
			point[1] -= shadevector[1] * (point[2] + lheight);
			point[2] = height;
//			height -= 0.001;
			qglVertex3fv(point);

			verts++;
		} while (--count);

		qglEnd();
	}
}



/*
=================
R_SetupAliasFrame

=================
*/
void R_SetupAliasFrame( int frame, aliashdr_t* paliashdr )
{
	int				pose, numposes;
	float			interval;

	if ((frame >= paliashdr->numframes) || (frame < 0))
	{
		Con_DPrintf("R_AliasSetupFrame: no such frame %d\n", frame);
		frame = 0;
	}

	pose = paliashdr->frames[frame].firstpose;
	numposes = paliashdr->frames[frame].numposes;

	if (numposes > 1)
	{
		interval = paliashdr->frames[frame].interval;
		pose += (int)(cl.time / interval) % numposes;
	}

	GL_DrawAliasFrame(paliashdr, pose);
}



/*
=================
R_DrawAliasModel

=================
*/
void R_DrawAliasModel( cl_entity_t* e )
{
	int			i;
	int			lnum;
	vec3_t		dist;
	float		add;
	model_t* clmodel;
	vec3_t		mins, maxs;
	aliashdr_t* paliashdr;
	float		an;

	clmodel = currententity->model;

	VectorAdd(currententity->origin, clmodel->mins, mins);
	VectorAdd(currententity->origin, clmodel->maxs, maxs);

	if (R_CullBox(mins, maxs))
		return;


	VectorCopy(currententity->origin, r_entorigin);
	VectorSubtract(r_origin, r_entorigin, modelorg);

	//
	// get lighting information
	//

	// allways give the gun some light
	if (e == &cl.viewent && ambientlight < 24)
		ambientlight = shadelight = 24;

	for (lnum = 0; lnum < MAX_DLIGHTS; lnum++)
	{
		if (cl_dlights[lnum].die >= cl.time)
		{
			VectorSubtract(currententity->origin,
				cl_dlights[lnum].origin,
				dist);
			add = cl_dlights[lnum].radius - Length(dist);

			if (add > 0) {
				ambientlight += add;
				//ZOID models should be affected by dlights as well
				//shadelight += add;
			}
		}
	}

	// clamp lighting so it doesn't overbright as much
	if (ambientlight > 128)
		ambientlight = 128;
	if (ambientlight + shadelight > 192)
		shadelight = 192 - ambientlight;

	// HACK HACK HACK -- no fullbright colors, so make torches full light
	if (!strcmp(clmodel->name, "progs/flame2.mdl")
		|| !strcmp(clmodel->name, "progs/flame.mdl"))
		ambientlight = shadelight = 256;

	shadedots = r_avertexnormal_dots[((int)(e->angles[1] * (SHADEDOT_QUANT / 360.0))) & (SHADEDOT_QUANT - 1)];
	shadelight = shadelight / 200.0;

	an = e->angles[1] / 180 * M_PI;
	shadevector[0] = cos(-an);
	shadevector[1] = sin(-an);
	shadevector[2] = 1;
	VectorNormalize(shadevector);

	//
	// locate the proper data
	//
	paliashdr = (aliashdr_t*)Mod_Extradata(currententity->model);

	c_alias_polys += paliashdr->numtris;

	//
	// draw all the triangles
	//

	qglPushMatrix();
	R_RotateForEntity(e);

	qglTranslatef(paliashdr->scale_origin[0], paliashdr->scale_origin[1], paliashdr->scale_origin[2]);
	qglScalef(paliashdr->scale[0], paliashdr->scale[1], paliashdr->scale[2]);

	GL_Bind(paliashdr->gl_texturenum[currententity->skin]);

	// we can't dynamically colormap textures, so they are cached
	// seperately for the players.  Heads are just uncolored.
	if (currententity->colormap != vid.colormap && !gl_nocolors.value)
	{
		i = currententity - cl_entities;
		if (i >= 1 && i <= cl.maxclients && !strcmp(currententity->model->name, "progs/player.mdl"))
			GL_Bind(playertextures - 1 + i);
	}

	if (gl_smoothmodels.value)
		qglShadeModel(GL_SMOOTH);
	qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	if (gl_affinemodels.value)
		qglHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

	R_SetupAliasFrame(currententity->frame, paliashdr);

	qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	qglShadeModel(GL_FLAT);
	if (gl_affinemodels.value)
		qglHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	qglPopMatrix();

	if (r_shadows.value)
	{
		qglPushMatrix();
		R_RotateForEntity(e);
		qglDisable(GL_TEXTURE_2D);
		qglEnable(GL_BLEND);
		qglColor4f(0, 0, 0, 0.5);
		GL_DrawAliasShadow(paliashdr, lastposenum);
		qglEnable(GL_TEXTURE_2D);
		qglDisable(GL_BLEND);
		qglColor4f(1, 1, 1, 1);
		qglPopMatrix();
	}

}

//==================================================================================

/*
=============
R_DrawEntitiesOnList
=============
*/
void R_DrawEntitiesOnList( void )
{
	int		i;

	if (!r_drawentities.value)
		return;

	// draw sprites seperately, because of alpha blending
	for (i = 0; i < cl_numvisedicts; i++)
	{
		currententity = &cl_visedicts[i];

		if (currententity->rendermode != kRenderNormal)
		{
			AddTEntity(currententity);
			continue;
		}

		switch (currententity->model->type)
		{
		case mod_brush:
			R_DrawBrushModel(currententity);
			break;

		case mod_alias:
			R_DrawAliasModel(currententity);
			break;

		case mod_studio:
			if (currententity->index > 0 && currententity->index <= cl.maxclients)
			{
				R_StudioDrawPlayer(STUDIO_RENDER | STUDIO_EVENTS,
					&cl.frames[cl.parsecount & UPDATE_MASK].playerstate[currententity->index - 1]);
			}
			else
			{
				R_StudioDrawModel(STUDIO_RENDER | STUDIO_EVENTS);
			}
			break;

		default:
			break;
		}
	}

	r_blend = 1.0;

	for (i = 0; i < cl_numvisedicts; i++)
	{
		currententity = &cl_visedicts[i];

		if (currententity->rendermode != kRenderNormal)
			continue;

		switch (currententity->model->type)
		{
		case mod_sprite:
			if (currententity->body)
			{
				float* pAttachment;

				pAttachment = R_GetAttachmentPoint(currententity->skin, currententity->body);
				VectorCopy(pAttachment, r_entorigin);
			}
			else
			{
				VectorCopy(currententity->origin, r_entorigin);
			}

			R_DrawSpriteModel(currententity);
			break;

		default:
			break;
		}
	}
}

/*
=============
R_DrawViewModel
=============
*/
void R_DrawViewModel( void )
{
	float		lightvec[3];
	colorVec	c;
	int			j;
	int			lnum;
	vec3_t		dist;
	float		add, oldShadows;
	dlight_t* dl;

	lightvec[0] = -1;
	lightvec[1] = 0;
	lightvec[2] = 0;

	currententity = &cl.viewent;

	if (!r_drawviewmodel.value)
	{
		c = R_LightPoint(currententity->origin);
		cl.light_level = (c.r + c.g + c.b) / 3;
		return;
	}

	// Don't draw if we are in a third person mode
	if (cam_thirdperson || chase_active.value || envmap || !r_drawentities.value)
	{
		c = R_LightPoint(currententity->origin);
		cl.light_level = (c.r + c.g + c.b) / 3;
		return;
	}

	if (cl.stats[STAT_HEALTH] <= 0 || !currententity->model || cl.viewentity > cl.maxclients || cl.spectator)
	{
		c = R_LightPoint(currententity->origin);
		cl.light_level = (c.r + c.g + c.b) / 3;
		return;
	}

	qglDepthRange(gldepthmin, gldepthmin + (gldepthmax - gldepthmin) * 0.3);

	switch (currententity->model->type)
	{
	case mod_brush:
		R_DrawBrushModel(currententity);
		break;

	case mod_alias:
		c = R_LightPoint(currententity->origin);

		j = (c.r + c.g + c.b) / 3;

		if (j < 24)
			j = 24;		// allways give some light on gun
		r_viewlighting.ambientlight = j;
		r_viewlighting.shadelight = j;

	// add dynamic lights
		for (lnum = 0; lnum < MAX_DLIGHTS; lnum++)
		{
			dl = &cl_dlights[lnum];
			if (!dl->radius)
				continue;
			if (dl->die < cl.time)
				continue;

			VectorSubtract(currententity->origin, dl->origin, dist);
			add = dl->radius - Length(dist);
			if (add > 0)
				r_viewlighting.ambientlight += add;
		}

		// clamp lighting so it doesn't overbright as much
		if (r_viewlighting.ambientlight > 128)
			r_viewlighting.ambientlight = 128;
		if (r_viewlighting.ambientlight + r_viewlighting.shadelight > 192)
			r_viewlighting.shadelight = 192 - r_viewlighting.ambientlight;

		r_viewlighting.plightvec = lightvec;
		R_DrawAliasModel(currententity);
		break;

	case mod_studio:
		if (cl.weaponstarttime == 0.0)
			cl.weaponstarttime = cl.time;
		
		currententity->sequence = cl.weaponsequence;
		currententity->frame = 0.0;
		currententity->framerate = 1.0;
		currententity->animtime = cl.weaponstarttime;

		cl.light_level = 128;

		oldShadows = r_shadows.value;
		r_shadows.value = 0.0;
		R_StudioDrawModel(STUDIO_RENDER);
		r_shadows.value = oldShadows;
		break;
	}

	qglDepthRange(gldepthmin, gldepthmax);
}

void R_PreDrawViewModel( void )
{
	currententity = &cl.viewent;

	// Don't draw if it's disabled
	if (!r_drawviewmodel.value)
		return;

	// Don't draw if we are in a third person mode
	if (cam_thirdperson || chase_active.value || envmap || !r_drawentities.value)
		return;

	if (cl.stats[STAT_HEALTH] <= 0 || !currententity->model || cl.viewentity > cl.maxclients || cl.spectator)
		return;

	if (cl.viewent.model->type != mod_studio)
		return;

	if (cl.weaponstarttime == 0.0)
		cl.weaponstarttime = cl.time;

	cl.viewent.frame = 0.0;
	cl.viewent.framerate = 1.0;
	cl.viewent.sequence = cl.weaponsequence;
	cl.viewent.animtime = cl.weaponstarttime;

	R_StudioDrawModel(STUDIO_EVENTS);
}

/*
============
R_PolyBlend
============
*/
void R_PolyBlend( void )
{
	unsigned char color[4];
	int alpha;

	alpha = V_FadeAlpha();
	if (!alpha)
		return;

	GL_DisableMultitexture();
	qglDisable(GL_ALPHA_TEST);
	qglEnable(GL_BLEND);
	qglDisable(GL_DEPTH_TEST);
	qglDisable(GL_TEXTURE_2D);
	if (cl.sf.fadeFlags & FFADE_MODULATE)
	{
		qglBlendFunc(GL_ZERO, GL_SRC_COLOR);
		color[0] = (alpha * (cl.sf.fader - 255) - 511) >> 8;
		color[1] = (alpha * (cl.sf.fadeg - 255) - 511) >> 8;
		color[2] = (alpha * (cl.sf.fadeb - 255) - 511) >> 8;
		color[3] = 255;
	}
	else
	{
		qglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		color[0] = cl.sf.fader;
		color[1] = cl.sf.fadeg;
		color[2] = cl.sf.fadeb;
		color[3] = alpha;
	}

	qglLoadIdentity();

	qglRotatef(-90, 1, 0, 0);	    // put Z going up
	qglRotatef(90, 0, 0, 1);	    // put Z going up

	qglColor4ubv(color);

	qglBegin(GL_QUADS);

	qglVertex3f(10, 10, 10);
	qglVertex3f(10, -10, 10);
	qglVertex3f(10, -10, -10);
	qglVertex3f(10, 10, -10);
	qglEnd();

	qglDisable(GL_BLEND);
	qglEnable(GL_TEXTURE_2D);
	qglEnable(GL_ALPHA_TEST);
}


int SignbitsForPlane( mplane_t* out )
{
	int bits, j;

	// for fast box on planeside test

	bits = 0;
	for (j = 0; j < 3; j++)
	{
		if (out->normal[j] < 0)
			bits |= 1 << j;
	}
	return bits;
}


void R_SetFrustum( void )
{
	int		i;
	
	float	fovx, fovy;

	fovx = scr_fov_value;

	fovy = CalcFov(fovx, glwidth, glheight);

	// rotate VPN right by FOV_X/2 degrees
	RotatePointAroundVector(frustum[0].normal, vup, vpn, -(90 - fovx / 2));
	// rotate VPN left by FOV_X/2 degrees
	RotatePointAroundVector(frustum[1].normal, vup, vpn, 90 - fovx / 2);
	// rotate VPN up by FOV_X/2 degrees
	RotatePointAroundVector(frustum[2].normal, vright, vpn, 90 - fovy / 2);
	// rotate VPN down by FOV_X/2 degrees
	RotatePointAroundVector(frustum[3].normal, vright, vpn, -(90 - fovy / 2));

	for (i = 0; i < 4; i++)
	{
		frustum[i].type = PLANE_ANYZ;
		frustum[i].dist = DotProduct(r_origin, frustum[i].normal);
		frustum[i].signbits = SignbitsForPlane(&frustum[i]);
	}
}



/*
===============
R_SetupFrame
===============
*/
void R_SetupFrame( void )
{
// don't allow cheats in multiplayer
	if (cl.maxclients > 1)
		Cvar_Set("r_fullbright", "0");

	R_AnimateLight();

	r_framecount++;

// build the transformation matrix for the given view angles
	VectorCopy(r_refdef.vieworg, r_origin);

	AngleVectors(r_refdef.viewangles, vpn, vright, vup);

// current viewleaf
	r_oldviewleaf = r_viewleaf;
	r_viewleaf = Mod_PointInLeaf(r_origin, cl.worldmodel);

	if (cl.waterlevel > 2)
	{
		float fogColor[4];

		// Calculate fog color
		fogColor[0] = cshift_water.destcolor[0] / 255.0;
		fogColor[1] = cshift_water.destcolor[1] / 255.0;
		fogColor[2] = cshift_water.destcolor[2] / 255.0;
		fogColor[3] = 1.0;

		qglFogi(GL_FOG_MODE, GL_LINEAR);
		qglFogfv(GL_FOG_COLOR, fogColor);
		qglFogf(GL_FOG_START, GL_ZERO);
		qglFogf(GL_FOG_END, 1536 - 4 * cshift_water.percent);
		qglEnable(GL_FOG);
	}

	V_CalcBlend();

	r_cache_thrash = FALSE;

	c_brush_polys = 0;
	c_alias_polys = 0;
}


void MYgluPerspective( GLdouble fovy, GLdouble aspect,
	GLdouble zNear, GLdouble zFar )
{
	GLdouble xmin, xmax, ymin, ymax;

	ymax = zNear * tan(fovy * M_PI / 360.0);
	ymin = -ymax;

	xmin = ymin * aspect;
	xmax = ymax * aspect;

	qglFrustum(xmin, xmax, ymin, ymax, zNear, zFar);
}


/*
====================
CalcFov
====================
*/
float CalcFov( float fov_x, float width, float height )
{
	float	a;
	float	x;

	if (fov_x < 1 || fov_x > 179)
		fov_x = 90;	// error, set to 90

	x = width / tan(fov_x / 360 * M_PI);

	a = atan(height / x);

	a = a * 360 / M_PI;

	return a;
}

/*
=============
R_SetupGL
=============
*/
void R_SetupGL( void )
{
	float	screenaspect;
	float	yfov;
	int		i, j, k;
	extern	int glwidth, glheight;
	int		x, x2, y2, y, w, h;

	//
	// set up viewpoint
	//
	qglMatrixMode(GL_PROJECTION);
	qglLoadIdentity();
	x = r_refdef.vrect.x;
	x2 = r_refdef.vrect.x + r_refdef.vrect.width;
	y = glheight - r_refdef.vrect.y;
	y2 = glheight - (r_refdef.vrect.y + r_refdef.vrect.height);

	// fudge around because of frac screen scale
	if (x > 0)
		x--;
	if (x2 < glwidth)
		x2++;
	if (y2 < 0)
		y2--;
	if (y < glheight)
		y++;

	w = x2 - x;
	h = y - y2;

	if (envmap)
	{
		x = y2 = 0;
		glwidth = glheight = w = h = gl_envmapsize.value;
	}

	qglViewport(glx + x, gly + y2, w, h);
	screenaspect = (float)glwidth / glheight;

	// Calculate the FOV value
	yfov = CalcFov(scr_fov_value, glwidth, glheight);
	MYgluPerspective(yfov, screenaspect, 4.0, gl_zmax.value);

	if (mirror)
	{
		if (mirror_plane->normal[2])
			qglScalef(1, -1, 1);
		else
			qglScalef(-1, 1, 1);
		qglCullFace(GL_BACK);
	}
	else
		qglCullFace(GL_FRONT);

	qglGetFloatv(GL_PROJECTION_MATRIX, gProjectionMatrix);

	qglMatrixMode(GL_MODELVIEW);
	qglLoadIdentity();

	qglRotatef(-90, 1, 0, 0);	    // put Z going up
	qglRotatef(90, 0, 0, 1);	    // put Z going up
	qglRotatef(-r_refdef.viewangles[2], 1, 0, 0);
	qglRotatef(-r_refdef.viewangles[0], 0, 1, 0);
	qglRotatef(-r_refdef.viewangles[1], 0, 0, 1);
	qglTranslatef(-r_refdef.vieworg[0], -r_refdef.vieworg[1], -r_refdef.vieworg[2]);

	qglGetFloatv(GL_MODELVIEW_MATRIX, r_world_matrix);

	//
	// set drawing parms
	//
	if (gl_cull.value)
		qglEnable(GL_CULL_FACE);
	else
		qglDisable(GL_CULL_FACE);

	qglDisable(GL_BLEND);
	qglDisable(GL_ALPHA_TEST);
	qglEnable(GL_DEPTH_TEST);

	//
	// Transform world-to-screen translation matrices
	//

	if (gl_flipmatrix.value)
	{
		// inverted matrix
		for (i = 0; i < 4; i++)
		{
			for (j = 0; j < 4; j++)
			{
				gWorldToScreen[i * 4 + j] = 0;
				for (k = 0; k < 4; k++)
				{
					gWorldToScreen[i * 4 + j] += r_world_matrix[k * 4 + i] * gProjectionMatrix[j * 4 + k];
				}
			}
		}
	}
	else
	{
		for (i = 0; i < 4; i++)
		{
			for (j = 0; j < 4; j++)
			{
				gWorldToScreen[i * 4 + j] = 0;
				for (k = 0; k < 4; k++)
				{
					gWorldToScreen[i * 4 + j] += r_world_matrix[i * 4 + k] * gProjectionMatrix[k * 4 + j];
				}
			}
		}
	}
}

/*
================
R_RenderScene

r_refdef must be set before the first call
================
*/
void R_RenderScene( void )
{
	R_SetupFrame();

	R_SetFrustum();

	R_SetupGL();

	R_MarkLeaves();	// done here so we know if we're in water

	R_DrawWorld();		// adds static entities to the list

	S_ExtraUpdate();	// don't let sound get messed up if going slow

	R_DrawEntitiesOnList();

	qglDisable(GL_FOG);

	R_DrawTEntitiesOnList();

	S_ExtraUpdate();

	R_RenderDlights();

	GL_DisableMultitexture();

	R_DrawParticles();
}


/*
=============
R_Clear
=============
*/
void R_Clear( void )
{
	if (r_mirroralpha.value != 1.0)
	{
		if (gl_clear.value)
			qglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		else
			qglClear(GL_DEPTH_BUFFER_BIT);
		gldepthmin = 0.0;
		gldepthmax = 0.5;
		qglDepthFunc(GL_LEQUAL);
	}
	else if (gl_ztrick.value)
	{
		static int trickframe;

		if (gl_clear.value)
			qglClear(GL_COLOR_BUFFER_BIT);

		trickframe++;
		if (trickframe & 1)
		{
			gldepthmin = 0;
			gldepthmax = 0.49999;
			qglDepthFunc(GL_LEQUAL);
		}
		else
		{
			gldepthmin = 1;
			gldepthmax = 0.5;
			qglDepthFunc(GL_GEQUAL);
		}
	}
	else
	{
		if (gl_clear.value)
			qglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		else
			qglClear(GL_DEPTH_BUFFER_BIT);
		gldepthmin = 0;
		gldepthmax = 1;
		qglDepthFunc(GL_LEQUAL);
	}

	qglDepthRange(gldepthmin, gldepthmax);
}

void R_SetStackBase( void )
{
	// get stack position so we can guess if we are going to overflow
	//r_stack_start = (byte*)&dummy;
}

/*
================
R_RenderView

r_refdef must be set before the first call
================
*/
void R_RenderView( void )
{
	double	time1 = 0, time2;

	if (r_norefresh.value)
		return;

	if (!r_worldentity.model || !cl.worldmodel)
		Sys_Error("R_RenderView: NULL worldmodel");

	if (r_speeds.value)
	{
		qglFinish();
		time1 = Sys_FloatTime();
		c_brush_polys = 0;
		c_alias_polys = 0;
	}

	mirror = FALSE;

	R_Clear();

	R_PreDrawViewModel();

	R_RenderScene();

	R_DrawViewModel();

	R_PolyBlend();

	S_ExtraUpdate();

	if (r_speeds.value)
	{
		float framerate = cl.time - cl.oldtime;

		if (framerate > 0.0)
			framerate = 1.0 / framerate;

		time2 = Sys_FloatTime();
		Con_Printf("%3ifps %3i ms  %4i wpoly %4i epoly\n", (int)(framerate + 0.5), (int)((time2 - time1) * 1000.0), c_brush_polys, c_alias_polys);
	}
}