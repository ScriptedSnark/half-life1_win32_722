// gl_warp.c -- sky and water polygons

#include "quakedef.h"
#include "gl_water.h"

extern model_t* loadmodel;

cshift_t	cshift_water = { 130, 80, 50, 128 };

//cshift_t	cshift_water; TODO: Implement

int		skytexturenum;

int		solidskytexture;
int		alphaskytexture;
float	speedscale;		// for top sky and bottom sky
colorVec gWaterColor;

msurface_t* warpface;

#define	BLOCK_WIDTH		128
#define	BLOCK_HEIGHT	128

#define SUBDIVIDE_SIZE	64.0

void R_DrawSkyBox( void );

void BoundPoly( int numverts, float* verts, vec_t* mins, vec_t* maxs )
{
	int		i, j;
	float* v;

	mins[0] = mins[1] = mins[2] = 9999;
	maxs[0] = maxs[1] = maxs[2] = -9999;
	v = verts;
	for (i = 0; i < numverts; i++)
		for (j = 0; j < 3; j++, v++)
		{
			if (*v < mins[j])
				mins[j] = *v;
			if (*v > maxs[j])
				maxs[j] = *v;
		}
}

void SubdividePolygon( int numverts, float* verts )
{
	int		i, j, k;
	vec3_t	mins, maxs;
	float	m;
	float* v;
	vec3_t	front[64], back[64];
	int		f, b;
	float	dist[64];
	float	frac;
	glpoly_t* poly;
	float	s, t;

	if (numverts > 60)
		Sys_Error("numverts = %i", numverts);

	BoundPoly(numverts, verts, mins, maxs);

	for (i = 0; i < 3; i++)
	{
		m = (mins[i] + maxs[i]) * 0.5;
		m = SUBDIVIDE_SIZE * floor(m / SUBDIVIDE_SIZE + 0.5);
		if (maxs[i] - m < 8)
			continue;
		if (m - mins[i] < 8)
			continue;

		// cut it
		v = verts + i;
		for (j = 0; j < numverts; j++, v += 3)
			dist[j] = *v - m;

		// wrap cases
		dist[j] = dist[0];
		v -= i;
		VectorCopy(verts, v);

		f = b = 0;
		v = verts;
		for (j = 0; j < numverts; j++, v += 3)
		{
			if (dist[j] >= 0)
			{
				VectorCopy(v, front[f]);
				f++;
			}
			if (dist[j] <= 0)
			{
				VectorCopy(v, back[b]);
				b++;
			}
			if (dist[j] == 0 || dist[j + 1] == 0)
				continue;
			if ((dist[j] > 0) != (dist[j + 1] > 0))
			{
				// clip point
				frac = dist[j] / (dist[j] - dist[j + 1]);
				for (k = 0; k < 3; k++)
					front[f][k] = back[b][k] = v[k] + frac * (v[3 + k] - v[k]);
				f++;
				b++;
			}
		}

		SubdividePolygon(f, front[0]);
		SubdividePolygon(b, back[0]);
		return;
	}

	poly = (glpoly_t*)Hunk_Alloc(sizeof(glpoly_t) + (numverts - 4) * VERTEXSIZE * sizeof(float));
	poly->next = warpface->polys;
	poly->flags = warpface->flags;
	warpface->polys = poly;
	poly->numverts = numverts;
	for (i = 0; i < numverts; i++, verts += 3)
	{
		VectorCopy(verts, poly->verts[i]);
		s = DotProduct(verts, warpface->texinfo->vecs[0]);
		t = DotProduct(verts, warpface->texinfo->vecs[1]);
		poly->verts[i][3] = s;
		poly->verts[i][4] = t;

		//
		// lightmap texture coordinates
		//
		s = DotProduct(verts, warpface->texinfo->vecs[0]) + warpface->texinfo->vecs[0][3];
		s -= warpface->texturemins[0];
		s += (float)(warpface->light_s * (BLOCK_WIDTH * 16));
		s += 8;
		s /= BLOCK_WIDTH * 16; //fa->texinfo->texture->width;

		t = DotProduct(verts, warpface->texinfo->vecs[1]) + warpface->texinfo->vecs[1][3];
		t -= warpface->texturemins[1];
		t += (float)(warpface->light_t * (BLOCK_HEIGHT * 16));
		t += 8;
		t /= BLOCK_HEIGHT * 16; //fa->texinfo->texture->height;

		poly->verts[i][5] = s;
		poly->verts[i][6] = t;
	}
}

/*
================
GL_SubdivideSurface

Breaks a polygon up along axial 64 unit
boundaries so that turbulent and sky warps
can be done reasonably.
================
*/
void GL_SubdivideSurface( msurface_t* fa )
{
	vec3_t		verts[64];
	int			numverts;
	int			i;
	int			lindex;
	float* vec;

	warpface = fa;

	//
	// convert edges back to a normal polygon
	//
	numverts = 0;
	for (i = 0; i < fa->numedges; i++)
	{
		lindex = loadmodel->surfedges[fa->firstedge + i];

		if (lindex > 0)
			vec = loadmodel->vertexes[loadmodel->edges[lindex].v[0]].position;
		else
			vec = loadmodel->vertexes[loadmodel->edges[-lindex].v[1]].position;
		VectorCopy(vec, verts[numverts]);
		numverts++;
	}

	SubdividePolygon(numverts, verts[0]);
}

//=========================================================




// TODO: Implement


void R_LoadSkys( void )
{
	// TODO: Implement
}


// TODO: Implement


//===============================================================

/*
==================
R_InitSky
==================
*/
void R_InitSky( void )
{
	 // TODO: Implement
}