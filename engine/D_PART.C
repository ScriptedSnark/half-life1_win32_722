// d_part.c: software driver module for drawing particles

#include "quakedef.h"
#include "d_local.h"


/*
==============
D_EndParticles
==============
*/
void D_EndParticles( void )
{
// not used by software driver
}


/*
==============
D_StartParticles
==============
*/
void D_StartParticles( void )
{
// not used by software driver
}


/*
==============
D_DrawParticle
==============
*/
void D_DrawParticle( particle_t* pparticle )
{
	vec3_t	local, transformed;
	float	zi;
	unsigned short* pdest;
	short* pz;
	int		i, izi, pix, count, u, v;

// transform point
	VectorSubtract(pparticle->org, r_origin, local);

	transformed[0] = DotProduct(local, r_pright);
	transformed[1] = DotProduct(local, r_pup);
	transformed[2] = DotProduct(local, r_ppn);

	if (transformed[2] < PARTICLE_Z_CLIP)
		return;

// project the point
// FIXME: preadjust xcenter and ycenter
	zi = 1.0 / transformed[2];
	u = (int)(xcenter + zi * transformed[0] + 0.5);
	v = (int)(ycenter - zi * transformed[1] + 0.5);

	if (pparticle->type == pt_vox_slowgrav || pparticle->type == pt_vox_grav)
	{
		if ((v > d_vrectbottom_vox) ||
			(u > d_vrectright_vox) ||
			(v < d_vrecty) ||
			(u < d_vrectx))
		{
			return;
		}
	}
	else
	{
		if ((v > d_vrectbottom_particle) ||
			(u > d_vrectright_particle) ||
			(v < d_vrecty) ||
			(u < d_vrectx))
		{
			return;
		}
	}

	pz = &zspantable[v][u];
	pdest = (unsigned short*)(d_viewbuffer + d_scantable[v] + u * 2);
	izi = (int)(zi * 0x8000);

	pix = izi >> d_pix_shift;

	if (pparticle->type == pt_vox_slowgrav || pparticle->type == pt_vox_grav)
	{
		if (pix < d_vox_min)
			pix = d_vox_min;
		else if (pix > d_vox_max)
			pix = d_vox_max;
	}
	else
	{
		if (pix < d_pix_min)
			pix = d_pix_min;
		else if (pix > d_pix_max)
			pix = d_pix_max;
	}

	switch (pix)
	{
	case 1:
		count = 1 << d_y_aspect_shift;

		for (; count; count--, pz += d_zwidth, pdest += screenwidth / 2)
		{
			if (pz[0] <= izi)
			{
				pz[0] = izi;
				pdest[0] = pparticle->packedColor;
			}
		}
		break;

	case 2:
		count = 2 << d_y_aspect_shift;

		for (; count; count--, pz += d_zwidth, pdest += screenwidth / 2)
		{
			if (pz[0] <= izi)
			{
				pz[0] = izi;
				pdest[0] = pparticle->packedColor;
			}

			if (pz[1] <= izi)
			{
				pz[1] = izi;
				pdest[1] = pparticle->packedColor;
			}
		}
		break;

	case 3:
		count = 3 << d_y_aspect_shift;

		for (; count; count--, pz += d_zwidth, pdest += screenwidth / 2)
		{
			if (pz[0] <= izi)
			{
				pz[0] = izi;
				pdest[0] = pparticle->packedColor;
			}

			if (pz[1] <= izi)
			{
				pz[1] = izi;
				pdest[1] = pparticle->packedColor;
			}

			if (pz[2] <= izi)
			{
				pz[2] = izi;
				pdest[2] = pparticle->packedColor;
			}
		}
		break;

	case 4:
		count = 4 << d_y_aspect_shift;

		for (; count; count--, pz += d_zwidth, pdest += screenwidth / 2)
		{
			if (pz[0] <= izi)
			{
				pz[0] = izi;
				pdest[0] = pparticle->packedColor;
			}

			if (pz[1] <= izi)
			{
				pz[1] = izi;
				pdest[1] = pparticle->packedColor;
			}

			if (pz[2] <= izi)
			{
				pz[2] = izi;
				pdest[2] = pparticle->packedColor;
			}

			if (pz[3] <= izi)
			{
				pz[3] = izi;
				pdest[3] = pparticle->packedColor;
			}
		}
		break;

	default:
		count = pix << d_y_aspect_shift;

		for (; count; count--, pz += d_zwidth, pdest += screenwidth / 2)
		{
			for (i = 0; i < pix; i++)
			{
				if (pz[i] <= izi)
				{
					pz[i] = izi;
					pdest[i] = pparticle->packedColor;
				}
			}
		}
		break;
	}
}