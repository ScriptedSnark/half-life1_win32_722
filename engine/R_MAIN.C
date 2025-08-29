// r_main.c

#include "quakedef.h"
#include "winquake.h"

//define	PASSAGES

void* colormap;
int			r_numallocatededges;
int			r_pixbytes = 1;
float		r_aliasuvscale = 1.0;

qboolean	r_dowarp, r_dowarpold, r_viewchanged;

// TODO: Implement

int			c_surf;
int			r_maxsurfsseen, r_maxedgesseen, r_cnumsurfs;
qboolean	r_surfsonstack;
int			r_clipflags;

byte* r_warpbuffer;

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

float	pixelAspect;
float	screenAspect;
float	verticalFieldOfView;
float	xOrigin, yOrigin;

mplane_t	screenedge[4];

//
// refresh flags
//
int		r_framecount = 1;	// so frame counts initialized to 0 don't match
int		r_visframecount;

// TODO: Implement

mleaf_t* r_viewleaf, * r_oldviewleaf;

texture_t* r_notexture_mip;

float		r_aliastransition, r_resfudge;

int		d_lightstylevalue[256];	// 8.8 fraction of base light value

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
}

/*
===============
R_NewMap
===============
*/
void R_NewMap( void )
{
	int		i;
	vrect_t vrect;

// clear out efrags in case the level hasn't been reloaded
// FIXME: is this one short?
	for (i = 0; i < cl.worldmodel->numleafs; i++)
		cl.worldmodel->leafs[i].efrags = NULL;

	r_viewleaf = NULL;
	R_ClearParticles();

	R_DecalInit();
	V_InitLevel();

	r_cnumsurfs = r_maxsurfs.value;

	if (r_cnumsurfs <= MINSURFACES)
		r_cnumsurfs = MINSURFACES;

	if (r_cnumsurfs > NUMSTACKSURFACES)
	{
		surfaces = (surf_t*)Hunk_AllocName(r_cnumsurfs * sizeof(surf_t), "surfaces");
		surface_p = surfaces;
		surf_max = &surfaces[r_cnumsurfs];
		r_surfsonstack = FALSE;
	// surface 0 doesn't really exist; it's just a dummy because index 0
	// is used to indicate no edge attached to surface
		surfaces--;
		R_SurfacePatch();
	}
	else
	{
		r_surfsonstack = TRUE;
	}

	r_maxedgesseen = 0;
	r_maxsurfsseen = 0;

	r_numallocatededges = r_maxedges.value;

	if (r_numallocatededges < MINEDGES)
		r_numallocatededges = MINEDGES;

	if (r_numallocatededges <= NUMSTACKEDGES)
	{
		auxedges = NULL;
	}
	else
	{
		auxedges = (edge_t*)Hunk_AllocName(r_numallocatededges * sizeof(edge_t),
			"edges");
	}

	r_dowarpold = FALSE;
	r_viewchanged = FALSE;
#ifdef PASSAGES
	CreatePassages();
#endif

	vrect.x = 0;
	vrect.y = 0;
	vrect.width = vid.width;
	vrect.height = vid.height;

	R_ViewChanged(&vrect, sb_lines, vid.aspect);

	R_LoadSkys();
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
	int		i;
	float	res_scale;

	r_viewchanged = TRUE;

	R_SetVrect(pvrect, &r_refdef.vrect, lineadj);

	r_refdef.horizontalFieldOfView = 2.0 * tan(scr_fov_value / 360 * M_PI);
	r_refdef.fvrectx = (float)r_refdef.vrect.x;
	r_refdef.fvrectx_adj = (float)r_refdef.vrect.x - 0.5;
	r_refdef.vrect_x_adj_shift20 = (r_refdef.vrect.x << 20) + (1 << 19) - 1;
	r_refdef.fvrecty = (float)r_refdef.vrect.y;
	r_refdef.fvrecty_adj = (float)r_refdef.vrect.y - 0.5;
	r_refdef.vrectright = r_refdef.vrect.x + r_refdef.vrect.width;
	r_refdef.vrectright_adj_shift20 = (r_refdef.vrectright << 20) + (1 << 19) - 1;
	r_refdef.fvrectright = (float)r_refdef.vrectright;
	r_refdef.fvrectright_adj = (float)r_refdef.vrectright - 0.5;
	r_refdef.vrectrightedge = (float)r_refdef.vrectright - 0.99;
	r_refdef.vrectbottom = r_refdef.vrect.y + r_refdef.vrect.height;
	r_refdef.fvrectbottom = (float)r_refdef.vrectbottom;
	r_refdef.fvrectbottom_adj = (float)r_refdef.vrectbottom - 0.5;

	r_refdef.aliasvrect.x = (int)(r_refdef.vrect.x * r_aliasuvscale);
	r_refdef.aliasvrect.y = (int)(r_refdef.vrect.y * r_aliasuvscale);
	r_refdef.aliasvrect.width = (int)(r_refdef.vrect.width * r_aliasuvscale);
	r_refdef.aliasvrect.height = (int)(r_refdef.vrect.height * r_aliasuvscale);
	r_refdef.aliasvrectright = r_refdef.aliasvrect.x +
		r_refdef.aliasvrect.width;
	r_refdef.aliasvrectbottom = r_refdef.aliasvrect.y +
		r_refdef.aliasvrect.height;

	pixelAspect = aspect;
	xOrigin = r_refdef.xOrigin;
	yOrigin = r_refdef.yOrigin;

	screenAspect = r_refdef.vrect.width * pixelAspect /
		r_refdef.vrect.height;
// 320*200 1.0 pixelAspect = 1.6 screenAspect
// 320*240 1.0 pixelAspect = 1.3333 screenAspect
// proper 320*200 pixelAspect = 0.8333333

	verticalFieldOfView = r_refdef.horizontalFieldOfView / screenAspect;

// values for perspective projection
// if math were exact, the values would range from 0.5 to to range+0.5
// hopefully they wll be in the 0.000001 to range+.999999 and truncate
// the polygon rasterization will never render in the first row or column
// but will definately render in the [range] row and column, so adjust the
// buffer origin to get an exact edge to edge fill
	xcenter = ((float)r_refdef.vrect.width * XCENTERING) +
		r_refdef.vrect.x - 0.5;
	aliasxcenter = xcenter * r_aliasuvscale;
	ycenter = ((float)r_refdef.vrect.height * YCENTERING) +
		r_refdef.vrect.y - 0.5;
	aliasycenter = ycenter * r_aliasuvscale;

	xscale = r_refdef.vrect.width / r_refdef.horizontalFieldOfView;
	aliasxscale = xscale * r_aliasuvscale;
	xscaleinv = 1.0 / xscale;
	yscale = xscale * pixelAspect;
	aliasyscale = yscale * r_aliasuvscale;
	yscaleinv = 1.0 / yscale;
	xscaleshrink = (r_refdef.vrect.width - 6) / r_refdef.horizontalFieldOfView;
	yscaleshrink = xscaleshrink * pixelAspect;

// left side clip
	screenedge[0].normal[0] = -1.0 / (xOrigin * r_refdef.horizontalFieldOfView);
	screenedge[0].normal[1] = 0;
	screenedge[0].normal[2] = 1;
	screenedge[0].type = PLANE_ANYZ;

// right side clip
	screenedge[1].normal[0] =
		1.0 / ((1.0 - xOrigin) * r_refdef.horizontalFieldOfView);
	screenedge[1].normal[1] = 0;
	screenedge[1].normal[2] = 1;
	screenedge[1].type = PLANE_ANYZ;

// top side clip
	screenedge[2].normal[0] = 0;
	screenedge[2].normal[1] = -1.0 / (yOrigin * verticalFieldOfView);
	screenedge[2].normal[2] = 1;
	screenedge[2].type = PLANE_ANYZ;

// bottom side clip
	screenedge[3].normal[0] = 0;
	screenedge[3].normal[1] = 1.0 / ((1.0 - yOrigin) * verticalFieldOfView);
	screenedge[3].normal[2] = 1;
	screenedge[3].type = PLANE_ANYZ;

	for (i = 0; i < 4; i++)
		VectorNormalize(screenedge[i].normal);

	res_scale = sqrt((double)(r_refdef.vrect.width * r_refdef.vrect.height) /
		(320.0 * 152.0)) *
		(2.0 / r_refdef.horizontalFieldOfView);
	r_aliastransition = r_aliastransbase.value * res_scale;
	r_resfudge = r_aliastransadj.value * res_scale;

	if (scr_fov_value <= 90.0)
		r_fov_greater_than_90 = FALSE;
	else
		r_fov_greater_than_90 = TRUE;

// TODO: collect 386-specific code in one place
#if	id386
	if (r_pixbytes == 1)
	{
		colormap = vid.colormap;

		Sys_MakeCodeWriteable((long)R_Surf8Start,
			(long)R_Surf8End - (long)R_Surf8Start);
		R_Surf8Patch();
	}
	else
	{
		colormap = vid.colormap16;

		Sys_MakeCodeWriteable((long)R_Surf16Start,
			(long)R_Surf16End - (long)R_Surf16Start);
		R_Surf16Patch();
	}
#endif	// id386

	D_ViewChanged();
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