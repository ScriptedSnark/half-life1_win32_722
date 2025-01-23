// r_main.c

#include "quakedef.h"

cl_entity_t	r_worldentity;

// TODO: Implement


int			c_brush_polys, c_alias_polys;


int			currenttexture = -1;	// to avoid unnecessary texture sets
int			cnttextures[2] = { -1, -1 };     // cached




int			mirrortexturenum;	// quake texturenum, not gltexturenum
qboolean	mirror;
mplane_t*	mirror_plane;





float	r_world_matrix[16];
float	r_base_world_matrix[16];
float	gProjectionMatrix[16];
float	gWorldToScreen[16];
float	gScreenToWorld[16];

//
// screen size info
//
refdef_t	r_refdef;

mleaf_t*	r_viewleaf, * r_oldviewleaf;

texture_t*	r_notexture_mip;

int			d_lightstylevalue[256];	// 8.8 fraction of base light value

// TODO: Implement

/*
=============
R_DrawViewModel
=============
*/
void R_DrawViewModel( void )
{
	// TODO: Implement
}

void R_PreDrawViewModel( void )
{
	// TODO: Implement
}

/*
============
R_PolyBlend
============
*/
void R_PolyBlend( void )
{

}

// TODO: Implement

/*
=============
R_SetupGL
=============
*/
void R_SetupGL( void )
{

}

/*
================
R_RenderScene

r_refdef must be set before the first call
================
*/
void R_RenderScene( void )
{
	// TODO: Implement
}


/*
=============
R_Clear
=============
*/
void R_Clear( void )
{
	// TODO: Implement
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

	if (r_speeds.value != 0.0)
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