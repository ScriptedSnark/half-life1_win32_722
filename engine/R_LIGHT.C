// r_light.c

#include "quakedef.h"

int	r_dlightframecount;
int	r_dlightchanged;
int r_dlightactive;


/*
==================
R_AnimateLight
==================
*/
void R_AnimateLight( void )
{
	int			i, j, k;

//
// light animations
// 'm' is normal light, 'a' is no light, 'z' is double bright
	i = (int)(cl.time * 10);
	for (j = 0; j < MAX_LIGHTSTYLES; j++)
	{
		if (!cl_lightstyle[j].length)
		{
			d_lightstylevalue[j] = 256;
			continue;
		}
		k = i % cl_lightstyle[j].length;
		k = cl_lightstyle[j].map[k] - 'a';
		k = k * 22;
		d_lightstylevalue[j] = k;
	}
}


/*
=============================================================================

DYNAMIC LIGHTS

=============================================================================
*/

/*
=============
R_MarkLights
=============
*/
void R_MarkLights( dlight_t* light, int bit, mnode_t* node )
{
	mplane_t*splitplane;
	float		dist;
	msurface_t* surf;
	int			i;

	if (node->contents < 0)
		return;

	splitplane = node->plane;
	dist = DotProduct(light->origin, splitplane->normal) - splitplane->dist;

	if (dist > light->radius)
	{
		R_MarkLights(light, bit, node->children[0]);
		return;
	}
	if (dist < -light->radius)
	{
		R_MarkLights(light, bit, node->children[1]);
		return;
	}

// mark the polygons
	surf = cl.worldmodel->surfaces + node->firstsurface;
	for (i = 0; i < node->numsurfaces; i++, surf++)
	{
		float rad, minlight;
		int s, t;
		int smax, tmax;
		mtexinfo_t* tex;

		rad = light->radius - fabs(dist);
		if (light->minlight > rad)
			continue;

		tex = surf->texinfo;

		// Project light center into texture coordinates
		s = DotProduct(light->origin, tex->vecs[0])
			+ tex->vecs[0][3] - surf->texturemins[0];
		t = DotProduct(light->origin, tex->vecs[1])
			+ tex->vecs[1][3] - surf->texturemins[1];

		smax = surf->extents[0];
		tmax = surf->extents[1];

		minlight = rad - light->minlight;

		if (s <= -minlight || t <= -minlight || s > minlight + smax || t > minlight + tmax)
			continue;

		if (surf->dlightframe != r_dlightframecount)
		{
			surf->dlightframe = r_dlightframecount;
		}
		surf->dlightbits |= bit;
	}

	R_MarkLights(light, bit, node->children[0]);
	R_MarkLights(light, bit, node->children[1]);
}


/*
=============
R_PushDlights
=============
*/
void R_PushDlights( void )
{
	int		i;
	dlight_t* l;

	r_dlightframecount = r_framecount + 1;	// because the count hasn't
											//  advanced yet for this frame
	l = cl_dlights;

	for (i = 0; i < MAX_DLIGHTS; i++, l++)
	{
		if (l->die < cl.time || !l->radius)
			continue;
		R_MarkLights(l, 1 << i, cl.worldmodel->nodes);
	}
}


/*
=============================================================================

LIGHT SAMPLING

=============================================================================
*/

colorVec RecursiveLightPoint( mnode_t* node, vec_t* start, vec_t* end )
{
	colorVec	c;
	float		front, back, frac;
	int			side;
	mplane_t* plane;
	vec3_t		mid;
	msurface_t* surf;
	int			s, t, ds, dt;
	int			i;
	mtexinfo_t* tex;
	color24* lightmap;
	unsigned	scale;
	int			maps;

	if (node->contents < 0)		// didn't hit anything
	{
		c.r = c.g = c.b = c.a = 0;
		return c;
	}

// calculate mid point

// FIXME: optimize for axial
	plane = node->plane;
	front = DotProduct(start, plane->normal) - plane->dist;
	back = DotProduct(end, plane->normal) - plane->dist;
	side = front < 0;

	if ((back < 0) == side)
		return RecursiveLightPoint(node->children[side], start, end);

	frac = front / (front - back);
	mid[0] = start[0] + (end[0] - start[0]) * frac;
	mid[1] = start[1] + (end[1] - start[1]) * frac;
	mid[2] = start[2] + (end[2] - start[2]) * frac;

// go down front side
	c = RecursiveLightPoint(node->children[side], start, mid);
	if (c.r != 0 || c.g != 0 || c.b != 0)
		return c;		// hit something

	if ((back < 0) == side)
	{
		c.r = c.g = c.b = c.a = 0;
		return c;		// didn't hit anuthing
	}

// check for impact on this node

	surf = cl.worldmodel->surfaces + node->firstsurface;
	for (i = 0; i < node->numsurfaces; i++, surf++)
	{
		if (surf->flags & SURF_DRAWTILED)
			continue;	// no lightmaps

		tex = surf->texinfo;

		s = DotProduct(mid, tex->vecs[0]) + tex->vecs[0][3];
		t = DotProduct(mid, tex->vecs[1]) + tex->vecs[1][3];

		if (s < surf->texturemins[0] ||
			t < surf->texturemins[1])
			continue;

		ds = s - surf->texturemins[0];
		dt = t - surf->texturemins[1];

		if (ds > surf->extents[0] || dt > surf->extents[1])
			continue;

		if (!surf->samples)
		{
			c.r = c.g = c.b = c.a = 0;
			return c;
		}

		ds >>= 4;
		dt >>= 4;

		lightmap = surf->samples;
		c.r = c.g = c.b = c.a = 0;
		if (lightmap)
		{
			lightmap += dt * ((surf->extents[0] >> 4) + 1) + ds;

			for (maps = 0; maps < MAXLIGHTMAPS && surf->styles[maps] != 255;
				maps++)
			{
				scale = d_lightstylevalue[surf->styles[maps]];
				c.r += lightmap->r * scale;
				c.g += lightmap->g * scale;
				c.b += lightmap->b * scale;
				lightmap += ((surf->extents[0] >> 4) + 1) *
					((surf->extents[1] >> 4) + 1);
			}

			c.r >>= 8;
			c.g >>= 8;
			c.b >>= 8;

			if (c.r == 0)
				c.r = 1;
		}

		return c;
	}

// go down back side
	return RecursiveLightPoint(node->children[!side], mid, end);
}

colorVec R_LightPoint( vec_t* p )
{
	vec3_t		end;

	end[0] = p[0];
	end[1] = p[1];
	end[2] = p[2] - 2048;

	return R_LightVec(p, end);
}

colorVec R_LightVec( vec_t* start, vec_t* end )
{
	colorVec	c;

	if (!cl.worldmodel->lightdata)
	{
		c.r = c.g = c.b = 255;
		c.a = 0;
		return c;
	}

	c = RecursiveLightPoint(cl.worldmodel->nodes, start, end);

	c.r += r_refdef.ambientlight.r;
	c.g += r_refdef.ambientlight.g;
	c.b += r_refdef.ambientlight.b;

	if (c.r > 255)
		c.r = 255;
	if (c.g > 255)
		c.g = 255;
	if (c.b > 255)
		c.b = 255;

	return c;
}