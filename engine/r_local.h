// r_local.h -- private refresh defs

#ifndef GLQUAKE

#include "r_shared.h"
#include "color.h"

#define BMODEL_FULLY_CLIPPED	0x10 // value returned by R_BmodelCheckBBox ()
									 //  if bbox is trivially rejected

//===========================================================================
// clipped bmodel edges

typedef struct bedge_s
{
	mvertex_t* v[2];
	struct bedge_s* pnext;
} bedge_t;

//===========================================================================

typedef struct auxvert_s {
	float	fv[3];		// viewspace x, y
} auxvert_t;

typedef struct alight_s {
	int			ambientlight;	// clip at 128
	int			shadelight;		// clip at 192 - ambientlight
	vec3_t		color;
	float*		plightvec;
} alight_t;

extern	cvar_t	r_cachestudio;
extern	cvar_t	r_draworder;
extern	cvar_t	r_speeds;
extern	cvar_t	r_timegraph;
extern	cvar_t	r_graphheight;
extern	cvar_t	r_luminance;
extern	cvar_t	r_clearcolor;
extern	cvar_t	r_waterwarp;
extern	cvar_t	r_fullbright;
extern	cvar_t	r_decals;
extern	cvar_t	r_lightmap;
extern	cvar_t	r_lightstyle;
extern	cvar_t	r_drawentities;
extern	cvar_t	r_drawviewmodel;
extern	cvar_t	r_aliasstats;
extern	cvar_t	r_dspeeds;
extern	cvar_t	r_drawflat;
extern	cvar_t	r_ambient_r;
extern	cvar_t	r_ambient_g;
extern	cvar_t	r_ambient_b;
extern	cvar_t	r_numsurfs;
extern	cvar_t	r_numedges;
extern	cvar_t	r_mmx;
extern	cvar_t	r_traceglow;
extern	cvar_t	r_wadtextures;

#define XCENTERING	(1.0 / 2.0)
#define YCENTERING	(1.0 / 2.0)

#define BACKFACE_EPSILON	0.01

//===========================================================================

// !!! if this is changed, it must be changed in asm_draw.h too !!!
typedef struct clipplane_s
{
	vec3_t		normal;
	float		dist;
	struct		clipplane_s* next;
	byte		leftedge;
	byte		rightedge;
	byte		reserved[2];
} clipplane_t;

extern	clipplane_t	view_clipplanes[4];

//=============================================================================

void R_RenderWorld( void );

//=============================================================================

extern	mplane_t	screenedge[4];

extern	vec3_t	r_entorigin;

// TODO: Implement


extern	int		r_visframecount;

//=============================================================================

//
// current entity info
//
extern	qboolean		insubmodel;
extern	vec3_t			r_worldmodelorg;


void R_DrawSprite( void );
void R_TransformPlane( mplane_t* p, float* normal, float* dist );
void R_TransformFrustum( void );

// TODO: Implement

void R_Surf8Patch( void );
void R_Surf16Patch( void );
void R_DrawSubmodelPolygons( model_t* pmodel, int clipflags );
void R_DrawSolidClippedSubmodelPolygons( model_t* pmodel );

void R_BeginEdgeFrame( void );
void R_ScanEdges( void );

// TODO: Implement

void R_ScanEdges( void );

// TODO: Implement

void R_AliasDrawModel( alight_t* plighting );

// TODO: Implement

extern void R_Surf8Start( void );
extern void R_Surf8End( void );
extern void R_Surf16Start( void );
extern void R_Surf16End( void );
extern void R_EdgeCodeStart( void );
extern void R_EdgeCodeEnd( void );

extern void R_RotateBmodel( void );

extern int	c_faceclip;
extern int	r_polycount;
extern int	r_wholepolycount;

// TODO: Implement

extern int		r_currentkey;
extern int		r_currentbkey;

typedef struct btofpoly_s {
	int			clipflags;
	msurface_t* psurf;
} btofpoly_t;

#define MAX_BTOFPOLYS	5000	// FIXME: tune this

extern int			numbtofpolys;
extern btofpoly_t* pbtofpolys;

void	R_InitTurb( void );
void	R_ZDrawSubmodelPolys( model_t* clmodel );

//=========================================================
// Alias models
//=========================================================

#define MAXALIASVERTS		2000

qboolean R_AliasCheckBBox( void );

//=========================================================
// turbulence stuff

#define	AMP		8 * 0x10000
#define	AMP2	3
#define	SPEED	20

//=========================================================
// particle stuff

void R_DrawParticles( void );
void R_InitParticles( void );
void R_ClearParticles( void );
void R_ReadPointFile_f( void );
void R_SurfacePatch( void );

extern int		r_amodels_drawn;
extern edge_t* auxedges;
extern int		r_numallocatededges;
extern edge_t* r_edges, * edge_p, * edge_max;

extern	edge_t* newedges[MAXHEIGHT];
extern	edge_t* removeedges[MAXHEIGHT];

extern	int	screenwidth;

extern	espan_t* span_p;

extern	int	current_iv;
extern  int	edge_head_u_shift20, edge_tail_u_shift20;

// FIXME: make stack vars when debugging done
extern	edge_t	edge_head;
extern	edge_t	edge_tail;
extern	edge_t	edge_aftertail;
extern  int		r_bmodelactive;

extern  float	fv;

extern	float		aliastransform[3][4];
extern	float		aliasxscale, aliasyscale, aliasxcenter, aliasycenter;
extern	int			r_ambientlight;
extern	float		r_shadelight;

extern	vec3_t		r_plightvec;

extern int		r_outofsurfaces;
extern int		r_outofedges;

extern mvertex_t* r_pcurrentvertbase;

void R_AliasClipTriangle( mtriangle_t* ptri );

extern float	r_time1;
extern float	dp_time1, dp_time2, db_time1, db_time2, rw_time1, rw_time2;
extern float	se_time1, se_time2, de_time1, de_time2, dv_time1, dv_time2;
extern int		r_frustum_indexes[4 * 6];
extern int		r_maxsurfsseen, r_maxedgesseen, r_cnumsurfs;
extern qboolean	r_surfsonstack;
extern qboolean	r_dowarpold, r_viewchanged;

extern mleaf_t* r_viewleaf, * r_oldviewleaf;

extern vec3_t	r_emins, r_emaxs;
extern mnode_t* r_pefragtopnode;
extern int		r_clipflags;
extern int		r_dlightframecount;
extern int		r_dlightchanged;	// which ones changed
extern int		r_dlightactive;		// which ones are active
extern qboolean	r_fov_greater_than_90;

void R_TimeRefresh_f( void );
void R_TimeGraph( void );
void R_PrintAliasStats( void );
void R_PrintTimes( void );
void R_PrintDSpeeds( void );
void R_ScreenLuminance( void );
void R_AnimateLight( void );
colorVec R_LightPoint( vec_t* p );
colorVec R_LightVec( vec_t* start, vec_t* end );
void R_SetupFrame( void );
void R_EmitEdge( mvertex_t* pv0, mvertex_t* pv1 );
void R_ClipEdge( mvertex_t* pv0, mvertex_t* pv1, clipplane_t* clip );
void R_SplitEntityOnNode2( mnode_t* node );
void R_MarkLights( dlight_t* light, int bit, mnode_t* node );
void R_DecalInit( void );
void UnpackPalette( unsigned short* pDest, unsigned short* pSource, int r, int g, int b );

#endif