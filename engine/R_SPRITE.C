// r_sprite.c

#include "quakedef.h"

static int				clip_current;
static vec5_t			clip_verts[2][MAXWORKINGVERTS];
static int				sprite_width, sprite_height;

spritedesc_t			r_spritedesc;


/*
=============
R_ClipSpriteFace

Clips the winding at clip_verts[clip_current] and changes clip_current
Throws out the back side
==============
*/
int R_ClipSpriteFace( int nump, clipplane_t* pclipplane )
{
	int		i, outcount;
	float	dists[MAXWORKINGVERTS + 1];
	float	frac, clipdist, * pclipnormal;
	float* in, * instep, * outstep, * vert2;

	clipdist = pclipplane->dist;
	pclipnormal = pclipplane->normal;

// calc dists
	if (clip_current)
	{
		in = clip_verts[1][0];
		outstep = clip_verts[0][0];
		clip_current = 0;
	}
	else
	{
		in = clip_verts[0][0];
		outstep = clip_verts[1][0];
		clip_current = 1;
	}

	instep = in;
	for (i = 0; i < nump; i++, instep += sizeof(vec5_t) / sizeof(float))
	{
		dists[i] = DotProduct(instep, pclipnormal) - clipdist;
	}

// handle wraparound case
	dists[nump] = dists[0];
	Q_memcpy(instep, in, sizeof(vec5_t));


// clip the winding
	instep = in;
	outcount = 0;

	for (i = 0; i < nump; i++, instep += sizeof(vec5_t) / sizeof(float))
	{
		if (dists[i] >= 0)
		{
			Q_memcpy(outstep, instep, sizeof(vec5_t));
			outstep += sizeof(vec5_t) / sizeof(float);
			outcount++;
		}

		if (dists[i] == 0 || dists[i + 1] == 0)
			continue;

		if ((dists[i] > 0) == (dists[i + 1] > 0))
			continue;

	// split it into a new vertex
		frac = dists[i] / (dists[i] - dists[i + 1]);

		vert2 = instep + sizeof(vec5_t) / sizeof(float);

		outstep[0] = instep[0] + frac * (vert2[0] - instep[0]);
		outstep[1] = instep[1] + frac * (vert2[1] - instep[1]);
		outstep[2] = instep[2] + frac * (vert2[2] - instep[2]);
		outstep[3] = instep[3] + frac * (vert2[3] - instep[3]);
		outstep[4] = instep[4] + frac * (vert2[4] - instep[4]);

		outstep += sizeof(vec5_t) / sizeof(float);
		outcount++;
	}

	return outcount;
}


/*
================
R_SetupAndDrawSprite
================
*/
void R_SetupAndDrawSprite( void )
{
	int			i, nump;
	float		dot, scale, * pv;
	vec5_t*	pverts;
	vec3_t		left, up, right, down, transformed, local;
	emitpoint_t	outverts[MAXWORKINGVERTS + 1], * pout;

	dot = DotProduct(r_spritedesc.vpn, modelorg);

// backface cull
	if (dot >= 0)
		return;

// build the sprite poster in worldspace
	scale = r_spritedesc.pspriteframe->right * r_spritedesc.scale;
	VectorScale(r_spritedesc.vright, scale, right);
	scale = r_spritedesc.pspriteframe->up * r_spritedesc.scale;
	VectorScale(r_spritedesc.vup, scale, up);
	scale = r_spritedesc.pspriteframe->left * r_spritedesc.scale;
	VectorScale(r_spritedesc.vright, scale, left);
	scale = r_spritedesc.pspriteframe->down * r_spritedesc.scale;
	VectorScale(r_spritedesc.vup, scale, down);

	pverts = clip_verts[0];

	pverts[0][0] = r_entorigin[0] + up[0] + left[0];
	pverts[0][1] = r_entorigin[1] + up[1] + left[1];
	pverts[0][2] = r_entorigin[2] + up[2] + left[2];
	pverts[0][3] = 0;
	pverts[0][4] = 0;

	pverts[1][0] = r_entorigin[0] + up[0] + right[0];
	pverts[1][1] = r_entorigin[1] + up[1] + right[1];
	pverts[1][2] = r_entorigin[2] + up[2] + right[2];
	pverts[1][3] = sprite_width;
	pverts[1][4] = 0;

	pverts[2][0] = r_entorigin[0] + down[0] + right[0];
	pverts[2][1] = r_entorigin[1] + down[1] + right[1];
	pverts[2][2] = r_entorigin[2] + down[2] + right[2];
	pverts[2][3] = sprite_width;
	pverts[2][4] = sprite_height;

	pverts[3][0] = r_entorigin[0] + down[0] + left[0];
	pverts[3][1] = r_entorigin[1] + down[1] + left[1];
	pverts[3][2] = r_entorigin[2] + down[2] + left[2];
	pverts[3][3] = 0;
	pverts[3][4] = sprite_height;

// clip to the frustum in worldspace
	nump = 4;
	clip_current = 0;

	for (i = 0; i < 4; i++)
	{
		nump = R_ClipSpriteFace(nump, &view_clipplanes[i]);
		if (nump < 3)
			return;
		if (nump >= MAXWORKINGVERTS)
			Sys_Error("R_SetupAndDrawSprite: too many points");
	}

// transform vertices into viewspace and project
	pv = &clip_verts[clip_current][0][0];
	r_spritedesc.nearzi = -999999;

	for (i = 0; i < nump; i++)
	{
		VectorSubtract(pv, r_origin, local);
		TransformVector(local, transformed);

		if (transformed[2] < 1.0)
			return;

		pout = &outverts[i];
		pout->zi = 1.0 / transformed[2];
		if (pout->zi > r_spritedesc.nearzi)
			r_spritedesc.nearzi = pout->zi;

		pout->s = pv[3];
		pout->t = pv[4];

		scale = xscale * pout->zi;
		pout->u = (xcenter + scale * transformed[0]);
		if (pout->u < 0)
			pout->u = 0;

		scale = yscale * pout->zi;
		pout->v = (ycenter - scale * transformed[1]);
		if (pout->v < 0)
			pout->v = 0;

		pv += sizeof(vec5_t) / sizeof(*pv);
	}

// draw it
	r_spritedesc.nump = nump;
	r_spritedesc.pverts = outverts;
	D_DrawSprite();
}


/*
================
R_DrawSprite
================
*/
void R_DrawSprite( void )
{
	msprite_t* psprite;
	colorVec color;
	word palette[256];

	psprite = (msprite_t*)currententity->model->cache.data;
	r_spritedesc.pspriteframe = R_GetSpriteFrame(psprite, currententity->frame);

	if (currententity->scale > 0.0)
		r_spritedesc.scale = currententity->scale;
	else
		r_spritedesc.scale = 1.0;

	// Get sprite color
	R_SpriteColor(&color, currententity, r_blend);

	UnpackPalette(palette, (word*)((byte*)psprite + psprite->paloffset), color.r, color.g, color.b);
	r_palette = palette;

	sprite_width = r_spritedesc.pspriteframe->width;
	sprite_height = r_spritedesc.pspriteframe->height;
	// Get orthonormal bases
	R_GetSpriteAxes(currententity, psprite->type, r_spritedesc.vpn, r_spritedesc.vright, r_spritedesc.vup);
	
	R_SetupAndDrawSprite();
}