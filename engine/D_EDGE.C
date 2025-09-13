// d_edge.c

#include "quakedef.h"
#include "d_local.h"
#include "pr_cmds.h"
#include "water.h"

static int	miplevel;

float		scale_for_mip;

int			ubasestep, errorterm, erroradjustup, erroradjustdown;


// FIXME: should go away
extern void			R_RotateBmodel( void );
extern void			R_TransformFrustum( void );

vec3_t		transformed_modelorg;

/*
==============
D_DrawPoly

==============
*/
void D_DrawPoly( void )
{
// this driver takes spans, not polygons
}


/*
=============
D_MipLevelForScale
=============
*/
int D_MipLevelForScale( float scale )
{
	int		lmiplevel;

	if (scale >= d_scalemip[0])
		lmiplevel = 0;
	else if (scale >= d_scalemip[1])
		lmiplevel = 1;
	else if (scale >= d_scalemip[2])
		lmiplevel = 2;
	else
		lmiplevel = 3;

	if (lmiplevel < d_minmip)
		lmiplevel = d_minmip;

	return lmiplevel;
}


/*
==============
D_DrawSolidSurface
==============
*/

// FIXME: clean this up

void D_DrawSolidSurface( surf_t* surf, int color )
{
	espan_t* span;
	unsigned short* pdest;
	int		u, u2;

	for (span = surf->spans; span; span = span->pnext)
	{
		pdest = (unsigned short*)((byte*)d_viewbuffer + screenwidth * span->v);
		u = span->u;
		u2 = span->u + span->count - 1;
		pdest[u] = color & 0xFFFF;

		if (u2 - u < 2)
		{
		}
		else
		{
			for (u++; u + 1 <= u2; u += 2)
				*(unsigned int*)(pdest + u) = (color & 0xFFFF) | (color << 16);

			if (u <= u2)
				pdest[u] = color & 0xFFFF;
		}
	}
}


/*
==============
D_CalcGradients
==============
*/
void D_CalcGradients( msurface_t* pface )
{
	float		mipscale;
	vec3_t		p_temp1;
	vec3_t		p_saxis, p_taxis;
	float		t;

	mipscale = 1.0 / (float)(1 << miplevel);

	TransformVector(pface->texinfo->vecs[0], p_saxis);
	TransformVector(pface->texinfo->vecs[1], p_taxis);

	t = xscaleinv * mipscale;
	d_sdivzstepu = p_saxis[0] * t;
	d_tdivzstepu = p_taxis[0] * t;

	t = yscaleinv * mipscale;
	d_sdivzstepv = -p_saxis[1] * t;
	d_tdivzstepv = -p_taxis[1] * t;

	d_sdivzorigin = p_saxis[2] * mipscale - xcenter * d_sdivzstepu -
		ycenter * d_sdivzstepv;
	d_tdivzorigin = p_taxis[2] * mipscale - xcenter * d_tdivzstepu -
		ycenter * d_tdivzstepv;

	VectorScale(transformed_modelorg, mipscale, p_temp1);

	t = 0x10000 * mipscale;
	sadjust = ((fixed16_t)(DotProduct(p_temp1, p_saxis) * 0x10000 + 0.5)) -
		((pface->texturemins[0] << 16) >> miplevel)
		+ pface->texinfo->vecs[0][3] * t;
	tadjust = ((fixed16_t)(DotProduct(p_temp1, p_taxis) * 0x10000 + 0.5)) -
		((pface->texturemins[1] << 16) >> miplevel)
		+ pface->texinfo->vecs[1][3] * t;

//
// -1 (-epsilon) so we never wander off the edge of the texture
//
	bbextents = ((pface->extents[0] << 16) >> miplevel) - 1;
	bbextentt = ((pface->extents[1] << 16) >> miplevel) - 1;
}


// Find the leaf number associated with the surface
int LeafId( msurface_t* psurface )
{
	int	i;
	int surfIndex;

	for (i = 0; i < cl.worldmodel->numleafs; i++)
	{
		for (surfIndex = 0; surfIndex < cl.worldmodel->leafs[i].nummarksurfaces; surfIndex++)
		{
			if (psurface == cl.worldmodel->leafs[i].firstmarksurface[surfIndex])
			{
				return i;
			}
		}
	}
	
	surfIndex = psurface - cl.worldmodel->surfaces;
	for (i = 0; i < cl.worldmodel->numnodes; i++)
	{
		if (surfIndex >= cl.worldmodel->nodes[i].firstsurface &&
			surfIndex < (cl.worldmodel->nodes[i].firstsurface + cl.worldmodel->nodes[i].numsurfaces))
		{
			return cl.worldmodel->numleafs + i;
		}
	}

	return 1;
}


/*
==============
D_DrawSurfaces
==============
*/
void D_DrawSurfaces( void )
{
	surf_t*	s;
	msurface_t*	pface;
	surfcache_t* pcurrentcache;
	vec3_t			world_transformed_modelorg;
	vec3_t			local_modelorg;
	static word		flatcolors[256 * 4];

	currententity = &cl_entities[0];
	TransformVector(modelorg, transformed_modelorg);
	VectorCopy(transformed_modelorg, world_transformed_modelorg);

// TODO: could preset a lot of this at mode set time
	if (r_drawflat.value)
	{
		if (!flatcolors[1])
		{
			int i;

			for (i = 0; i < 256 * 4; i += 4)
			{
				do
				{
					flatcolors[i] = RandomLong(0, 255);
					flatcolors[i + 1] = RandomLong(0, 255);
					flatcolors[i + 2] = RandomLong(0, 255);
				} while (flatcolors[i] + flatcolors[i + 1] + flatcolors[i + 2] < 256);
				flatcolors[0] = 0;
				flatcolors[1] = 0xFF;
				flatcolors[2] = 0xFF;
				flatcolors[3] = 0;
				flatcolors[4] = 0xFF;
				flatcolors[5] = 0xFF;
				flatcolors[6] = 0xFF;
				flatcolors[7] = 0;
			}
		}

		for (s = &surfaces[1]; s < surface_p; s++)
		{
			if (!s->spans)
				continue;

			if (r_intentities && (s->flags & SURF_DRAWBACKGROUND))
				continue;

			d_zistepu = s->d_zistepu;
			d_zistepv = s->d_zistepv;
			d_ziorigin = s->d_ziorigin;

			if (s->flags & SURF_DRAWBACKGROUND)
			{
				D_DrawSolidSurface(s, 0);
			}
			else if (r_drawflat.value == 2)
			{
				if (s->insubmodel)
				{
					D_DrawSolidSurface(s, hlRGB(flatcolors, 0));
				}
				else
				{
					int id;

					id = LeafId((msurface_t*)s->data);
					if (id > 2)
					{
						id %= 255;
						if (id < 2)
							id += 2;
					}

					D_DrawSolidSurface(s, hlRGB(flatcolors, id));
				}
			}
			else
			{
				D_DrawSolidSurface(s, hlRGB(flatcolors, ((int)s->data >> 4) % 255));
			}
	
			D_DrawZSpans(s->spans);
		}
	}
	else
	{
		for (s = &surfaces[1]; s < surface_p; s++)
		{
			if (!s->spans)
				continue;

			r_drawnpolycount++;

			d_zistepu = s->d_zistepu;
			d_zistepv = s->d_zistepv;
			d_ziorigin = s->d_ziorigin;

			if (s->flags & SURF_DRAWSKY)
			{
			// FIXME: we don't want to do all this for every polygon!
			// TODO: store once at start of frame
				currententity = s->entity;	//FIXME: make this passed in to
											// R_RotateBmodel()
				VectorSubtract(r_origin, currententity->origin, local_modelorg);
				TransformVector(local_modelorg, transformed_modelorg);

				pface = (msurface_t*)s->data;
				miplevel = 0;

				cacheblock = (pixel_t*)pface->samples;
				cachewidth = pface->extents[0];

				D_CalcGradients(pface);

				(*d_drawspans)(s->spans);

			// set up a gradient for the background surface that places it
			// effectively at infinity distance from the viewpoint
				d_zistepu = 0.0;
				d_zistepv = 0.0;
				d_ziorigin = -0.9;

				D_DrawZSpans(s->spans);
			}
			else if (s->flags & SURF_DRAWBACKGROUND)
			{
				if (!r_intentities)
				{
				// set up a gradient for the background surface that places it
				// effectively at infinity distance from the viewpoint
					d_zistepu = 0.0;
					d_zistepv = 0.0;
					d_ziorigin = -0.9;

					D_DrawSolidSurface(s, 0);
					D_DrawZSpans(s->spans);
				}
			}
			else if (s->flags & SURF_DRAWTILED)
			{
				pface = (msurface_t*)s->data;

				if (s->insubmodel)
				{
				// FIXME: we don't want to do all this for every polygon!
				// TODO: store once at start of frame
					currententity = s->entity;	//FIXME: make this passed in to
												// R_RotateBmodel()
					VectorSubtract(r_origin, currententity->origin,
						local_modelorg);
					TransformVector(local_modelorg, transformed_modelorg);

					R_RotateBmodel();	// FIXME: don't mess with the frustum,
										// make entity passed in
				}

				if (s->flags & SURF_DRAWTURB)
				{
					miplevel = 0;
					cacheblock = (pixel_t*)((byte*)pface->texinfo->texture + pface->texinfo->texture->offsets[0]);

					r_palette = (word*)((byte*)pface->texinfo->texture + pface->texinfo->texture->paloffset);
					D_CalcGradients(pface);
					WaterTextureUpdate(r_palette, 0.1, pface->texinfo->texture);
				}
				else
				{
					float speed;
					int	width, height;

					miplevel = D_MipLevelForScale(s->nearzi * scale_for_mip
						* pface->texinfo->mipadjust);

				// FIXME: make this passed in to D_CacheSurface
					pcurrentcache = D_CacheSurface(pface, miplevel);

					cacheblock = (pixel_t*)pcurrentcache->data;
					cachewidth = pcurrentcache->width;

					D_CalcGradients(pface);

					width = pface->texinfo->texture->width >> miplevel;
					height = pface->texinfo->texture->height >> miplevel;
					TilingSetup(width - 1, height - 1, Q_log2(width));

					speed = ((currententity->rendercolor.g << 8) + currententity->rendercolor.b) * (1.0 / 16);
					if (currententity->rendercolor.r == 0)
						speed = -speed;
					speed = 1.0 / (1 << miplevel) * speed * cl.time * (float)0x10000;
					sadjust += speed;
				}

				if (r_intentities)
				{
					D_DrawTiled8Trans(s->spans);
				}
				else
				{
					D_DrawTiled8(s->spans);
					D_DrawZSpans(s->spans);
				}

				if (s->insubmodel)
				{
				//
				// restore the old drawing state
				// FIXME: we don't want to do this every time!
				// TODO: speed up
				//
					currententity = &cl_entities[0];
					VectorCopy(world_transformed_modelorg,
						transformed_modelorg);
					VectorCopy(base_vpn, vpn);
					VectorCopy(base_vup, vup);
					VectorCopy(base_vright, vright);
					VectorCopy(base_modelorg, modelorg);
					R_TransformFrustum();
				}
			}
			else
			{
				if (s->insubmodel)
				{
				// FIXME: we don't want to do all this for every polygon!
				// TODO: store once at start of frame
					currententity = s->entity;	//FIXME: make this passed in to
												// R_RotateBmodel()
					VectorSubtract(r_origin, currententity->origin, local_modelorg);
					TransformVector(local_modelorg, transformed_modelorg);

					R_RotateBmodel();	// FIXME: don't mess with the frustum,
										// make entity passed in
				}

				pface = (msurface_t*)s->data;
				miplevel = D_MipLevelForScale(s->nearzi * scale_for_mip
					* pface->texinfo->mipadjust);

			// FIXME: make this passed in to D_CacheSurface
				pcurrentcache = D_CacheSurface(pface, miplevel);

				cacheblock = (pixel_t*)pcurrentcache->data;
				cachewidth = pcurrentcache->width;

				D_CalcGradients(pface);

				(*d_drawspans)(s->spans);

				if (!r_intentities)
				{
					D_DrawZSpans(s->spans);
				}

				if (s->insubmodel)
				{
				//
				// restore the old drawing state
				// FIXME: we don't want to do this every time!
				// TODO: speed up
				//
					VectorCopy(world_transformed_modelorg,
						transformed_modelorg);
					VectorCopy(base_vpn, vpn);
					VectorCopy(base_vup, vup);
					VectorCopy(base_vright, vright);
					VectorCopy(base_modelorg, modelorg);
					R_TransformFrustum();
					currententity = &cl_entities[0];
				}
			}
		}
	}
}