// d_iface.h: interface header file for rasterization driver modules

#if !defined( D_IFACE_H )
#define D_IFACE_H
#ifdef _WIN32
#pragma once
#endif

#define WARP_WIDTH		320
#define WARP_HEIGHT		200

#define MAX_LBM_HEIGHT	480

typedef struct
{
	float	u, v;
	float	s, t;
	float	zi;
} emitpoint_t;

#define PARTICLE_Z_CLIP	8.0

typedef struct polyvert_s {
	float	u, v, zi, s, t;
} polyvert_t;

typedef struct polydesc_s {
	int			numverts;
	float		nearzi;
	msurface_t* pcurrentface;
	polyvert_t* pverts;
} polydesc_t;

// !!! if this is changed, it must be changed in d_ifacea.h too !!!
typedef struct finalvert_s {
	int		v[6];		// u, v, s, t, l, 1/z
	int		flags;
	float	reserved;
} finalvert_t;

// !!! if this is changed, it must be changed in d_ifacea.h too !!!
typedef struct
{
	void* pskin;
	maliasskindesc_t* pskindesc;
	int					skinwidth;
	int					skinheight;
	mtriangle_t* ptriangles;
	finalvert_t* pfinalverts;
	int					numtriangles;
	int					drawtype;
	int					seamfixupX16;
} affinetridesc_t;

typedef struct
{
	int			nump;
	emitpoint_t* pverts;	// there's room for an extra element at [nump], 
							//  if the driver wants to duplicate element [0] at
							//  element [nump] to avoid dealing with wrapping
	mspriteframe_t* pspriteframe;
	vec3_t			vup, vright, vpn;	// in worldspace
	float			nearzi;
	float			scale;
} spritedesc_t;

typedef struct
{
	int		u, v;
	float	zi;
	int		color;
} zpointdesc_t;

extern cvar_t	r_drawflat;

extern int		d_spanpixcount;
extern int		r_framecount;		// sequence # of current frame since Quake started

extern qboolean	r_drawpolys;		// 1 if driver wants clipped polygons
									//  rather than a span list
extern qboolean	r_drawculledpolys;	// 1 if driver wants clipped polygons that
									//  have been culled by the edge list
extern qboolean	r_worldpolysbacktofront;	// 1 if driver wants polygons
											//  delivered back to front rather
											//  than front to back
extern qboolean	r_recursiveaffinetriangles;	// true if a driver wants to use
											//  recursive triangular subdivison
											//  and vertex drawing via
											//  D_PolysetDrawFinalVerts() past
											//  a certain distance (normally 
											//  only used by the software
											//  driver)
extern float	r_aliasuvscale;		// scale-up factor for screen u and v
									//  on Alias vertices passed to driver
extern int		r_pixbytes;
extern qboolean	r_dowarp, r_dowarpold, r_viewchanged;

// TODO: Implement

#ifdef __cplusplus
extern "C" {
#endif
extern void (*D_BeginDirectRect)( int x, int y, byte* pbitmap, int width, int height );
extern void (*D_EndDirectRect)( int x, int y, int width, int height );
#ifdef __cplusplus
}
#endif
void D_DisableBackBufferAccess( void );
void D_DrawParticle( particle_t* pparticle );
void D_EnableBackBufferAccess( void );
void D_EndParticles( void );
void D_Init( void );
void D_ViewChanged( void );
void D_SetupFrame( void );
void D_StartParticles( void );
void D_TurnZOn( void );
void D_WarpScreen( void );

void D_UpdateRects( vrect_t* prect );

// !!! must be kept the same as in quakeasm.h !!!
#define TRANSPARENT_COLOR	0xFF

//=======================================================================//

// callbacks to Quake



int DecalListCreate( DECALLIST* pList );



// !!! if this is changed, it must be changed in d_ifacea.h too !!!
#define	CYCLE			128		// turbulent cycle size
#define	CYCLE_MASK		(CYCLE - 1)

extern int		c_surf;
extern vrect_t	scr_vrect;

extern byte* r_warpbuffer;

#endif // D_IFACE_H