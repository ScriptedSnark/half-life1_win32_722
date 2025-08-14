// r_main.c

#include "quakedef.h"

//
// view origin
//
vec3_t	vup, base_vup;
vec3_t	vpn, base_vpn;
vec3_t	vright, base_vright;
vec3_t	r_origin;

//
// screen size info
//
refdef_t	r_refdef;
float		xcenter, ycenter;
float		xscale, yscale;
float		xscaleinv, yscaleinv;
float		xscaleshrink, yscaleshrink;
float		aliasxscale, aliasyscale, aliasxcenter, aliasycenter;


//
// refresh flags
//
int		r_framecount = 1;	// so frame counts initialized to 0 don't match
int		r_visframecount;



mleaf_t* r_viewleaf, * r_oldviewleaf;



cvar_t	r_cachestudio = { "r_cachestudio", "1" };




cvar_t	r_fullbright = { "r_fullbright", "0" };
cvar_t	r_decals = { "r_decals", "1" };
cvar_t	r_lightmap = { "r_lightmap", "-1" };
cvar_t	r_lightstyle = { "r_lightstyle", "-1" };






cvar_t	r_ambient_r = { "r_ambient_r", "0" };
cvar_t	r_ambient_g = { "r_ambient_g", "0" };
cvar_t	r_ambient_b = { "r_ambient_b", "0" };





cvar_t	r_traceglow = { "r_traceglow", "0" };



/*
==================
R_InitTextures
==================
*/
void R_InitTextures( void )
{
	// TODO: Implement
}

/*
===============
R_Init
===============
*/
void R_Init( void )
{
	// TODO: Implement
}



/*
===============
R_NewMap
===============
*/
void R_NewMap( void )
{
	// TODO: Implement
}


/*
===============
R_SetVrect
===============
*/
void R_SetVrect( vrect_t* pvrectin, vrect_t* pvrect, int lineadj )
{
	int		h;
	float	size;

	size = scr_viewsize.value > 100 ? 100 : scr_viewsize.value;
	if (cl.intermission)
	{
		size = 100;
		lineadj = 0;
	}
	size /= 100;

	h = pvrectin->height - lineadj;
	pvrect->width = pvrectin->width * size;
	if (pvrect->width < 96)
	{
		size = 96.0 / pvrectin->width;
		pvrect->width = 96;	// min for icons
	}
	pvrect->width &= ~7;
	pvrect->height = pvrectin->height * size;
	if (pvrect->height > pvrectin->height - lineadj)
		pvrect->height = pvrectin->height - lineadj;

	pvrect->height &= ~1;

	pvrect->x = (pvrectin->width - pvrect->width) / 2;
	pvrect->y = (h - pvrect->height) / 2;

	{
		if (lcd_x.value)
		{
			pvrect->y >>= 1;
			pvrect->height >>= 1;
		}
	}
}


/*
===============
R_ViewChanged

Called every time the vid structure or r_refdef changes.
Guaranteed to be called before the first refresh
===============
*/
void R_ViewChanged( vrect_t* pvrect, int lineadj, float aspect )
{
	// TODO: Implement
}




/*
===============
R_MarkLeaves
===============
*/
void R_MarkLeaves( void )
{
	// TODO: Implement
}

// JAY: Setup frustum
/*
=================
R_CullBox

Returns true if the box is completely outside the frustom
=================
*/
qboolean R_CullBox( vec_t* mins, vec_t* maxs )
{
	// TODO: Implement
	return FALSE;
}

void R_SetStackBase( void )
{
	// TODO: Implement
}


void R_RenderView( void )
{
	// TODO: Implement
}