// d_modech.c: called when mode has just changed

#include "quakedef.h"
#include "d_local.h"

int	d_vrectx, d_vrecty, d_vrectright_particle, d_vrectbottom_particle;

int d_vox_min, d_vox_max, d_vrectright_vox, d_vrectbottom_vox;

int	d_y_aspect_shift, d_pix_min, d_pix_max, d_pix_shift;

int		d_scantable[MAXHEIGHT];
short*	zspantable[MAXHEIGHT];

/*
================
D_Patch
================
*/
void D_Patch( void )
{
#if id386

	static qboolean protectset8 = FALSE;

	if (!protectset8)
	{
		Sys_MakeCodeWriteable((int)D_PolysetAff8Start,
			(int)D_PolysetAff8End - (int)D_PolysetAff8Start);
		protectset8 = TRUE;
	}

#endif	// id386
}


/*
================
D_ViewChanged
================
*/
void D_ViewChanged( void )
{
	int rowbytes;

	if (r_dowarp)
		rowbytes = WARP_WIDTH * 2;
	else
		rowbytes = vid.rowbytes;

	scale_for_mip = xscale;
	if (yscale > xscale)
		scale_for_mip = yscale;

	d_zrowbytes = vid.width * 2;
	d_zwidth = vid.width;

	d_pix_min = r_refdef.vrect.width / 320;
	if (d_pix_min < 1)
		d_pix_min = 1;

	d_pix_max = (int)((float)r_refdef.vrect.width / (320.0 / 4.0) + 0.5);
	d_vox_min = r_refdef.vrect.width / 200;
	d_vox_max = (int)((float)r_refdef.vrect.width / (320.0 / 18.0) + 0.5);
	d_pix_shift = 8 - (int)((float)r_refdef.vrect.width / 320.0 + 0.5);
	if (d_pix_max < 1)
		d_pix_max = 1;

	if (pixelAspect > 1.4)
		d_y_aspect_shift = 1;
	else
		d_y_aspect_shift = 0;

	d_vrectx = r_refdef.vrect.x;
	d_vrecty = r_refdef.vrect.y;
	d_vrectright_particle = r_refdef.vrectright - d_pix_max;
	d_vrectbottom_particle = r_refdef.vrectbottom - (d_pix_max << d_y_aspect_shift);
	d_vrectright_vox = r_refdef.vrectright - d_vox_max;
	d_vrectbottom_vox = r_refdef.vrectbottom - (d_vox_max << d_y_aspect_shift);

	{
		int		i;

		for (i = 0; i < (int)vid.height; i++)
		{
			d_scantable[i] = i * rowbytes;
			zspantable[i] = d_pzbuffer + i * vid.width;
		}
	}

	D_Patch();
}