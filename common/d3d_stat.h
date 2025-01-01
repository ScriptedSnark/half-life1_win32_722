
typedef struct tagD3DGlobals
{
LPDIRECTDRAW            lpDD;           // DirectDraw object
LPDIRECTDRAWSURFACE     lpDDSPrimary;   // DirectDraw primary surface
LPDIRECTDRAWSURFACE     lpDDSBack;      // DirectDraw back surface
LPDIRECTDRAWSURFACE     lpDDSMem;		// DirectDraw mem surface
}D3DGLOBALS;
D3DGLOBALS d3dG;
//
void (*QGL_D3DShared)(D3DGLOBALS *d3dGShared);

