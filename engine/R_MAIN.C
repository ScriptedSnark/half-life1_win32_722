// r_main.c

#include "quakedef.h"
#include "winquake.h"

//define	PASSAGES

// TODO: Implement

int			r_pixbytes = 1;

// TODO: Implement

byte* r_stack_start;

qboolean	r_fov_greater_than_90;

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

int			screenwidth;

// TODO: Implement

//
// refresh flags
//
int		r_framecount = 1;	// so frame counts initialized to 0 don't match
int		r_visframecount;

// TODO: Implement

mleaf_t* r_viewleaf, * r_oldviewleaf;

texture_t* r_notexture_mip;

// TODO: Implement

float	dp_time1, dp_time2, db_time1, db_time2, rw_time1, rw_time2;
float	se_time1, se_time2, de_time1, de_time2, dv_time1, dv_time2;

void R_MarkLeaves( void );

cvar_t	r_cachestudio = { "r_cachestudio", "1" };
cvar_t	r_draworder = { "r_draworder", "0" };
cvar_t	r_speeds = { "r_speeds", "0" };
cvar_t	r_timegraph = { "r_timegraph", "0" };
cvar_t	r_graphheight = { "r_graphheight", "10" };
cvar_t	r_luminance = { "r_luminance", "0" };
cvar_t	r_clearcolor = { "r_clearcolor", "2" };
cvar_t	r_waterwarp = { "r_waterwarp", "1" };
cvar_t	r_fullbright = { "r_fullbright", "0" };

cvar_t	r_decals = { "r_decals", "1" };
cvar_t	r_lightmap = { "r_lightmap", "-1" };
cvar_t	r_lightstyle = { "r_lightstyle", "-1" };
cvar_t	r_drawentities = { "r_drawentities", "1" };
cvar_t	r_drawviewmodel = { "r_drawviewmodel", "1" };
cvar_t	r_aliasstats = { "r_polymodelstats", "0" };
cvar_t	r_dspeeds = { "r_dspeeds", "0" };
cvar_t	r_drawflat = { "r_drawflat", "0" };
cvar_t	r_ambient_r = { "r_ambient_r", "0" };
cvar_t	r_ambient_g = { "r_ambient_g", "0" };
cvar_t	r_ambient_b = { "r_ambient_b", "0" };

cvar_t	r_reportsurfout = { "r_reportsurfout", "1" };
cvar_t	r_reportedgeout = { "r_reportedgeout", "1" };

cvar_t	r_maxsurfs = { "r_maxsurfs", "0" };
cvar_t	r_numsurfs = { "r_numsurfs", "0" };
cvar_t	r_maxedges = { "r_maxedges", "0" };
cvar_t	r_numedges = { "r_numedges", "0" };
cvar_t	r_aliastransbase = { "r_aliastransbase", "200" };
cvar_t	r_aliastransadj = { "r_aliastransadj", "100" };
cvar_t	r_mmx = { "r_mmx", "0" };
cvar_t	r_traceglow = { "r_traceglow", "0" };
cvar_t	r_wadtextures = { "r_wadtextures", "0" };

void CreatePassages( void );
void SetVisibilityByPassages( void );

void R_DrawInitLut( void );

/*
==================
R_InitTextures
==================
*/
void R_InitTextures( void )
{
	int		x, y, m;
	byte* dest;

// create a simple checkerboard texture for the default
	r_notexture_mip = (texture_t*)Hunk_AllocName(sizeof(texture_t) + 16 * 16 + 8 * 8 + 4 * 4 + 2 * 2, "notexture");

	r_notexture_mip->width = r_notexture_mip->height = 16;
	r_notexture_mip->offsets[0] = sizeof(texture_t);
	r_notexture_mip->offsets[1] = r_notexture_mip->offsets[0] + 16 * 16;
	r_notexture_mip->offsets[2] = r_notexture_mip->offsets[1] + 8 * 8;
	r_notexture_mip->offsets[3] = r_notexture_mip->offsets[2] + 4 * 4;

	for (m = 0; m < 4; m++)
	{
		dest = (byte*)r_notexture_mip + r_notexture_mip->offsets[m];
		for (y = 0; y < (16 >> m); y++)
		{
			for (x = 0; x < (16 >> m); x++)
			{
				if ((y < (8 >> m)) ^ (x < (8 >> m)))
					*dest++ = 0;
				else
					*dest++ = 0xFF;
			}
		}
	}
}

/*
===============
R_Init
===============
*/
void R_Init( void )
{
	int		dummy;

// get stack position so we can guess if we are going to overflow
	r_stack_start = (byte*)&dummy;

	R_InitTurb();

	Cmd_AddCommand("timerefresh", R_TimeRefresh_f);
	Cmd_AddCommand("pointfile", R_ReadPointFile_f);

	Cvar_RegisterVariable(&r_luminance);
	Cvar_RegisterVariable(&r_ambient_r);
	Cvar_RegisterVariable(&r_ambient_b);
	Cvar_RegisterVariable(&r_ambient_g);
	Cvar_RegisterVariable(&r_aliastransbase);
	Cvar_RegisterVariable(&r_aliastransadj);
	Cvar_RegisterVariable(&r_mmx);
	Cvar_RegisterVariable(&r_cachestudio);
	Cvar_RegisterVariable(&r_traceglow);
	Cvar_RegisterVariable(&r_maxsurfs);
	Cvar_RegisterVariable(&r_numsurfs);
	Cvar_RegisterVariable(&r_maxedges);
	Cvar_RegisterVariable(&r_numedges);
	Cvar_RegisterVariable(&r_draworder);
	Cvar_RegisterVariable(&r_clearcolor);
	Cvar_RegisterVariable(&r_waterwarp);
	Cvar_RegisterVariable(&r_fullbright);
	Cvar_RegisterVariable(&r_decals);
	Cvar_RegisterVariable(&r_lightmap);
	Cvar_RegisterVariable(&r_lightstyle);
	Cvar_RegisterVariable(&r_drawentities);
	Cvar_RegisterVariable(&r_drawviewmodel);
	Cvar_RegisterVariable(&r_speeds);
	Cvar_RegisterVariable(&r_timegraph);
	Cvar_RegisterVariable(&r_graphheight);
	Cvar_RegisterVariable(&r_drawflat);
	Cvar_RegisterVariable(&r_aliasstats);
	Cvar_RegisterVariable(&r_dspeeds);
	Cvar_RegisterVariable(&r_reportsurfout);
	Cvar_RegisterVariable(&r_reportedgeout);
	Cvar_RegisterVariable(&r_wadtextures);

	Cvar_SetValue("r_mmx", gHasMMXTechnology);
	Cvar_SetValue("r_maxedges", NUMSTACKEDGES);
	Cvar_SetValue("r_maxsurfs", NUMSTACKSURFACES);

	view_clipplanes[0].leftedge = TRUE;
	view_clipplanes[1].rightedge = TRUE;
	view_clipplanes[1].leftedge = view_clipplanes[2].leftedge =
		view_clipplanes[3].leftedge = FALSE;
	view_clipplanes[0].rightedge = view_clipplanes[2].rightedge =
		view_clipplanes[3].rightedge = FALSE;

	r_refdef.xOrigin = XCENTERING;
	r_refdef.yOrigin = YCENTERING;

	R_InitParticles();

// TODO: collect 386-specific code in one place
#if	id386
	Sys_MakeCodeWriteable((long)R_EdgeCodeStart,
		(long)R_EdgeCodeEnd - (long)R_EdgeCodeStart);
#endif	// id386

	D_Init();

	R_DrawInitLut();
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
	int		dummy;

// get stack position so we can guess if we are going to overflow
	r_stack_start = (byte*)&dummy;
}


void R_RenderView( void )
{
	// TODO: Implement
}

/*
================
R_InitTurb
================
*/
void R_InitTurb( void )
{
	int		i;

	for (i = 0; i < (SIN_BUFFER_SIZE); i++)
	{
		sintable[i] = AMP + sin(i * 3.14159 * 2 / CYCLE) * AMP;
		intsintable[i] = AMP2 + sin(i * 3.14159 * 2 / CYCLE) * AMP2;	// AMP2, not 20
	}
}