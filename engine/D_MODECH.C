// d_modech.c: called when mode has just changed

#include "quakedef.h"
#include "d_local.h"
#include "vid.h"

int	d_vrectx, d_vrecty, d_vrectright_particle, d_vrectbottom_particle;

int d_vox_min, d_vox_max, d_vrectright_vox, d_vrectbottom_vox;

int	d_y_aspect_shift, d_pix_min, d_pix_max, d_pix_shift;

int		d_scantable[MAXHEIGHT];


/*
================
D_ViewChanged
================
*/
void D_ViewChanged( void )
{
	// TODO: Implement
}