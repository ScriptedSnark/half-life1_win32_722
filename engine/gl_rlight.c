// gl_rlight.c

#include "quakedef.h"
#include "r_local.h"

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

DYNAMIC LIGHTS BLEND RENDERING

=============================================================================
*/

void AddLightBlend( float r, float g, float b, float a2 )
{
	float	a;

	v_blend[3] = a = v_blend[3] + a2 * (1 - v_blend[3]);

	a2 = a2 / a;

	v_blend[0] = v_blend[1] * (1 - a2) + r * a2;
	v_blend[1] = v_blend[1] * (1 - a2) + g * a2;
	v_blend[2] = v_blend[2] * (1 - a2) + b * a2;
}

void R_RenderDlight( dlight_t* light )
{
	int		i, j;
	float	a;
	vec3_t	v;
	float	rad;

	rad = light->radius * 0.35;

	VectorSubtract(light->origin, r_origin, v);
	if (Length(v) < rad)
	{	// view is inside the dlight
		AddLightBlend(1, 0.5, 0, light->radius * 0.0003);
		return;
	}

	qglBegin(GL_TRIANGLE_FAN);
	qglColor3f(0.2, 0.1, 0.0);
	for (i = 0; i < 3; i++)
		v[i] = light->origin[i] - vpn[i] * rad;
	qglVertex3fv(v);
	qglColor3f(0, 0, 0);
	for (i = 16; i >= 0; i--)
	{
		a = i / 16.0 * M_PI * 2;
		for (j = 0; j < 3; j++)
			v[j] = light->origin[j] + vright[j] * cos(a) * rad
				+ vup[j] * sin(a) * rad;
		qglVertex3fv(v);
	}
	qglEnd();
}

/*
=============
R_RenderDlights
=============
*/
void R_RenderDlights( void )
{
	int		i;
	dlight_t* l;

	if (!gl_flashblend.value)
		return;

	r_dlightframecount = r_framecount + 1; // because the count hasn't
											//  advanced yet for this frame
	qglDepthMask(GL_FALSE);
	qglDisable(GL_TEXTURE_2D);
	qglShadeModel(GL_SMOOTH);
	qglEnable(GL_BLEND);
	qglBlendFunc(GL_ONE, GL_ONE);

	l = cl_dlights;
	for (i = 0; i < MAX_DLIGHTS; i++, l++)
	{
		if (l->die < cl.time || !l->radius)
			continue;
		R_RenderDlight(l);
	}

	qglColor3f(1, 1, 1);
	qglDisable(GL_BLEND);
	qglEnable(GL_TEXTURE_2D);
	qglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	qglDepthMask(GL_TRUE);
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
	mplane_t*	splitplane;
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

		minlight = rad - light->minlight;

		// Project light center into texture coordinates
		s = DotProduct(light->origin, tex->vecs[0])
			+ tex->vecs[0][3] - surf->texturemins[0];
		t = DotProduct(light->origin, tex->vecs[1])
			+ tex->vecs[1][3] - surf->texturemins[1];

		smax = surf->extents[0];
		tmax = surf->extents[1];

		if (s <= -minlight || t <= -minlight || s > minlight + smax || t > minlight + tmax)
		{
			continue;
		}

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

	if (gl_flashblend.value)
		return;

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

// TODO: Implement