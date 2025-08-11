// d_iface.h: interface header file for rasterization driver modules

#if !defined( D_IFACE_H )
#define D_IFACE_H
#ifdef _WIN32
#pragma once
#endif

#define WARP_WIDTH		320
#define WARP_HEIGHT		200

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
void D_StartParticles( void );

void D_UpdateRects( vrect_t* prect );

// !!! must be kept the same as in quakeasm.h !!!
#define TRANSPARENT_COLOR	0xFF

//=======================================================================//

// callbacks to Quake



int DecalListCreate( DECALLIST* pList );








#endif // D_IFACE_H